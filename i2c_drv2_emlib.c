/*Simplified BSD License (FreeBSD License)
Copyright (c) 2021, Vladimir Birjukov, All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are
those of the authors and should not be interpreted as representing official
policies, either expressed or implied, of the FreeBSD Project.
*/



#include "em_device.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_i2c.h"
#include "i2c_drv.h"
#include "resources.h"

#define I2C_DEV_NUM 1
#define I2C_DEV     I2C1
#define I2C_IRQn    I2C1_IRQn
#define I2C_IRQHandler  I2C1_IRQHandler
#define CMU_CLOCK   cmuClock_I2C1


#define SDA_PORT    gpioPortC
#define SCL_PORT    gpioPortC
#define SDA_PIN         (4)
#define SCL_PIN         (5)
#define SDA_LOC     I2C_ROUTELOC0_SDALOC_LOC0
#define SCL_LOC     I2C_ROUTELOC0_SCLLOC_LOC0


#define I2C_DEV     I2C1
#define I2C_IRQn    I2C1_IRQn
#define I2C_IRQHandler  I2C1_IRQHandler
#define CMU_CLOCK   cmuClock_I2C1


I2C_Init_TypeDef iic_init = I2C_INIT_DEFAULT;

volatile I2C_TransferReturn_TypeDef i2c_state = i2cTransferDone;

#define DEBUG_I2C

#ifdef DEBUG_I2C
volatile uint8_t debug_str[256], *debug_str_ptr = 0;
uint8_t hex_str[] = "0123456789ABCDEF";
#endif

void i2c_master_init(void) {

  CMU_ClockEnable(CMU_CLOCK, 1);

  // включаем внутренние подтяжки на всякий случай.
  // И тогда в некоторых случаях можно обойтись и без внешних резисторов.

  GPIO_PinModeSet(SDA_PORT, SDA_PIN, gpioModeWiredAndPullUp, 1);
  GPIO_PinModeSet(SCL_PORT, SCL_PIN, gpioModeWiredAndPullUp, 1);

  // подключаем выводы к EUSCI -
  // ИЗМЕНИТЬ В СЛУЧАЕ ВЫБОРА ДРУГОГО ПОРТА!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  I2C_DEV->ROUTEPEN = I2C_ROUTEPEN_SCLPEN | I2C_ROUTEPEN_SDAPEN;
  I2C_DEV->ROUTELOC0 = SCL_LOC | SDA_LOC;

  iic_init.freq = I2C_FREQ_FAST_MAX;
  iic_init.clhr = i2cClockHLRAsymetric;

  I2C_Init(I2C_DEV, &iic_init);

  NVIC_SetPriority(I2C_IRQn, I2C_IRQ_PRI);
  NVIC_EnableIRQ(I2C_IRQn);

}

void I2C_IRQHandler(void) {
  i2c_state = I2C_Transfer(I2C_DEV);
}

I2C_TransferSeq_TypeDef wr_seq;
uint8_t reg_addr_buffer[2];

I2C_TransferReturn_TypeDef i2c_wr(uint8_t address, unsigned char * data, unsigned int length) {

  while (i2c_state == i2cTransferInProgress) continue;

  wr_seq.addr = address;
  wr_seq.flags = I2C_FLAG_WRITE;
  wr_seq.buf[0].data = data;
  wr_seq.buf[0].len = length;

  if (i2cTransferInProgress != (i2c_state = I2C_TransferInit(I2C_DEV, &wr_seq))) return 1;

  while (i2c_state == i2cTransferInProgress) continue;
  return i2c_state;
}

I2C_TransferReturn_TypeDef i2c_rd(uint8_t address, unsigned char * data, unsigned int length) {

  while (i2c_state == i2cTransferInProgress) continue;

  wr_seq.addr = address;
  wr_seq.flags = I2C_FLAG_READ;
  wr_seq.buf[0].data = data;
  wr_seq.buf[0].len = length;

  if (i2cTransferInProgress != (i2c_state = I2C_TransferInit(I2C_DEV, &wr_seq))) return 1;

  while (i2c_state == i2cTransferInProgress) continue;
  return i2c_state;
}

I2C_TransferReturn_TypeDef i2c_wr_reg(uint8_t address, uint8_t reg_addr, unsigned char * data, unsigned int length) {

  while (i2c_state == i2cTransferInProgress) continue;

  reg_addr_buffer[0] = reg_addr;
  wr_seq.addr = address;
  wr_seq.flags = I2C_FLAG_WRITE_WRITE;

  wr_seq.buf[0].data = reg_addr_buffer;
  wr_seq.buf[0].len = 1;

  wr_seq.buf[1].data = data;
  wr_seq.buf[1].len = length;

  if (i2cTransferInProgress != (i2c_state = I2C_TransferInit(I2C_DEV, &wr_seq))) return 1;

  while (i2c_state == i2cTransferInProgress) continue;
  return i2c_state;
}

I2C_TransferReturn_TypeDef i2c_rd_reg(uint8_t address, uint8_t reg_addr, unsigned char * data, unsigned int length) {

  while (i2c_state == i2cTransferInProgress) continue;

  reg_addr_buffer[0] = reg_addr;
  wr_seq.addr = address;
  wr_seq.flags = I2C_FLAG_WRITE_READ;

  wr_seq.buf[0].data = reg_addr_buffer;
  wr_seq.buf[0].len = 1;

  wr_seq.buf[1].data = data;
  wr_seq.buf[1].len = length;

  if (i2cTransferInProgress != (i2c_state = I2C_TransferInit(I2C_DEV, &wr_seq))) return 1;

  while (i2c_state == i2cTransferInProgress) continue;
  return i2c_state;
}

I2C_TransferReturn_TypeDef i2c_wr_reg16(uint8_t address, uint16_t reg_addr, unsigned char * data, unsigned int length) {

  while (i2c_state == i2cTransferInProgress) continue;

  reg_addr_buffer[0] = (reg_addr & 0xFF00) >> 8;
  reg_addr_buffer[1] = reg_addr  & 0x00FF;

  wr_seq.addr = address;
  wr_seq.flags = I2C_FLAG_WRITE_WRITE;

  wr_seq.buf[0].data = reg_addr_buffer;
  wr_seq.buf[0].len = 2;

  wr_seq.buf[1].data = data;
  wr_seq.buf[1].len = length;

  if (i2cTransferInProgress != (i2c_state = I2C_TransferInit(I2C_DEV, &wr_seq))) return 1;

  while (i2c_state == i2cTransferInProgress) continue;
  return i2c_state;
}

I2C_TransferReturn_TypeDef i2c_rd_reg16(uint8_t address, uint16_t reg_addr, unsigned char * data, unsigned int length) {

  while (i2c_state == i2cTransferInProgress) continue;

  reg_addr_buffer[0] = (reg_addr & 0xFF00) >> 8;
  reg_addr_buffer[1] = reg_addr  & 0x00FF;

  wr_seq.addr = address;
  wr_seq.flags = I2C_FLAG_WRITE_READ;

  wr_seq.buf[0].data = reg_addr_buffer;
  wr_seq.buf[0].len = 2;

  wr_seq.buf[1].data = data;
  wr_seq.buf[1].len = length;

  if (i2cTransferInProgress != (i2c_state = I2C_TransferInit(I2C_DEV, &wr_seq))) return 1;

  while (i2c_state == i2cTransferInProgress) continue;
  return i2c_state;
}


#ifdef OLED_I2C
#ifdef SSD1306
#include "Adafruit_SSD1306.h"
#include "display.h"
#define OLED_I2C_ADDR SSD1306_I2C_ADDRESS
#elif defined SH1106
#include "Adafruit_SH1106.h"
#define OLED_I2C_ADDR SH1106_I2C_ADDRESS
#endif

void lcdwrite(unsigned char* data_ptr, unsigned int count, lcddatacommand dc){
    i2c_wr_reg(OLED_I2C_ADDR, (dc << 6), data_ptr, count);
}
#endif
