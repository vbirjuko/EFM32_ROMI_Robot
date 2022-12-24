/*
 * profiler.h
 *
 *  Created on: 17 мар. 2022 г.
 *      Author: wl
 */

#ifndef PROFILER_H_
#define PROFILER_H_

enum commands {
    command_stop =      0x00000000,
    // движение прямо по сегменту. В младших битах длина сегмента для расчета разгона и торможения
    command_forward =   0x10000000,
    // Поворот. Параметр 1 - вправо, 3 - влево, 2 - разворот.
    command_turn =      0x20000000,
    // finish, entrance: Параметр сколько миллиметров проехать выходя излабиринта или сколько искать вход.
    command_finish  =   0x30000000,
    command_entrance =  0x40000000,
    // Ожидание. Параметр - сколько фреймов ждать. По окончании, двигатели переволятся в спящий режим.
    command_wait    =   0x50000000,
    // Аналогично finish, но задним ходом: Параметр сколько миллиметров проехать выходя излабиринта
    command_back    =   0x60000000,
    // Старт или останов измерения времени.
    command_benchmark = 0x70000000,
    command_eof     =   0xF0000000
};

#define COMMAND_MASK    0xF0000000

void block_profiler(unsigned int state);
unsigned int put_command(unsigned int command_data);
void drop_command_queue(void);
void profiler(void);
void PendSVInit(void);

enum profiler_error_code {
  no_profiler_error = 0,
  profiler_error_no_entrance = 1,
  profiler_error_turn = 2,
  profiler_error_turn_arg = 3,
  profiler_error_state_err = 4,
  profiler_error_cmd_err  = 5,
};

enum profiler_log {
  log_segment = 0x1000,
  log_check   = 0x2000,
  log_blind   = 0x3000,
  log_entrance= 0x4000,
  log_turn    = 0x5000,
  log_wait    = 0x6000,
  log_finish  = 0x7000,
  log_back    = 0x8000,

  log_segment0 = 0x0100,
  log_segment1 = 0x0200,
  log_segment2 = 0x0300,
  log_segment3 = 0x0400,
};
extern volatile unsigned int segment_length, node_configuration, real_direction, profiler_data_ready,
        profiler_queue_empty, profiler_error_code;

void test_profile(void);

#endif /* PROFILER_H_ */
