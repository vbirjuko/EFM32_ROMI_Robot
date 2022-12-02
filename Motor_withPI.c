#include <em_device.h>
#include "em_gpio.h"
#include "em_cmu.h"
#include "configure.h"
#include "Tachometer.h"
#include "LaunchPad.h"
#include "keyboard.h"
#include "display.h"
#include "resources.h"
#include "Logging.h"
#include "Motor.h"

#define ENABLE_PORT           gpioPortB
#define ENABLE_LEFT_PIN            (12)
#define ENABLE_RIGHT_PIN           (11)
#define DIRECTION_PORT_LEFT   gpioPortD
#define DIRECTION_PORT_RIGHT  gpioPortD
#define DIRECTION_LEFT	            (3)
#define DIRECTION_RIGHT	            (2)
#define SLEEP_PORT            gpioPortD
#define SLEEP_PIN                   (4)

// 50 000 000 / 4 * 60 * 100 / 360
#define T_CONSTANT  (CPU_FREQ * 25 / 6)

void Motor_PWM (int16_t left, int16_t right) {
    if (left < 0) {
        BUS_RegBitWrite(&GPIO->P[DIRECTION_PORT_LEFT].DOUT, DIRECTION_LEFT, 1);
        TIMER1->CC[3].CCV = -left;
    } else {
        BUS_RegBitWrite(&GPIO->P[DIRECTION_PORT_LEFT].DOUT, DIRECTION_LEFT, 0);
        TIMER1->CC[3].CCV = left;
    }
    if (right < 0) {
        BUS_RegBitWrite(&GPIO->P[DIRECTION_PORT_RIGHT].DOUT, DIRECTION_RIGHT, 1);
//        BUS_RamBitWrite(&launchpad_LED_state, DIRECTION_RIGHT, 1);
        TIMER1->CC[2].CCV = -right;
    } else {
        BUS_RegBitWrite(&GPIO->P[DIRECTION_PORT_RIGHT].DOUT, DIRECTION_RIGHT, 0);
//        BUS_RamBitWrite(&launchpad_LED_state, DIRECTION_RIGHT, 0);
        TIMER1->CC[2].CCV = right;
    }
}

//Tach_stru_t Left, Right;
volatile int XstartL  = 0, XstartR = 0;
int profiler_log_data[5];

void Controller(void){

// write this as part of Lab 17
    Tach_stru_t Left, Right;
    static int XprimeL, ErrorL, UIL, UPR, UR;
    static int XprimeR, ErrorR, UIR, UPL, UL;
//		unsigned int backlight = 0;

		Tachometer_Get(&Left, &Right);
		XprimeL = (Left.Period)  ? T_CONSTANT/Left.Period  : T_CONSTANT/0x20000;
		XprimeR = (Right.Period) ? T_CONSTANT/Right.Period : T_CONSTANT/0x20000;
    if (Left.Dir == REVERSE) XprimeL = -XprimeL;
    if (Right.Dir == REVERSE) XprimeR = -XprimeR;

    ErrorL = XstartL - XprimeL;
    ErrorR = XstartR - XprimeR;

    profiler_log_data[0] = 0;
    profiler_log_data[1] = XstartL;
    profiler_log_data[2] = XstartR;
    profiler_log_data[3] = XprimeL;
    profiler_log_data[4] = XprimeR;
    add_data_log(profiler_log_data);

    UIL += (data.motor_Kint*ErrorL) >> 10;
    UIR += (data.motor_Kint*ErrorR) >> 10;

    if (UIL > 14999) UIL = 14999;
    else if (UIL < -14999) UIL = -14999;

    if (UIR > 14999) UIR = 14999;
    else if (UIR < -14999) UIR = -14999;

    UPL = (data.motor_Kprop*ErrorL) >> 10;
    UPR = (data.motor_Kprop*ErrorR) >> 10;

    UL = UIL + UPL;
    UR = UIR + UPR;

    if (XstartL == 0 && Left.Dir  == STOPPED) UIL = UL = 0;
    if (XstartR == 0 && Right.Dir == STOPPED) UIR = UR = 0;

    if (UL > 14999) UL = 14999;
    else if (UL < -14999) UL = -14999;

    if (UR > 14999) UR = 14999;
    else if (UR < -14999) UR = -14999;

		Motor_PWM(UL, UR);
}

#include "sl_board_control_config.h"
void Motor_Init(void){
  // write this as part of Lab 13
    // disable PDM on PB12/PB11
  GPIO_PinModeSet(SL_BOARD_ENABLE_SENSOR_MICROPHONE_PORT, SL_BOARD_ENABLE_SENSOR_MICROPHONE_PIN, gpioModePushPull, 0);
    // Direction
  GPIO_PinModeSet(DIRECTION_PORT_LEFT, DIRECTION_LEFT, gpioModePushPull, 0);
  GPIO_PinModeSet(DIRECTION_PORT_RIGHT, DIRECTION_RIGHT, gpioModePushPull, 0);
      // nSleep
  GPIO_PinModeSet(SLEEP_PORT, SLEEP_PIN, gpioModePushPull, 0);
      // PWM
  GPIO_PinModeSet(ENABLE_PORT, ENABLE_LEFT_PIN, gpioModePushPull, 0);
  GPIO_PinModeSet(ENABLE_PORT, ENABLE_RIGHT_PIN, gpioModePushPull, 0);

  CMU_ClockEnable(cmuClock_TIMER1, 1);

  TIMER1->CTRL = TIMER_CTRL_DISSYNCOUT | TIMER_CTRL_MODE_UPDOWN | TIMER_CTRL_DEBUGRUN |
      TIMER_CTRL_PRESC_DIV16 | TIMER_CTRL_CLKSEL_PRESCHFPERCLK;

  TIMER1->TOP = 15000 - 1; // Period is 2*period*16*20ns ns is 0.640*period -> 9.6 ms

  TIMER1->CC[2].CTRL = TIMER_CC_CTRL_MODE_PWM;
  TIMER1->CC[3].CTRL = TIMER_CC_CTRL_MODE_PWM;

  TIMER1->CC[2].CCV = 0;
  TIMER1->CC[3].CCV = 0;

  TIMER1->ROUTEPEN = TIMER_ROUTEPEN_CC2PEN | TIMER_ROUTEPEN_CC3PEN;
  TIMER1->ROUTELOC0 = TIMER_ROUTELOC0_CC2LOC_LOC3 | TIMER_ROUTELOC0_CC3LOC_LOC3;

  NVIC_SetPriority(TIMER1_IRQn, TIMER1_IRQ_PRI);
  NVIC_EnableIRQ(TIMER1_IRQn);

  TIMER1->IEN = TIMER_IEN_OF | TIMER_IEN_UF;
  TIMER1->CMD = TIMER_CMD_START;

}

void TIMER1_IRQHandler(void){
  // write this as part of lab 16
    if ((TIMER1->IEN & TIMER_IEN_OF) && (TIMER1->IF & TIMER_IF_OF)) {
        TIMER1->IFC = TIMER_IFC_OF;
            Controller();
    }
    if ((TIMER1->IEN & TIMER_IEN_UF) && (TIMER1->IF & TIMER_IF_UF)) {
        TIMER1->IFC = TIMER_IFC_UF;
            Controller();
    }
}

void Motor_Speed(int left, int right) {
  __disable_irq();
  XstartL = left;
	XstartR = right;
	__enable_irq();
}

// ------------Motor_Stop------------
void Motor_Stop(void){
	XstartL = 0;
	XstartR = 0;
}

void Motor_Enable(void) {
  BUS_RegBitWrite(&GPIO->P[SLEEP_PORT].DOUT, SLEEP_PIN, 1);
}

void Motor_Disable(void) {
	BUS_RegBitWrite(&GPIO->P[SLEEP_PORT].DOUT, SLEEP_PIN, 0);
}

volatile unsigned int motor_delay = 0;
void TestMotor(void) {
  unsigned int step = 0, update = 0;
  squareXY( 0,  0, 127, 63, 0);
  putstr(3, 1, "Motor test\0", 0);
  putstr(1, 6, "Press Key Down\0", 0);
  putstr(3, 7, "to finish\0", 0);
  squareXY(55, 23, 72, 40, 1);
  update_display();
  Motor_Enable();
  motor_delay = SCANPERSECOND*750/1000;
  while(kbdread() != KEY_DOWN) {
    switch (step) {
      case 0:
      case 1:
        if (update) update_display();
        Motor_Speed(0, 0);
        break;

      case 2:
        if (update) show_arrow(56, 3, right);
        Motor_Speed(data.turnspeed, -data.turnspeed);
        break;

      case 3:
        if (update) show_arrow(56, 3, left);
        Motor_Speed(-data.turnspeed, data.turnspeed);
        break;

      case 4:
        if (update) show_arrow(56, 3, straight);
        Motor_Speed(data.turnspeed, data.turnspeed);
        break;

      case 5:
        if (update) show_arrow(56, 3, back);
        Motor_Speed(-data.turnspeed, -data.turnspeed);
        break;
    }
    update = 0;
    if (!motor_delay) {
      if (++step > 5) step = 0;
      motor_delay = SCANPERSECOND*750/1000;
      update = 1;
    }
  }
  Motor_Speed(0, 0);
  Motor_Disable();
}
