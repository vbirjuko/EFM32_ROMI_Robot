#include "color_sensor.h"
#include "i2c_drv.h"
#include "configure.h"
#include "launchpad.h"

#define TCS_ADDR	(0x29 << 1)
#define TCS_COMMAND	0x80
#define TCS_INCREMENT	0x20
#define TCS_SF        0x66

enum classReg {
  ENABLE = 0x00,        // Enables states and interrupts
  ATIME = 0x01,         // RGBC time
  WTIME = 0x03,         // Wait time
  AILTL = 0x04,         // Clear interrupt low threshold low byte
  AILTH = 0x05,         // Clear interrupt low threshold high byte
  AIHTL = 0x06,         // Clear interrupt high threshold low byte
  AIHTH = 0x07,         // Clear interrupt high threshold high byte
  PERS = 0x0C,          // Interrupt persistence filter
  CONFIG = 0x0D,        // Configuration
  CONTROL = 0x0F,       // Control
  ID = 0x12,            // Device ID
  STATUS = 0x13,        // Device status
  CDATAL = 0x14,        // Clear data low byte
  CDATAH = 0x15,        // Clear data high byte
  RDATAL = 0x16,        // Red data low byte
  RDATAH = 0x17,        // Red data high byte
  GDATAL = 0x18,        // Green data low byte
  GDATAH = 0x19,        // Green data high byte
  BDATAL = 0x1A,        // Blue data low byte
  BDATAH = 0x1B,        // Blue data high byte
};

enum classMask {
  ENABLE_AIEN = 0x10,
  ENABLE_WEN = 0x08,
  ENABLE_AEN = 0x02,
  ENABLE_PON = 0x01,
  STATUS_AINT = 0x10,
  STATUS_AVALID = 0x01
};

enum classGain { X01 = 0, X04 = 1, X16 = 2, X60 = 3 };

unsigned char param;
volatile uint16_t color_sensors[4];
unsigned int color_sensor_present = 0;
#ifdef DEBUG_COLOR_SENS
static uint16_t *log_array_ptr = 0;
#endif

unsigned int color_sensor_init(void) {
  param = 0;
  i2c_rd_reg(TCS_ADDR, (TCS_COMMAND | ID), &param, 1);
  if ((param != 0x44) && (param != 0x4D)) return 1;
// ATIME = 256 − (Integration Time / 2.4) ms
  param = 256 - (75*5 + 6)/12 ; // ATIME 75 ms
  i2c_wr_reg(TCS_ADDR, TCS_COMMAND | ATIME, &param, sizeof(param));
  param = 0; // WLONG = 0
  i2c_wr_reg(TCS_ADDR, TCS_COMMAND | CONFIG, &param, sizeof(param));
  param = 0xff; //
  i2c_wr_reg(TCS_ADDR, TCS_COMMAND | WTIME, &param, sizeof(param));
  param = X04; // AGAIN 1x
  i2c_wr_reg(TCS_ADDR, TCS_COMMAND | CONTROL, &param, sizeof(param));
  param = 0;
  i2c_wr_reg(TCS_ADDR, TCS_COMMAND | PERS, &param, sizeof(param));
  param = ENABLE_PON | ENABLE_AEN | ENABLE_AIEN; // PON & AEN
  i2c_wr_reg(TCS_ADDR, TCS_COMMAND | ENABLE, &param, sizeof(param));
  color_sensor_present = 1;
  return 0;
}

color_t check_color(void) {

  if (color_sensor_present == 0) return black; // я слеп.

  param = TCS_COMMAND | TCS_SF;
  while (i2cTransferDone != i2c_wr(TCS_ADDR, &param, 1)) LaunchPad_Output1(RED|GREEN|BLUE);   // clear interrupt
  param = ENABLE_PON | ENABLE_AEN | ENABLE_AIEN; // PON & AEN
  while (i2cTransferDone != i2c_wr_reg(TCS_ADDR, TCS_COMMAND | ENABLE, &param, sizeof(param))) LaunchPad_Output1(RED|GREEN|BLUE);
  do {
      param = 0x00;
      while (i2cTransferDone != i2c_rd_reg(TCS_ADDR, TCS_COMMAND | STATUS, &param, sizeof(param))) LaunchPad_Output1(RED|GREEN|BLUE);
  } while ((param & STATUS_AINT) == 0);
  while (i2cTransferDone != i2c_rd_reg(TCS_ADDR, TCS_COMMAND | TCS_INCREMENT | CDATAL, (uint8_t *) color_sensors, sizeof(color_sensors))) LaunchPad_Output1(RED|GREEN|BLUE);
  param = TCS_COMMAND | TCS_SF;
  while (i2cTransferDone != i2c_wr(TCS_ADDR, &param, 1)) LaunchPad_Output1(RED|GREEN|BLUE);   // clear interrupt

#ifdef DEBUG_COLOR_SENS
  if (log_array_ptr) {
      *log_array_ptr++ = color_sensors[0];
      *log_array_ptr++ = color_sensors[1];
      *log_array_ptr++ = color_sensors[2];
      *log_array_ptr++ = color_sensors[3];
      if (log_array_ptr > (uint16_t *)&log_array[1999][15]) log_array_ptr = 0;
  }
#endif

  color_sensors[1] = (color_sensors[1] * data.color_red_thr) >> 10;
  color_sensors[2] = (color_sensors[2] * data.color_green_thr) >> 10;
  color_sensors[3] = (color_sensors[3] * data.color_blue_thr) >> 10;

  if (color_sensors[0] < data.color_threshold) {
      return black;
  } else {
      unsigned int i, invsaturation, minRGB=0xFFFFFFFFul;
      int hueX, hueY;
      for (i=1; i<4; i++) {
          if (minRGB > color_sensors[i]) minRGB = color_sensors[i];
      }
      invsaturation = 3*1024 * minRGB / (color_sensors[1]+color_sensors[2]+color_sensors[3]);
      if ((int)invsaturation > data.color_saturation) return white;

      hueY = 3*(color_sensors[2] - color_sensors[3]);
      hueX = 2*color_sensors[1] - color_sensors[2] - color_sensors[3];

#define ABS(x)  (((x) < 0) ? (-(x)) : (x))
      if (ABS(hueX) > ABS(hueY)) {
          // Red or Cyan:
          if (hueX < 0)       return cyan;
          else 				return red;
      } else {
          if (hueX < 0) {
              if (hueY < 0)   return blue;
              else 			return green;
          } else {
              if (hueY < 0)   return magenta;
              else 			return yellow;
          }
      }
  }
}


#include "keyboard.h"
#include "LaunchPad.h"
#include "display.h"

void TestColor(void) {
  uint16_t color_array[4] = {0, 0, 0, 0};
  unsigned int i;
  color_t field_test_color;

  ColorSensorTestHSI(color_array, 1);

#ifdef DEBUG_COLOR_SENS
  log_array_ptr = (uint16_t *)&log_array[0][0];
  LaunchPad_Output1(GREEN);
#endif

  while (kbdread() != KEY_DOWN) {
      field_test_color = check_color();
#ifdef DEBUG_COLOR_SENS
      if (log_array_ptr == 0) LaunchPad_Output1(RED);
#endif
      //        copy_data_dma((uint8_t *)color_sensors, (uint8_t *)color_array, sizeof(color_array));
      switch (field_test_color) {
        case red:       LaunchPad_Output(RED); break;
        case green:     LaunchPad_Output(GREEN); break;
        case blue:      LaunchPad_Output(BLUE); break;
        case yellow:    LaunchPad_Output(RED  | GREEN); break;
        case cyan:      LaunchPad_Output(BLUE | GREEN); break;
        case magenta:   LaunchPad_Output(BLUE | RED); break;
        case white:     LaunchPad_Output(BLUE | RED | GREEN); break;
        case black:     LaunchPad_Output(0x00); break;
      }
      for (i=0; i<4; i++) {
          color_array[i] = color_sensors[i];
      }
      //    while(dma_copy_busy) WaitForInterrupt();
      ColorSensorTestHSI(color_array, 0);
  }
  LaunchPad_Output(0);
}
