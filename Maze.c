/*
 * Maze.c
 *
 *  Created on: 24 февр. 2019 г.
 *      Author: wl
 */
#include "spi_drv.h"
#include "configure.h"
#include "display.h"
#include "keyboard.h"
#include "LaunchPad.h"
#include "color_sensor.h"
#include "profiler.h"
#include "Logging.h"
#include "rand.h"
//#include "math.h"

#define MAX_PATH_LENGTH (sizeof(data.path)/sizeof(data.path[0]))
rotation_dir_t path[MAX_PATH_LENGTH];
int length[MAX_PATH_LENGTH];

map_cell_t map[MAX_MAP_SIZE];

const unsigned int turn_sequence[][3] = {
				{RIGHT_MASK, STRAIGHT_MASK, LEFT_MASK},
				{LEFT_MASK, STRAIGHT_MASK, RIGHT_MASK},
				{RIGHT_MASK, LEFT_MASK, STRAIGHT_MASK},
				{LEFT_MASK, RIGHT_MASK, STRAIGHT_MASK},
				{STRAIGHT_MASK, RIGHT_MASK, LEFT_MASK},
				{STRAIGHT_MASK, LEFT_MASK, RIGHT_MASK},
};

// This function decides which way to turn during the learning phase of
// maze solving.  It uses the variable available_directions which indicate
// whether there is an exit in each of the three directions. If no - turn back
rotation_dir_t SelectTurn(unsigned int available_directions, const unsigned int * sequence) {
    unsigned int  i;
    if (available_directions) {
        for (i = 0; i < 3; i++) {
            if (available_directions & sequence[i]) break;
        }
        switch (sequence[i]) {
            case STRAIGHT_MASK:
                return straight;
            case LEFT_MASK:
                return left;
            case RIGHT_MASK:
                return right;
            default:
                return back;
        }
    }
    return back;
}

// simplifyPath analyzes the path[] array and reduces all the
// back turns. For example: Right turn + Back + Right turn = Straight.
void simplifyPath(void) {
    int total_angle = straight;
    int i;

    // only simplify the path if the second-to-last turn was a "Back"
    if(data.pathlength < 3 || path[data.pathlength - 2] != back) return;


    for(i = 1; i <= 3; i++) total_angle += path[data.pathlength - i];
    total_angle &= TURN_MASK;
    path[data.pathlength - 3] = (rotation_dir_t)total_angle;
    // The path is now two steps shorter.
    data.pathlength -= 2;
}


void update_coordinate (coordinate_t *current_coordinate, unsigned int length, bearing_dir_t moving_direction) {
	switch (moving_direction) {
		case north:	current_coordinate->north += length;	break;
		case south:	current_coordinate->north -= length;	break;
		case east:	current_coordinate->east += length;		break;
		case west:	current_coordinate->east -= length;		break;
	}
}

unsigned int PlayMaze(void) {
    unsigned int step=0;

    if (data.pathlength == 0) return 1;
    copy_data_dma(data.path, path, data.pathlength);
    copy_data32_dma((uint32_t*)data.length, (uint32_t*)length, data.pathlength);

    while (put_command(command_wait | 2 * FRAMESCANPERSECOND)) continue;
    while (put_command(command_entrance | data.cell_step)) continue;
    while(dma_flag) continue;

    while (put_command(command_forward | length[step])) continue;
    do {

        if (path[step] == straight) {
            while (put_command(command_forward | length[++step])) continue;
        } else {
            if (path[step] == back) break;
            while (put_command(command_turn | path[step++])) continue;
            while (put_command(command_forward | length[step])) continue;
        }
    } while(1);
    while (put_command(command_finish)) continue;
    while (put_command(command_wait | 2 * FRAMESCANPERSECOND)) continue;
    while (profiler_queue_empty == 0) continue;
    profiler_data_ready = 0;
    return 0;
}

void SpeedPlay(void) {
  squareXY(0, 0, 63, 128, 0);
  show_number(data.runnumber, 0);
  PlayMaze();
}

// The solveMaze() function works by applying a Luc-Tremo algorithm.
// Turn selection depends on strategy selected by variable data.lefthand
// (values from 0 to 5, otherwise uses random),
// excluding branches where robot already was checked.
unsigned int solveMaze(void) {
  unsigned int available, ii, next_segment_length = 0;
  unsigned int count_green, count_yellow, count_red,
               skiplevel,         // переменная выбора проходов: 0 - разворот,
                                  // 1 - зелёные - ни разу не посещенные,
                                  // 2 - желтые - посещенные один раз,
                                  // 3 - красные не выбираются - приехали.
               skippasscount = 0, // переменная запрещающая вносить изменения в поле passcount карты сразу
                                  // после окончания сгенерённого маршрута напрямик.
               play = 0;          // Переменная укзывающая сколько еще узлов надо проехать по сгенеренному маршруту
                                  //
  unsigned int last_index = 0, current_index, found_red = 0;
  unsigned int expect_index = UNKNOWN, turn_index;
  //    volatile unsigned int record_count = 0;
  rotation_dir_t   turn_direction = straight;
  int		move_direction = north, pathsequence;
  coordinate_t my_coordinate = {0, 0};
  unsigned int max_map_cell = 0;

  data_log_init();
  drop_command_queue();
  // Проезжаем вперед на половину корпуса вслепую
  put_command(command_wait | 2 * FRAMESCANPERSECOND);
  if (data.cell_step) put_command(command_entrance | data.cell_step);
  else                put_command(command_entrance | 150);
  put_command(command_forward | next_segment_length);
  {
      max_map_cell = 0;
      found_red = 0;
      last_index = 0;
      data.pathlength = 0;
      data.green_cell_nr = 0;

      map[0].coordinate.north = my_coordinate.north;
      map[0].coordinate.east  = my_coordinate.east;
      map[0].pass_count[0] = 1;
      map[0].pass_count[1] = 4;
      map[0].pass_count[2] = 4;
      map[0].pass_count[3] = 4;
      map[0].node_link[0] = 0;
      map[0].node_link[1] = UNKNOWN;
      map[0].node_link[2] = UNKNOWN;
      map[0].node_link[3] = UNKNOWN;
      max_map_cell++;
  }

  do {
      // Navigate current line segment

      while (!profiler_data_ready) {
          if (profiler_error_code) {
              char err_code_str[] = "Error Code 0", last_idx_str[] = "Last_index =  0";
              squareXY(0,0, 127, 63, 1);
              update_display();
              err_code_str[11] = '0'+profiler_error_code;
              last_idx_str[14] = last_index % 10 + '0';
              last_idx_str[13] = (last_index / 10) ? (last_index / 10 + '0') : ' ';
              putstr(0, 3, "Profiler 1", 1);
              putstr(0, 4, err_code_str, 1);
              putstr(0, 5, last_idx_str, 1);
              profiler_error_code = 0;
              data_log_finish();
              return 1;
          }
          Rand();
          continue;
      }
      LaunchPad_Output(0);
      available = node_configuration;
      profiler_data_ready = 0;

      // если указана величина клетки, то подгоняем длину сегмента под этот шаг.
      if (data.cell_step) {
          segment_length = ((segment_length + data.cell_step/2) / data.cell_step) * data.cell_step;
          if (segment_length == 0) segment_length = data.cell_step;
      }
      update_coordinate(&my_coordinate, segment_length, (bearing_dir_t)move_direction);

      for (current_index = 0; current_index < max_map_cell; current_index++) { // ищем похожие координаты

          // Теперь выполняем "выравнивание" робота в координатной сетке ортогонального лабиринта
          //				if ((ABS(my_coordinate.east - map[current_index].coordinate.east)) < data.tolerance) {
          //					my_coordinate.east = map[current_index].coordinate.east;
          //				}
          //				if ((ABS(my_coordinate.north - map[current_index].coordinate.north)) < data.tolerance) {
          //					my_coordinate.north = map[current_index].coordinate.north;
          //				}

          // И проверяем, были ли мы уже здесь?
          if ((ABS(my_coordinate.east - map[current_index].coordinate.east) < data.tolerance) &&
              (ABS(my_coordinate.north - map[current_index].coordinate.north) < data.tolerance)) {
              // я тут был.
              my_coordinate.east = map[current_index].coordinate.east;
              my_coordinate.north = map[current_index].coordinate.north;
              break; // current_index указывает на ячейку карты.
          }  // нет, мы тут еще не были
      }

      if (data.pathlength == 0) {
          if (current_index == max_map_cell && expect_index == UNKNOWN) {  // новая ячейка
              map[current_index].coordinate.east = my_coordinate.east;
              map[current_index].coordinate.north = my_coordinate.north;
              map[current_index].node_link [(move_direction + back) & TURN_MASK] = last_index;
              map[current_index].node_link [(move_direction+straight) & TURN_MASK] = UNKNOWN;
              map[current_index].node_link [(move_direction + left)  & TURN_MASK] = UNKNOWN;
              map[current_index].node_link [(move_direction + right) & TURN_MASK] = UNKNOWN;

              map[current_index].pass_count[(move_direction + back) & TURN_MASK] = 1;
              map[current_index].pass_count[(move_direction+straight) & TURN_MASK]	=	(available & STRAIGHT_MASK) ? 0 : 4;
              map[current_index].pass_count[(move_direction + left) & TURN_MASK] = 	(available & LEFT_MASK)     ? 0 : 4;
              map[current_index].pass_count[(move_direction + right)& TURN_MASK] = 	(available & RIGHT_MASK)    ? 0 : 4;

              if (++max_map_cell > MAX_MAP_SIZE) {
                  drop_command_queue();
                  squareXY(0,0, 127, 63, 1);
                  update_display();
                  putstr(0, 3, " OUT OF MEMORY  ", 1);
                  max_map_cell = MAX_MAP_SIZE;
              }
          } else { // старая ячейка
              if ((expect_index != UNKNOWN) && (expect_index != current_index)) {
                  if ((expect_index != UNKNOWN) && data.ignore_coordinate_error) {
                      current_index = expect_index;
                      my_coordinate.north = map[expect_index].coordinate.north;
                      my_coordinate.east  = map[expect_index].coordinate.east;
                  } else {
                      char number_str[16];
                      unsigned int strlen;
                      drop_command_queue();
                      squareXY(0,0, 127, 63, 1);
                      update_display();
                      putstr(0, 2, "I AM LOST AGAIN", 1);
                      putstr(0, 3, "Expect ", 1);
                      num2str(expect_index, number_str);
                      putstr(8, 3, number_str, 1);
                      strlen = num2str(map[expect_index].coordinate.east, number_str);
                      putstr(0, 4, number_str, 1);
                      putstr(strlen, 4, ",", 1);
                      num2str(map[expect_index].coordinate.north, number_str);
                      putstr(strlen+1, 4, number_str, 1);
                      putstr(0, 5, "Current ", 1);
                      num2str(current_index, number_str);
                      putstr(8, 5, number_str, 1);
                      strlen = num2str(my_coordinate.east, number_str);
                      putstr(0, 6, number_str, 1);
                      putstr(strlen, 6, ",", 1);
                      num2str(my_coordinate.north, number_str);
                      putstr(strlen+1, 6, number_str, 1);
                      data_log_finish();
                      return 1;
                  }
              }
              if (skippasscount == 0) {
                  if (map[current_index].pass_count[(move_direction + back) & TURN_MASK] < 2) {
                      map[current_index].pass_count[(move_direction + back) & TURN_MASK] += 1;
                  }
              } else skippasscount = 0;
              map[current_index].node_link [(move_direction + back) & TURN_MASK] = last_index;
          }
          if (!available) {
              while (profiler_queue_empty == 0) continue;
              switch (check_color()) {
                case red:
                  data.red_cell_nr = current_index;

                  // для четырёх команд в буфере место должно быть,
                  // поэтому положим в очередь без проверки.
                  put_command(command_turn | back);
                  put_command(command_back | 100);
                  put_command(command_wait | 2 * FRAMESCANPERSECOND);
                  if (data.cell_step) put_command(command_entrance | data.cell_step);
                  else                put_command(command_entrance | 150);
                  break;

                case green:
                  data.green_cell_nr = current_index;
                  put_command(command_wait | 2 * FRAMESCANPERSECOND);
                  break;

                default:
                  break;
              }
          }

          count_green = 0; count_yellow = 0; count_red = 0;
          for (ii = 0; ii < 4; ii++) {
              switch (map[current_index].pass_count[ii]) {
                case 0:	count_green++; 	break;
                case 1: count_yellow++; break;
                case 2: count_red++;	break;
              }
          }
          if (count_yellow == 3) skiplevel = 0; // есть одиночная нить пересекающая узел - возвращаемся
          else if (count_green)  skiplevel = 1;  // оставить только зелёные проходы
          else if (count_yellow) skiplevel = 2;  // оставить все не красные проходы
          else {
              put_command(command_finish | 100);
              put_command(command_wait | 2 * FRAMESCANPERSECOND);
              goto finish;   // если нет ни зелёного, ни желтого прохода - больше идти некуда.
          }
          // ********************************
          //   Попробуем так. Будем использовать данные от первоначального анализа узла.
          available = LEFT_MASK | RIGHT_MASK | STRAIGHT_MASK;
          // ********************************
          if (map[current_index].pass_count[(move_direction+left) & TURN_MASK] >= skiplevel) available &= ~LEFT_MASK;
          if (map[current_index].pass_count[(move_direction+right) & TURN_MASK] >= skiplevel) available &= ~RIGHT_MASK;
          if (map[current_index].pass_count[(move_direction+straight) & TURN_MASK] >= skiplevel) available &= ~STRAIGHT_MASK;
          map[last_index].node_link[move_direction & TURN_MASK] = current_index;

          turn_index = ((unsigned int)data.lefthand < 6u) ? data.lefthand : Rand() % 6;
          turn_direction = SelectTurn(available, turn_sequence[turn_index]);
      } // if data.pathlength == 0

      if (data.runnumber == 0) {
          show_number(current_index, 0);
          if (available & LEFT_MASK)      show_arrow(40, 6, left);
          if (available & STRAIGHT_MASK)  show_arrow(56, 6, straight);
          if (available & RIGHT_MASK)     show_arrow(72, 6, right);
          switch (turn_direction) {
            case left:      show_arrow(40, 4, left);    break;
            case straight:  show_arrow(56, 4, straight);  break;
            case right:     show_arrow(72, 4, right);     break;
            case back:      show_arrow(56, 4, back);    break;
          }
      }

      // *************************************************************************************************
      // Если возвращаемся, то сначала делаем это виртуально

      bearing_dir_t back_bearing = move_direction;

      if (1 && (skiplevel != 1)) {
          unsigned int virtual_index = current_index;

          do {
              back_bearing += turn_direction;
              back_bearing &= TURN_MASK;

              if (map[virtual_index].pass_count[back_bearing] < 2) {
                  map[virtual_index].pass_count[back_bearing] += 1;
              } else LaunchPad_Output(RED); // Ошибка!!! Выбран красный коридор!

              virtual_index = map[virtual_index].node_link[back_bearing];
              if (map[virtual_index].pass_count[(back_bearing+back) & TURN_MASK] < 2) {
                  map[virtual_index].pass_count[(back_bearing+back) & TURN_MASK] += 1;
              }

              count_green = 0; count_yellow = 0; count_red = 0;
              for (ii = 0; ii < 4; ii++) {
                  switch (map[virtual_index].pass_count[ii]) {
                    case 0: count_green++;  break;
                    case 1: count_yellow++; break;
                    case 2: count_red++;  break;
                  }
              }
              if (count_yellow == 3) skiplevel = 0; // есть одиночная нить пересекающая узел - возвращаемся
              else if (count_green)  skiplevel = 1;  // оставить только зелёные проходы
              else if (count_yellow) skiplevel = 2;  // оставить все не красные проходы
              else skiplevel = 3;

              available = LEFT_MASK | RIGHT_MASK | STRAIGHT_MASK;
              if (map[virtual_index].pass_count[(back_bearing+left) & TURN_MASK] >= skiplevel) available &= ~LEFT_MASK;
              if (map[virtual_index].pass_count[(back_bearing+right) & TURN_MASK] >= skiplevel) available &= ~RIGHT_MASK;
              if (map[virtual_index].pass_count[(back_bearing+straight) & TURN_MASK] >= skiplevel) available &= ~STRAIGHT_MASK;

              turn_index = ((unsigned int)data.lefthand < 6u) ? data.lefthand : data.lefthand % 6;//Rand() % 6;
              turn_direction = SelectTurn(available, turn_sequence[turn_index]);

          } while (skiplevel == 2);
          // Теперь надо реально переместиться
          search_way_simple(current_index, virtual_index, move_direction);
          // left=3      +=      south=2         east=1   => 0
          turn_direction += (back_bearing - move_direction);
          turn_direction &= TURN_MASK;
//          play = 0;
//          for (ii = 0; ii < (unsigned int)data.pathlength; ii++) {
//              while (put_command(length[ii]));
//              if ((length[ii] & COMMAND_MASK) == command_forward) play++;
//              if ((length[ii] & COMMAND_MASK) == command_turn) {
//                  move_direction += length[ii];
//                  move_direction &= TURN_MASK;
//              }
//          }
          skiplevel = 1;
          pathsequence = 0;
          play = 1;
          // *
          // *************************************************************************************************
      }

      if (data.pathlength) {
          LaunchPad_Output(BLUE);
          if (--play == 0) {
              while (((length[pathsequence] & COMMAND_MASK) == command_turn) && (pathsequence < data.pathlength)) {
                  while (put_command(length[pathsequence]));
                  turn_direction =  length[pathsequence]; // для отображения на дисплее
                  move_direction += length[pathsequence];
                  move_direction &= TURN_MASK;
                  pathsequence++;
              }
              block_profiler(1);
              while (((length[pathsequence] & COMMAND_MASK) == command_forward) && (pathsequence < data.pathlength)) {
                  while (put_command(length[pathsequence])); // после поворота обязательна команда вперед
                  pathsequence++;
                  play++;
              }
              block_profiler(0);
              if (pathsequence >= data.pathlength) {
                  data.pathlength = 0;
                  skippasscount = 1;
              }
          }
      } else {
          LaunchPad_Output(GREEN);
          switch (turn_direction) {
            case straight:
              break;

            case left:
            case right:
            case back:
              put_command(command_turn | turn_direction);
              break;
          }

          move_direction += turn_direction;
          move_direction &= TURN_MASK;

          if (map[current_index].pass_count[move_direction] < 2) {
              map[current_index].pass_count[move_direction] += 1;
          } else LaunchPad_Output(RED); // Ошибка!!! Выбран красный коридор!
          expect_index = map[current_index].node_link[move_direction];

          if (expect_index != UNKNOWN) { // то надо дать знать длину сегмента.
            switch (move_direction) {
              case north:
                next_segment_length = map[expect_index].coordinate.north - map[current_index].coordinate.north;
                break;
              case south:
                next_segment_length = map[current_index].coordinate.north - map[expect_index].coordinate.north;
                break;
              case east:
                next_segment_length = map[expect_index].coordinate.east - map[current_index].coordinate.east;
                break;
              case west:
                next_segment_length = map[current_index].coordinate.east - map[expect_index].coordinate.east;
                break;
            }
        } else next_segment_length = 0;
        put_command(command_forward | next_segment_length);
      }
      last_index = current_index;
  } while (1);

  finish:
  while (profiler_queue_empty == 0) continue;
  data_log_finish();
//  copy_data32_dma((uint32_t *)length, (uint32_t *)data.length, data.pathlength);
//  copy_data_dma(path, data.path, data.pathlength);
  if (found_red) data.red_cell_nr = found_red;

  data.map_size = max_map_cell;
  spi_write_eeprom(ROM_map_addr, (uint8_t *)&map, sizeof(map));
  return 0;
}
//


#include "em_common.h"

SL_WEAK void benchmark_start(void) { };
SL_WEAK unsigned int benchmark_stop(void) { return 0; };


unsigned int calculation_time;
static unsigned int path_length_table[MAX_MAP_SIZE*2];

#define QUEUE_SIZE  64
static unsigned int sort_idx[QUEUE_SIZE], sort_tail=0;

static unsigned int insert_val(unsigned int val, unsigned int idx) {
    unsigned int i;

    if (++sort_tail > (QUEUE_SIZE-1)) return 1;
    for (i = sort_tail-1; i > 0; i--) {
        if(path_length_table[sort_idx[i-1]] < val) {
            sort_idx[i] = sort_idx[i-1];
        } else break;
    }
    sort_idx[i] = idx;
    return 0;
}

static unsigned int extract_val(void) {
    return sort_idx[--sort_tail];
}


void Search_Short_Way(void) {

  unsigned int k, min_index, destination;
  int	distance;
  int prev_bearing, bearing;

  fill_data32_dma(0xffffffff, (uint32_t*)path_length_table, data.map_size*2);
  data.pathlength = 0;

  spi_read_eeprom(ROM_map_addr, (uint8_t *)&map, sizeof(map));

  // переводим в двухслойную карту
  // однозначное соответствие: каждый узел делится на два n*2 и (n*2)+1
  // между ними сегмент длиной стоимостью поворота. К четным узлам примыкают
  // меридианальные сегменты - север/юг, к нечетным широтные - запад/восток.
  // каждый из этих сегментов удлиняется на стоимость прямого прохода узла.

  benchmark_start();
  while (dma_flag) continue; // ждём окончания fill_dma path_length[] <= 0xFFFFFFFF
  // Начинаем поиск с конца - стартовая точка финиш.
  // Причем, прибытие с любой стороны допустимо.
  path_length_table[data.red_cell_nr * 2] = 0;
  path_length_table[data.red_cell_nr * 2 + 1] = 0;

  sort_tail = 0;
  insert_val(0, data.red_cell_nr * 2);
  insert_val(0, data.red_cell_nr * 2 + 1);

  // Составляем карту высот по алгоритму Дейкстры
  while (sort_tail) {
      min_index  = extract_val();
      for (k = 0; k < 2; k++) {
          if ((destination = map[min_index/2].node_link[2*k + (min_index & 0x01)]) != UNKNOWN) {
              //                distance = map2d[min_index].length[k];
              distance =   (map[min_index/2].coordinate.north - map[destination].coordinate.north) +
                  (map[min_index/2].coordinate.east  - map[destination].coordinate.east);
              if (distance < 0) distance = -distance + data.crosscost;
              else              distance += data.crosscost;
              destination = destination * 2 + (min_index & 0x01);
              if (path_length_table[destination] > (path_length_table[min_index] + distance)) {
                  path_length_table[destination] = path_length_table[min_index] + distance;
                  insert_val(path_length_table[destination], destination);
              }
          }
      }
      distance = data.turncost;
      destination = min_index ^ 0x01;
      if (path_length_table[destination] > (path_length_table[min_index] + distance)) {
          path_length_table[destination] = path_length_table[min_index] + distance;
          if (insert_val(path_length_table[destination], destination)) LaunchPad_Output(RED);
      }
  }

  // Теперь скатываемся вниз для создания кратчайшего пути
  // от старта к финишу.
  data.pathlength = 0; // указатель пути на начало
  prev_bearing = north;  // не обязательно
  min_index = data.green_cell_nr << 1 ;  // начинаем с точки 0
  if (path_length_table[min_index] > path_length_table[min_index+1]) {
      min_index++;
      prev_bearing = east;
  }
  // 1 line offset
  while (path_length_table[min_index]) {
      for (k = 0; k < 3; k++) {
          if (k < 2) {
              if ((destination = map[min_index/2].node_link[2*k + (min_index & 0x01)]) != UNKNOWN) {
                  distance =   (map[min_index/2].coordinate.north - map[destination].coordinate.north) +
                      (map[min_index/2].coordinate.east  - map[destination].coordinate.east);
                  if (distance < 0) distance = -distance + data.crosscost;
                  else              distance += data.crosscost;
                  destination = destination*2 + (min_index & 0x01);
                  if ((path_length_table[min_index] - distance) == (path_length_table[destination])) {
                      bearing = (bearing_dir_t)((min_index & 0x01) + 2*k);
                      path[data.pathlength] = (rotation_dir_t)(bearing - prev_bearing);
                      path[data.pathlength] &= TURN_MASK;
                      prev_bearing = bearing;
                      if ((int)(min_index >> 1) != data.green_cell_nr) {
                          data.pathlength++;
                          length[data.pathlength] = distance-data.crosscost;  // тут проблема возможно
                      } else {
                          length[data.pathlength] = distance-data.crosscost;  // тут проблема возможно
                      }
                      min_index = destination;
                      break;
                  }
              }
          } else {
              if ((path_length_table[min_index] - data.turncost) == (path_length_table[min_index ^ 0x01])) {
                  min_index = min_index ^ 0x01;
                  break;
              }
          }
      }
  }

  path[data.pathlength++] = back;
  calculation_time= benchmark_stop();

  copy_data_dma(path, data.path, data.pathlength);
  copy_data_dma((uint8_t *) length, (uint8_t *)data.length, (data.pathlength) * sizeof(length[0]));
}

void Explore_Maze(void) {
  squareXY(0, 0, 127, 63, 0);
  update_display();
  if (data.runnumber) {
      show_number(data.runnumber, 0);
  }
  if (solveMaze()) {
      while (kbdread() != KEY_DOWN);
  }

}

void DrawMap(void) {
  spi_read_eeprom(ROM_map_addr, (uint8_t *)&map, sizeof(map));
  Draw_Map();
  while(kbdread() != KEY_DOWN) continue;
}

unsigned int search_way_simple(unsigned int start, unsigned int finish, bearing_dir_t my_bearing){
  unsigned int k, min_index, destination;
  int distance;
  bearing_dir_t prev_bearing, bearing;

  fill_data32_dma(0xffffffff, (uint32_t*)path_length_table, MAX_MAP_SIZE*2);
  while (dma_flag) continue; // ждём окончания fill_dma path_length[] <= 0xFFFFFFFF
  // Начинаем поиск с конца - стартовая точка финиш.
  // Причем, прибытие с любой стороны допустимо.
  // Хотя нет, надо бы передавать желательное направление... но пока пусть будет так.
  path_length_table[finish * 2] = 0;
  path_length_table[finish * 2 + 1] = 0;

  sort_tail = 0;
  insert_val(0, finish * 2);
  insert_val(0, finish * 2 + 1);

  // Составляем карту высот по алгоритму Дейкстры
  while (sort_tail) {
      min_index  = extract_val();
      for (k = 0; k < 2; k++) {
          if ((destination = map[min_index/2].node_link[2*k + (min_index & 0x01)]) != UNKNOWN) {
              //                distance = map2d[min_index].length[k];
              distance =   (map[min_index/2].coordinate.north - map[destination].coordinate.north) +
                  (map[min_index/2].coordinate.east  - map[destination].coordinate.east);
              if (distance < 0) distance = -distance;
              // до этого destination был индекс в карте,
              // а теперь будет индексом в таблице расстояний
              destination = destination * 2 + (min_index & 0x01);
              if (path_length_table[destination] > (path_length_table[min_index] + distance)) {
                  path_length_table[destination] = path_length_table[min_index] + distance;
                  insert_val(path_length_table[destination], destination);
              }
          }
      }

      distance = data.turncost;
      destination = min_index ^ 0x01;
      if (path_length_table[destination] > (path_length_table[min_index] + distance)) {
          path_length_table[destination] = path_length_table[min_index] + distance;
          if (insert_val(path_length_table[destination], destination)) LaunchPad_Output(RED);
      }
  }
  // таблица расстояний составлена - надо строить маршрут.
  // Теперь скатываемся вниз для создания кратчайшего пути
  // от старта к финишу.
  data.pathlength = 0; // указатель пути на начало
  prev_bearing = my_bearing;
  min_index = (start << 1) | (my_bearing & 0x01);  // начинаем с того места где находимся и куда повернуты
  //  if (path_length_table[min_index] > path_length_table[min_index+1]) {
  //      min_index++;
  //      prev_bearing = east;
  //  }

  while (path_length_table[min_index]) {
      for (k = 0; k < 3; k++) {
          if (k < 2) {
              if ((destination = map[min_index/2].node_link[2*k + (min_index & 0x01)]) != UNKNOWN) {
                  distance =   (map[min_index/2].coordinate.north - map[destination].coordinate.north) +
                      (map[min_index/2].coordinate.east  - map[destination].coordinate.east);
                  if (distance < 0) distance = -distance;
                  // до этого destination был индекс в карте,
                  // а теперь будет индексом в таблице расстояний
                  destination = destination*2 + (min_index & 0x01);
                  if ((path_length_table[min_index] - distance) == (path_length_table[destination])) {
                      bearing = (bearing_dir_t)((min_index & 0x01) + 2*k);
                      if (bearing != prev_bearing) {
                          length[data.pathlength++] = (command_turn | ((bearing - prev_bearing) & TURN_MASK));
                      }
                      length[data.pathlength++] = (command_forward | distance);

                      prev_bearing = bearing;
                      min_index = destination;
                      break;
                  }
              }
          } else {
              if ((path_length_table[min_index] - data.turncost) == (path_length_table[min_index ^ 0x01])) {
                  min_index = min_index ^ 0x01;
                  break;
              }
          }
      }
  }
  return 0;
}
