/*
 * resources.h
 *
 *  Created on: 20 сент. 2021 г.
 *      Author: wl
 */

#ifndef RESOURCES_H_
#define RESOURCES_H_

#define SH1106
#define OLED_SPI
#define PCA
#define DATALOG
//#define DEBUG_MAZE
//#define PROFILER_PENDSV

#define CPU_FREQ    (50000000LL)

// Период опроса фотосенсоров
#define REFL_PERIOD   (2500)
#define FRAMESCANPERSECOND     (1000000/REFL_PERIOD)

// SCANPERSECOND задаёт все временные задержки, а так же частоту запуска АЦП.
#define SCANPERSECOND 2000

// Ширина колеи робота для вычисления поворотов
#define TRACK_WIDE      (143L)


#ifdef SPEED_MM_PER_SECOND
// сколько импульсов придёт на таймер если колесо вращается/движется со скоростью 0,1 мм/с
// при заданной частоте тактовых импульсов
// 220.0 mm - 720 pulses. => 2200/720 = 220/72 = 55/18 = 3.055(5) pulses per second. Freq = CPU_Freq divide 4
#define T_CONSTANT (CPU_FREQ * 55/4/18)
#else
// 50 000 000 / 4 * 60 * 100 / 360 = CPU_FREQ * 6000/(360*4) = CPU_FREQ * 25 / 6
// И еще делим на 2 так как тахометр срабатывает и по фронту и по спаду.
#define T_CONSTANT  (CPU_FREQ * 25 / 12)
#endif

// 220mm per 720 tick of two wheels.
// 220 mm  ~ 1440  / 20
//      11 ~ 72
#define STEPS_TO_MM(X)    ((X) * 11L / 72)
#define MM_TO_STEPS(X)    ((X) * 72L / 11)
// 143mm wide * PI => 449 mm length / 220mm wheel step and multiply 720 pulse = 1470 pulse
#define DEGREE_TO_STEPS(X) ((TRACK_WIDE*314L*72L*(X)+(11L*360*100/2))/(11L*360*100))


// TIMER1 - Motor PWM
// TIMER2 - Reflectance sensor Light on/off, ADC start

// PCNT1   - Quadrature Encoder Left
// PCNT2   - Quadrature Encoder Right

// WTIMER0 - Tacho Period
// WTIMER1 - Benchmark counter

// LE_TIMER0 - Time delay

// USART0 - UART over JLink CDC
// USART3 - SPI for OLED display / SPI EEPROM

// I2C1 - I2C bus for PCA keys, Color sensor


// LDMA channels
#define LDMA_ADC_CHANNEL                    0
#define LDMA_USART3_RX_CHANNEL              1
#define LDMA_USART3_TX_CHANNEL              2
#define LDMA_USART_OLED_CHANNEL             2
//#define                                   3
#define LDMA_TACH_LEFT_CHANNEL              4
#define LDMA_TACH_RIGHT_CHANNEL             5
//#define                                   6
#define LDMA_MEM2MEM_CHANNEL                7


// PRS channels
// Должен быть канал 2 для вывода через PF5
// и еще два перед ним для формирования RS триггера
//#define USE_RS_TRIGGER
#define REFL_LED_PRS_CH                     2
// Следующие два должны быть по порядку так как объединяются по ИЛИ!
#define REFL_CAPT_LIGHT_PRS_CH              3
#define REFL_CAPT_DARK_PRS_CH               4

#define TACH_PRS_CH                         5

// IRQ priorities:
#define SysTick_IRQ_Priority  2
#define I2C_IRQ_PRI           2
#define WTIMER0_IRQ_PRI        3
#define TIMER1_IRQ_PRI        3
#define USART0_IRQ_PRI        4
#define USART3_TX_IRQ_PRI     4
#define LDMA_IRQ_PRI          5
#define ADC0_IRQ_Priority     5
#define LETIMER0_IRQ_Priority 5
#define PendSV_IRQ_priority   7

#define constrain(amt, min, max)  ((amt) < (min)) ? (min) : (((amt) > (max)) ? (max) : (amt))

#endif /* RESOURCES_H_ */
