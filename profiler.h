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
    command_finish  =   0x30000000,
    command_entrance =  0x40000000,
    command_wait    =   0x50000000,
    command_back    =   0x60000000,
    command_eof     =   0xF0000000
};
#define COMMAND_MASK    0xF0000000

void block_profiler(unsigned int state);
void drop_command_queue(void);
unsigned int put_command(unsigned int command_data);
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
};
extern volatile unsigned int segment_length, node_configuration, real_direction, profiler_data_ready,
        profiler_queue_empty, profiler_error_code;

void test_profile(void);

#endif /* PROFILER_H_ */
