/*
 * battery.c
 *
 *  Created on: 4 янв. 2022 г.
 *      Author: wl
 */

#include "em_device.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "battery.h"
#include "configure.h"
#include "resources.h"

volatile uint32_t LPF_start = 0;
LPF_t LPF_battery;

void LPF__Init(LPF_t * instance, uint32_t initial, uint32_t size){
  unsigned int i;
  if(size>256) size=256; // max
  instance->Size = size;
  instance->I1 = size-1;
  instance->Sum = size*initial; // prime MACQ with initial data
  for(i=0; i<size; i++){
    instance->x[i] = initial;
  }
}

// calculate one filter output, called at sampling rate
// Input: new ADC data   Output: filter output
// y(n) = (x(n)+x(n-1)+...+x(n-Size-1)/Size
uint32_t LPF__Calc(LPF_t * instance, uint32_t newdata){
  if(instance->I1 == 0){
    instance->I1 = instance->Size-1;              // wrap
  } else{
    instance->I1--;                     // make room for data
  }
  instance->Sum = instance->Sum+newdata-instance->x[instance->I1];   // subtract oldest, add newest
  instance->x[instance->I1] = newdata;     // save new data
  instance->data_ready = 1;
  return instance->Sum/instance->Size;
}

void init_battery(void) {
  GPIO_PinModeSet(gpioPortA, 6, gpioModeDisabled, 0);
  CMU_ClockEnable(cmuClock_VDAC0, 1);
  LPF_start = 1;
  if (data.low_battery_level * data.volt_calibr < 10000) {
      ADC0->CMPTHR = (0xFFF << 16) | (data.low_battery_level * data.volt_calibr * 0xfff / 10000);
  } else {
//        assert("ERROR: incorrect values in low_battery_level or/and volt_calibr\r\n");
  }
  VDAC0->OPA[2].MUX = VDAC_OPA_MUX_POSSEL_APORT1XCH6 | VDAC_OPA_MUX_NEGSEL_UG | VDAC_OPA_MUX_RESINMUX_DISABLE;
  VDAC0->OPA[2].OUT = 0;
  VDAC0->CMD = VDAC_CMD_OPA2EN;

  ADC0->SINGLECTRL = ADC_SINGLECTRL_CMPEN | ADC_SINGLECTRL_AT_256CYCLES |
//    ADC_SINGLECTRL_POSSEL_APORT1XCH6 | ADC_SINGLECTRL_REF_2V5 | ADC_SINGLECTRL_RES_12BIT;
      ADC_SINGLECTRL_POSSEL_OPA2 | ADC_SINGLECTRL_REF_2V5 | ADC_SINGLECTRL_RES_12BIT;
  ADC0->SINGLECTRLX = ADC_SINGLECTRLX_FIFOOFACT_OVERWRITE | ADC_SINGLECTRLX_VREFSEL_VBGR;

  BUS_RegMaskedSet(&ADC0->IEN, ADC_IEN_SINGLE);
  NVIC_SetPriority(ADC0_IRQn, ADC0_IRQ_Priority);
  NVIC_EnableIRQ(ADC0_IRQn);
}

void ADC0_IRQHandler(void) {
  // write this for Lab 15
  if (LPF_start) {
      LPF_start = 0;
      LPF__Init(&LPF_battery, ADC0->SINGLEDATA, 256);
  } else {
      LPF__Calc(&LPF_battery, ADC0->SINGLEDATA);      // 4) return result 0 to 16383
  }
}

unsigned int get_battery_voltage(void) {
  return ((LPF_battery.Sum/LPF_battery.Size) * data.volt_calibr) >> 12;
}

#include "display.h"
#include "keyboard.h"
#include "LE_timer0.h"

void ShowBattery(void) {
  squareXY(0, 0, 127, 63, 0); // clear screen
  update_display();
  while(kbdread() != KEY_DOWN) {
    if (LPF_battery.data_ready) {
      LPF_battery.data_ready = 0;
      show_number(get_battery_voltage(), 1);
      if (ADC0->IF & ADC_IF_SINGLECMP) {
        putstr(3, 7, "Battery Low", 0);
      }
    }
//    Clock_Delay1ms(50);
    letimer_delay_ms(50);//delay_us(50000);
  }
  ADC0->IFC = ADC_IFC_SINGLECMP;
}
