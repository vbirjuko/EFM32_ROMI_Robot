/*
 * display_ssd1327.c
 *
 *  Created on: 24 апр. 2022 г.
 *      Author: wl
 */

#include "Adafruit_ssd1327.h"
#include "spi_drv.h"
#include "LaunchPad.h"
#include "LE_timer0.h"
#include "fonts.h"
#include "display.h"

void update_display_1327(void);
void squareXY_1327(unsigned int startX, unsigned int startY, unsigned int endX, unsigned int endY, unsigned int fill);
void lineXY_1327(unsigned int startX, unsigned int startY, unsigned int endX, unsigned int endY, unsigned int fill);
void show_number_1327(unsigned int number, int decimal);

#include "em_device.h"
#include "em_ldma.h"
#include "em_gpio.h"

const unsigned char start_col[] = {SSD1327_SETCOLUMN, 0, 63, SSD1327_SETROW, 0, 127};
static unsigned char oled_buffer[128*128/2];
static unsigned char videobuff[8];


LDMA_TransferCfg_t ssd1327_txferCfg = LDMA_TRANSFER_CFG_PERIPHERAL(ldmaPeripheralSignal_USART3_TXBL);

LDMA_Descriptor_t ssd1327_tx_descriptor[] = {
    LDMA_DESCRIPTOR_LINKREL_SYNC(0, 0x80, 0x00, 0x00, 1),
    LDMA_DESCRIPTOR_LINKREL_WRITE(USART_CMD_RXDIS, &USART3->CMD, 1),
    LDMA_DESCRIPTOR_LINKREL_WRITE(USART_ROUTELOC0_CLKLOC_LOC0 | USART_ROUTELOC0_CSLOC_LOC0 |
                                  USART_ROUTELOC0_RXLOC_LOC0 | USART_ROUTELOC0_TXLOC_LOC0,
                                  &USART3->ROUTELOC0, 1),
    LDMA_DESCRIPTOR_LINKREL_WRITE(0, &GPIO->P[gpioPortA].DOUT, 1),
    LDMA_DESCRIPTOR_LINKREL_M2P_BYTE(start_col, &USART3->TXDATA, sizeof(start_col), 1),
    LDMA_DESCRIPTOR_LINKREL_SYNC(0, 0x80, 0x80, 0x80, 1),
    LDMA_DESCRIPTOR_LINKREL_WRITE(USART_CMD_RXDIS, &USART3->CMD, 1),
    LDMA_DESCRIPTOR_LINKREL_WRITE(USART_ROUTELOC0_CLKLOC_LOC0 | USART_ROUTELOC0_CSLOC_LOC0 |
                                  USART_ROUTELOC0_RXLOC_LOC0 | USART_ROUTELOC0_TXLOC_LOC0,
                                  &USART3->ROUTELOC0, 1),
    LDMA_DESCRIPTOR_LINKREL_WRITE(0, &GPIO->P[gpioPortA].DOUT, 1),
    LDMA_DESCRIPTOR_LINKREL_M2P_BYTE(&oled_buffer[0], &USART3->TXDATA, sizeof(oled_buffer)/4, 1),
    LDMA_DESCRIPTOR_LINKREL_M2P_BYTE(&oled_buffer[2048], &USART3->TXDATA, sizeof(oled_buffer)/4, 1),
    LDMA_DESCRIPTOR_LINKREL_M2P_BYTE(&oled_buffer[4096], &USART3->TXDATA, sizeof(oled_buffer)/4, 1),
    LDMA_DESCRIPTOR_LINKREL_M2P_BYTE(&oled_buffer[6144], &USART3->TXDATA, sizeof(oled_buffer)/4, 1),
    LDMA_DESCRIPTOR_SINGLE_WRITE(0, &tx_busy)
};

/*********************************************************************
This is a library for our grayscale OLEDs based on SSD1327 drivers
  Pick one up today in the adafruit shop!
  ------> https://www.adafruit.com/products/4741
These displays use I2C or SPI to communicate
Adafruit invests time and resources providing this open source code,
please support Adafruit and open-source hardware by purchasing
products from Adafruit!
Written by Limor Fried/Ladyada  for Adafruit Industries.
BSD license, check license.txt for more information
All text above, and the splash screen below must be included in any
redistribution
*********************************************************************/

// Init sequence, make sure its under 32 bytes, or split into multiples!
  static const uint8_t init_128x128[] = {
      // Init sequence for 128x32 OLED module
      SSD1327_DISPLAYOFF, // 0xAE
      SSD1327_SETCONTRAST,
      0x80,             // 0x81, 0x80
      SSD1327_SEGREMAP, // 0xA0 0x53
      0x51, // remap memory, odd even columns, com flip and column swap
      SSD1327_SETSTARTLINE,
      0x00, // 0xA1, 0x00
      SSD1327_SETDISPLAYOFFSET,
      0x00, // 0xA2, 0x00
      SSD1327_DISPLAYALLOFF, SSD1327_SETMULTIPLEX,
      0x7F, // 0xA8, 0x7F (1/64)
      SSD1327_PHASELEN,
      0x11, // 0xB1, 0x11
      /*
      SSD1327_GRAYTABLE,
      0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
      0x07, 0x08, 0x10, 0x18, 0x20, 0x2f, 0x38, 0x3f,
      */
      SSD1327_DCLK,
      0x00, // 0xb3, 0x00 (100hz)
      SSD1327_REGULATOR,
      0x01, // 0xAB, 0x01
      SSD1327_PRECHARGE2,
      0x04, // 0xB6, 0x04
      SSD1327_SETVCOM,
      0x0F, // 0xBE, 0x0F
      SSD1327_PRECHARGE,
      0x08, // 0xBC, 0x08
      SSD1327_FUNCSELB,
      0x62, // 0xD5, 0x62
      SSD1327_CMDLOCK,
      0x12, // 0xFD, 0x12
      SSD1327_NORMALDISPLAY, SSD1327_DISPLAYON
  };

void oled_ssd1327_Init(void) {
  spi_oled_init();
  lcdwrite((uint8_t *)init_128x128, sizeof(init_128x128), lcdcommand);

  ssd1327_tx_descriptor[4].xfer.doneIfs = 0;
  ssd1327_tx_descriptor[9].xfer.doneIfs = 0;
  ssd1327_tx_descriptor[10].xfer.doneIfs = 0;
  ssd1327_tx_descriptor[11].xfer.doneIfs = 0;
  ssd1327_tx_descriptor[12].xfer.doneIfs = 0;
  ssd1327_tx_descriptor[13].xfer.doneIfs = 0;

  letimer_delay_ms(100);   // 100ms delay recommended

//  oled_command(SSD1327_DISPLAYON); // 0xaf
//  setContrast(0x2F);

  // memset(buffer, 0x81, _bpp * WIDTH * ((HEIGHT + 7) / 8));

  squareXY_1327(0, 0, 127, 127, 0);
  squareXY_1327(0, 0, 31, 31, 0x0F);
  squareXY_1327(64, 0, 95, 31, 0x0F);
  squareXY_1327(32, 32, 63, 63, 0x0F);
  squareXY_1327(96, 32, 127, 63, 0x0F);

  update_display_1327();

  letimer_delay_ms(2500);
  show_number_1327(4, 1);

  letimer_delay_ms(2500);
  lineXY_1327(0, 0, 127, 127,8);
  lineXY_1327(0,  127, 127, 0, 15);

  update_display_1327();
  return; // Success
}


void update_display_1327(void) {
  while (tx_busy) continue;

  tx_busy = 1;
  ssd1327_tx_descriptor[3].wri.immVal = launchpad_LED_state | (lcdcommand << 5)|(1 << 4)|(1 << 3)|(1 << 15);
  ssd1327_tx_descriptor[8].wri.immVal = launchpad_LED_state | (lcddata    << 5)|(1 << 4)|(1 << 3)|(1 << 15);

  LDMA_StartTransfer(LDMA_USART_OLED_CHANNEL, &ssd1327_txferCfg, (void*)&ssd1327_tx_descriptor);
}

void show_number_1327(unsigned int number, int decimal) {


    int digit, i, k, leadingzero, digitcount;
    static const int offset[] = {3, 23, 43, 0, 15, 30, 46, 0, 18, 36, 46, 0, 18, 28, 46, 0, 10, 28, 46};
    const int *offset_ptr;
    const unsigned int base[] = {0xffffffff, 1, 10, 100, 1000, 10000}, *divider;
    unsigned char *scrbuffer_ptr;
    unsigned int row_mask;

    if (decimal > 3) decimal = 0;

//    while (buffer_count) continue;  // если на экран передаётся буфер - подождём.

    if (number < 1000) {
        divider = base + 3;
//      number = (number % 1000);
        if (!decimal) {
          offset_ptr = &offset[0];
          digitcount = 3;
        } else  {
          offset_ptr = &offset[3] + decimal*4;
          digitcount = 4;
        }
    } else {
        divider = base + 4;
        number = (number % 10000);
        offset_ptr = &offset[3];
        digitcount = 4;
        decimal = 0;        // если больше 3 цифр - запятую не рисуем.
    }

    k = 0;
//    for (k = 0; k < digitcount; k++) {
    while (k < digitcount) {
        digit = number / *divider;
        number  %= *divider--;
        if (decimal && ((digitcount - 1 - decimal) == k)) videobuff[k++] = 10;
        videobuff[k++] = digit;
    }

    leadingzero = 1;
    for (k = 0; k < digitcount; k++, offset_ptr++) {
      if (videobuff[k]) leadingzero = 0;

      if (!leadingzero) {
#ifdef DMA2D
          while (dma_flag) continue;
//          LDMA_TransferCfg_t memTransfer = LDMA_TRANSFER_CFG_MEMORY_LOOP(
//                TRANSFER_HEIGHT-2);

          // First descriptor gets the absolute source and destination address
          descLink[0] = (LDMA_Descriptor_t)LDMA_DESCRIPTOR_LINKREL_M2M_BYTE(
              &Verdana36x64[videobuff[k]][0][0],    //SRC[SRC_ROW_INDEX][SRC_COL_INDEX],
              buffer + *offset_ptr,                 //DST[DST_ROW_INDEX][DST_COL_INDEX],
              TRANSFER_WIDTH,
              1);

          // Second descriptor uses relative addressing and performs looping
          descLink[1] = (LDMA_Descriptor_t)LDMA_DESCRIPTOR_LINKREL_M2M_BYTE(
              0, //(BUFFER_2D_WIDTH - TRANSFER_WIDTH),
              (BUFFER_2D_WIDTH - TRANSFER_WIDTH),
              TRANSFER_WIDTH,
              0);

          // Use relative addressing for source & destination, enable looping
          descLink[1].xfer.srcAddrMode = ldmaCtrlSrcAddrModeRel;
          descLink[1].xfer.dstAddrMode = ldmaCtrlDstAddrModeRel;
          descLink[1].xfer.decLoopCnt = 1;

          // Stop after looping
          descLink[1].xfer.link = 0;
          dma_flag = 1;
          LDMA_StartTransfer(LDMA_CHANNEL, (void*)&memTransfer, (void*)&descLink);
      }
      while(dma_flag) continue;
#else
        for (unsigned int j = 0; j < 8; j++) {
          static const unsigned char *ptr;
          row_mask = (1 << 0);
          for (unsigned int ii = 0; ii < 8; ii++) {
              scrbuffer_ptr = oled_buffer + (((j+5)*8+ii) * 64) + *offset_ptr;
              ptr = &Verdana36x64[videobuff[k]][j][0];
              for (i = 0; i < 18; i++){
                  *scrbuffer_ptr  = (*ptr++ & row_mask) ? (SSD1327_WHITE << 4) : (SSD1327_BLACK << 4);
                  *scrbuffer_ptr |= (*ptr++ & row_mask) ? (SSD1327_WHITE << 0) : (SSD1327_BLACK << 0);
                  scrbuffer_ptr++;
              }
              row_mask <<= 1;
          }
        }
      }
#endif
    }
    update_display_1327();
}

void squareXY_1327(unsigned int startX, unsigned int startY, unsigned int endX, unsigned int endY, unsigned int fill) {
  unsigned int x, y;
  unsigned char * scrbuffer_ptr;

  if ((startX > 128-1) || (endX > 128-1) || (startY > 128-1) || (endY > 128-1)) return;
  if (startX > endX) {
      startX = startX + endX;   // A + B = AB  =>  AB B
      endX = startX - endX;     // AB - B = A  =>  AB A
      startX = startX - endX;   // AB - A = B  =>  B  A
  }
  if (startY > endY) {
      startY = startY + endY;   // A + B = AB  =>  AB B
      endY   = startY - endY;   // AB - B = A  =>  AB A
      startY = startY - endY;   // AB - A = B  =>  B  A
  }

  for (y = (startY); y <= ((endY)); y++) {
      scrbuffer_ptr = oled_buffer + y * 64 + startX/2;
      for (x = startX; x <= endX; x++) {
          if (x & 1) {
              *scrbuffer_ptr &= 0xF0;
              *scrbuffer_ptr |= fill & 0x0F;
              scrbuffer_ptr++;
          } else {
              *scrbuffer_ptr &= 0x0F;
              *scrbuffer_ptr |= (fill & 0x0F) << 4;
          }
      }
  }
}

#define ABS(x)  (((x) < 0) ? (-(x)) : (x))

void lineXY_1327(unsigned int startX, unsigned int startY, unsigned int endX, unsigned int endY, unsigned int fill) {
  int deltaX, deltaY, error, deltaerr, dirY;
  unsigned int X, Y;
  if ((startX > 128-1) || (endX > 128-1) || (startY > 128-1) || (endY > 128-1)) return;

  deltaX = ABS((int)endX - (int)startX);
  deltaY = ABS((int)endY - (int)startY);

  if (((deltaX >= deltaY) && (startX > endX)) || ((deltaX < deltaY) && (startY > endY))) {
      startX = startX + endX;   // A + B = AB  =>  AB B
      endX = startX - endX;     // AB - B = A  =>  AB A
      startX = startX - endX;   // AB - A = B  =>  B  A
      startY = startY + endY;   // A + B = AB  =>  AB B
      endY   = startY - endY;   // AB - B = A  =>  AB A
      startY = startY - endY;   // AB - A = B  =>  B  A
  }

  error = 0;
  if (deltaX >= deltaY) {
      deltaerr = deltaY + 1;
      Y = startY;
      dirY = (((int)endY - (int)startY) > 0) ? 1 : -1;
      for (X = startX; X <= endX; X++) {
          //   plot (X, Y);
          if (X & 0x01) {
              *(oled_buffer + (Y*64)+ X/2) &= 0xF0;
              *(oled_buffer + (Y*64)+ X/2) |= fill & 0x0F;
          } else {
              *(oled_buffer + (Y*64)+ X/2) &= 0x0F;
              *(oled_buffer + (Y*64)+ X/2) |= (fill & 0x0F) << 4;
          }
          error = error + deltaerr;
          if (error >= deltaX + 1) {
              Y += dirY;
              error -= deltaX + 1;
          }
      }
  } else {
      deltaerr = deltaX + 1;
      X = startX;
      dirY = (((int)endX - (int)startX) > 0) ? 1 : -1;
      for (Y = startY; Y <= endY; Y++) {
          //   plot (X, Y);
          if (X & 0x01) {
              *(oled_buffer + (Y*64)+ X/2) &= 0xF0;
              *(oled_buffer + (Y*64)+ X/2) |= fill & 0x0F;
          } else {
              *(oled_buffer + (Y*64)+ X/2) &= 0x0F;
              *(oled_buffer + (Y*64)+ X/2) |= (fill & 0x0F) << 4;
          }
          error = error + deltaerr;
          if (error >= deltaY + 1) {
              X += dirY;
              error -= deltaY + 1;
          }
      }
  }
}
