/*
 * LaunchPad.h
 *
 *  Created on: 9 мар. 2021 г.
 *      Author: wl
 */

#ifndef LAUNCHPAD_H_
#define LAUNCHPAD_H_
#include "stdint.h"

/**
 * @file      LaunchPad.h
 * @brief     Provide simple I/O functions for the LaunchPad
 * @details   Input from switches, output to LED
 * built-in LED1 connected to P1.0 (not used here)
 * negative logic built-in Button 1 connected to P1.1
 * negative logic built-in Button 2 connected to P1.4
 * built-in red LED connected to P2.0
 * built-in green LED connected to P2.1
 * built-in blue LED connected to P2.2
 * @version   TI-RSLK MAX v1.1
 * @author    Daniel Valvano and Jonathan Valvano
 * @copyright Copyright 2019 by Jonathan W. Valvano, valvano@mail.utexas.edu,
 * @warning   AS-IS
 * @note      For more information see  http://users.ece.utexas.edu/~valvano/
 * @date      June 28, 2019
<table>
<caption id="LaunchPad">MSP432 LaunchPad</caption>
<tr><th>Pin  <th>Connection
<tr><td>P1.1 <td>negative logic built-in Button 1
<tr><td>P1.4 <td>negative logic built-in Button 2
<tr><td>P2.0 <td>built-in red LED
<tr><td>P2.1 <td>built-in green LED
<tr><td>P2.2 <td>built-in blue LED
<tr><td>P1.0 <td>built-in LED1
</table>
 ******************************************************************************/

/*!
 * @defgroup RSLK_Input_Output
 * @brief
 * @{*/

/**
 * \brief red color P2.2-P2.0 LED
 */
#define RED       0x01
/**
 * \brief green color P2.2-P2.0 LED
 */
#define GREEN     0x02
/**
 * \brief blue color P2.2-P2.0 LED
 */
#define BLUE      0x04

/**
 * Initialize LaunchPad I/O
 *
 * @param  none
 * @return none
 * @brief  Initialize Switch inputs and LED outputs
 */
void LaunchPad_Init(void);


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
uint8_t LaunchPad_Input(void);

/**
 * Output to red LED on LaunchPad
 *
 * @param  data is value to send to LED,
 * 0 is off; 1 is on
 * @return none
 * @brief  Output to red LED
 */
void LaunchPad_LED(uint8_t data);

/**
 * Output to 3-color LED on LaunchPad
 *
 * @param  data is value to send to LEDs
 * 0 is off; bit0 is red; bit1 is green; bit2 is blue
 * @return none
 * @brief  Output to 3-color LED
 */
void LaunchPad_Output(uint8_t data);

/**
 * Output to 3-color LED on LaunchPad
 *
 * @param  data is value to send to LEDs
 * 0 is off; bit0 is red; bit1 is green; bit2 is blue
 * @return none
 * @brief  Output to 3-color LED
 */
void LaunchPad_Output1(uint8_t data);
extern volatile uint32_t launchpad_LED_state;

#endif /* LAUNCHPAD_H_ */
