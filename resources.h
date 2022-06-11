/*
 * resources.h
 *
 *  Created on: 20 сент. 2021 г.
 *      Author: wl
 */

#ifndef RESOURCES_H_
#define RESOURCES_H_

#define SSD1306
#define OLED_SPI
#define PCA

// Период опроса фотосенсоров
#define REFL_PERIOD   (2500)
#define FRAMESCANPERSECOND     (1000000/REFL_PERIOD)

// TIMER1 - Motor PWM
// TIMER2 - Reflectance sensor Light on/off, ADC start

// PCNT1   - Quadrature Encoder Left
// WTIMER0 - Tacho Period
// WTIMER1 - Quadrature Encoder Right

// LE_TIMER0 - Time delay

// USART0 - UART over JLink CDC
// USART3 - SPI for OLED display / SPI EEPROM

// I2C1 - I2C bus for PCA keys, Color sensor


// LDMA channels
#define LDMA_USART3_RX_CHANNEL              1
#define LDMA_USART3_TX_CHANNEL              2
#define LDMA_USART_OLED_CHANNEL             3
#define LDMA_ADC_CHANNEL                    0
#define LDMA_MEM2MEM_CHANNEL                7


// PRS channels
#define TACH_PRS_CH                         1
// Должен быть канал 2 для вывода через PF5
#define REFL_LED_PRS_CH                     2
// Следующие два должны быть по порядку так как объединяются по ИЛИ!
#define REFL_CAPT_LIGHT_PRS_CH              3
#define REFL_CAPT_DARK_PRS_CH               4

// IRQ priorities:
#define SysTick_IRQ_Priority  2
#define I2C_IRQ_PRI           2
#define TIMER0_IRQ_PRI        3
#define TIMER1_IRQ_PRI        3
#define USART0_IRQ_PRI        4
#define USART3_TX_IRQ_PRI     4
#define LDMA_IRQ_PRI          5
#define ADC0_IRQ_Priority     5
#define LETIMER0_IRQ_Priority 5
#define PendSV_IRQ_priority   7

#endif /* RESOURCES_H_ */
