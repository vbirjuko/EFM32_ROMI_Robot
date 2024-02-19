//#include "msp.h"
#include "resources.h"

#ifdef SSD1306
#include <Adafruit_SSD1306.h>
#elif defined SH1106
#include "Adafruit_SH1106.h"
#endif
#include "keyboard.h"
#include "configure.h"
#include "crc.h"
#include "display.h"
#include "fonts.h"
#include "spi_drv.h"
#include "Reflectance.h"
#include "math.h"

#define 	INVERT	(1u<<7)
#ifdef SSD1306
#define ROWS    (SSD1306_LCDHEIGHT/8)
#elif defined SH1106
#define ROWS    (SH1106_LCDHEIGHT/8)
#endif
static unsigned char videobuffer[ROWS*16];
static const  unsigned int base[] = {0, 1, 10, 100, 1000, 10000}, *divider;
const char hextable[16] = "0123456789ABCDEF";

typedef enum {select_mode, edit_mode} mode_t;
typedef void (*func_ptr)(void);

eedata_t data;
unsigned int config_validate(void);

unsigned int do_menu(menuitem_t* item, unsigned int last_menu_item) {
  unsigned int update;
  int i, k;
  unsigned int j;
  int *stru_ptr;
  uint8_t *stru8_ptr;
  int leadingzero, select=0, editnum=0, line_offset=0;
  mode_t mode = select_mode;
  const unsigned char *str_ptr, speedtable[][4] = {"stop", "slow", "fast", "max "},
      directiontable[][5] = {"Forw ", "Right", "Back ", "Left "};
  unsigned char *txt_ptr, *vbuff_ptr;
  unsigned int number, keyb;


  update = 1;

  while(1) {
      if (update) {
          update = 0;
          // рисуем базовый текст
          for (i=0; i<ROWS; i++) {
              vbuff_ptr = videobuffer + i*16;
              str_ptr = (item[line_offset+i].todisplay);
              for (j=0; j<16; j++) *vbuff_ptr++ = *str_ptr++;
              vbuff_ptr -= 5;
              // в зависимости от типа данных выводим значение
              switch (item[line_offset+i].datatype) {
                case decimal:
                  number =  *((unsigned int*)item[line_offset+i].variable);
                  divider = base + 5;
                  // если число слишком большое переделываем его в формат '*9999'
                  if (number > 99999L) number = 2509999;
                  leadingzero = 1;
                  for (j = 11; j < 16; j++) {
                      *vbuff_ptr = '0';
                      while (number >= *divider) {
                          number -= *divider;
                          (*vbuff_ptr)++;
                          leadingzero = 0;
                      }
                      if (leadingzero && (*vbuff_ptr == '0')) *vbuff_ptr = ' ';
                      divider--; vbuff_ptr++;
                      //        if (*divider == 0) break;
                  }
                  break;

                case hex:
                  number =  *((unsigned int*)item[line_offset+i].variable);
                  *vbuff_ptr++ = ' ';
                  *vbuff_ptr++ = hextable[(number & 0xF000u) >> 12];
                  *vbuff_ptr++ = hextable[(number & 0x0F00u) >> 8];
                  *vbuff_ptr++ = hextable[(number & 0x00F0u) >> 4];
                  *vbuff_ptr++ = hextable[(number & 0x000Fu) >> 0];
                  break;

                case speed_sel:
                  number =  *((unsigned int*)item[line_offset+i].variable);
                  *vbuff_ptr++ = directiontable[(number >> 2)  & 0x03][0];
                  *vbuff_ptr++ = speedtable[number & 3][0];
                  *vbuff_ptr++ = speedtable[number & 3][1];
                  *vbuff_ptr++ = speedtable[number & 3][2];
                  *vbuff_ptr++ = speedtable[number & 3][3];
                  break;

                case direction_sel:
                  number =  *((uint8_t*)item[line_offset+i].variable);
                  *vbuff_ptr++ = directiontable[number & 3][0];
                  *vbuff_ptr++ = directiontable[number & 3][1];
                  *vbuff_ptr++ = directiontable[number & 3][2];
                  *vbuff_ptr++ = directiontable[number & 3][3];
                  *vbuff_ptr++ = directiontable[number & 3][4];
                  break;

                case hex32:
                  number =  *((unsigned int*)item[line_offset+i].variable);
                  vbuff_ptr -= 3;
                  *vbuff_ptr++ = hextable[(number & 0xF0000000u) >> 28];
                  *vbuff_ptr++ = hextable[(number & 0x0F000000u) >> 24];
                  *vbuff_ptr++ = hextable[(number & 0x00F00000u) >> 20];
                  *vbuff_ptr++ = hextable[(number & 0x000F0000u) >> 16];
                  *vbuff_ptr++ = hextable[(number & 0x0000F000u) >> 12];
                  *vbuff_ptr++ = hextable[(number & 0x00000F00u) >> 8];
                  *vbuff_ptr++ = hextable[(number & 0x000000F0u) >> 4];
                  *vbuff_ptr++ = hextable[(number & 0x0000000Fu) >> 0];
                  break;

                case none:
                default:
                  //            for (j = 11; j < 16; j++) {
                  //              *vbuff_ptr++ = ' ';
                  //            }
                  break;

              }
              // выделяем инверсией выбранную строку
              if ((select-line_offset) == i) {
                  unsigned int sel_length;
                  vbuff_ptr = videobuffer + i*16;
                  switch (item[select].datatype) {
                    case decimal:
                    case hex:
                    case direction_sel:
                    case speed_sel:
                      sel_length = 11; break;
                    case hex32:
                      sel_length = 8; break;
                    default: sel_length = 16;
                  }
                  for (j=0; j<sel_length; j++)  *vbuff_ptr++ |= INVERT;
                  // выделяем инверсией редактируемый знак
                  if (mode == edit_mode) {
                      switch (item[select].datatype) {
                        case speed_sel:
                          if (editnum == 1) {
                              *(vbuff_ptr+4) |= INVERT;
                              *(vbuff_ptr+3) |= INVERT;
                              *(vbuff_ptr+2) |= INVERT;
                              *(vbuff_ptr+1) |= INVERT;
                          } else {
                              *(vbuff_ptr+0) |= INVERT;
                          }
                          break;

                        case direction_sel:
                          *(vbuff_ptr+4) |= INVERT;
                          *(vbuff_ptr+3) |= INVERT;
                          *(vbuff_ptr+2) |= INVERT;
                          *(vbuff_ptr+1) |= INVERT;
                          *(vbuff_ptr+0) |= INVERT;
                          break;

                        case hex32:
                          *(vbuff_ptr+8-editnum) |= INVERT;
                          break;

                        default:
                          *(vbuff_ptr+5-editnum) |= INVERT;
                          break;
                      }
                  }
              }
          }

          // Преобразовываем символы в графический образ.
          txt_ptr = buffer;
          vbuff_ptr = videobuffer;
          for (k=0; k < ROWS; k++) { // знакоряд
              for (j=0; j<16;  j++) { // ряд в знаке
                  for (i=0; i<8; i++) {  // символ в ряду.
                      *txt_ptr++ = ASCIITAB[*vbuff_ptr & ~INVERT][i] ^ ((*vbuff_ptr & INVERT) ? 0xff : 0x00);
                  };
                  ++vbuff_ptr;
              };
          };

          // засылаем картинку в дисплей
          update_display();
      }

      // Опрос клавиатуры и реакция на кнопки
      keyb = kbdread();
      switch (mode) {
        case select_mode:
          switch (keyb) {
            case KEY_UP:
//            case KEY_LEFT:
              if (--select < 0) select=0;
              if ((select - line_offset) < 0) line_offset = select;
              update = 1;
              break;

            case KEY_DOWN:
//            case KEY_RIGHT:
              if (++select > (int)last_menu_item) select = last_menu_item;
              if ((select - line_offset) >= ROWS) line_offset = select - (ROWS - 1);
              update = 1;
              break;

            case KEY_SET:
//            case KEY_LEFT | KEY_RIGHT:

              switch (item[select].datatype) {
                case decimal:
                  mode = edit_mode;
                  editnum = 5; break;
                case hex:
                  mode = edit_mode;
                  editnum = 4; break;
                case hex32:
                  mode = edit_mode;
                  editnum = 8; break;
                case speed_sel:
                  mode = edit_mode;
                  editnum = 2; break;
                case direction_sel:
                  mode = edit_mode;
                  editnum = 1; break;
                case execute:
                  ((func_ptr)item[select].variable)();
                  break;
                case none:
                  return select;
              }
              update = 1;
              break;

          }
          break;

            case edit_mode:
              switch (keyb) {
                case KEY_UP:
                case KEY_UP | HOLDED:
//                case KEY_LEFT:
                update = 1;
                stru_ptr = item[select].variable;// &data->threshold + select;
                stru8_ptr = item[select].variable;
                switch (item[select].datatype) {
                  case decimal:
                    *stru_ptr += base[editnum];
                    if ((*stru_ptr > 65535) || (*stru_ptr < 0)) *stru_ptr = 65535;
                    break;

                  case hex:
                    *stru_ptr += 1 << (editnum-1) * 4;
                    *stru_ptr &= 0xFFFFu;
                    break;

                  case hex32:
                    *stru_ptr += 1 << (editnum-1) * 4;
                    break;

                  case speed_sel:
                    if (editnum == 1) {
                        *stru_ptr = ((*stru_ptr + 1) & 0x03) | (*stru_ptr & ~(0x03));
                        *stru_ptr &= 0x0F;
                    } else {
                        *stru_ptr += 4;
                        *stru_ptr &= 0x0F;
                    }
                    break;

                  case direction_sel:
                    *stru8_ptr += 1;
                    *stru8_ptr &= 0x03;
                    break;

                  default:
                    break;
                }
                break;

                  case KEY_DOWN:
                  case KEY_DOWN | HOLDED:
//                  case KEY_RIGHT:
                  update = 1;

                  stru_ptr = item[select].variable; // &data->threshold + select;
                  stru8_ptr = item[select].variable;
                  switch (item[select].datatype) {
                    case decimal:
                      if (*stru_ptr - (int)base[editnum] >= 0) *stru_ptr -= base[editnum];
                      break;

                    case hex:
                      *stru_ptr -= 1 << (editnum-1) * 4;
                      *stru_ptr &= 0xFFFFu;
                      break;

                    case hex32:
                      *stru_ptr -= 1 << (editnum-1) * 4;
                      break;

                    case speed_sel:
                      if (editnum == 1) {
                          *stru_ptr = ((*stru_ptr - 1) & 0x03) | (*stru_ptr & ~(0x03));
                          *stru_ptr &= 0x0F;
                      } else {
                          *stru_ptr -= 4;
                          *stru_ptr &= 0x0F;
                      }
                      break;

                    case direction_sel:
                      *stru8_ptr -= 1;
                      *stru8_ptr &= 0x03;
                      break;

                    default:
                      break;
                  }
                  break;

                    case KEY_SET:
//                    case KEY_LEFT | KEY_RIGHT:
                      update = 1;
                      if (--editnum == 0) {
                          mode = select_mode;
                      }
                      break;
              }
              break;
      }
  }
}

void edit_path(void) {
  static menuitem_t segments[MAX_MAP_SIZE * 2];
  unsigned int i, j, num, leadingzero,menu_length, *length_ptr;
  char *str_ptr, exit_str[] = "Save & Exit     ", segm_str[] = "  0:     0 xxxxx";
  uint8_t *turn_ptr;

  turn_ptr = data.path;
  length_ptr = (unsigned int*)data.length;
  menu_length = data.pathlength + 1;
  if (menu_length < 7) menu_length = 7;
  segments[0].datatype = none;
  segments[0].variable = 0;
  str_ptr = exit_str;
  for (i = 0; i < 16; i++) {
      segments[0].todisplay[i] = *str_ptr++;
  }
  for (i = 1; i <= menu_length; i++) {
      segments[i].datatype = direction_sel;
      segments[i].variable = turn_ptr++;
      str_ptr = segm_str;
      for ( j = 0; j < 16; j++) {
          segments[i].todisplay[j] = *str_ptr++;
      }
      num = i; j = 0;
      leadingzero = 0;
      divider = base + 3;
      while (*divider){
          if ((leadingzero != 0) || ((num / *divider) != 0)) {
              leadingzero = 1;
              segments[i].todisplay[j] = '0' + (num / *divider);
          }
          j++; num %= *divider;  divider--;
      }
      num = *length_ptr++; j = 5;
      leadingzero = 0;
      divider = base + 5;
      while (*divider){
          if ((leadingzero != 0) || ((num / *divider) != 0)) {
              leadingzero = 1;
              segments[i].todisplay[j] = '0' + (num / *divider);
          }
          j++; num %= *divider;  divider--;
      }
  }
  do_menu((menuitem_t *)segments, menu_length);
}

void Configure(void) {
  const menuitem_t menu_item[] = {
      {"Edit path       ", &edit_path,            execute},
      {"PathLength xxxxx", &data.pathlength,      decimal},
      {"SQ init HHHHHHHH", &data.sq_init,         hex32},
      {"RunNumber  xxxxx", &data.runnumber,       decimal},
      {"Map size   xxxxx", &data.map_size,        decimal},
      {"StartPoint xxxxx", &data.green_cell_nr,   decimal},
      {"End Point  xxxxx", &data.red_cell_nr,     decimal},
      {"Save&Exit       ", 0,                     none},
      {"Threshold  xxxxx", &data.threshold,       decimal},
      {"X_offset   xxxxx", &data.x_offset,        decimal},
      {"Y_slope    xxxxx", &data.y_slope,         decimal},
      {"IR level   xxxxx", &data.ir_led_level,    decimal},
      {"On Way     xxxxx", &data.on_way,          decimal},
      {"MAX motor  xxxxx", &data.maxmotor,        decimal},
      {"MAX speed  xxxxx", &data.maxspeed,        decimal},
      {"MIN speed  xxxxx", &data.minspeed,        decimal},
      {"Turn speed xxxxx", &data.turnspeed,       decimal},
      {"Accelerat  xxxxx", &data.acceleration,    decimal},
      {"K*error    xxxxx", &data.k_error,         decimal},
      {"K*de/dt    xxxxx", &data.k_diff,          decimal},
      {"Kintegral  xxxxx", &data.k_integral,       decimal},
      {"Left Hand  xxxxx", &data.lefthand,        decimal},
      {"Color red  xxxxx", &data.color_red_thr,   decimal},
      {"Color greenxxxxx", &data.color_green_thr, decimal},
      {"Color blue xxxxx", &data.color_blue_thr,  decimal},
      {"Color thre xxxxx", &data.color_threshold, decimal},
      {"Color sat  xxxxx", &data.color_saturation,decimal},
      {"DstToleran xxxxx", &data.tolerance,     decimal},
      {"Guard Dist xxxxx", &data.guarddist,     decimal},
      {"SensorOffs xxxxx", &data.sensor_offset,   decimal},
      {"IgnoreErrorxxxxx", &data.ignore_coordinate_error, decimal},
      {"Brake Path xxxxx", &data.brake_path,      decimal},
      {"MotorKprop xxxxx", &data.motor_Kprop,     decimal},
      {"MotorKint  xxxxx", &data.motor_Kint,      decimal},
      {"TimeToRun  xxxxx", &data.timetorun,     decimal},
      {"Turn cost  xxxxx", &data.turncost,      decimal},
      {"Cross cost xxxxx", &data.crosscost,     decimal},
      {"Step cost  xxxxx", &data.stepcost,            decimal},
      {"Str Accel  xxxxx", &data.str_accel,           decimal},
      {"Cell step  xxxxx", &data.cell_step,              decimal},
      {"Red Wall   xxxxx", &data.red_wall,      decimal},
      {"Green Wall xxxxx", &data.green_wall,      decimal},
      {"Voltage Calxxxxx", &data.volt_calibr,     decimal},
      {"LowBat Lvl xxxxx", &data.low_battery_level, decimal},
      {"Edit path       ", &edit_path,            execute},
      {"Save&Exit       ", 0,                     none}
  };

  //  Motor_Speed(0, 0);
  do_menu((menuitem_t*) menu_item, ((sizeof(menu_item)/sizeof(menu_item[0]))-1));
  //  data_ptr = (uint32_t*) &data;
  unsigned int validation_error_code = (config_validate());
  if (validation_error_code  & 0x01) {
      squareXY(0, 0, 127, 63, 0);
      update_display();
      putstr(0, 2, "data validation", 0);
      putstr(0, 3, "Cell step/Guard", 0);
      putstr(0, 4, "    error.     ", 0);
      putstr(0, 5, "  Corrected.   ", 0);
      while (kbdread() != KEY_DOWN) continue;
  }
  if (validation_error_code & 0x02) {
      squareXY(0, 0, 127, 63, 0);
      update_display();
      putstr(0, 3, "data validation", 0);
      putstr(0, 4, "  Vbat error.  ", 0);
      while (kbdread() != KEY_DOWN) continue;
  }
  if (validation_error_code & 0x04) {
      squareXY(0, 0, 127, 63, 0);
      update_display();
      putstr(0, 2, "data validation", 0);
      putstr(0, 3, "Min speed/Offs.", 0);
      putstr(0, 4, "    error.     ", 0);
      putstr(0, 5, "  Corrected.   ", 0);
      while (kbdread() != KEY_DOWN) continue;
  }

  data.crc32 = calc_crc32((uint8_t*)&data, sizeof(data)-4);
  spi_write_eeprom(EEPROM_COPY_ADDRESS, (uint8_t*) &data, sizeof(data));  // сначала записываем резерв
  spi_write_eeprom(EEPROM_CONFIG_ADDRESS, (uint8_t*) &data, sizeof(data));  // теперь, нормальный вариант.
  Reflectance_Init(data.threshold, data.ir_led_level);
  //  ADC0_InitSWTriggerCh21();
}

unsigned int config_validate(void) {
  unsigned int result = 0;
  if (data.cell_step) {
      if ((data.cell_step*3/4) < data.tolerance) {
          data.tolerance = data.cell_step*3/4;
          result |= (1 << 0);
      }
  }
  if (data.low_battery_level * data.volt_calibr >= 10000) {
      result |= (1 << 1);
  }
  if ((data.sensor_offset*100) < ((((long long)(data.minspeed*data.minspeed)*121)+(data.acceleration/2))/data.acceleration+12000)/24000) {
      data.minspeed = sqrt(data.sensor_offset*data.acceleration*2400000LL/121);
      result |= (1 << 2);
  }
  return result;
}
