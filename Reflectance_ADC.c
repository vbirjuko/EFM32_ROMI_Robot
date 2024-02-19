/*
 * Reflectance_ADC.c
 *
 *  Created on: 23 апр. 2021 г.
 *      Author: wl
 *
 */

#include "em_device.h"
#include "em_gpio.h"
#include "em_cmu.h"
#include "resources.h"
#include "configure.h"
#include "keyboard.h"
#include "display.h"
#include "spi_drv.h"
#include "em_ldma.h"
#include "Reflectance.h"
#include "LaunchPad.h"

#define PRS_ROUTELOC(A) PRS_ROUTELOC0_CH ## A ## LOC_LOC1
#define PRS_ROUTELOC_CH(A)  PRS_ROUTELOC(A)
#define PRS_ROUTEPEN(A) PRS_ROUTEPEN_CH ## A ## PEN
#define PRS_ROUTEPEN_CH(A)  PRS_ROUTEPEN(A)
#define ADC_SCAN_PRS(A) ADC_SCANCTRLX_PRSSEL_PRSCH ## A
#define ADC_SCAN_PRSCH(A) ADC_SCAN_PRS(A)

// reflectance LED illuminate connected to PF5
//                                          (robot's right)
// reflectance sensor 7 connected to PE.11  APORT4XCH11               27

// reflectance sensor 5 connected to PE.15  APORT4XCH15               23
// reflectance sensor 3 connected to PE.13  APORT4XCH13               21
// reflectance sensor 1 connected to PE.9   APORT4XCH9                17
//
// reflectance sensor 4 connected to PE.14              APORT3XCH14   14
// reflectance sensor 6 connected to PE.10              APORT3XCH10   10
// reflectance sensor 0 connected to PE.8               APORT3XCH8    8
//
// reflectance sensor 2 connected to PE.5   APORT4XCH5                5
//                                          (robot's left)

#define REFL_PORT_7   (1ul << 11)
#define REFL_PORT_6   (1ul << 15)
#define REFL_PORT_5   (1ul << 13)
#define REFL_PORT_4   (1ul << 9)
#define REFL_PORT_3   (1ul << 14)
#define REFL_PORT_2   (1ul << 10)
#define REFL_PORT_1   (1ul << 8)
#define REFL_PORT_0   (1ul << 5)

#define REFL_PORT_MASK (REFL_PORT_0 | REFL_PORT_1 | REFL_PORT_2 | REFL_PORT_3 |\
                        REFL_PORT_4 | REFL_PORT_5 | REFL_PORT_6 | REFL_PORT_7)

#define DEBUG_REFL_SENS


volatile uint8_t photo_array[16];
volatile uint32_t current_sensor = 0, photo_data_ready;
#ifdef DEBUG_REFL_SENS
static unsigned char log_array[2000][16];
static unsigned char *log_array_ptr = 0;
#endif

LDMA_TransferCfg_t adcXferCfg = LDMA_TRANSFER_CFG_PERIPHERAL(ldmaPeripheralSignal_ADC0_SCAN);

LDMA_Descriptor_t
  adc_descriptor = LDMA_DESCRIPTOR_LINKREL_P2M_BYTE(&(ADC0->SCANDATA), photo_array, sizeof(photo_array)/sizeof(photo_array[0]), 0);

void Reflectance_Init(unsigned int threshold, int ir_led_level){
  (void) ir_led_level;
  (void) threshold;
  // write this as part of Lab 6
  GPIO_PinModeSet(gpioPortF, 5, gpioModePushPull, 0);

  GPIO_PinModeSet(gpioPortE, 5, gpioModeDisabled, 0);
  GPIO_PinModeSet(gpioPortE, 8, gpioModeDisabled, 0);
  GPIO_PinModeSet(gpioPortE, 9, gpioModeDisabled, 0);
  GPIO_PinModeSet(gpioPortE, 10, gpioModeDisabled, 0);
  GPIO_PinModeSet(gpioPortE, 11, gpioModeDisabled, 0);
  GPIO_PinModeSet(gpioPortE, 13, gpioModeDisabled, 0);
  GPIO_PinModeSet(gpioPortE, 14, gpioModeDisabled, 0);
  GPIO_PinModeSet(gpioPortE, 15, gpioModeDisabled, 0);

  GPIO->P[gpioPortE].OVTDIS = REFL_PORT_MASK;

#ifdef DEBUG_ADC
  GPIO_PinModeSet(gpioPortD, 1, gpioModePushPull, 0);
#endif

  CMU_ClockEnable(cmuClock_TIMER2, 1);
  CMU_ClockEnable(cmuClock_PRS, 1);
  CMU_ClockEnable(cmuClock_ADC0, 1);

  TIMER2->CTRL = TIMER_CTRL_MODE_UP | TIMER_CTRL_PRESC_DIV8
      | TIMER_CTRL_CLKSEL_PRESCHFPERCLK;

  TIMER2->TOP = (REFL_PERIOD*CPU_FREQ/8000000) - 1; // Period is 2.5ms
  TIMER2->CNT = (REFL_PERIOD*CPU_FREQ/8000000);     // 8ms first delay

  TIMER2->CC[2].CTRL = TIMER_CC_CTRL_MODE_PWM | TIMER_CC_CTRL_PRSCONF_PULSE;
  TIMER2->CC[2].CCV = (REFL_PERIOD-200)*CPU_FREQ / 8000000;  // Считывание за 50 мкс до включения подсветки

#ifdef USE_RS_TRIGGER
  TIMER2->CC[0].CTRL = TIMER_CC_CTRL_MODE_PWM | TIMER_CC_CTRL_PRSCONF_PULSE;
#else
  TIMER2->CC[0].CTRL = TIMER_CC_CTRL_MODE_PWM | TIMER_CC_CTRL_PRSCONF_LEVEL;
#endif
  TIMER2->CC[0].CCV = (REFL_PERIOD-150)*CPU_FREQ / 8000000;  // Подсветку включаем за 150 мкс до конца цикла
  TIMER2->CC[1].CTRL = TIMER_CC_CTRL_MODE_PWM | TIMER_CC_CTRL_PRSCONF_PULSE;
  TIMER2->CC[1].CCV = (REFL_PERIOD-50)*CPU_FREQ / 8000000;  // Считывание через 100 мкс после включения подсветки (и за 50 мкс до выключения)

  // включение и выключение ИК подсветки
#ifdef USE_RS_TRIGGER
  PRS->CH[REFL_LED_PRS_CH-2].CTRL = PRS_CH_CTRL_SOURCESEL_PRS | PRS_CH_CTRL_SIGSEL_PRSCH1;
  PRS->CH[REFL_LED_PRS_CH-1].CTRL = PRS_CH_CTRL_SOURCESEL_TIMER2 | PRS_CH_CTRL_SIGSEL_TIMER2CC0 | PRS_CH_CTRL_ORPREV | PRS_CH_CTRL_INV;
  PRS->CH[REFL_LED_PRS_CH].CTRL   = PRS_CH_CTRL_SOURCESEL_ADC0   | PRS_CH_CTRL_SIGSEL_ADC0SCAN  | PRS_CH_CTRL_ORPREV | PRS_CH_CTRL_INV;
#else
  PRS->CH[REFL_LED_PRS_CH].CTRL = PRS_CH_CTRL_SOURCESEL_TIMER2 | PRS_CH_CTRL_SIGSEL_TIMER2CC0 | PRS_CH_CTRL_INV;
#endif
  PRS->ROUTELOC0 = PRS_ROUTELOC_CH(REFL_LED_PRS_CH);
  PRS->ROUTEPEN  = PRS_ROUTEPEN_CH(REFL_LED_PRS_CH);

  // Сигнал на запуск преобразования АЦП
  PRS->CH[REFL_CAPT_LIGHT_PRS_CH].CTRL = PRS_CH_CTRL_SOURCESEL_TIMER2 | PRS_CH_CTRL_SIGSEL_TIMER2CC1;
  PRS->CH[REFL_CAPT_DARK_PRS_CH].CTRL = PRS_CH_CTRL_SOURCESEL_TIMER2 | PRS_CH_CTRL_SIGSEL_TIMER2CC2 | PRS_CH_CTRL_ORPREV;

  ADC0->CTRL = ADC_CTRL_CHCONREFWARMIDLE_PREFSCAN | ADC_CTRL_CHCONMODE_MAXSETTLE |
      ADC_CTRL_ADCCLKMODE_SYNC | (1uL << _ADC_CTRL_PRESC_SHIFT) |
      ADC_CTRL_OVSRSEL_DEFAULT | (50ul << _ADC_CTRL_TIMEBASE_SHIFT) | ADC_CTRL_TAILGATE |
      ADC_CTRL_WARMUPMODE_KEEPADCWARM;

  ADC0->SCANCTRL = ADC_SCANCTRL_PRSEN | ADC_SCANCTRL_REF_VDD | ADC_SCANCTRL_RES_8BIT |
      ADC_SCANCTRL_AT_16CYCLES;

  // REFL_CAPT_LIGHT_PRS_CH канал PRS - импульс при подсветке
  // REFL_CAPT_DARK_PRS_CH канал PRS - 2 импульса: без и с подсветкой.
  ADC0->SCANCTRLX = ADC_SCAN_PRSCH(REFL_CAPT_DARK_PRS_CH) | ADC_SCANCTRLX_PRSMODE_PULSED |
      ADC_SCANCTRLX_DVL_DEFAULT | (2ul << 12);

  ADC0->SCANINPUTSEL = ADC_SCANINPUTSEL_INPUT0TO7SEL_APORT4CH0TO7 |
      ADC_SCANINPUTSEL_INPUT8TO15SEL_APORT3CH8TO15 |
      ADC_SCANINPUTSEL_INPUT16TO23SEL_APORT4CH8TO15 |
      ADC_SCANINPUTSEL_INPUT24TO31SEL_APORT4CH8TO15;

  ADC0->SCANMASK = (0x00000001ul << 5)  | (0x00000001ul << 8)  |
      (0x00000001ul << 10) | (0x00000001ul << 14) |
      (0x00000001ul << (9+8)) | (0x00000001ul << (13+8)) |
      (0x00000001ul << (15+8)) | (0x00000001ul << (11+16)) ;

  ADC0->CMD = ADC_CMD_SCANSTOP;
  LDMA_StartTransfer(LDMA_ADC_CHANNEL, &adcXferCfg, &adc_descriptor);
  TIMER2->CMD = TIMER_CMD_START;
}

volatile unsigned int refl_fail = 0;

void ldma_adc_eoc(void) {
  unsigned int mask = 0x01;
#ifdef DEBUG_ADC
  GPIO_PinOutSet(gpioPortD, 1);
#endif

// Формула наклонной границы Y = Y_SLOPE * (X - X_OFFSET)
#define X_OFFSET  80
#define Y_SLOPE   8ul/10

  while (ADC0->STATUS & ADC_STATUS_SCANACT) continue;
  ADC0->SCANFIFOCLEAR = ADC_SCANFIFOCLEAR_SCANFIFOCLEAR;
  for (unsigned int ii = 0; ii < 8; ii++) {

#ifdef DEBUG_REFL_SENS
      if (log_array_ptr) {
          *log_array_ptr++ = photo_array[ii];
          *log_array_ptr++ = photo_array[ii+8];
          if (log_array_ptr > &log_array[1999][15]) log_array_ptr = 0;
      }
#endif
      if (photo_array[ii+8] > (photo_array[ii]+30)) refl_fail = 1;
      if (photo_array[ii+8] < (data.threshold >> 3)) current_sensor &=  ~mask;
      else  if ((photo_array[ii] > (data.x_offset + (data.threshold >> 3))) && (((photo_array[ii]-data.x_offset)*data.y_slope/256) > photo_array[ii+8])) {
          current_sensor &= ~mask;
      } else {
          current_sensor |= mask;
      }
      mask <<= 1;
  }
  photo_data_ready = 1;
//  LDMA_StartTransfer(LDMA_ADC_CHANNEL, &adcXferCfg, &adc_descriptor);
#ifdef PROFILER_PENDSV
  SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
#else
  TIMER3->IFS = TIMER_IFS_CC0;
#endif
#ifdef DEBUG_ADC
  GPIO_PinOutClear(gpioPortD, 1);
#endif
}

int line_error(unsigned int first_sensor, unsigned int last_sensor) {
    const int W[] = {329, 235, 141, 47, -47, -141, -235, -329};
    if ((first_sensor == 256) || (last_sensor == 256)) return 0;
    if (last_sensor == first_sensor) {
        return W[last_sensor];
    } else {
        return ((W[first_sensor] + W[last_sensor])/2 - 47) + ((photo_array[first_sensor+8]-(data.threshold >> 3))*94/(photo_array[first_sensor+8]+photo_array[last_sensor+8]-((2 * data.threshold) >> 3))) ;
    }
}

void set_threshold(unsigned int threshold) {
  (void) threshold;
}

void TestReflect(void) {
  uint32_t photo_sensor, photo_mask, first_sensor, last_sensor, ii;
  uint8_t photo_sensor_copy[8];

#ifdef DEBUG_REFL_SENS
  log_array_ptr = &log_array[0][0];
#endif

  while (kbdread() != KEY_DOWN) {
#ifdef DEBUG_REFL_SENS
      if (log_array_ptr) LaunchPad_Output1(GREEN);
      else               LaunchPad_Output1(RED);
#endif
      if (photo_data_ready ) {
          photo_data_ready  = 0;
          photo_sensor = current_sensor; // битовая маска тёмных сенсоров
          photo_mask = photo_sensor;
          first_sensor = 256;
          last_sensor = 256;
          for (ii=0; ii<8; ii++) {
              photo_sensor_copy[ii] = photo_array[ii+8];
              if (photo_mask & 0x01u) {
                  last_sensor = ii;
                  if (first_sensor == 256) first_sensor = ii;
              }
              photo_mask >>= 1;
          }
          if (first_sensor == 256) {first_sensor = 3; last_sensor = 4; }
          show_hystogram(photo_sensor, photo_sensor_copy, (data.threshold), line_error(first_sensor, last_sensor));
      }
  }
  LaunchPad_Output1(0);
#ifdef DEBUG_REFL_SENS
  squareXY(0, 0, 127, 63, 0);
  for (ii = 0;  ii < 2000; ii++) {
      squareXY(log_array[ii][0]   >> 1, 63-(log_array[ii][1]  >> 2), log_array[ii][0]   >> 1, 63-(log_array[ii][1]  >> 2),1);
      squareXY(log_array[ii][2]   >> 1, 63-(log_array[ii][3]  >> 2), log_array[ii][2]   >> 1, 63-(log_array[ii][3]  >> 2),1);
      squareXY(log_array[ii][4]   >> 1, 63-(log_array[ii][5]  >> 2), log_array[ii][4]   >> 1, 63-(log_array[ii][5]  >> 2),1);
      squareXY(log_array[ii][6]   >> 1, 63-(log_array[ii][7]  >> 2), log_array[ii][6]   >> 1, 63-(log_array[ii][7]  >> 2),1);
      squareXY(log_array[ii][8]   >> 1, 63-(log_array[ii][9]  >> 2), log_array[ii][8]   >> 1, 63-(log_array[ii][9]  >> 2),1);
      squareXY(log_array[ii][10]  >> 1, 63-(log_array[ii][11] >> 2), log_array[ii][10]  >> 1, 63-(log_array[ii][11] >> 2),1);
      squareXY(log_array[ii][12]  >> 1, 63-(log_array[ii][13] >> 2), log_array[ii][12]  >> 1, 63-(log_array[ii][13] >> 2),1);
      squareXY(log_array[ii][14]  >> 1, 63-(log_array[ii][15] >> 2), log_array[ii][14]  >> 1, 63-(log_array[ii][15] >> 2),1);
  }
  lineXY(0, 63-(data.threshold >> 5), 127, 63-(data.threshold >> 5), 1);
  // Формула наклонной границы Y = Y_SLOPE * (X - X_OFFSET)
//  #define X_OFFSET  80
//  #define Y_SLOPE   8ul/10

  lineXY(data.x_offset >> 1, 63, 127, 63-(((255-data.x_offset)*data.y_slope/256) >> 2) , 1);
  if (refl_fail) {
      putstr(0, 0, "FAIL", 1);
  }
  update_display();
  while (kbdread() != KEY_DOWN) continue;
#endif
  refl_fail = 0;
}
