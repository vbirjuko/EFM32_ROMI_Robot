/*
 * UART0.h
 *
 *  Created on: 11 мар. 2021 г.
 *      Author: wl
 */

#ifndef UART0_H_
#define UART0_H_

void UART0_Init(void);
unsigned int UART0_Out(uint8_t data);
unsigned int UART0_In(uint8_t * data);
void UART0_OutString(char *pt);
void UART0_OutUDec(uint32_t n);
void UART0_OutChar(uint8_t data);
#endif /* UART0_H_ */
