/*
 * Модуль таходатчика.
 * Используется TIMER0 для измерения периода импульсов правого и левого
 * моторов. WTIMER0 и WTIMER1 работают в режиме инкрементального энкодера
 * и считают число импульсов и фиксируют направление счета.
 *
 */

#define PERIOD_AVERAGE

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

//#define ENC_Left_A 	(BUS_RegBitRead(&GPIO->P[gpioPortD].DIN, 1))
//#define ENC_Left_B	(BUS_RegBitRead(&GPIO->P[gpioPortB].DIN, 4))
//#define ENC_Right_A	(BUS_RegBitRead(&GPIO->P[gpioPortD].DIN, 2))
//#define ENC_Right_B	(BUS_RegBitRead(&GPIO->P[gpioPortD].DIN, 3))


volatile Tach_stru_t TachLeft, TachRight;

volatile unsigned int RollOverLeft = 0, RollOverRight = 0;
volatile uint32_t LeftPeriodInc = 0, RightPeriodInc = 0;

volatile uint32_t EventCountLeft = 0, EventCountRight = 0;


//volatile int LeftSteps = 0, RightSteps = 0;

void tachometerwLeftInt(uint32_t currenttime){
  static uint32_t Tachometer_PrevTime = 0;
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
#endif
}

void tachometerwRightInt(uint32_t currenttime){
  static uint32_t Tachometer_PrevTime = 0;
  if (WTIMER1->STATUS & WTIMER_STATUS_DIR) {
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
#endif
}

void tachometerLeftInt(uint16_t currenttime){
  static uint32_t Tachometer_FirstTime, Tachometer_SecondTime = 0;
  Tachometer_FirstTime = Tachometer_SecondTime & 0xFFFF;
  Tachometer_SecondTime = currenttime + ((RollOverLeft) << 16);
  if (PCNT1->STATUS & PCNT_STATUS_DIR) {
      TachLeft.Dir = REVERSE;
  } else {
      TachLeft.Dir = FORWARD;
  }
  if (RollOverLeft < 2) {
      TachLeft.Period = Tachometer_SecondTime - Tachometer_FirstTime;
  } else {
      TachLeft.Dir = STOPPED;
      TachLeft.Period = 0x20000; //65535;
  }
  RollOverLeft = 0;

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
#endif
}

void tachometerRightInt(uint16_t currenttime){
  static uint32_t Tachometer_FirstTime, Tachometer_SecondTime = 0;
  Tachometer_FirstTime = Tachometer_SecondTime & 0xFFFF;
  Tachometer_SecondTime = currenttime + ((RollOverRight) << 16);
  if (WTIMER1->STATUS & WTIMER_STATUS_DIR) {
      TachRight.Dir = REVERSE;
  } else {
      TachRight.Dir = FORWARD;
  }
  if (RollOverRight < 2) {
      TachRight.Period = Tachometer_SecondTime - Tachometer_FirstTime;
  } else {
      TachRight.Dir = STOPPED;
      TachRight.Period = 0x20000; //65535;
  }
  RollOverRight = 0;

#ifdef PERIOD_AVERAGE
#define LPF_FACTOR  3
    static unsigned int PeriodBuff[1 << LPF_FACTOR] = {0}, PeriodSum = 0, PeriodIndex=0;
    PeriodSum -= PeriodBuff[PeriodIndex];
    PeriodBuff[PeriodIndex] = TachRight.Period;
    PeriodSum +=  PeriodBuff[PeriodIndex++];
    PeriodIndex &= (1 << LPF_FACTOR) - 1;
    TachRight.Period = PeriodSum >> LPF_FACTOR;
#endif
}

void tachometrInt(void) {
    if (++RollOverLeft  > 2) TachLeft.Dir  = STOPPED;
    if (++RollOverRight > 2) TachRight.Dir = STOPPED;
}


void tachometer_init() {
	TachLeft.Dir = TachRight.Dir = STOPPED;
	TachLeft.Period = TachRight.Period = 65535;
	CMU_ClockEnable(cmuClock_TIMER0, 1);

//  CMU_ClockEnable(cmuClock_WTIMER0, 1);
  CMU_ClockEnable(cmuClock_WTIMER1, 1);
  CMU_ClockEnable(cmuClock_PRS, 1);
  CMU_ClockEnable(cmuClock_PCNT1, 1); // счетчик левый

  // initialize Ports and make it GPIO
  GPIO_PinModeSet(gpioPortD, 2, gpioModeInputPull, 1);
  GPIO_PinModeSet(gpioPortD, 3, gpioModeInputPull, 1);
  GPIO_PinModeSet(gpioPortB, 3, gpioModeInputPull, 1);
  GPIO_PinModeSet(gpioPortB, 4, gpioModeInputPull, 1);

  GPIO->EXTIPSELL = GPIO_EXTIPSELL_EXTIPSEL3_PORTB;
//  GPIO->EXTIPSELL = GPIO_EXTIPSELL_EXTIPSEL3_PORTD;
  GPIO->EXTIPINSELL = GPIO_EXTIPINSELL_EXTIPINSEL3_PIN3;
  PRS->CH[TACH_PRS_CH].CTRL = PRS_CH_CTRL_SOURCESEL_GPIOL | PRS_CH_CTRL_SIGSEL_GPIOPIN3;

  PCNT1->CTRL = PCNT_CTRL_MODE_OVSQUAD2X;
  PCNT1->TOPB  = 0xFFFF;
  PCNT1->ROUTELOC0 = PCNT_ROUTELOC0_S0INLOC_LOC1 | PCNT_ROUTELOC0_S1INLOC_LOC1;


	WTIMER1->TOP = 0xFFFFFFFFul;

  WTIMER1->CTRL = WTIMER_CTRL_MODE_QDEC | WTIMER_CTRL_QDM_X2 |
                  WTIMER_CTRL_DISSYNCOUT | WTIMER_CTRL_DEBUGRUN;

  WTIMER1->CC[0].CTRL = WTIMER_CC_CTRL_INSEL_PIN | WTIMER_CC_CTRL_FILT_DISABLE;
  WTIMER1->CC[1].CTRL = WTIMER_CC_CTRL_INSEL_PIN | WTIMER_CC_CTRL_FILT_DISABLE;

  WTIMER1->ROUTEPEN = WTIMER_ROUTEPEN_CC0PEN | WTIMER_ROUTEPEN_CC1PEN;

  WTIMER1->ROUTELOC0 = WTIMER_ROUTELOC0_CC0LOC_LOC1 | WTIMER_ROUTELOC0_CC1LOC_LOC1;

  WTIMER1->CMD = WTIMER_CMD_START;

  TIMER0->CTRL = TIMER_CTRL_MODE_UP | TIMER_CTRL_DISSYNCOUT | TIMER_CTRL_PRESC_DIV4; // 1 tick = 80ns
  TIMER0->TOP  = 0xFFFF;

  TIMER0->CC[0].CTRL = TIMER_CC_CTRL_MODE_INPUTCAPTURE | TIMER_CC_CTRL_INSEL_PRS | TIMER_CC_PRSSEL_PRSCH(TACH_PRS_CH) |
                        TIMER_CC_CTRL_FILT_DISABLE | TIMER_CC_CTRL_ICEDGE_RISING;
  TIMER0->CC[1].CTRL = TIMER_CC_CTRL_MODE_INPUTCAPTURE | TIMER_CC_CTRL_INSEL_PIN |
                        TIMER_CC_CTRL_FILT_DISABLE | TIMER_CC_CTRL_ICEDGE_RISING;
  TIMER0->ROUTELOC0 = TIMER_ROUTELOC0_CC1LOC_LOC2;
  TIMER0->ROUTEPEN = TIMER_ROUTEPEN_CC1PEN;

  TIMER0->IEN = TIMER_IEN_CC0 | TIMER_IEN_CC1 | TIMER_IEN_OF;

  WTIMER0->CC[0].CTRL = WTIMER_CC_CTRL_MODE_INPUTCAPTURE | WTIMER_CC_CTRL_INSEL_PRS | WTIMER_CC_PRSSEL_PRSCH(TACH_PRS_CH) |
                        WTIMER_CC_CTRL_FILT_DISABLE | WTIMER_CC_CTRL_ICEDGE_RISING;
  WTIMER0->CC[1].CTRL = WTIMER_CC_CTRL_MODE_INPUTCAPTURE | WTIMER_CC_CTRL_INSEL_PIN |
                        WTIMER_CC_CTRL_FILT_DISABLE | WTIMER_CC_CTRL_ICEDGE_RISING;
  WTIMER0->ROUTELOC0 = WTIMER_ROUTELOC0_CC1LOC_LOC6;
  WTIMER0->ROUTEPEN =  WTIMER_ROUTEPEN_CC1PEN;

  WTIMER0->IEN = TIMER_IEN_CC0 | TIMER_IEN_CC1;

  NVIC_SetPriority(TIMER0_IRQn, TIMER0_IRQ_PRI);
  NVIC_EnableIRQ(TIMER0_IRQn);
  NVIC_SetPriority(WTIMER0_IRQn, TIMER0_IRQ_PRI);
//  NVIC_EnableIRQ(WTIMER0_IRQn);
  TIMER0->CMD = TIMER_CMD_START;
  WTIMER0->CMD = WTIMER_CMD_START;
}

void TIMER0_IRQHandler(void) {
  if (TIMER0->IF & TIMER_IF_OF) {
      TIMER0->IFC = TIMER_IFC_OF;
      tachometrInt();
  }
  if (TIMER0->IF & TIMER_IF_CC0) {
      TIMER0->IFC = TIMER_IFC_CC0;
      tachometerLeftInt(TIMER0->CC[0].CCV);
  }
  if (TIMER0->IF & TIMER_IF_CC1) {
      TIMER0->IFC = TIMER_IFC_CC1;
      tachometerRightInt(TIMER0->CC[1].CCV);
  }
}

void WTIMER0_IRQHandler(void) {
  if (WTIMER0->IF & WTIMER_IF_CC0) {
      WTIMER0->IFC = WTIMER_IFC_CC0;
      tachometerwLeftInt(WTIMER0->CC[0].CCV);
  }
  if (WTIMER0->IF & WTIMER_IF_CC1) {
      WTIMER0->IFC = WTIMER_IFC_CC1;
      tachometerwRightInt(WTIMER0->CC[1].CCV);
  }
}

void reset_steps(void) {
//  WTIMER0->CNT = 0;
  WTIMER1->CNT = 0;
  PCNT1->CMD = PCNT_CMD_LCNTIM;
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
  *rightTach = TachRight;
  *leftTach  = TachLeft;
//  if (EventCountLeft == 0)  leftTach->Dir  = STOPPED;
//  if (EventCountRight == 0) rightTach->Dir = STOPPED;

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
    str_len = num2str((RightSteps+LeftSteps)*11/(2*36), clipboard);
    for (i = 15; str_len--; i--) {
      dist_str[i] = clipboard[str_len];
    }
    while (i > 4) dist_str[i--] = ' ';

// *************************************************************
//    str_len = num2str((int16_t)PCNT1->CNT, clipboard);
//    for (i = 14; str_len--; i--) {
//      pcnt_str[i] = clipboard[str_len];
//    }
//    while (i > 4) pcnt_str[i--] = ' ';
//
//    if (PCNT1->STATUS & PCNT_STATUS_DIR) {
//            pcnt_str[15] = 'R';
//    } else {
//            pcnt_str[15] = 'F';
//    }
  }
}
