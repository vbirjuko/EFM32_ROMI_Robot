/*
 * LaunchPad.c
 *
 *  Created on: 9 мар. 2021 г.
 *      Author: wl
 */
#include "em_device.h"
#include "em_gpio.h"



void LaunchPad_Init(void) {
//  GPIO->P[gpioPortA].CTRL =
  GPIO_PinModeSet(gpioPortA, 12, gpioModePushPull, 1);
  GPIO_PinModeSet(gpioPortA, 13, gpioModePushPull, 1);
  GPIO_PinModeSet(gpioPortA, 14, gpioModePushPull, 1);
  GPIO_PinModeSet(gpioPortD,  6, gpioModePushPull, 1);
  GPIO_PinModeSet(gpioPortF, 12, gpioModePushPull, 1);
  GPIO_PinModeSet(gpioPortE, 12, gpioModePushPull, 1);

  GPIO_PinModeSet(gpioPortD,  5, gpioModeInputPull, 1);
  GPIO_PinModeSet(gpioPortD,  8, gpioModeInputPull, 1);
}

/**
 * Input from two switches on LaunchPad
 * Value returned in postive logic
 *
 * @param  none
 * @return two-bit value of switches
 *         0x00 none
 *         0x01 Button1
 *         0x02 Button2
 *         0x03 both Button1 and Button2
 * @brief  Input from Switches
 */
uint8_t LaunchPad_Input(void) {
  return (GPIO_PinInGet(gpioPortD, 5) | (GPIO_PinInGet(gpioPortD, 8) << 1)) ^ 0x03;
}

/**
 * Output to red LED on LaunchPad
 *
 * @param  data is value to send to LED,
 * 0 is off; 1 is on
 * @return none
 * @brief  Output to red LED
 */
void LaunchPad_LED(uint8_t data) {
  if (data)
    GPIO_PinOutClear(gpioPortD, 6);
  else
    GPIO_PinOutSet(gpioPortD, 6);
}

/**
 * Output to 3-color LED on LaunchPad
 *
 * @param  data is value to send to LEDs
 * 0 is off; bit0 is red; bit1 is green; bit2 is blue
 * @return none
 * @brief  Output to 3-color LED
 */
#include "em_bus.h"

//volatile uint32_t launchpad_LED_state = 0;

void LaunchPad_Output(uint8_t data) {
  data ^= 0x07;
  BUS_RegBitWrite((volatile uint32_t *)&GPIO->P[gpioPortA].DOUT, 12, data & 0x01);
//  BUS_RamBitWrite(&launchpad_LED_state, 12, data & 0x01);
  data >>= 1;
  BUS_RegBitWrite((volatile uint32_t *)&GPIO->P[gpioPortA].DOUT, 14, data & 0x01);
//  BUS_RamBitWrite(&launchpad_LED_state, 14, data & 0x01);
  data >>= 1;
  BUS_RegBitWrite((volatile uint32_t *)&GPIO->P[gpioPortA].DOUT, 13, data & 0x01);
//  BUS_RamBitWrite(&launchpad_LED_state, 13, data & 0x01);
}

void LaunchPad_Output1(uint8_t data) {
  data ^= 0x07;
  BUS_RegBitWrite((volatile uint32_t *)&GPIO->P[gpioPortD].DOUT,  6, data & 0x01);
  data >>= 1;
  BUS_RegBitWrite((volatile uint32_t *)&GPIO->P[gpioPortF].DOUT, 12, data & 0x01);
  data >>= 1;
  BUS_RegBitWrite((volatile uint32_t *)&GPIO->P[gpioPortE].DOUT, 12, data & 0x01);
}
