/*
 * Модуль таходатчика.
 * Используется WTIMER0 для измерения периода импульсов правого и левого
 * моторов. PCNT1 и PCNT2 работают в режиме инкрементального энкодера
 * и считают число импульсов и фиксируют направление счета.
 *
 */

//#define PERIOD_AVERAGE
#define PERIOD_EXP_AVERAGE

#include "em_device.h"
#include "em_gpio.h"
#include "em_cmu.h"

#include "Tachometer.h"
#include "display.h"
#include "keyboard.h"
#include "resources.h"

#define TIMER_CC_PRS(A) TIMER_CC_CTRL_PRSSEL_PRSCH ## A
#define TIMER_CC_PRSSEL_PRSCH(A) TIMER_CC_PRS(A)

#define WTIMER_CC_PRS(A) WTIMER_CC_CTRL_PRSSEL_PRSCH ## A
#define WTIMER_CC_PRSSEL_PRSCH(A) WTIMER_CC_PRS(A)

volatile Tach_stru_t TachLeft, TachRight;

volatile unsigned int RollOverLeft = 0, RollOverRight = 0;
volatile uint32_t LeftPeriodInc = 0, RightPeriodInc = 0;

volatile uint32_t EventCountLeft = 0, EventCountRight = 0;


//volatile int LeftSteps = 0, RightSteps = 0;

void tachometerwLeftInt(uint32_t currenttime){
  static uint32_t Tachometer_PrevTime = 0;
//  if ((currenttime - Tachometer_PrevTime) < 8000) return;
  if (PCNT1->STATUS & PCNT_STATUS_DIR) {
      TachLeft.Dir = REVERSE;
  } else {
      TachLeft.Dir = FORWARD;
  }
  TachLeft.Period = currenttime - Tachometer_PrevTime;
  Tachometer_PrevTime = currenttime;
  EventCountLeft++;

#ifdef PERIOD_AVERAGE
#define LPF_FACTOR  3
    static unsigned int PeriodBuff[1 << LPF_FACTOR] = {0}, PeriodSum = 0, PeriodIndex=0;
//    __disable_irq();
    PeriodSum -= PeriodBuff[PeriodIndex];
    PeriodBuff[PeriodIndex] = TachLeft.Period;
    PeriodSum +=  PeriodBuff[PeriodIndex++];
    PeriodIndex &= (1 << LPF_FACTOR) - 1;
//    __enable_irq();
    TachLeft.Period = PeriodSum >> LPF_FACTOR;
#elif defined(PERIOD_EXP_AVERAGE)
    static uint32_t average = 0;
    average = ((1*TachLeft.Period + (4-1)*average) + 2) >> 2;  // 0.25 exp filter.
    TachLeft.Period = average;
#endif
}

void tachometerwRightInt(uint32_t currenttime){
  static uint32_t Tachometer_PrevTime = 0;
//  if ((currenttime - Tachometer_PrevTime) < 8000) return;
  if (PCNT2->STATUS & PCNT_STATUS_DIR) {
      TachRight.Dir = REVERSE;
  } else {
      TachRight.Dir = FORWARD;
  }
  TachRight.Period = currenttime - Tachometer_PrevTime;
  Tachometer_PrevTime = currenttime;
  EventCountRight++;

#ifdef PERIOD_AVERAGE
#define LPF_FACTOR  3
    static unsigned int PeriodBuff[1 << LPF_FACTOR] = {0}, PeriodSum = 0, PeriodIndex=0;
//    __disable_irq();
    PeriodSum -= PeriodBuff[PeriodIndex];
    PeriodBuff[PeriodIndex] = TachRight.Period;
    PeriodSum +=  PeriodBuff[PeriodIndex++];
    PeriodIndex &= (1 << LPF_FACTOR) - 1;
//    __enable_irq();
    TachRight.Period = PeriodSum >> LPF_FACTOR;
#elif defined(PERIOD_EXP_AVERAGE)
    static uint32_t average = 0;
    average = ((1*TachRight.Period + (4-1)*average) + 2) >> 2;  // 0.25 exp filter.
    TachRight.Period = average;
#endif
}

void reset_steps(void) {
//  WTIMER0->CNT = 0;
  PCNT1->CMD = PCNT_CMD_LCNTIM;
  PCNT2->CMD = PCNT_CMD_LCNTIM;
}


void tachometer_init() {
	TachLeft.Dir = TachRight.Dir = STOPPED;
	TachLeft.Period = TachRight.Period = 0x20000;

  CMU_ClockEnable(cmuClock_WTIMER0, 1);
//  CMU_ClockEnable(cmuClock_WTIMER1, 1);
  CMU_ClockEnable(cmuClock_PRS, 1);
  CMU_ClockEnable(cmuClock_PCNT1, 1); // счетчик левый
  CMU_ClockEnable(cmuClock_PCNT2, 1); // счетчик правый

  // initialize Ports and make it GPIO
  GPIO_PinModeSet(gpioPortD, 0, gpioModeInputPull, 1); // Правый
  GPIO_PinModeSet(gpioPortD, 1, gpioModeInputPull, 1);
  GPIO_PinModeSet(gpioPortB, 3, gpioModeInputPull, 1); // Левый
  GPIO_PinModeSet(gpioPortB, 4, gpioModeInputPull, 1);

  GPIO->EXTIPSELL = GPIO_EXTIPSELL_EXTIPSEL1_PORTD;
  GPIO->EXTIPINSELL = GPIO_EXTIPINSELL_EXTIPINSEL1_PIN1;
  PRS->CH[TACH_PRS_CH].CTRL = PRS_CH_CTRL_SOURCESEL_GPIOL | PRS_CH_CTRL_SIGSEL_GPIOPIN1;

  // квадратурный счетчик левый
  PCNT1->CTRL = PCNT_CTRL_MODE_OVSQUAD2X;
  PCNT1->TOPB  = 0xFFFF;
  PCNT1->ROUTELOC0 = PCNT_ROUTELOC0_S0INLOC_LOC1 | PCNT_ROUTELOC0_S1INLOC_LOC1;

  // квадратурный счетчик правый
  PCNT2->CTRL = PCNT_CTRL_MODE_OVSQUAD2X;
  PCNT2->TOPB  = 0xFFFF;
  PCNT2->ROUTELOC0 = PCNT_ROUTELOC0_S0INLOC_LOC0 | PCNT_ROUTELOC0_S1INLOC_LOC0;

  reset_steps();

  // Таймер для измерения периода импульсов
  WTIMER0->CTRL = WTIMER_CTRL_MODE_UP | WTIMER_CTRL_DISSYNCOUT | WTIMER_CTRL_PRESC_DIV4; // 1 tick = 80ns
  WTIMER0->TOP  = 0xFFFFFFFFul;

  WTIMER0->CC[0].CTRL = WTIMER_CC_CTRL_MODE_INPUTCAPTURE | WTIMER_CC_CTRL_INSEL_PRS | WTIMER_CC_PRSSEL_PRSCH(TACH_PRS_CH) |
                        WTIMER_CC_CTRL_FILT_DISABLE | WTIMER_CC_CTRL_ICEDGE_BOTH; // Encoder Right PD1 over PRS
  WTIMER0->CC[1].CTRL = WTIMER_CC_CTRL_MODE_INPUTCAPTURE | WTIMER_CC_CTRL_INSEL_PIN |
                        WTIMER_CC_CTRL_FILT_DISABLE | WTIMER_CC_CTRL_ICEDGE_BOTH; // encoder Left PB4
  WTIMER0->ROUTELOC0  = WTIMER_ROUTELOC0_CC1LOC_LOC6;
  WTIMER0->ROUTEPEN   = WTIMER_ROUTEPEN_CC1PEN;

  WTIMER0->IEN = WTIMER_IEN_CC1 | WTIMER_IEN_CC0;

  NVIC_SetPriority(WTIMER0_IRQn, WTIMER0_IRQ_PRI);
  NVIC_EnableIRQ(WTIMER0_IRQn);

  WTIMER0->CMD = WTIMER_CMD_START;
}

void WTIMER0_IRQHandler(void) {
  if (WTIMER0->IF & WTIMER_IF_CC0) {
      WTIMER0->IFC = WTIMER_IFC_CC0;
      tachometerwRightInt(WTIMER0->CC[0].CCV);
  }
  if (WTIMER0->IF & WTIMER_IF_CC1) {
      WTIMER0->IFC = WTIMER_IFC_CC1;
      tachometerwLeftInt(WTIMER0->CC[1].CCV);
  }
}

// ------------Tachometer_Get------------
// Get the most recent tachometer measurements.
// Input: leftTach   is pointer to store last measured tachometer period of left wheel (units of 0.083 usec)
//        leftDir    is pointer to store enumerated direction of last movement of left wheel
//        leftSteps  is pointer to store total number of forward steps measured for left wheel (360 steps per ~220 mm circumference)
//        rightTach  is pointer to store last measured tachometer period of right wheel (units of 0.083 usec)
//        rightDir   is pointer to store enumerated direction of last movement of right wheel
//        rightSteps is pointer to store total number of forward steps measured for right wheel (360 steps per ~220 mm circumference)
// Output: none
// Assumes: Tachometer_Init() has been called
// Assumes: Clock_Init48MHz() has been called
void Tachometer_Get(Tach_stru_t *leftTach, Tach_stru_t *rightTach) {
  if (EventCountLeft == 0)  TachLeft.Dir  = STOPPED;
  if (EventCountRight == 0) TachRight.Dir = STOPPED;
  *rightTach = TachRight;
  *leftTach  = TachLeft;

  EventCountLeft = EventCountRight = 0;
}


void TestTachom(void)       {
  unsigned int keyb, str_len, i;
  char clipboard[12],
    left_str[17] = "Left ...........\0",
    right_str[17]= "Right...........\0",
    dist_str[17] = "Dist ...........\0";
//    pcnt_str[17] = "PCNT1...........\0";
  squareXY(0, 0, 127, 63, 0);  // очистка экрана
  putstr(0, 0, "Tachometer test \0", 0);
  update_display();

  while (1) {
    while (disp_delay) continue;
    disp_delay = 20*SCANPERSECOND/1000; // каждый фрейм через 20мс
    putstr(0, 2, left_str, 0);
    putstr(0, 3, right_str, 0);
    putstr(0, 4, dist_str, 0);
//    putstr(0, 5, pcnt_str, 0);
    keyb  = kbdread();
    switch (keyb) {
      case KEY_UP:
        reset_steps();
        break;

      case KEY_DOWN:
        return;
    }

// *************************************************************
    switch (TachLeft.Dir) {
        case FORWARD:
            left_str[15] = 'F'; break;
        case REVERSE:
            left_str[15] = 'R'; break;
        case STOPPED:
            left_str[15] = 'S'; break;
    }

    str_len = num2str(LeftSteps, clipboard);
    for (i = 14; str_len--; i--) {
      left_str[i] = clipboard[str_len];
    }
    while (i > 4) left_str[i--] = ' ';

// *************************************************************
    switch (TachRight.Dir) {
        case FORWARD:
            right_str[15] = 'F'; break;
        case REVERSE:
            right_str[15] = 'R'; break;
        case STOPPED:
            right_str[15] = 'S'; break;
    }

   str_len = num2str(RightSteps, clipboard);
    for (i = 14; str_len--; i--) {
      right_str[i] = clipboard[str_len];
    }
    while (i > 4) right_str[i--] = ' ';

// *************************************************************
    str_len = num2str(((int32_t)RightSteps+(int32_t)LeftSteps)*11/(2*36), clipboard);
    for (i = 15; str_len--; i--) {
      dist_str[i] = clipboard[str_len];
    }
    while (i > 4) dist_str[i--] = ' ';

  }
}
