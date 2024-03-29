/*
 * profiler.c
 *
 *  Created on: 17 мар. 2022 г.
 *      Author: wl
 */

#include "em_device.h"
#include "em_cmu.h"
#include "resources.h"
#include "profiler.h"
#include "Tachometer.h"
#include "Reflectance.h"
#include "LaunchPad.h"
#include "Motor.h"
#include "Maze.h"
#include "configure.h"
#include "Logging.h"
#include "benchmark.h"
#include "UART0.h"

#ifdef DEBUG_MAZE
#undef DATALOG
#endif

#ifdef DATALOG
#define LOGGING(A)       data_log(A, 1)
#else
#define LOGGING(A)
#endif


#define COMMAND_QUEUE_FACTOR  7

#define CURRENT_DISTANCE    (STEPS_TO_MM((int32_t)LeftSteps + (int32_t)RightSteps))

int speed = 0;
int slowdistance, startdistance, guarddistance, wait_counter;

// Exchange data
volatile unsigned int segment_length, node_configuration, real_direction = north, profiler_data_ready = 0,
    profiler_queue_empty = 1, profiler_error_code = no_profiler_error;

volatile unsigned int command_queue[1ul << COMMAND_QUEUE_FACTOR], command_read_idx = 126, command_write_idx = 126;
volatile unsigned int block_command_read = 0;

void block_profiler(unsigned int state) {
  block_command_read = state;
}

unsigned int get_command(void) {
  unsigned int result;
  if (command_read_idx == command_write_idx) return command_eof;
  result = command_queue[command_read_idx++];
  command_read_idx &= (1ul << COMMAND_QUEUE_FACTOR) - 1;
  return result;
}

unsigned int read_command(unsigned int ahead) {
  unsigned int result = command_eof, new_idx;
  new_idx = command_read_idx;
  while (ahead--) {
      if (new_idx == command_write_idx) return command_eof;
      result = command_queue[new_idx];
      ++new_idx;
      new_idx &= ((1ul << COMMAND_QUEUE_FACTOR) - 1);
  }
  return result;
}

#ifndef DEBUG_MAZE

unsigned int put_command(unsigned int command_data) {
  if (((command_write_idx + 1) & ((1ul << COMMAND_QUEUE_FACTOR) - 1)) == command_read_idx) return 1;
  command_queue[command_write_idx] = command_data;
  __disable_irq();
  command_write_idx = (command_write_idx + 1) & ((1ul << COMMAND_QUEUE_FACTOR) - 1);
  __enable_irq();
  return 0;
}

#else

unsigned int put_command(unsigned int command_data) {
  switch (command_data & COMMAND_MASK) {
    case command_forward:
      UART0_OutString("Forward: ");
      UART0_OutUDec(command_data & ~COMMAND_MASK);
      UART0_OutString("\r\n");
      break;

    case command_turn:
      switch (command_data & ~COMMAND_MASK) {
        case 1:
          UART0_OutString("Turn Right\r\n");
          break;
        case 2:
          UART0_OutString("Turn Back\r\n");
          break;
        case 3:
          UART0_OutString("Turn Left\r\n");
          break;
        default:
          UART0_OutString("Turn Error in parameter: ");
          UART0_OutUDec(command_data & ~COMMAND_MASK);
          UART0_OutString("\r\n");
          break;
      }
      break;

    case command_finish:
      UART0_OutString("Finish ");
      UART0_OutUDec(command_data & ~COMMAND_MASK);
      UART0_OutString("\r\n");
      break;

    case command_entrance:
      UART0_OutString("Entrance ");
      UART0_OutUDec(command_data & ~COMMAND_MASK);
      UART0_OutString("\r\n");
      break;

    case command_wait:
      UART0_OutString("Wait ");
      UART0_OutUDec(command_data & ~COMMAND_MASK);
      UART0_OutString("\r\n");
      break;

    case command_back:
      UART0_OutString("Back ");
      UART0_OutUDec(command_data & ~COMMAND_MASK);
      UART0_OutString("\r\n");
      break;

    case command_benchmark:
      if (command_data & ~COMMAND_MASK) {
          UART0_OutString("Benchmark start\r\n");
      } else {
          UART0_OutString("Benchmark stop\r\n");
      }
      break;

    default:
      UART0_OutString("Unknown command ");
      UART0_OutUDec(command_data & ~COMMAND_MASK);
      UART0_OutString("\r\n");
      break;
  }
  return 0;
}
#endif

void drop_command_queue(void) {
  command_read_idx = command_write_idx = 0;
  profiler_data_ready = 0;
  speed = 0;
  Motor_Speed(speed, speed);
}

// функция движения по сегменту.
// Робот движется со скоростью maxspeed на участке slowdistance, затем сбавляет до minspeed.
// в пределах guarddist игнорируется пропадание линии и срабатывание боковых сенсоров.

unsigned int run_segment(void) {
  unsigned int activ_sensor_count, last_sensor, first_sensor, photo_mask;
  unsigned int blind, i;
  static int blindcount = 0, finishcount = 0, sigma_error = 0, prev_track_error = 0;
  int track_error, de_dt, left_speed, right_speed, result, maxspeed;

  if (photo_data_ready ) {
      photo_data_ready = 0;

      maxspeed = (CURRENT_DISTANCE > slowdistance) ? data.minspeed : data.maxspeed;

      blind = 1; activ_sensor_count = 0; last_sensor = 256; first_sensor = 256;

      photo_mask = current_sensor;
      LOGGING(log_segment | photo_mask);

      unsigned int mask = 0x01;
      for (i=0; i<8; i++) {
          if (photo_mask & mask) {
              blind = 0;
              activ_sensor_count++;
              last_sensor = i;
              if (first_sensor == 256) first_sensor = i;
          }
          mask <<= 1;
      }

      //  *  Теперь исходя из данных фотосенсоров пытаемся установить координаты линии.
      //  *  0 - центр, края +/- 332.
      //  * Если нет сигнала вообще - тупик - выход из подпрограммы.

      if (blind && CURRENT_DISTANCE >= guarddistance) {
          if (++blindcount > 2) {
              blindcount = 0;
              sigma_error = 0;
              finishcount = 0;
              segment_length = CURRENT_DISTANCE - startdistance + data.sensor_offset;
              return 1; // нет линии - тупик
          }
          track_error = prev_track_error;
      } else {
          blindcount = 0;
          if (CURRENT_DISTANCE >= guarddistance) {
              if ((photo_mask & 0x81) && ((last_sensor - first_sensor) == (activ_sensor_count - 1))) {
                  // есть срабатывание боковых датчиков - поворот или перекрёсток
                  if (++finishcount > 1) {
                      blindcount = 0;
                      sigma_error = 0;
                      finishcount = 0;
                      segment_length = CURRENT_DISTANCE - startdistance + data.sensor_offset;
                      return 1;
                  }                   // Ждём повторного срабатывания и выходим
              } else {
                  finishcount = 0;    // Если нет повторного - показалось.
              }
              // Для ПИД коррекции сужаем аппертуру сенсоров, участвующих в вычислении ошибки позиции
              // так как к этому времени робот уже должен быть стабилизирован на линии. - нет, что-то не то.
              if (first_sensor < 2) first_sensor = 2;
              if (last_sensor  > 5) last_sensor  = 5;
          }
          track_error = line_error(first_sensor, last_sensor);
      }

      // Теперь имеем значение ошибки/смещения и дифференциальное значение ошибки

      de_dt = data.k_diff*(track_error - prev_track_error);
      prev_track_error = track_error;

      sigma_error += track_error;
      sigma_error = constrain(sigma_error, -2048, 2048);
//      if (sigma_error > 2048) sigma_error = 2048;
//      if (sigma_error < -2048) sigma_error = -2048;

      result = data.k_error*track_error + de_dt + ((sigma_error * data.k_integral) >> 10);

      if (speed < maxspeed) {
          if ((speed += data.acceleration) > maxspeed) speed = maxspeed;
      } else if (speed > maxspeed) {
          if ((speed -= data.acceleration) < maxspeed) speed = maxspeed;
      }

      int nom_speed = (speed < data.minspeed) ? data.minspeed : speed;
      left_speed  = speed+(result*nom_speed)/8192; // speed*(1+result/nom_speed)
      right_speed = speed-(result*nom_speed)/8192;

//      if (left_speed > data.maxmotor) left_speed = data.maxmotor;
//      else if (left_speed < -data.maxmotor) left_speed = -data.maxmotor;
//
//      if (right_speed > data.maxmotor) right_speed = data.maxmotor;
//      else if (right_speed < -data.maxmotor) right_speed = -data.maxmotor;

      Motor_Speed(left_speed, right_speed);

  }
  return 0;
}



unsigned int check_node(void) {
  unsigned int ii, photo_sensor;
  static unsigned int delay = 0, available = 0, blind = 0;


  if (photo_data_ready ) {
      unsigned int linecount = 0, prev_stat = 0;
      photo_data_ready  = 0;
      photo_sensor = current_sensor;
      LOGGING(log_check | (available << 4) | photo_sensor);

      if (delay) {
          delay--;
          if (photo_sensor == 0) blind = 1;
          if (delay == 0) {
              // now detect if we have straight segment
              if ((photo_sensor & ((1u << 2) | (1u << 3) | (1u << 4) | (1u << 5))) && !blind) {
                  available |= STRAIGHT_MASK;
              }

              // Тут передать информацию о конфигурации узла.

              node_configuration = available;
              //              segment_length = CURRENT_DISTANCE - startdistance + data.sensor_offset;
              LOGGING(log_segment0 | ((segment_length >>  0) & 0xFF));
              LOGGING(log_segment1 | ((segment_length >>  8) & 0xFF));
              LOGGING(log_segment2 | ((segment_length >> 16) & 0xFF));
              LOGGING(log_segment3 | ((segment_length >> 24) & 0xFF));
              profiler_data_ready = 1;

              delay = 0; blind = 0; available = 0;  // очистка перед следующим узлом.
              return 1;
          }
      } else {
          if (photo_sensor & ((1u << 0) | (1u << 7))) {  // есть отходы в сторону
              // проверка условия непрерывности линии
              unsigned int sensor_copy = photo_sensor;
              for (ii=0; ii < 8; ii++) {
                  if (prev_stat != (sensor_copy & 0x01)) {
                      if ((prev_stat = (sensor_copy & 0x01)) == 1) linecount++;
                  }
                  sensor_copy >>= 1;
              }
              // поворот детектируется только если есть одна непрерывная линия
              if (linecount == 1) {
                  if (photo_sensor & (1u << 7)) { available |= LEFT_MASK; }
                  if (photo_sensor & (1u << 0)) { available |= RIGHT_MASK; }
              }
          } else {  // больше нет отходов в стороны
              delay = 10; blind = 0;
          }
      } // delay

      Motor_Speed(speed, speed);
  } // photo_data_ready
  return 0;
}

// Торможение после анализа перекрёстка до скорости turnspeed.
unsigned int slowdown(void) {

  if (photo_data_ready) {
      photo_data_ready = 0;
      LOGGING(log_blind | current_sensor);
      if (CURRENT_DISTANCE < guarddistance) {
          if ((speed -= data.acceleration) < data.turnspeed) speed = data.turnspeed;
          Motor_Speed(speed, speed);
      } else {
          speed = 0; // ??????????????????????
          Motor_Speed(speed, speed);
          return 1;
      }
  }
  return 0;
}

unsigned int back_run(void) {
  if (photo_data_ready) {
      photo_data_ready = 0;
      LOGGING(log_back | current_sensor);
      if (CURRENT_DISTANCE > guarddistance) {
          if (CURRENT_DISTANCE > slowdistance) {
              if ((speed += data.acceleration) > data.maxspeed) speed = data.maxspeed;
          } else {
              if ((speed -= data.acceleration) < data.turnspeed) speed = data.turnspeed;
          }
          Motor_Speed(-speed, -speed);
      } else {
          speed = 0;
          Motor_Speed(speed, speed);
          return 1;
      }
  }
  return 0;
}

unsigned int blind_entrance(void) {
  static unsigned int find_line = 0, photo_sensor;
  if (photo_data_ready) {
      photo_data_ready = 0;
      photo_sensor = current_sensor;
      LOGGING(log_entrance | photo_sensor);
      if (photo_sensor) find_line++;
      else find_line = 0;

      if (find_line > 1) {
          find_line = 0;
          segment_length = CURRENT_DISTANCE - startdistance + data.sensor_offset;
          return 1;
      }

      if (CURRENT_DISTANCE < guarddistance) {
          if (CURRENT_DISTANCE < slowdistance) {
              if ((speed += data.acceleration) > data.maxspeed) speed = data.maxspeed;
          } else {
              if ((speed -= data.acceleration) < data.minspeed) speed = data.minspeed;
          }
          Motor_Speed(speed, speed);
      } else {
          find_line = 0;
          speed = 0; // ??????????????????????
          Motor_Speed(speed, speed);
          return 3;
      }
  }
  return 0;
}

unsigned int wait(void) {
  if (photo_data_ready) {
      photo_data_ready = 0;
      LOGGING(log_wait | current_sensor);
      return  !(wait_counter--);
  }
  return 0;
}


// returns 0 if OK, 1 if ERROR
static rotation_dir_t last_turn = right;
unsigned int  turn_dir, degree;
int stop_difference, fail_difference, slow_difference;

#define MAKE_TURN(DIFF, SENSOR, SPEEDL, SPEEDR)                                 \
  unsigned int photo_sensor;                                                    \
  static unsigned int count = 0, last_status = 0;                               \
  if (photo_data_ready ) {                                                      \
      photo_data_ready  = 0;                                                    \
      photo_sensor = current_sensor;                                            \
      LOGGING(log_turn | photo_sensor);                                         \
      if ((DIFF) < slow_difference) {                                           \
          if ((speed += data.acceleration) > data.maxmotor) speed = data.maxmotor;                     \
      } else {                                                                  \
          if ((speed -= data.acceleration) < data.turnspeed) speed = data.turnspeed;\
      }                                                                         \
                                                                                \
      if ((photo_sensor & (1 << SENSOR)) ^ last_status) count++;                \
      last_status = photo_sensor & (1 << SENSOR);                               \
      if ((DIFF) > fail_difference) {                                           \
          count = 0;                                                            \
          last_status = 0;                                                      \
          return 3;                                                             \
      }                                                                         \
      if (count == 2) {                                                         \
          if ((DIFF) < stop_difference) count = 0;                              \
          else {                                                                \
              speed = 0;                                                        \
              count = 0;                                                        \
              last_status = 0;                                                  \
              Motor_Speed(0, 0);                                                \
              return 1;                                                         \
          }                                                                     \
      }                                                                         \
      Motor_Speed(SPEEDL, SPEEDR);                                              \
  }                                                                             \
  return 0;

unsigned int make_turn_left(void) {  // Turn left
    MAKE_TURN(((int32_t)RightSteps - (int32_t)LeftSteps), (4), -speed, speed)
}

unsigned int make_turn_right(void) {  // Turn right.
    MAKE_TURN(((int32_t)LeftSteps - (int32_t)RightSteps), (3), speed, -speed)
}

typedef enum {
  idle,
  segment_run,
  check,
  blind_run,
  turn_right,
  turn_left,
  wait_still,
  do_entrance,
  blind_back
} command_state_t;

void profiler(void) {
  static command_state_t state = idle;
  unsigned int command, command_param, total_length, ii, cmd_status;
  int brakepath1, brakepath2;

  while (state == idle) {
      if (block_command_read) return;
      command  = get_command();
      command_param = command & ~COMMAND_MASK;
      profiler_queue_empty = 0;
      switch (command & COMMAND_MASK) {
        case command_eof:
          profiler_queue_empty = 1;
          return;

        case command_wait:
          wait_counter = command_param;
          state = wait_still;
          break;

        // движение прямо по сегменту. В младших битах длина сегмента для расчета разгона и торможения.
        // разделяется на 3 фазы: движение по прямой линии, исследование узла и выравнивание ЦТ над узлом.
        // так же проверяются следующие команды и если они тоже движение прямо, разгон и торможение расчитываются
        // с учетом последующих сегментов и тогда на промежуточных узлах выравнивание не выполняется.
        case command_forward:
          state = segment_run;
          Motor_Enable();
          total_length = command_param;
          if (total_length) {
              ii = 1;
              while ((((command = read_command(ii++)) & COMMAND_MASK) == command_forward) ||
                      ((command & COMMAND_MASK) == command_finish)) {
                  total_length += (command & ~COMMAND_MASK);
              }
          }
          if (speed == 0) startdistance = CURRENT_DISTANCE;
#ifdef SPEED_MM_PER_SECOND
          if (speed == 0) startdistance = CURRENT_DISTANCE;
          // Расстояние, необходимое для торможения, если максимальная скорость не набирается:
          // (V^2 - v^2) / (4 * a)  + distance/2
          // V = speed * 0.1
          // a = data.acceleration * 0.1 / FRAMESCANPERSECOND;
          brakepath1 = ((long long)(speed + data.minspeed)*(speed - data.minspeed)+(data.acceleration*2*10*FRAMESCANPERSECOND))/
                                    (data.acceleration*4*10*FRAMESCANPERSECOND) + total_length/2;
          // Расстояние, необходимое для торможения от максимальной скорости:
          // (V^2 - v^2) / (2 * a)
          brakepath2 = ((long long)(data.maxspeed + data.minspeed)*(data.maxspeed - data.minspeed)+(data.acceleration*10*FRAMESCANPERSECOND))/
                                    (data.acceleration*2*10*FRAMESCANPERSECOND);
#else
          // Расстояние, необходимое для торможения, если максимальная скорость не набирается:
          // (220mm/100)^2 * (V^2 - v^2) / (4 * a * 400*60)  + distance/2
          brakepath1 = (long long)(speed + data.minspeed)*(speed - data.minspeed) * 121/data.acceleration/2400000 + total_length/2;
          // Расстояние, необходимое для торможения от максимальной скорости:
          // (220mm/100)^2 * (V^2 - v^2) / (2 * a * 400*60)
          brakepath2 = (long long)(data.maxspeed + data.minspeed)*(data.maxspeed - data.minspeed) * 121/data.acceleration/1200000;
#endif
          // используем вариант с самым коротким тормозным путём
          if ((brakepath1 < brakepath2) && (brakepath1 >= 0)) {
              slowdistance = startdistance + total_length - data.sensor_offset - brakepath1;
          } else {
              slowdistance = startdistance + total_length - data.sensor_offset - brakepath2;
          }
          guarddistance = startdistance + data.guarddist;
          break;


        // Поворот в узле. В младших битах параметр поворота 1-вправо на 90 градусов,
        // 2- разворот на 180 градусов, 3-влево на 90 градусов.
        // любой другой аргумент приводит к отмене очереди команд. Направление разворота зависит
        // от предыдущего поворота и выполняется в противоположном направлении.
        // Если при выполнении поворота или разворота поворот по одометру не попадает в валидное
        // окно, то считается ошибкой и происходит отмена очереди команд.
        case command_turn:
          turn_dir = command_param;
          if (turn_dir == back) {
              degree = 180;
              turn_dir = (last_turn == left) ? right : left;
          } else {
              degree = 90;
          }

//          reset_steps();
          Motor_Enable();
//#define SLOW_DIST_OLD
          stop_difference = ((degree == 180) ? DEGREE_TO_STEPS(135) : DEGREE_TO_STEPS(45));
          fail_difference = ((degree == 180) ? DEGREE_TO_STEPS(270) : DEGREE_TO_STEPS(135));
#ifdef SLOW_DIST_OLD
          slow_difference = ((degree == 180) ? DEGREE_TO_STEPS(120) : DEGREE_TO_STEPS(60));
#else
          {
              int circle_length = (degree == 180) ? ((180 * TRACK_WIDE * 314)+(100*360/2)) / (100 * 360) : ((90 * TRACK_WIDE * 314)+(100*360/2)) / (100 * 360);
              int brakepath = (long long)data.maxmotor*data.maxmotor * 121/data.acceleration/1200000;
              if ((brakepath * 2) > circle_length) {
                  slow_difference = MM_TO_STEPS(circle_length/2);
              } else {
                  slow_difference = MM_TO_STEPS(circle_length - brakepath);
              }
          }
#endif
          switch (turn_dir) {
            case left:      // turn left
              last_turn = left;
              state = turn_left;
              stop_difference += ((int32_t)RightSteps - (int32_t)LeftSteps);
              slow_difference += ((int32_t)RightSteps - (int32_t)LeftSteps);
              fail_difference += ((int32_t)RightSteps - (int32_t)LeftSteps);
              break;

            case right:
              last_turn = right;
              state = turn_right;
              stop_difference += ((int32_t)LeftSteps - (int32_t)RightSteps);
              slow_difference += ((int32_t)LeftSteps - (int32_t)RightSteps);
              fail_difference += ((int32_t)LeftSteps - (int32_t)RightSteps);
              break;

            default:
              profiler_error_code = profiler_error_turn_arg;
              drop_command_queue();
              state = idle;
          }
          break;

        case command_entrance:
          state = do_entrance;
          speed = 0;
          Motor_Enable();
          total_length = command_param;
          if (total_length) {
              ii = 1;
              while (((command = read_command(ii++)) & COMMAND_MASK) == command_forward) {
                  total_length += (command & ~COMMAND_MASK);
              }
          }
          startdistance = CURRENT_DISTANCE;
#ifdef SPEED_MM_PER_SECOND
          if (speed == 0) startdistance = CURRENT_DISTANCE;
          // Расстояние, необходимое для торможения, если максимальная скорость не набирается:
          // (V^2 - v^2) / (4 * a)  + distance/2
          // V = speed * 0.1
          // a = data.acceleration * 0.1 / FRAMESCANPERSECOND;
          brakepath1 = ((long long)(speed + data.minspeed)*(speed - data.minspeed)+(data.acceleration*2*10*FRAMESCANPERSECOND))/
                                    (data.acceleration*4*10*FRAMESCANPERSECOND) + total_length/2;
          // Расстояние, необходимое для торможения от максимальной скорости:
          // (V^2 - v^2) / (2 * a)
          brakepath2 = ((long long)(data.maxspeed + data.minspeed)*(data.maxspeed - data.minspeed)+(data.acceleration*10*FRAMESCANPERSECOND))/
                                    (data.acceleration*2*10*FRAMESCANPERSECOND);
#else
          // Расстояние, необходимое для торможения, если максимальная скорость не набирается:
          // (220mm/100)^2 * (V^2 - v^2) / (4 * a * 400*60)  + distance/2
          brakepath1 = (long long)(speed + data.minspeed)*(speed - data.minspeed) * 121/data.acceleration/2400000 + total_length/2;
          // Расстояние, необходимое для торможения от максимальной скорости:
          // (220mm/100)^2 * (V^2 - v^2) / (2 * a * 400*60)
          brakepath2 = (long long)(data.maxspeed + data.minspeed)*(data.maxspeed - data.minspeed) * 121/data.acceleration/1200000;
#endif
          // используем вариант с самым коротким тормозным путём
          if ((brakepath1 < brakepath2) && (brakepath1 >= 0)) {
              slowdistance = startdistance + total_length - data.sensor_offset - brakepath1;
          } else {
              slowdistance = startdistance + total_length - data.sensor_offset - brakepath2;
          }
//          guarddistance = (data.cell_step) ? data.cell_step : 150;
          guarddistance = startdistance + command_param; //   15см защитный интервал
          break;


        // Проезжаем на 10 см вперед на скорости data.turnspeed, чтобы встать на финишное поле.
        case command_finish:
          state = blind_run;
          Motor_Enable();
          if (speed == 0) startdistance = CURRENT_DISTANCE;
          guarddistance = command_param;
          if (guarddistance == 0) guarddistance = 100;
          guarddistance += startdistance;
          break;

        case command_back:
          state = blind_back;
          Motor_Enable();
          guarddistance = CURRENT_DISTANCE - command_param;
          slowdistance  = CURRENT_DISTANCE - command_param/2;
          break;

        case command_benchmark:
          if (command_param) {
              benchmark_start();
          } else {
              benchmark_stop();
          }
          break;

        default:
          state = idle;
          profiler_error_code = profiler_error_cmd_err;
          drop_command_queue();
          break;
      }
//      break;
      // конец интерпретации команд.
  }

  switch (state) {
    // Состояния профайлера. switch (state) продолжение
    case segment_run:
      if (run_segment()) {
          state = check;
          guarddistance = startdistance = CURRENT_DISTANCE + data.sensor_offset;
      }
      break;

    case check:
      if (check_node()) {
          unsigned int next_command = read_command(1) & COMMAND_MASK;
          if ((next_command == command_forward) ||
              (next_command == command_finish)) {
              state = idle; // читаем следующую команду
          } else {
              state = blind_run;
          }
      }
      break;

    case blind_run:
      if ((read_command(1) & COMMAND_MASK) != command_forward){
          if (slowdown()) {
              speed = 0;
              Motor_Speed(0, 0);
              state = idle;
          }
      } else {
          state = idle;
      }
      break;

    case blind_back:
      if (back_run()) {
          speed = 0;
          Motor_Speed(0, 0);
          state = idle;
      }
      break;

    case wait_still:
      if (wait()) {
          state = idle;
          Motor_Disable();
      }
      break;

    case turn_left:
      if ((cmd_status = make_turn_left()) > 0) {
          speed = 0;
          state = idle;
          reset_steps();
          real_direction -= (degree == 90) ? 1 : 2;
          real_direction &= TURN_MASK;
          if (cmd_status > 1) {
              profiler_error_code = profiler_error_turn;
              drop_command_queue();
              state = idle;
          }
      }
      break;

    case turn_right:
      if ((cmd_status = make_turn_right()) > 0) {
          speed = 0;
          state = idle;
          reset_steps();
          real_direction += (degree == 90) ? 1 : 2;
          real_direction &= TURN_MASK;
          if (cmd_status > 1) {
              profiler_error_code = profiler_error_turn;
              drop_command_queue();
              state = idle;
          }
      }
      break;

    case do_entrance:
      if ((cmd_status = blind_entrance()) > 0) {
          state = idle;
          startdistance = CURRENT_DISTANCE + data.sensor_offset;
//          guarddistance = startdistance + data.guarddist;
//          state = segment_run;
          if (cmd_status > 1) {
              profiler_error_code = profiler_error_no_entrance;
              drop_command_queue();
              state = idle;
          }
      }
      break;

    default:
      if (state != idle) {
        profiler_error_code = profiler_error_state_err;
        drop_command_queue();
        state = idle;
      }
  }
}

void test_profile(void) {
  data_log_init();
  //  block_profiler(1);
  put_command(command_wait | 2 * FRAMESCANPERSECOND);
  put_command(command_entrance | (data.cell_step/2));
  put_command(command_forward);
  put_command(command_turn | left);
  put_command(command_forward | 350);
  put_command(command_forward | 300);
  put_command(command_wait | 1 * FRAMESCANPERSECOND);
  put_command(command_turn | back);
  put_command(command_wait | 1 * FRAMESCANPERSECOND);
  put_command(command_forward | 300);
  put_command(command_forward | 350);
  put_command(command_turn | right);
  put_command(command_forward);
  put_command(command_finish | 100);
  //  block_profiler(0);
  data_log_finish();
}

#ifdef PROFILER_PENDSV
void PendSVInit(void) {
  NVIC_SetPriority(PendSV_IRQn, PendSV_IRQ_priority);
}
#else
void PendSVInit(void) {
  CMU_ClockEnable(cmuClock_TIMER3, 1);
  NVIC_SetPriority(TIMER3_IRQn, PendSV_IRQ_priority);
  NVIC_EnableIRQ(TIMER3_IRQn);
  TIMER3->IEN = TIMER_IEN_CC0;
}
#endif

#ifdef PROFILER_PENDSV
void PendSV_Handler(void) {
  profiler();
}
#else

void TIMER3_IRQHandler(void) {
  if ((TIMER3->IEN & TIMER_IEN_CC0) && (TIMER3->IF & TIMER_IF_CC0)) {
      TIMER3->IFC = TIMER_IFC_CC0;
      profiler();
  }
}
#endif
