//#include <stdlib.h>
#include <stdint.h>
#include "display.h"
#include "fonts.h"

#ifdef OLED_SPI
#include "spi_drv.h"
#endif
#ifdef OLED_I2C
#include "i2c_drv.h"
#endif

#ifdef SSD1306
#include "Adafruit_SSD1306.h"
#elif defined SH1106
#include "Adafruit_SH1106.h"
#endif

#include "math.h"
#include "Maze.h"
#include "display.h"
#include "resources.h"
#include "LaunchPad.h"
#include "profiler.h"

//#include "configure.h"
//#include "Clock.h"
//#include "dma.h"
//#include "Timer32.h"

volatile unsigned int disp_delay = 0;
unsigned char videobuff[8];

unsigned char buffer[LCDHEIGHT * LCDWIDTH / 8] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80,
	0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x80, 0x80, 0xC0, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xF8, 0xE0, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80,
	0x80, 0x80, 0x00, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00, 0xFF,
#if (LCDHEIGHT * LCDWIDTH > 96*16)
	0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x80, 0x80, 0x00, 0x00,
	0x80, 0xFF, 0xFF, 0x80, 0x80, 0x00, 0x80, 0x80, 0x00, 0x80, 0x80, 0x80, 0x80, 0x00, 0x80, 0x80,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x00, 0x00, 0x8C, 0x8E, 0x84, 0x00, 0x00, 0x80, 0xF8,
	0xF8, 0xF8, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xE0, 0xE0, 0xC0, 0x80,
	0x00, 0xE0, 0xFC, 0xFE, 0xFF, 0xFF, 0xFF, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFE, 0xFF, 0xC7, 0x01, 0x01,
	0x01, 0x01, 0x83, 0xFF, 0xFF, 0x00, 0x00, 0x7C, 0xFE, 0xC7, 0x01, 0x01, 0x01, 0x01, 0x83, 0xFF,
	0xFF, 0xFF, 0x00, 0x38, 0xFE, 0xC7, 0x83, 0x01, 0x01, 0x01, 0x83, 0xC7, 0xFF, 0xFF, 0x00, 0x00,
	0x01, 0xFF, 0xFF, 0x01, 0x01, 0x00, 0xFF, 0xFF, 0x07, 0x01, 0x01, 0x01, 0x00, 0x00, 0x7F, 0xFF,
	0x80, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x7F, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x01, 0xFF,
	0xFF, 0xFF, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x03, 0x0F, 0x3F, 0x7F, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xE7, 0xC7, 0xC7, 0x8F,
	0x8F, 0x9F, 0xBF, 0xFF, 0xFF, 0xC3, 0xC0, 0xF0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xFC, 0xFC,
	0xFC, 0xFC, 0xFC, 0xFC, 0xFC, 0xF8, 0xF8, 0xF0, 0xF0, 0xE0, 0xC0, 0x00, 0x01, 0x03, 0x03, 0x03,
	0x03, 0x03, 0x01, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x03, 0x03, 0x03, 0x01, 0x01,
	0x03, 0x01, 0x00, 0x00, 0x00, 0x01, 0x03, 0x03, 0x03, 0x03, 0x01, 0x01, 0x03, 0x03, 0x00, 0x00,
	0x00, 0x03, 0x03, 0x00, 0x00, 0x00, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
	0x03, 0x03, 0x03, 0x03, 0x03, 0x01, 0x00, 0x00, 0x00, 0x01, 0x03, 0x01, 0x00, 0x00, 0x00, 0x03,
	0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
#if (LCDHEIGHT == 64)
	0x00, 0x00, 0x00, 0x80, 0xC0, 0xE0, 0xF0, 0xF9, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x3F, 0x1F, 0x0F,
	0x87, 0xC7, 0xF7, 0xFF, 0xFF, 0x1F, 0x1F, 0x3D, 0xFC, 0xF8, 0xF8, 0xF8, 0xF8, 0x7C, 0x7D, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0x3F, 0x0F, 0x07, 0x00, 0x30, 0x30, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0xFE, 0xFE, 0xFC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0xC0, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x30, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0xC0, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0x7F, 0x3F, 0x1F,
	0x0F, 0x07, 0x1F, 0x7F, 0xFF, 0xFF, 0xF8, 0xF8, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xF8, 0xE0,
	0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFE, 0xFE, 0x00, 0x00,
	0x00, 0xFC, 0xFE, 0xFC, 0x0C, 0x06, 0x06, 0x0E, 0xFC, 0xF8, 0x00, 0x00, 0xF0, 0xF8, 0x1C, 0x0E,
	0x06, 0x06, 0x06, 0x0C, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0xFE, 0xFE, 0x00, 0x00, 0x00, 0x00, 0xFC,
	0xFE, 0xFC, 0x00, 0x18, 0x3C, 0x7E, 0x66, 0xE6, 0xCE, 0x84, 0x00, 0x00, 0x06, 0xFF, 0xFF, 0x06,
	0x06, 0xFC, 0xFE, 0xFC, 0x0C, 0x06, 0x06, 0x06, 0x00, 0x00, 0xFE, 0xFE, 0x00, 0x00, 0xC0, 0xF8,
	0xFC, 0x4E, 0x46, 0x46, 0x46, 0x4E, 0x7C, 0x78, 0x40, 0x18, 0x3C, 0x76, 0xE6, 0xCE, 0xCC, 0x80,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x01, 0x07, 0x0F, 0x1F, 0x1F, 0x3F, 0x3F, 0x3F, 0x3F, 0x1F, 0x0F, 0x03,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x0F, 0x00, 0x00,
	0x00, 0x0F, 0x0F, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x0F, 0x00, 0x00, 0x03, 0x07, 0x0E, 0x0C,
	0x18, 0x18, 0x0C, 0x06, 0x0F, 0x0F, 0x0F, 0x00, 0x00, 0x01, 0x0F, 0x0E, 0x0C, 0x18, 0x0C, 0x0F,
	0x07, 0x01, 0x00, 0x04, 0x0E, 0x0C, 0x18, 0x0C, 0x0F, 0x07, 0x00, 0x00, 0x00, 0x0F, 0x0F, 0x00,
	0x00, 0x0F, 0x0F, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x0F, 0x00, 0x00, 0x00, 0x07,
	0x07, 0x0C, 0x0C, 0x18, 0x1C, 0x0C, 0x06, 0x06, 0x00, 0x04, 0x0E, 0x0C, 0x18, 0x0C, 0x0F, 0x07,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
#endif
#endif
	};


void print_dump(char *pData) {
	int i, j, k;
	unsigned char *scrbuffer_ptr;
	
	scrbuffer_ptr = buffer;
	for (j=0; j<8; j++) {
		for (i=0; i<14; i++) {
			for (k=0; k<8; k++) {
				unsigned char show_data;
				show_data = *(pData + ((13-i)*4 + (j>>1) ));
				*scrbuffer_ptr++ = (j & 0x01) ? ((char) IMAGES[((j & 0x01) ?  (show_data & 0x0F) : (show_data >> 4))] [k]) >> 1 :
																		 ((char) IMAGES[((j & 0x01) ?  (show_data & 0x0F) : (show_data >> 4))] [k]);
			};
			*scrbuffer_ptr++ = 0;
		};
		for (i=0; i<2; i++) {
			*scrbuffer_ptr++ = 0;
		}
	}
	update_display();
}

void show_hystogram(uint8_t mask, uint8_t *photo_sensor, unsigned int threshold, int number) {
	int i, j;
	int divider = 1000;
	unsigned char *scrbuffer_ptr;
	
//	if (threshold > 99) threshold = 99;
//	if (number >  99999) number =  99999;
//	if (number < -99999) number = -99999;
	
	for (i=0; i<3; i++) videobuff[i] = 0x11;
	
	videobuff[0] =  threshold / 100;
	videobuff[1] = (threshold % 100) / 10;
	videobuff[2] =  threshold % 10;

	for (i=3; i<8; i++) videobuff[i] = 0;
	
	if (number < 0) {
		number = -number;
		videobuff[3] = 0x10;
	} else {
	    videobuff[3] = 0x11;
	}
	
	for (i = 4; i < 8; i++) {
		while (number >= divider) {
			number -= divider;
			videobuff[i]++;
		}
		divider /= 10;
	}
	
	scrbuffer_ptr = buffer;
	for (i=0; i<8 ; i++) {
		for (j=0; j < 116; j++) { 
			if (j < (photo_sensor[i]>>1)) {
				if (mask & (1<<i)) *scrbuffer_ptr = 0x7e;
				else 			   			*scrbuffer_ptr = (j & 0x01) ? 0x2a : 0x2a << 1;
			} else 							*scrbuffer_ptr = 0x00; 
			
			if (j == (int)(threshold>>4)) *scrbuffer_ptr |= 0xAA;   //   10101010
			scrbuffer_ptr++;
		}
		for (j=0; j<4; j++) {
			*scrbuffer_ptr++ = ((mask & (1<<i)) ? IMAGES[18][j] : IMAGES[17][j]);
		}
		for (j=0; j<8; j++) {
			*scrbuffer_ptr++ = IMAGES[videobuff[i]][j];
		}
	}
	update_display();
}
#define USEDMA
#include "em_device.h"
#include "em_ldma.h"
#include "em_gpio.h"


#ifdef SSD1306
static const unsigned char start_col[] = {SSD1306_COLUMNADDR, 0, SSD1306_LCDWIDTH-1, SSD1306_PAGEADDR, 0, 7};
LDMA_TransferCfg_t txferCfg = LDMA_TRANSFER_CFG_PERIPHERAL(ldmaPeripheralSignal_USART3_TXBL);

LDMA_Descriptor_t tx_descriptor[] = {
    LDMA_DESCRIPTOR_LINKREL_SYNC(0, 0x80, 0x00, 0x00, 1),
    LDMA_DESCRIPTOR_LINKREL_WRITE(USART_CMD_RXDIS, &USART3->CMD, 1),
    LDMA_DESCRIPTOR_LINKREL_WRITE(USART_ROUTELOC0_CLKLOC_LOC0 | USART_ROUTELOC0_CSLOC_LOC0 |
                                  USART_ROUTELOC0_RXLOC_LOC0 | USART_ROUTELOC0_TXLOC_LOC0,
                                  &USART3->ROUTELOC0, 1),
//    LDMA_DESCRIPTOR_LINKREL_WRITE(0, &GPIO->P[gpioPortA].DOUT, 1),
    LDMA_DESCRIPTOR_LINKREL_M2P_BYTE(start_col, &USART3->TXDATA, sizeof(start_col), 1),
    LDMA_DESCRIPTOR_LINKREL_SYNC(0, 0x80, 0x80, 0x80, 1),
    LDMA_DESCRIPTOR_LINKREL_WRITE(USART_CMD_RXDIS, &USART3->CMD, 1),
    LDMA_DESCRIPTOR_LINKREL_WRITE(USART_ROUTELOC0_CLKLOC_LOC0 | USART_ROUTELOC0_CSLOC_LOC0 |
                                  USART_ROUTELOC0_RXLOC_LOC0 | USART_ROUTELOC0_TXLOC_LOC0,
                                  &USART3->ROUTELOC0, 1),
//    LDMA_DESCRIPTOR_LINKREL_WRITE(0, &GPIO->P[gpioPortA].DOUT, 1),
    LDMA_DESCRIPTOR_LINKREL_WRITE(1 << 5, &GPIO->P[gpioPortA].DOUTTGL, 1),
    LDMA_DESCRIPTOR_LINKREL_M2P_BYTE(buffer, &USART3->TXDATA, sizeof(buffer), 1),
    LDMA_DESCRIPTOR_SINGLE_WRITE(0, &tx_busy)
};
#elif defined(SH1106)
static unsigned char start_col[8][3] = {
    {SH1106_SETLOWCOLUMN | 2, SH1106_SETHIGHCOLUMN, SH1106_PAGEADDR | 0},
    {SH1106_SETLOWCOLUMN | 2, SH1106_SETHIGHCOLUMN, SH1106_PAGEADDR | 1},
    {SH1106_SETLOWCOLUMN | 2, SH1106_SETHIGHCOLUMN, SH1106_PAGEADDR | 2},
    {SH1106_SETLOWCOLUMN | 2, SH1106_SETHIGHCOLUMN, SH1106_PAGEADDR | 3},
    {SH1106_SETLOWCOLUMN | 2, SH1106_SETHIGHCOLUMN, SH1106_PAGEADDR | 4},
    {SH1106_SETLOWCOLUMN | 2, SH1106_SETHIGHCOLUMN, SH1106_PAGEADDR | 5},
    {SH1106_SETLOWCOLUMN | 2, SH1106_SETHIGHCOLUMN, SH1106_PAGEADDR | 6},
    {SH1106_SETLOWCOLUMN | 2, SH1106_SETHIGHCOLUMN, SH1106_PAGEADDR | 7},
};
LDMA_TransferCfg_t txferCfg = LDMA_TRANSFER_CFG_PERIPHERAL(ldmaPeripheralSignal_USART3_TXBL);

LDMA_Descriptor_t tx_descriptor[] = {
    LDMA_DESCRIPTOR_LINKREL_SYNC(0, 0x80, 0x00, 0x00, 1),
    LDMA_DESCRIPTOR_LINKREL_WRITE(USART_CMD_RXDIS, &USART3->CMD, 1),
    LDMA_DESCRIPTOR_LINKREL_WRITE(USART_ROUTELOC0_CLKLOC_LOC0 | USART_ROUTELOC0_CSLOC_LOC0 |
                                  USART_ROUTELOC0_RXLOC_LOC0 | USART_ROUTELOC0_TXLOC_LOC0,
                                  &USART3->ROUTELOC0, 1),
    LDMA_DESCRIPTOR_LINKREL_M2P_BYTE_NOIRQ(start_col[0], &USART3->TXDATA, sizeof(start_col[0]), 1),
    LDMA_DESCRIPTOR_LINKREL_SYNC(0, 0x80, 0x80, 0x80, 1),
    LDMA_DESCRIPTOR_LINKREL_WRITE(1 << 5, &GPIO->P[gpioPortA].DOUTTGL, 1),
    LDMA_DESCRIPTOR_LINKREL_M2P_BYTE_NOIRQ(buffer, &USART3->TXDATA, 128, 1),
// 1
    LDMA_DESCRIPTOR_LINKREL_SYNC(0, 0x80, 0x80, 0x80, 1),
    LDMA_DESCRIPTOR_LINKREL_WRITE(1 << 5, &GPIO->P[gpioPortA].DOUTTGL, 1),
    LDMA_DESCRIPTOR_LINKREL_M2P_BYTE_NOIRQ(start_col[1], &USART3->TXDATA, sizeof(start_col[1]), 1),
    LDMA_DESCRIPTOR_LINKREL_SYNC(0, 0x80, 0x80, 0x80, 1),
    LDMA_DESCRIPTOR_LINKREL_WRITE(1 << 5, &GPIO->P[gpioPortA].DOUTTGL, 1),
    LDMA_DESCRIPTOR_LINKREL_M2P_BYTE_NOIRQ(buffer+1*128, &USART3->TXDATA, 128, 1),
// 2
    LDMA_DESCRIPTOR_LINKREL_SYNC(0, 0x80, 0x80, 0x80, 1),
    LDMA_DESCRIPTOR_LINKREL_WRITE(1 << 5, &GPIO->P[gpioPortA].DOUTTGL, 1),
    LDMA_DESCRIPTOR_LINKREL_M2P_BYTE_NOIRQ(start_col[2], &USART3->TXDATA, sizeof(start_col[2]), 1),
    LDMA_DESCRIPTOR_LINKREL_SYNC(0, 0x80, 0x80, 0x80, 1),
    LDMA_DESCRIPTOR_LINKREL_WRITE(1 << 5, &GPIO->P[gpioPortA].DOUTTGL, 1),
    LDMA_DESCRIPTOR_LINKREL_M2P_BYTE_NOIRQ(buffer+2*128, &USART3->TXDATA, 128, 1),
// 3
    LDMA_DESCRIPTOR_LINKREL_SYNC(0, 0x80, 0x80, 0x80, 1),
    LDMA_DESCRIPTOR_LINKREL_WRITE(1 << 5, &GPIO->P[gpioPortA].DOUTTGL, 1),
    LDMA_DESCRIPTOR_LINKREL_M2P_BYTE_NOIRQ(start_col[3], &USART3->TXDATA, sizeof(start_col[3]), 1),
    LDMA_DESCRIPTOR_LINKREL_SYNC(0, 0x80, 0x80, 0x80, 1),
    LDMA_DESCRIPTOR_LINKREL_WRITE(1 << 5, &GPIO->P[gpioPortA].DOUTTGL, 1),
    LDMA_DESCRIPTOR_LINKREL_M2P_BYTE_NOIRQ(buffer+3*128, &USART3->TXDATA, 128, 1),
// 4
    LDMA_DESCRIPTOR_LINKREL_SYNC(0, 0x80, 0x80, 0x80, 1),
    LDMA_DESCRIPTOR_LINKREL_WRITE(1 << 5, &GPIO->P[gpioPortA].DOUTTGL, 1),
    LDMA_DESCRIPTOR_LINKREL_M2P_BYTE_NOIRQ(start_col[4], &USART3->TXDATA, sizeof(start_col[4]), 1),
    LDMA_DESCRIPTOR_LINKREL_SYNC(0, 0x80, 0x80, 0x80, 1),
    LDMA_DESCRIPTOR_LINKREL_WRITE(1 << 5, &GPIO->P[gpioPortA].DOUTTGL, 1),
    LDMA_DESCRIPTOR_LINKREL_M2P_BYTE_NOIRQ(buffer+4*128, &USART3->TXDATA, 128, 1),
// 5
    LDMA_DESCRIPTOR_LINKREL_SYNC(0, 0x80, 0x80, 0x80, 1),
    LDMA_DESCRIPTOR_LINKREL_WRITE(1 << 5, &GPIO->P[gpioPortA].DOUTTGL, 1),
    LDMA_DESCRIPTOR_LINKREL_M2P_BYTE_NOIRQ(start_col[5], &USART3->TXDATA, sizeof(start_col[5]), 1),
    LDMA_DESCRIPTOR_LINKREL_SYNC(0, 0x80, 0x80, 0x80, 1),
    LDMA_DESCRIPTOR_LINKREL_WRITE(1 << 5, &GPIO->P[gpioPortA].DOUTTGL, 1),
    LDMA_DESCRIPTOR_LINKREL_M2P_BYTE_NOIRQ(buffer+5*128, &USART3->TXDATA, 128, 1),
// 6
    LDMA_DESCRIPTOR_LINKREL_SYNC(0, 0x80, 0x80, 0x80, 1),
    LDMA_DESCRIPTOR_LINKREL_WRITE(1 << 5, &GPIO->P[gpioPortA].DOUTTGL, 1),
    LDMA_DESCRIPTOR_LINKREL_M2P_BYTE_NOIRQ(start_col[6], &USART3->TXDATA, sizeof(start_col[6]), 1),
    LDMA_DESCRIPTOR_LINKREL_SYNC(0, 0x80, 0x80, 0x80, 1),
    LDMA_DESCRIPTOR_LINKREL_WRITE(1 << 5, &GPIO->P[gpioPortA].DOUTTGL, 1),
    LDMA_DESCRIPTOR_LINKREL_M2P_BYTE_NOIRQ(buffer+6*128, &USART3->TXDATA, 128, 1),
// 7
    LDMA_DESCRIPTOR_LINKREL_SYNC(0, 0x80, 0x80, 0x80, 1),
    LDMA_DESCRIPTOR_LINKREL_WRITE(1 << 5, &GPIO->P[gpioPortA].DOUTTGL, 1),
    LDMA_DESCRIPTOR_LINKREL_M2P_BYTE_NOIRQ(start_col[7], &USART3->TXDATA, sizeof(start_col[7]), 1),
    LDMA_DESCRIPTOR_LINKREL_SYNC(0, 0x80, 0x80, 0x80, 1),
    LDMA_DESCRIPTOR_LINKREL_WRITE(1 << 5, &GPIO->P[gpioPortA].DOUTTGL, 1),
    LDMA_DESCRIPTOR_LINKREL_M2P_BYTE_NOIRQ(buffer+7*128, &USART3->TXDATA, 128, 1),
//
    LDMA_DESCRIPTOR_SINGLE_WRITE(0, &tx_busy)
};
#endif

void update_display(void) {
#ifdef SSD1306
#ifndef USEDMA
    unsigned char start_col[] = {SSD1306_COLUMNADDR, 0, SSD1306_LCDWIDTH-1, SSD1306_PAGEADDR, 0, 7};
    lcdwrite(start_col, sizeof(start_col), lcdcommand);
    lcdwrite(buffer, SSD1306_LCDHEIGHT * SSD1306_LCDWIDTH / 8, lcddata);
#else
    while (tx_busy) continue;

    tx_busy = 1;
    BUS_RegBitWrite(&GPIO->P[gpioPortA].DOUT, 5, lcdcommand);
//    tx_descriptor[3].wri.immVal = launchpad_LED_state | (lcdcommand << 5)|(1 << 4)|(1 << 3)| (0 << 8) | (1 << 15);
//    tx_descriptor[8].wri.immVal = launchpad_LED_state | (lcddata    << 5)|(1 << 4)|(1 << 3)| (0 << 8) | (1 << 15);
    tx_descriptor[3].xfer.doneIfs = 0;
    tx_descriptor[8].xfer.doneIfs = 0;
    LDMA_StartTransfer(LDMA_USART_OLED_CHANNEL, &txferCfg, (void*)&tx_descriptor);

#endif
#elif defined(SH1106)
#ifndef USEDMA
    unsigned char start_col[] = {SH1106_SETLOWCOLUMN | 2, SH1106_SETHIGHCOLUMN, SH1106_PAGEADDR};
		unsigned int i;

		for (i = 0; i < 8; i++) {
			start_col[2] = SH1106_PAGEADDR | i;
			lcdwrite(start_col, sizeof(start_col), lcdcommand);
			lcdwrite(buffer + 128*i, SH1106_LCDWIDTH, lcddata);
		}
#else
    while (tx_busy) continue;
    tx_busy = 1;
    BUS_RegBitWrite(&GPIO->P[gpioPortA].DOUT, 5, lcdcommand);
    LDMA_StartTransfer(LDMA_USART_OLED_CHANNEL, &txferCfg, (void*)&tx_descriptor);
#endif
#endif
//  i2c_wr_reg(SSD1306_I2C_ADDRESS, 0x00, start_col, sizeof(start_col));
//  i2c_wr_reg(SSD1306_I2C_ADDRESS, 0x40, buffer, 1024);
}

void display_init(void) {

#ifdef SSD1306
		static const unsigned char init_sequence[] = {
        // Init sequence for 128x64 OLED module
        SSD1306_DISPLAYOFF,
        SSD1306_SETDISPLAYCLOCKDIV,	0x80,
        SSD1306_SETMULTIPLEX,	0x3F,
        SSD1306_SETDISPLAYOFFSET,	0x0,
        SSD1306_SETSTARTLINE | 0x0,
        SSD1306_CHARGEPUMP,	0x14,
        SSD1306_MEMORYMODE,	0x00,
#ifndef UPSIDEDOWN
        SSD1306_SEGREMAP | 0x1,     // orientation
				SSD1306_COMSCANDEC,
#else
        SSD1306_SEGREMAP | 0x0,     // orientation
				SSD1306_COMSCANINC,
#endif
				SSD1306_SETCOMPINS,	0x12,
        SSD1306_SETCONTRAST,	0xCF,
        SSD1306_SETPRECHARGE,	0xF1,
        SSD1306_SETVCOMDETECT,	0x40,
        SSD1306_DISPLAYALLON_RESUME,
        SSD1306_NORMALDISPLAY,
        SSD1306_DISPLAYON //--turn on oled panel
    };
#elif defined(SH1106)
		// Init sequence for 128x64 OLED module
    static const unsigned char init_sequence[] = {
    SH1106_DISPLAYOFF,                    // 0xAE
    SH1106_SETDISPLAYCLOCKDIV, 0x80,			// the suggested ratio 0x80
    SH1106_SETMULTIPLEX, 0x3F,
    SH1106_SETDISPLAYOFFSET, 0x00,				// no offset
	
    SH1106_SETSTARTLINE | 0x0,						// line #0 0x40
 //   SH1106_CHARGEPUMP, 0x14,
//    SH1106_MEMORYMODE, 0x00,              // 0x0 act like ks0108
    SH1106_SEGREMAP | 0x1,
    SH1106_COMSCANDEC,
    SH1106_SETCOMPINS, 0x12,
    SH1106_SETCONTRAST, 0xCF,
    SH1106_SETPRECHARGE, 0xF1,
    SH1106_SETVCOMDETECT, 0x40,
    SH1106_DISPLAYALLON_RESUME,           // 0xA4
    SH1106_NORMALDISPLAY,                 // 0xA6
    SH1106_DISPLAYON //--turn on oled panel
		};
#endif
#ifdef OLED_SPI
    spi_oled_init();
#endif

    lcdwrite((unsigned char *)init_sequence, sizeof(init_sequence), lcdcommand);
    update_display();

    //	return i2c_wr_reg(SSD1306_I2C_ADDRESS, 0x00, init_sequence, sizeof(init_sequence));
}

#define DMA2D


#ifdef DMA2D
#define LDMA_CHANNEL  LDMA_MEM2MEM_CHANNEL
#define TRANSFER_WIDTH  36
#ifdef SSD1306
#define BUFFER_2D_WIDTH SSD1306_LCDWIDTH
#elif defined(SH1106)
#define BUFFER_2D_WIDTH SH1106_LCDWIDTH
#endif
#define TRANSFER_HEIGHT 8
LDMA_Descriptor_t descLink[2];
LDMA_TransferCfg_t memTransfer = LDMA_TRANSFER_CFG_MEMORY_LOOP(TRANSFER_HEIGHT-2);
#endif

void show_number(unsigned int number, int decimal) {


    int digit, i, k, leadingzero, digitcount;
		static const int offset[] = {6, 46, 86, 0, 30, 61, 92, 0, 36, 72, 92, 0, 36, 56, 92, 0, 20, 56, 92};
		const int *offset_ptr;
    const unsigned int base[] = {0xffffffff, 1, 10, 100, 1000, 10000}, *divider;
    unsigned char *scrbuffer_ptr;

		if (decimal > 3) decimal = 0;
		
//		while (buffer_count) continue;	// если на экран передаётся буфер - подождём.
		
    scrbuffer_ptr = buffer;
    for (i=0; i<(LCDHEIGHT * LCDWIDTH / 8); i++) {
        *scrbuffer_ptr++ = 0;
    }
    if (number < 1000) {
        divider = base + 3;
//      number = (number % 1000);
        if (!decimal) {
					offset_ptr = &offset[0];
					digitcount = 3;
				}	else	{
					offset_ptr = &offset[3] + decimal*4;
					digitcount = 4;
				}
    } else {
        divider = base + 4;
        number = (number % 10000);
        offset_ptr = &offset[3];
        digitcount = 4;
        decimal = 0;				// если больше 3 цифр - запятую не рисуем.
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
//			    LDMA_TransferCfg_t memTransfer = LDMA_TRANSFER_CFG_MEMORY_LOOP(
//			          TRANSFER_HEIGHT-2);

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
					ptr = &Verdana36x64[videobuff[k]][j][0];
					scrbuffer_ptr = buffer + (j * LCDWIDTH) + *offset_ptr;
					for (i = 0; i < 36; i++){
						*scrbuffer_ptr++ = *ptr++;
					}
				}
			}
#endif
    }
    update_display();
}

void putstr(int x, int y, char * string, unsigned int invert) {
#ifdef SSD1306
  unsigned char start_col[] = {SSD1306_COLUMNADDR, 0, SSD1306_LCDWIDTH-1, SSD1306_PAGEADDR, 0, 7};
#elif defined SH1106
	unsigned char start_col[] = {SH1106_SETLOWCOLUMN, SH1106_SETHIGHCOLUMN, SH1106_PAGEADDR};
#endif
	unsigned int i, bytecount; //
	unsigned char *scrbuffer_ptr;

	bytecount = x*8;
	scrbuffer_ptr = buffer + 8*x + LCDWIDTH*y;
	while (*string) {
		for (i = 0; i < 8; i++) {
			*scrbuffer_ptr++ = ASCIITAB[(int)*string][i] ^ ((invert) ? 0xff : 0x00);
			bytecount++;
		}
		string++;
	}
#ifdef SSD1306
	if ((bytecount-1) < SSD1306_LCDWIDTH) {
		start_col[1] = x*8;
		start_col[2] = bytecount-1;
		start_col[4] = y;
		start_col[5] = y;
	} else {
		x = 0;
		start_col[1] = x;
		start_col[2] = SSD1306_LCDWIDTH-1;
		start_col[4] = y;
		start_col[5] = y;
		while (bytecount > (SSD1306_LCDWIDTH-1)) {
			start_col[5]++;
			bytecount -=SSD1306_LCDWIDTH;
		}
	}
	scrbuffer_ptr = buffer + 8*x + SSD1306_LCDWIDTH*y;
	lcdwrite(start_col, sizeof(start_col), lcdcommand);
	lcdwrite(scrbuffer_ptr, (start_col[5]-start_col[4]+1) * (start_col[2]-start_col[1]+1), lcddata);
#elif defined SH1106
	while (bytecount) {
		start_col[0] = SH1106_SETLOWCOLUMN | ((x*8+2) & 0x0f);
		start_col[1] = SH1106_SETHIGHCOLUMN | (((x*8+2) >> 4) & 0x0f);
		start_col[2] = SH1106_PAGEADDR | y;
		scrbuffer_ptr = buffer + 8*x + SH1106_LCDWIDTH*y;
		lcdwrite(start_col, sizeof(start_col), lcdcommand);
		lcdwrite(scrbuffer_ptr, (bytecount-x), lcddata);
		if (bytecount > SH1106_LCDWIDTH) {
			bytecount -= SH1106_LCDWIDTH;
		} else {
			break;
		}		
		x = 0;
		y++;
	}
#endif
}

void show_arrow(unsigned int x, unsigned int y, rotation_dir_t direction) {
	unsigned int index;
#ifdef SSD1306
  static unsigned char start_col[] = {SSD1306_COLUMNADDR, 0, 31, SSD1306_PAGEADDR, 0, 1};
#elif defined SH1106
	unsigned char start_col[] = {SH1106_SETLOWCOLUMN, SH1106_SETHIGHCOLUMN, SH1106_PAGEADDR};
#endif
		 
		switch (direction) {
			case left:
			 index = 0; break;
			 
			case straight:
			 index = 1; break;
			 
			case right:
				index = 2; break;
			 
			default:
				index = 3; break;
		}
#ifdef SSD1306		 
		start_col[1] = x;
		start_col[2] = x+15;
		 
		start_col[4] = y;
		start_col[5] = y+1;
		 
		lcdwrite(start_col, sizeof(start_col), lcdcommand);
		lcdwrite((uint8_t *)arrow[index], 32, lcddata);
#elif defined(SH1106)
		start_col[0] = SH1106_SETLOWCOLUMN | ((2 + x) & 0x0f);
		start_col[1] = SH1106_SETHIGHCOLUMN | (((2 + x) >> 4) & 0x0f);
		start_col[2] = SH1106_PAGEADDR | y;
		lcdwrite(start_col, sizeof(start_col), lcdcommand);
		lcdwrite((uint8_t *)arrow[index], 16, lcddata);

		start_col[0] = SH1106_SETLOWCOLUMN | ((2 + x) & 0x0f);
		start_col[1] = SH1106_SETHIGHCOLUMN | (((2 + x) >> 4) & 0x0f);
		start_col[2] = SH1106_PAGEADDR | (y+1);
		lcdwrite(start_col, sizeof(start_col), lcdcommand);
		lcdwrite((uint8_t *)arrow[index] + 16, 16, lcddata);
#endif
}

void squareXY(unsigned int startX, unsigned int startY, unsigned int endX, unsigned int endY, unsigned int fill) {
	unsigned int i, x, y,	mask, mask_even, mask_odd;
	unsigned char * scrbuffer_ptr;

	if ((startX > LCDWIDTH-1) || (endX > LCDWIDTH-1) || (startY > LCDHEIGHT-1) || (endY > LCDHEIGHT-1)) return;
	if (startX > endX) {	
		startX = startX + endX;   // A + B = AB  =>  AB B
		endX = startX - endX;     // AB - B = A  =>  AB A
		startX = startX - endX;		// AB - A = B  =>  B  A
	}
	if (startY > endY) {	
		startY = startY + endY;   // A + B = AB  =>  AB B
		endY 	 = startY - endY;   // AB - B = A  =>  AB A
		startY = startY - endY;		// AB - A = B  =>  B  A
	}

	for (y = (startY >> 3); y <= ((endY >> 3)); y++) {
		mask = 0;
		for (i = y << 3; i <= ((y << 3) | 0x07); i++) {
			mask >>= 1;
			if ((i <= endY) && (i >= startY)) mask |= 0x80;
		}
		mask_even = mask_odd = mask;
		if (!fill) mask = ~mask;
		else if (fill == 2) mask_even = mask & 0xaa, mask_odd = mask & 0x55;
		else if (fill == 3) mask_even = mask & 0x55, mask_odd = mask & 0xaa;
		scrbuffer_ptr = buffer + y * LCDWIDTH + startX;
		for (x = startX; x <= endX; x++) {
			if (fill) {
				if (x & 1) *scrbuffer_ptr++ |= mask_odd;
				else 			 *scrbuffer_ptr++ |= mask_even;
			}	else			 *scrbuffer_ptr++ &= mask;
		}
	}
}

#define ABS(x)  (((x) < 0) ? (-(x)) : (x))

void lineXY(unsigned int startX, unsigned int startY, unsigned int endX, unsigned int endY, unsigned int fill) {
	int deltaX, deltaY, error, deltaerr, dirY;
	unsigned int X, Y;
	if ((startX > LCDWIDTH-1) || (endX > LCDWIDTH-1) || (startY > LCDHEIGHT-1) || (endY > LCDHEIGHT-1)) return;

	deltaX = ABS((int)endX - (int)startX);
	deltaY = ABS((int)endY - (int)startY);

	if (((deltaX >= deltaY) && (startX > endX)) || ((deltaX < deltaY) && (startY > endY))) {	
					startX = startX + endX;   // A + B = AB  =>  AB B
					endX = startX - endX;     // AB - B = A  =>  AB A
					startX = startX - endX;		// AB - A = B  =>  B  A
					startY = startY + endY;   // A + B = AB  =>  AB B
					endY 	 = startY - endY;   // AB - B = A  =>  AB A
					startY = startY - endY;		// AB - A = B  =>  B  A
	}

	error = 0;
	if (deltaX >= deltaY) {
		deltaerr = deltaY + 1;
		Y = startY;
		dirY = (((int)endY - (int)startY) > 0) ? 1 : -1;
		for (X = startX; X <= endX; X++) {
			//   plot (X, Y);
			if (fill) 			*(buffer + (Y >> 3)*LCDWIDTH + X) |=  (1 << (Y & 0x7));
			else 						*(buffer + (Y >> 3)*LCDWIDTH + X) &= ~(1 << (Y & 0x7));
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
			if (fill) 			*(buffer + (Y >> 3)*LCDWIDTH + X) |=  (1 << (Y & 0x7));
			else 						*(buffer + (Y >> 3)*LCDWIDTH + X) &= ~(1 << (Y & 0x7));
			error = error + deltaerr;
			if (error >= deltaY + 1) {
				X += dirY;
				error -= deltaY + 1;
			}
		}		
	}
}

char bin2hex(unsigned int x) {
	return (x > 9) ? x-10+'A' : x+'0';
}

unsigned int num2str(int x, char* string) {
	int divider;
	unsigned int leading_zero, digit, count = 0;
	divider = 1000000000;
	if (x < 0) {
		x = -x;
		*string++ = '-';
		count++;
	}
	leading_zero = 1;
	while (divider > 1) {
		digit = 0;
		while (x >= divider) {
			x -= divider;
			digit++;
		}
		divider /= 10;
		if ((leading_zero == 0) || (digit > 0)) {
			leading_zero = 0;
			*string++ = digit + '0';
			count++;
		}
	}
	*string++ = x + '0';
	*string = '\0';
	return ++count;
}


void Draw_Map(void) {
	unsigned int j, transpose=0;
	int i;
	int maxX=0, maxY=0, minX=0, minY=0;
//	uint32_t *data_ptr, *src_ptr;

	typedef struct {
		uint8_t		coordX;
		uint8_t		coordY;
	} scr_map_t;
	
	scr_map_t	scr_map[MAX_MAP_SIZE];

	if (data.map_size == 0) return;
	squareXY(0,0, 127, 63, 0);
	for (i=0; i<data.map_size; i++) {
		if (map[i].coordinate.north > maxY) maxY = map[i].coordinate.north;
		if (map[i].coordinate.north < minY) minY = map[i].coordinate.north;
		if (map[i].coordinate.east  > maxX) maxX = map[i].coordinate.east;
		if (map[i].coordinate.east  < minX) minX = map[i].coordinate.east;
	}
	if ((maxX - minX) < (maxY - minY)) transpose = 1;
	for (i=0; i<data.map_size; i++) {
		if (transpose) {
			scr_map[i].coordY = (0+2) + ((map[i].coordinate.north - minY) * (128 - 2 - 2) / (maxY - minY));
			scr_map[i].coordX = (0+2) + ((map[i].coordinate.east  - minX) * (63 - 2 - 2) / (maxX - minX));
		} else {
			scr_map[i].coordX = (0 +2) + ((map[i].coordinate.east  - minX) * (128 - 2 - 2) / (maxX - minX));
			scr_map[i].coordY = (63-2) - ((map[i].coordinate.north - minY) * (63 - 2 - 2) / (maxY - minY));
		}
	}
	update_display();
	for (i=0; i<data.map_size; i++) {
		if (transpose) {
			squareXY(	scr_map[i].coordY - 1, 
								scr_map[i].coordX - 1, 
								scr_map[i].coordY + 1, 
								scr_map[i].coordX + 1, 1);
			for ( j = 0; j < 4; j++) {
				if (map[i].node_link[j] != UNKNOWN) {
					lineXY(	scr_map[i].coordY, 
										scr_map[i].coordX, 
										scr_map[map[i].node_link[j]].coordY,
										scr_map[map[i].node_link[j]].coordX, 1);
				}
			}
		} else {
			squareXY(	scr_map[i].coordX - 1, 
								scr_map[i].coordY - 1, 
								scr_map[i].coordX + 1,
								scr_map[i].coordY + 1, 1);
			for ( j = 0; j < 4; j++) {
				if (map[i].node_link[j] != UNKNOWN) {
					lineXY(	scr_map[i].coordX, 
										scr_map[i].coordY, 
										scr_map[map[i].node_link[j]].coordX,
										scr_map[map[i].node_link[j]].coordY, 1);
				}
			}
		}
	}
	//  последний штрих - нулевая точка
	if (transpose) {
		squareXY(	scr_map[data.green_cell_nr].coordY-2, 
							scr_map[data.green_cell_nr].coordX-2,
							scr_map[data.green_cell_nr].coordY+2, 
							scr_map[data.green_cell_nr].coordX+2, 1);
		squareXY(	scr_map[data.green_cell_nr].coordY-1, 
							scr_map[data.green_cell_nr].coordX-1,
							scr_map[data.green_cell_nr].coordY+1, 
							scr_map[data.green_cell_nr].coordX+1, 0);
	} else {
		squareXY(	scr_map[data.green_cell_nr].coordX-2, 
							scr_map[data.green_cell_nr].coordY-2, 
							scr_map[data.green_cell_nr].coordX+2,
							scr_map[data.green_cell_nr].coordY+2, 1);
		squareXY(	scr_map[data.green_cell_nr].coordX-1, 
							scr_map[data.green_cell_nr].coordY-1, 
							scr_map[data.green_cell_nr].coordX+1,
							scr_map[data.green_cell_nr].coordY+1, 0);
	}	
	// и конечная точка.
	if (data.red_cell_nr) {
		if (transpose) {
			squareXY(	scr_map[data.red_cell_nr].coordY-2, 
								scr_map[data.red_cell_nr].coordX-2,
								scr_map[data.red_cell_nr].coordY+2, 
								scr_map[data.red_cell_nr].coordX+2, 1);
		} else {
			squareXY(	scr_map[data.red_cell_nr].coordX-2, 
								scr_map[data.red_cell_nr].coordY-2, 
								scr_map[data.red_cell_nr].coordX+2,
								scr_map[data.red_cell_nr].coordY+2, 1);
		}	
	}
//	update_display();
	// Теперь маршрут
	if (data.pathlength) 	{
	    int direction;
	    unsigned int prev_pos, curr_pos;
	    curr_pos = data.green_cell_nr;

	    for (direction = north; direction <= west; direction++) {
	        if ((map[curr_pos].node_link[direction]) != UNKNOWN) break;
	    }
	    for (i = 0; i < data.pathlength; i++) {
	        switch (data.length[i] & COMMAND_MASK) {
	          case command_forward:
	            prev_pos = curr_pos;
	            curr_pos = map[curr_pos].node_link[direction];
	            if (curr_pos == UNKNOWN) break;
	            if (transpose) {
	                if (direction == north || direction == west)
	                  squareXY(scr_map[prev_pos].coordY + 1, scr_map[prev_pos].coordX - 1,
	                           scr_map[curr_pos].coordY - 1, scr_map[curr_pos].coordX + 1, 2);
	                else
	                  squareXY(scr_map[prev_pos].coordY - 1, scr_map[prev_pos].coordX + 1,
	                           scr_map[curr_pos].coordY + 1, scr_map[curr_pos].coordX - 1, 2);
	            } else {
	                if (direction == north || direction == east)
	                  squareXY(scr_map[prev_pos].coordX + 1, scr_map[prev_pos].coordY - 1,
	                           scr_map[curr_pos].coordX - 1, scr_map[curr_pos].coordY + 1, 2);
	                else
	                  squareXY(scr_map[prev_pos].coordX - 1, scr_map[prev_pos].coordY + 1,
	                           scr_map[curr_pos].coordX + 1, scr_map[curr_pos].coordY - 1, 2);
	            }
	            break;

	          case command_turn :
	            direction += data.length[i];
	            direction &= TURN_MASK;
	            break;
	        }
          if (curr_pos == UNKNOWN) break;
	    }
	}
	update_display();
}

void ColorSensorTest(uint16_t *colors, unsigned int new) {
	unsigned int levels[3], i;
	char string[5];

	if (new) {
		squareXY(0, 0, 127, 63, 0);
		for (i = 1; i < 8; i++) {
			squareXY(0, i*8, 6, i*8, 1);
		}
		for (i = 0; i < 3; i++) {
			squareXY(i*40+8, 0, i*40+8, 63, 1);
		}
		squareXY( 9, 63-(data.color_red_thr>>4)  , 14, 63-(data.color_red_thr>>4)  , 1);
		squareXY(49, 63-(data.color_green_thr>>4), 54, 63-(data.color_green_thr>>4), 1);
		squareXY(89, 63-(data.color_blue_thr>>4) , 94, 63-(data.color_blue_thr>>4) , 1);
	}
	levels[0] = ((colors[1]<<10)/colors[0]) >> 4;
	levels[1] = ((colors[2]<<10)/colors[0]) >> 4;
	levels[2] = ((colors[3]<<10)/colors[0]) >> 4;

	for (i=0; i<3; i++) {
		squareXY(i*40+16, 63, (i+1)*40, 63-levels[i], 1);
		squareXY(i*40+16,  0, (i+1)*40, 63-levels[i], 0);
		string[0] = bin2hex((colors[i+1] >>12) & 0x0f);
		string[1] = bin2hex((colors[i+1] >> 8) & 0x0f);
		string[2] = bin2hex((colors[i+1] >> 4) & 0x0f);
		string[3] = bin2hex((colors[i+1] >> 0) & 0x0f);
		string[4] = 0;
		putstr(i*5+1, 0, string, 0);
	}
	update_display();
}

void ColorSensorTestHSI(uint16_t *colors, unsigned int new) {
	unsigned int levels[3], i;
	char string[5];

	if (new) {
		squareXY(0, 0, 127, 63, 0); // очистка экрана
		for (i = 1; i < 8; i++) {
			squareXY(0, i*8, 3, i*8, 1);  // 8 горизонтальных черточек x=0..3
		}
    squareXY(3, 0, 3, 63, 1);
    squareXY(33, 0, 33, 63, 1);
    // квадрат для цвета - центр x=100, y=35
    squareXY(72, 8, 72, 63, 1);
    squareXY(72, 8, 127, 8, 1);
    squareXY(72, 18, 127, 18, 2);
    squareXY(100, 8, 100, 63, 1);
    squareXY(72, 63, 127, 63, 1);
    squareXY(72, 52, 127, 52, 2);

		squareXY( 3, 63-(data.color_threshold>>4), 8, 63-(data.color_threshold>>4), 1); // порог яркости
		squareXY(33, 63-(data.color_saturation>>4), 36, 63-(data.color_saturation>>4), 1);    // порог насыщенности
//		squareXY(89, 63-(data.color_blue_thr>>4) , 94, 63-(data.color_blue_thr>>4) , 1);
	}
	{
		unsigned int invsaturation, minRGB=0xffffffff;
		int hueX, hueY, radius;

		for (i=1; i<4; i++) {
			if (minRGB > colors[i]) minRGB = colors[i];
		}
		invsaturation = 3*1024 * minRGB / (colors[1]+colors[2]+colors[3]);
    hueX = 2*colors[1] - colors[2] - colors[3];
    hueY = (colors[2] - colors[3]);
    radius = (int)sqrt((double)(hueX*hueX + hueY*hueY/9));

		levels[0] = ((colors[0]) >> 4);  // уровень канала яркости
		levels[1] = (invsaturation >> 4); // канал насыщенности
		levels[2] = 0;

    for (i=0; i<3; i++) {
      string[0] = bin2hex((colors[i] >>12) & 0x0f);
      string[1] = bin2hex((colors[i] >> 8) & 0x0f);
      string[2] = bin2hex((colors[i] >> 4) & 0x0f);
      string[3] = bin2hex((colors[i] >> 0) & 0x0f);
      string[4] = 0;
      putstr(i*5+1, 0, string, 0);
    }

		if (levels[0] > 55) levels[0] = 55;
    if (levels[1] > 55) levels[1] = 55;

    squareXY(10, 63, 30, 63-levels[0], 1);
    squareXY(10,  8, 30, 63-levels[0], 0);

    squareXY(36, 63, 66, 63-levels[1], 1);
    squareXY(36,  8, 66, 63-levels[1], 0);

//    =25*hueX/radius;
//    =25*hueY/radius;
	}
	update_display();
}
