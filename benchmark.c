/*
 * benchmark.c
 *
 *  Created on: 15 дек. 2022 г.
 *      Author: prg51
 */
#define BENCHMARK_USE_SYSTICK

#ifdef BENCHMARK_USE_SYSTICK

#include "resources.h"
#include "app.h"

unsigned int benchmark_start_time = 0, benchmark_stop_time = 0;
void benchmark_init(void) {
  return;
}

void benchmark_start(void) {
  benchmark_start_time = sys_time;
}

void benchmark_stop(void)  {
  benchmark_stop_time = sys_time;
}

unsigned int benchmark_time(unsigned int precision) {
  return ((benchmark_stop_time-benchmark_start_time)*(unsigned long long)precision+(SCANPERSECOND/2))/(SCANPERSECOND);
}

#else
#include "em_cmu.h"
#include "resources.h"

void benchmark_init(void) {
  CMU_ClockEnable(cmuClock_WTIMER1, 1);
  WTIMER1->CTRL = WTIMER_CTRL_PRESC_DIV128 | WTIMER_CTRL_MODE_UP | WTIMER_CTRL_OSMEN | WTIMER_CTRL_DISSYNCOUT;
  WTIMER1->TOP = 0xFFFFFFFFul;
  WTIMER1->CNT = 0;
}

void benchmark_start(void) {
  WTIMER1->CMD = WTIMER_CMD_STOP;
  WTIMER1->CNT = 0;
  WTIMER1->CMD = WTIMER_CMD_START;
}

void benchmark_stop(void)  {
  WTIMER1->CMD = WTIMER_CMD_STOP;
}

unsigned int benchmark_time(unsigned int precision) {
  return (WTIMER1->CNT*(unsigned long long)precision+(CPU_FREQ/256))/(CPU_FREQ/128);
}
#endif
