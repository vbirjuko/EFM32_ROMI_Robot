/**
 * @file      Reflectance.h
 * @brief     Provide functions for the QTR-8RC reflectance sensor array
 * @details   Provide functions to take measurements using a QTR-8RC reflectance
 * sensor array (Pololu part number 961).  This works by outputting to
 * the sensor, waiting, then reading the digital value of each of the
 * eight phototransistors.  The more reflective the target surface is,
 * the faster the voltage decays. The time to wait is a function of<br>
 1) reflectance of surface<br>
 2) electrical resistance and capacitance of the GPIO pin<br>
 3) distance to surface<br>
 * For these reasons, you will have to experimentally verify which
 * time works best for your system.

*/


#ifndef REFLECTANCE_H_
#define REFLECTANCE_H_


/**
 * Initialize the GPIO pins associated with the QTR-8RC.
 * One output to IR LED, 8 inputs from the sensor array.
 * Initially, the IR outputs are off.
 * @param  Sensor threshold 0-2048
 * @param  IR LED level 0-32
 * @return none
 * @brief  Initialize the GPIO pins and timers for the QTR-8RC reflectance sensor.
 */

void Reflectance_Init_with_Timer(unsigned int threshold, int ir_led_level);

extern volatile uint8_t photo_array[16];
extern volatile uint32_t current_sensor, photo_data_ready;


/**
 * <b>Calculate the weighted average for each bit</b>:<br>
 * Position varies from -332 (left) to +332 (right), with units of 0.1mm.<br>
<table>
<caption id="QTR_distance">8 element arrays</caption>
<tr><th>  <th>bit7<th>bit6<th>bit5<th>bit4<th>bit3<th>bit2<th>bit1<th>bit0
<tr><td>Weight<td>-332<td>-237<td>-142<td> -47<td>  47<td> 142<td> 237<td> 332
<tr><td>Mask  <td>0x01<td>0x02<td>0x04<td>0x08<td>0x10<td>0x20<td>0x40<td>0x80
</table>
 * count = 0<br>
 * sum = 0<br>
 * for i from 0 to 7 <br>
 * if (data&Mask[i]) then count++ and sum = sum+Weight[i]<br>
 * calculate <b>position</b> = sum/count
 * @param  first line sensor number
 * @param  last  line sensor number
 * @return position in 0.1mm relative to center of line
 * @brief  Perform sensor integration.
 **/
int line_error(unsigned int first_sensor, unsigned int last_sensor);
void set_threshold(unsigned int threshold);
void ldma_adc_eoc(void);

#endif /* REFLECTANCE_H_ */
