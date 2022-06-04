/*
 * battery.h
 *
 *  Created on: 4 янв. 2022 г.
 *      Author: wl
 */

#ifndef BATTERY_H_
#define BATTERY_H_

//**************Low pass Digital filter**************
typedef struct {
  uint32_t Size;      // Size-point average, Size=1 to 512
  uint32_t x[256];   // two copies of MACQ
  uint32_t I1;        // index to oldest
//  volatile uint32_t start;
  volatile uint32_t Sum, data_ready;    // sum of the last Size samples
} LPF_t;

extern LPF_t LPF_battery;
void init_battery(void);
unsigned int get_battery_voltage(void);

#endif /* BATTERY_H_ */
