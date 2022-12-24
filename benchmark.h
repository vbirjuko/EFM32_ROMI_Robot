/*
 * benchmark.h
 *
 *  Created on: 15 дек. 2022 г.
 *      Author: prg51
 */

#ifndef BENCHMARK_H_
#define BENCHMARK_H_

void benchmark_init(void);
void benchmark_start(void);
void benchmark_stop(void);
unsigned int benchmark_time(unsigned int precision);

#endif /* BENCHMARK_H_ */
