/***************************************************************************//**
 * @file
 * @brief Top level application functions
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/
#include "stdint.h"

#include "em_device.h"
#include "resources.h"
#include "LaunchPad.h"
#include "keyboard.h"
#include "SysTickInts.h"
#include "UART0.h"
#include "color_sensor.h"
#include "i2c_drv.h"
#include "spi_drv.h"
#include "Motor.h"
#include "Tachometer.h"
#include "crc.h"
#include "display.h"
#include "configure.h"
#include "Reflectance.h"
#include "battery.h"
#include "LE_timer0.h"
#include "profiler.h"
#include "Maze.h"
#include "rand.h"
#include "inject_data.h"
#include "benchmark.h"
#include "placeholder.h"

/***************************************************************************//**
 * Initialize application.
 ******************************************************************************/
void app_init(void)
{
  unsigned int crc_err = 0, no_color_sensor = 0;
  PendSVInit();
  LaunchPad_Init();
  LaunchPad_Output(0);
  LaunchPad_Output1(0);
  le_timer_Init();
  benchmark_init();
  i2c_master_init();
  if (color_sensor_init()) no_color_sensor = 1;
  spi_eeprom_init();
//  oled_ssd1327_Init();
//  while(1);
  kbd_init();
  display_init();
  crc32_init();
  spi_read_eeprom(EEPROM_CONFIG_ADDRESS, (unsigned char *)&data, sizeof(data));
  crc_err = calc_crc32((unsigned char *)&data, sizeof(data));
  if (crc_err) LaunchPad_LED(1);

  spi_read_eeprom(ROM_map_addr, (unsigned char *)&map, sizeof(map));
  SysTick_Init(CPU_FREQ/SCANPERSECOND, SysTick_IRQ_Priority);
  UART0_Init();
  tachometer_init();
  Motor_Init();
  Reflectance_Init(data.threshold, data.ir_led_level);
  init_battery();

  if (crc_err) {
      crc_err = 0;
      squareXY(0, 0, 127, 63, 2);
      putstr(2, 2, "EEPROM read", 0);
      putstr(2, 3, " CRC error ", 0);
      update_display();

      spi_read_eeprom(EEPROM_COPY_ADDRESS, (unsigned char *)&data, sizeof(data));
      crc_err = calc_crc32((unsigned char *)&data, sizeof(data));
      if (crc_err == 0) {
          putstr(2, 4, " Recovered.", 0);
          update_display();
      }

      while (kbdread() != KEY_DOWN) continue;
  }
  Seed(data.lefthand);
  if (no_color_sensor) {
      no_color_sensor = 0;
      squareXY(0, 0, 127, 63, 2);
      putstr(2, 2, "Color sensor", 0);
      putstr(2, 3, "not present ", 0);
      update_display();
      while (kbdread() != KEY_DOWN) continue;
  }
  if (LaunchPad_Input() == 0x03) {
      while (LaunchPad_Input() ) continue;
      map_select();
  }
}

/***************************************************************************//**
 * App ticking function.
 ******************************************************************************/
void app_process_action(void) {

  const menuitem_t main_menu_items[] = {
      {"Speed Play      ", SpeedPlay,     execute},
      {"Explore Maze    ", Explore_Maze,  execute},
      {"Map draw        ", DrawMap,       execute},
      {"Search Short Way", SearchShortWay, execute},
      {"Configure       ", Configure,     execute},
      {"Battery Voltage ", ShowBattery,   execute},
      {"Reflectance test", TestReflect,   execute},
      {"Color sens. test", TestColor,     execute},
      {"Motor test      ", TestMotor,     execute},
      {"Tachometer  test", TestTachom,    execute},
      {"OPT3101 test    ", opt_main2,     execute},
  };
  do_menu((menuitem_t*) main_menu_items, ((sizeof(main_menu_items)/sizeof(main_menu_items[0]))-1));
}

volatile unsigned int sys_time = 0;
void SysTick_Handler(void){ // every 1/2 ms
  sys_time++;
//  color_iteration();
  if (kbddelay) kbddelay--;
  if (disp_delay) disp_delay--;
  if (motor_delay) motor_delay--;
//  if (delay_us_count) delay_us_count--;
  ADC0->CMD = ADC_CMD_SINGLESTART;
}

void HardFault_Handler(void) {
  Motor_Disable();
  LaunchPad_Output1(7);
  LaunchPad_Output(7);
  while(1) continue;
}
