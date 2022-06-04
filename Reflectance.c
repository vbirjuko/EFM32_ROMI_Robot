/*
 * Reflectance.c
 *
 *  Created on: 23 апр. 2021 г.
 *      Author: wl
 */

#include "em_device.h"
#include "em_gpio.h"
#include "em_cmu.h"
#include "configure.h"
#include "keyboard.h"
#include "display.h"
#include "spi_drv.h"

#define REFL_USE_TIMER
// reflectance LED illuminate connected to PD1
//                                          (robot's right)
// reflectance sensor 7 connected to PE.11            PRS3#2
// reflectance sensor 6 connected to PE.10            PRS2#2
// reflectance sensor 5 connected to PE.15  tim3cc1#0 PRS14#3
// reflectance sensor 4 connected to PE.14  tim3cc0#0 PRS13#2
// reflectance sensor 3 connected to PE.13  tim2cc2#3 PRS2#3
// reflectance sensor 2 connected to PE.5   tim3cc2#2
// reflectance sensor 1 connected to PE.9             PRS8#2
// reflectance sensor 0 connected to PE.8             PRS3#1
//                                          (robot's left)
#define REFL_PORT_7   (1ul << 11)
#define REFL_PORT_6   (1ul << 10)
#define REFL_PORT_5   (1ul << 15)
#define REFL_PORT_4   (1ul << 14)
#define REFL_PORT_3   (1ul << 13)
#define REFL_PORT_2   (1ul << 5)
#define REFL_PORT_1   (1ul << 9)
#define REFL_PORT_0   (1ul << 8)

#define REFL_PORT_MASK (REFL_PORT_0 | REFL_PORT_1 | REFL_PORT_2 | REFL_PORT_3 |\
                        REFL_PORT_4 | REFL_PORT_5 | REFL_PORT_6 | REFL_PORT_7)

#define IRLED(x)    (BUS_RegBitWrite(&GPIO->P[gpioPortD].DOUT, 1, x))

volatile uint8_t photo_array[8];
volatile uint32_t current_sensor, photo_data_ready;


void TIMER2_IRQHandler(void) {
  if (TIMER2->IF & TIMER_IF_CC0) {                                // Прерывание на пороге Threshold
      TIMER2->IFC = TIMER_IFC_CC0;
      current_sensor = GPIO->P[gpioPortE].DIN;
#ifdef REFL_USE_TIMER
      current_sensor &= (REFL_PORT_0 | REFL_PORT_1 | REFL_PORT_6 | REFL_PORT_7);
      if (TIMER2->STATUS & TIMER_STATUS_CCPOL2) current_sensor |= (1<<3);
      if (TIMER3->STATUS & TIMER_STATUS_CCPOL0) current_sensor |= (1<<4);
      if (TIMER3->STATUS & TIMER_STATUS_CCPOL1) current_sensor |= (1<<5);
      if (TIMER3->STATUS & TIMER_STATUS_CCPOL2) current_sensor |= (1<<2);
#else
      current_sensor &= (REFL_PORT_MASK);
      BUS_RamBitWrite(&current_sensor, 2, BUS_RamBitRead(&current_sensor, 5));
      BUS_RamBitWrite(&current_sensor, 3, BUS_RamBitRead(&current_sensor, 13));
      BUS_RamBitWrite(&current_sensor, 4, BUS_RamBitRead(&current_sensor, 14));
      BUS_RamBitWrite(&current_sensor, 5, BUS_RamBitRead(&current_sensor, 15));
#endif
      BUS_RamBitWrite(&current_sensor, 0, BUS_RamBitRead(&current_sensor, 8));
      BUS_RamBitWrite(&current_sensor, 1, BUS_RamBitRead(&current_sensor, 9));
      BUS_RamBitWrite(&current_sensor, 6, BUS_RamBitRead(&current_sensor, 10));
      BUS_RamBitWrite(&current_sensor, 7, BUS_RamBitRead(&current_sensor, 11));
      current_sensor &= 0xFF;
  }
  if (TIMER2->IF & TIMER_IF_CC1) {                                    // прерывание -10 мкс до начала следующего цикла
      unsigned int captured_val[8];
      if (TIMER2->STATUS & TIMER_STATUS_ICV2) {captured_val[3] = TIMER2->CC[2].CCV; } else captured_val[3] = 0xFFFF;
      if (TIMER3->STATUS & TIMER_STATUS_ICV0) {captured_val[4] = TIMER3->CC[0].CCV; } else captured_val[4] = 0xFFFF;
      if (TIMER3->STATUS & TIMER_STATUS_ICV1) {captured_val[5] = TIMER3->CC[1].CCV; } else captured_val[5] = 0xFFFF;
      if (TIMER3->STATUS & TIMER_STATUS_ICV2) {captured_val[2] = TIMER3->CC[2].CCV; } else captured_val[2] = 0xFFFF;

      photo_array[0] = (current_sensor & (1 << 0)) ? 255 : 0;
      photo_array[1] = (current_sensor & (1 << 1)) ? 255 : 0;
#ifdef REFL_USE_TIMER
      photo_array[2] = (captured_val[2] < (15625-77)) ? captured_val[2] / (61) : 255;
      photo_array[3] = (captured_val[3] < (15625-77)) ? captured_val[3] / (61) : 255;
      photo_array[4] = (captured_val[4] < (15625-77)) ? captured_val[4] / (61) : 255;
      photo_array[5] = (captured_val[5] < (15625-77)) ? captured_val[5] / (61) : 255;
#else
      photo_array[2] = (current_sensor & (1 << 2)) ? 255 : 0;
      photo_array[3] = (current_sensor & (1 << 3)) ? 255 : 0;
      photo_array[4] = (current_sensor & (1 << 4)) ? 255 : 0;
      photo_array[5] = (current_sensor & (1 << 5)) ? 255 : 0;
#endif
      photo_array[6] = (current_sensor & (1 << 6)) ? 255 : 0;
      photo_array[7] = (current_sensor & (1 << 7)) ? 255 : 0;
      photo_data_ready   = 1;
      TIMER2->ROUTEPEN = 0;
      TIMER3->ROUTEPEN = 0;
      BUS_RegMaskedSet(&GPIO->P[gpioPortE].DOUT, REFL_PORT_MASK);
      TIMER2->IFC = TIMER_IFC_CC1 | TIMER_IFC_CC2;
      TIMER3->IFC = TIMER_IFC_CC0 | TIMER_IFC_CC1 | TIMER_IFC_CC2;
  }
  if ((TIMER2->IF & TIMER_IF_OF) & (TIMER2->IEN & TIMER_IEN_OF)) {    // Переполнение таймера - момент 0
      TIMER2->IFC = TIMER_IFC_OF;
      BUS_RegMaskedClear(&GPIO->P[gpioPortE].DOUT, REFL_PORT_MASK);   // Выключаем заряд конденсаторов
      TIMER2->ROUTEPEN = TIMER_ROUTEPEN_CC2PEN;                       // и подключаем входы захвата таймеров
      TIMER3->ROUTEPEN = TIMER_ROUTEPEN_CC0PEN |
                         TIMER_ROUTEPEN_CC1PEN |
                         TIMER_ROUTEPEN_CC2PEN;
  }
}


void Reflectance_Init_with_Timer(unsigned int threshold, int ir_led_level){
  int i;
    // write this as part of Lab 6
  GPIO_PinModeSet(gpioPortD, 1, gpioModePushPull, 1);
  GPIO_PinModeSet(gpioPortE, 5, gpioModeWiredOr, 1);
  GPIO_PinModeSet(gpioPortE, 8, gpioModeWiredOr, 1);
  GPIO_PinModeSet(gpioPortE, 9, gpioModeWiredOr, 1);
  GPIO_PinModeSet(gpioPortE, 10, gpioModeWiredOr, 1);
  GPIO_PinModeSet(gpioPortE, 11, gpioModeWiredOr, 1);
  GPIO_PinModeSet(gpioPortE, 13, gpioModeWiredOr, 1);
  GPIO_PinModeSet(gpioPortE, 14, gpioModeWiredOr, 1);
  GPIO_PinModeSet(gpioPortE, 15, gpioModeWiredOr, 1);
  // установка уровня инфракрасной подсветки
    IRLED(0);   // выключить на 3 мс
//    Clock_Delay1ms(3);
    delay_us(3000);
    IRLED(1);
//      Clock_Delay1ms(3);
    delay_us(3000);
    for (i = 32; i > ir_led_level; i--) {
      IRLED(0);
      delay_us(10);
      IRLED(1);
      delay_us(10);
    }

    CMU_ClockEnable(cmuClock_TIMER2, 1);
    CMU_ClockEnable(cmuClock_TIMER3, 1);
    TIMER2->CTRL = TIMER_CTRL_MODE_UP | TIMER_CTRL_ATI |
            TIMER_CTRL_PRESC_DIV8 | TIMER_CTRL_CLKSEL_PRESCHFPERCLK;
    TIMER3->CTRL = TIMER_CTRL_MODE_UP | TIMER_CTRL_ATI | TIMER_CTRL_SYNC |
            TIMER_CTRL_PRESC_DIV8 | TIMER_CTRL_CLKSEL_PRESCHFPERCLK;

    TIMER2->TOP = 15625 - 1; // Period is
    TIMER3->TOP = 15625 - 1;

    TIMER2->CC[0].CTRL = TIMER_CC_CTRL_MODE_PWM;
    TIMER2->CC[0].CCV = threshold * 76294 / 10000;
    TIMER2->CC[1].CTRL = TIMER_CC_CTRL_MODE_PWM | TIMER_CC_CTRL_PRSCONF_LEVEL;
    TIMER2->CC[1].CCV = 15625 - 154;
    TIMER2->CC[2].CTRL = TIMER_CC_CTRL_MODE_INPUTCAPTURE |
                         TIMER_CC_CTRL_ICEDGE_FALLING |
                         TIMER_CC_CTRL_FILT_ENABLE;

    TIMER3->CC[0].CTRL = TIMER_CC_CTRL_MODE_INPUTCAPTURE |
                          TIMER_CC_CTRL_ICEDGE_FALLING |
                          TIMER_CC_CTRL_FILT_ENABLE;
    TIMER3->CC[1].CTRL = TIMER_CC_CTRL_MODE_INPUTCAPTURE |
                          TIMER_CC_CTRL_ICEDGE_FALLING |
                          TIMER_CC_CTRL_FILT_ENABLE;
    TIMER3->CC[2].CTRL = TIMER_CC_CTRL_MODE_INPUTCAPTURE |
                          TIMER_CC_CTRL_ICEDGE_FALLING |
                          TIMER_CC_CTRL_FILT_ENABLE;

    TIMER2->ROUTELOC0 = TIMER_ROUTELOC0_CC2LOC_LOC3;    // pe5
    TIMER3->ROUTELOC0 = TIMER_ROUTELOC0_CC0LOC_LOC0 |   // pe14
                        TIMER_ROUTELOC0_CC1LOC_LOC0 |   // pe15
                        TIMER_ROUTELOC0_CC2LOC_LOC2;    // pe13
#ifdef REFL_USE_TIMER
    TIMER2->ROUTEPEN = TIMER_ROUTEPEN_CC2PEN;
    TIMER3->ROUTEPEN = TIMER_ROUTEPEN_CC0PEN |
                       TIMER_ROUTEPEN_CC1PEN |
                       TIMER_ROUTEPEN_CC2PEN;
#endif
    TIMER2->IEN = TIMER_IEN_OF | TIMER_IEN_CC0 | TIMER_IEN_CC1;

    NVIC_SetPriority(TIMER2_IRQn, 3);
    NVIC_EnableIRQ(TIMER2_IRQn);

//    CMU_ClockEnable(cmuClock_PRS, 1);
//    PRS->CH[2].CTRL = PRS_CH_CTRL_SOURCESEL_TIMER2 | PRS_CH_CTRL_SIGSEL_TIMER2CC1 | PRS_CH_CTRL_INV;
//    PRS->CH[3].CTRL = PRS_CH_CTRL_SOURCESEL_TIMER2 | PRS_CH_CTRL_SIGSEL_TIMER2CC1 | PRS_CH_CTRL_INV;
//    PRS->CH[8].CTRL = PRS_CH_CTRL_SOURCESEL_TIMER2 | PRS_CH_CTRL_SIGSEL_TIMER2CC1 | PRS_CH_CTRL_INV;
//    PRS->CH[13].CTRL = PRS_CH_CTRL_SOURCESEL_TIMER2 | PRS_CH_CTRL_SIGSEL_TIMER2CC1 | PRS_CH_CTRL_INV;
//    PRS->CH[14].CTRL = PRS_CH_CTRL_SOURCESEL_TIMER2 | PRS_CH_CTRL_SIGSEL_TIMER2CC1 | PRS_CH_CTRL_INV;
//    PRS->ROUTELOC0 = PRS_ROUTELOC0_CH2LOC_LOC2 | PRS_ROUTELOC0_CH3LOC_LOC2;
//    PRS->ROUTELOC2 = PRS_ROUTELOC2_CH8LOC_LOC2;
//    PRS->ROUTELOC3 = PRS_ROUTELOC3_CH13LOC_LOC2 | PRS_ROUTELOC3_CH14LOC_LOC2;
//    PRS->ROUTEPEN  = PRS_ROUTEPEN_CH2PEN | PRS_ROUTEPEN_CH3PEN | PRS_ROUTEPEN_CH8PEN |
//                     PRS_ROUTEPEN_CH13PEN | PRS_ROUTEPEN_CH14PEN;

    TIMER2->CMD = TIMER_CMD_START;
}

int line_error(unsigned int first_sensor, unsigned int last_sensor) {
    const int W[] = {329, 235, 141, 47, -47, -141, -235, -329};
    if (first_sensor == 256) return 0;
    if (last_sensor == first_sensor) {
        return W[last_sensor];
    } else {
        return ((W[first_sensor] + W[last_sensor])/2 - 47) + ((photo_array[first_sensor]-(data.threshold >> 3))*94/(photo_array[first_sensor]+photo_array[last_sensor]-((2 * data.threshold) >> 3))) ;
    }
}

void set_threshold(unsigned int threshold) {
  TIMER2->CC[0].CCV = threshold * 76294 / 10000;
}

void TestReflect(void) {
  uint32_t photo_sensor, photo_mask, first_sensor, last_sensor, i;
  uint8_t photo_sensor_copy[8];
  while (kbdread() != KEY_DOWN) {
    if (photo_data_ready ) {
      photo_data_ready  = 0;
      photo_sensor = current_sensor;
      photo_mask = photo_sensor;
      first_sensor = 256;
      last_sensor = 256;
      for (i=0; i<8; i++) {
        photo_sensor_copy[i] = photo_array[i];
        if (photo_mask & 0x01u) {
          last_sensor = i;
          if (first_sensor == 256) first_sensor = i;
        }
        photo_mask >>= 1;
      }
      if (first_sensor == 256) {first_sensor = 3; last_sensor = 4; }
      show_hystogram(photo_sensor, photo_sensor_copy, (data.threshold), line_error(first_sensor, last_sensor));
    }
  }
}
