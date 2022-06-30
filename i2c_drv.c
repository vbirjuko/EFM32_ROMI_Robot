#include "em_device.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "i2c_drv.h"
#include "resources.h"

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

typedef enum {
    send_reg_hi,
    send_reg_lo,
    send_data,

    rcv_reg_hi,
    rcv_reg_lo,
    rcv_restart,
    rcv_data,
    rcv_stop,

    i2c_stop,
    i2c_idle,
} i2c_state_t;

volatile i2c_state_t i2c_state = i2c_idle;
volatile unsigned int data_count, i2c_register, i2c_addr;
unsigned char * volatile data_ptr;
volatile t_i2c_status i2c_error_code;

void i2c_master_init(void) {
  // включаем внутренние подтяжки на всякий случай.
  // И тогда в некоторых случаях можно обойтись и без внешних резисторов.
  CMU_ClockEnable(CMU_CLOCK, 1);

  GPIO_DriveStrengthSet(SCL_PORT, gpioDriveStrengthStrongAlternateStrong);
  GPIO_PinModeSet(SDA_PORT, SDA_PIN, gpioModeWiredAndAlternatePullUp, 1);
  GPIO_PinModeSet(SCL_PORT, SCL_PIN, gpioModeWiredAndAlternatePullUp, 1);

#ifdef DEBUG_I2C
  GPIO_PinModeSet(gpioPortD, 1, gpioModePushPull, 0);
#endif
  // Конфигурируем EUSCI

#if CPU_FREQ != 50
#warning Recalculate divider
#endif
  I2C_DEV->CTRL = I2C_CTRL_GIBITO | I2C_CTRL_BITO_40PCC; // | I2C_CTRL_AUTOSN;
  I2C_DEV->CLKDIV = 14; // 50000 KHz / 400 kbps = 125
  // (125 - 8) /(4+4) - 1 = 13.625
  I2C_DEV->IEN = I2C_IEN_ARBLOST | I2C_IEN_NACK | I2C_IEN_RSTART | I2C_IEN_RXDATAV |
      I2C_IEN_START | I2C_IEN_MSTOP | I2C_IEN_RXFULL;

  // подключаем выводы к EUSCI -
  // ИЗМЕНИТЬ В СЛУЧАЕ ВЫБОРА ДРУГОГО ПОРТА!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  I2C_DEV->ROUTEPEN = I2C_ROUTEPEN_SCLPEN | I2C_ROUTEPEN_SDAPEN;
  I2C_DEV->ROUTELOC0 = SCL_LOC | SDA_LOC;

  BUS_RegMaskedSet(&I2C_DEV->CTRL, I2C_CTRL_EN);
  I2C_DEV->CMD = I2C_CMD_ABORT;

  NVIC_SetPriority(I2C_IRQn, I2C_IRQ_PRI);
  NVIC_EnableIRQ(I2C_IRQn);
}

void I2C_IRQHandler(void) {

  if ((I2C_DEV->IEN & I2C_IEN_ARBLOST) && (I2C_DEV->IF & I2C_IF_ARBLOST)) {// Arbitration lost; Interrupt Flag: UCALIFG; Interrupt
      i2c_error_code = I2C_ERROR;
      i2c_state = i2c_idle;
      I2C_DEV->IFC = I2C_IFC_ARBLOST;
  }
  if ((I2C_DEV->IEN & I2C_IEN_NACK) && (I2C_DEV->IF & I2C_IF_NACK)) { // Not acknowledgment; Interrupt Flag: UCNACKIFG
      i2c_error_code = I2C_ERROR;
      i2c_state = i2c_stop;
      I2C_DEV->IFC = I2C_IFC_NACK;
      BUS_RegMaskedClear(&I2C_DEV->IEN, I2C_IEN_TXBL);
      I2C_DEV->CMD = I2C_CMD_STOP;
  }
  if ((I2C_DEV->IEN & I2C_IEN_START) && (I2C_DEV->IF & I2C_IF_START)) { // Start condition received; Interrupt Flag: UCSTTIFG
      I2C_DEV->IFC = I2C_IFC_START;
      I2C_DEV->TXDATA = i2c_addr;
      if (i2c_state != rcv_data) {
          BUS_RegMaskedSet(&I2C_DEV->IEN, I2C_IEN_TXBL);
      }
  }
  if ((I2C_DEV->IEN & I2C_IEN_RXFULL) && (I2C_DEV->IF & I2C_IF_RXFULL)) { // Check errata race condition
      if (((I2C_DEV->STATUS & I2C_STATUS_RXDATAV) == 0) && (I2C_DEV->STATUS & I2C_STATUS_RXFULL)) {
          I2C_DEV->RXDATA; // make dummy read.
      }
  }
  if ((I2C_DEV->IEN & I2C_IEN_RXDATAV) && (I2C_DEV->IF & I2C_IF_RXDATAV)) { // Data received; Interrupt Flag: UCRXIFG0
      *data_ptr++ = I2C_DEV->RXDATA;
      if (--data_count) {
          I2C_DEV->CMD = I2C_CMD_ACK;
      } else {
          i2c_state = rcv_stop;
          I2C_DEV->CMD = I2C_CMD_NACK | I2C_CMD_STOP;
      }
  }
  if ((I2C_DEV->IEN & I2C_IEN_TXBL) && (I2C_DEV->IF & I2C_IF_TXBL)) { // Transmit buffer empty; Interrupt Flag: UCTXIFG0
      switch  (i2c_state) {
        case  rcv_reg_hi:
          i2c_state = rcv_restart;
          I2C_DEV->TXDOUBLE = (i2c_register & 0xFFFFu);
          break;
        case  rcv_reg_lo:
          i2c_state =  rcv_restart;
          I2C_DEV->TXDATA = i2c_register & 0xFFu;
          break;
        case  rcv_restart:
          i2c_state = rcv_data;
          i2c_addr |= 0x01;
          BUS_RegMaskedClear(&I2C_DEV->IEN, I2C_IEN_TXBL);
          I2C_DEV->CMD = I2C_CMD_START;
          break;
        case send_reg_hi:
          i2c_state = send_data;
          I2C_DEV->TXDOUBLE = (i2c_register & 0xFFFFu);
          break;
        case send_reg_lo:
          i2c_state = send_data;;
          I2C_DEV->TXDATA = i2c_register & 0xFFu;
          break;
        case send_data:
          if (data_count) {
              I2C_DEV->TXDATA = *data_ptr++;
              --data_count;
          } else {
              BUS_RegMaskedClear(&I2C_DEV->IEN, I2C_IEN_TXBL);
              i2c_state = i2c_stop;
              I2C_DEV->CMD = I2C_CMD_STOP;
          }
          break;

        default:
          break;
      }
  }
  if ((I2C_DEV->IEN & I2C_IEN_MSTOP) && (I2C_DEV->IF & I2C_IF_MSTOP)) { // Stop condition received; Interrupt Flag: UCSTPIFG
      i2c_state = i2c_idle;
      I2C_DEV->CMD = I2C_CMD_CLEARTX | I2C_CMD_CLEARPC;
      I2C_DEV->IFC = I2C_IFC_MSTOP;
      BUS_RegMaskedClear(&I2C_DEV->IEN, I2C_IEN_TXBL);
  }
}


t_i2c_status i2c_wr(uint8_t address, unsigned char * data, unsigned int length) {

  if (i2c_state != i2c_idle) return I2C_BUSY;

  data_ptr = data;
  data_count = length;
  //  i2c_register = reg_addr;
  i2c_state = send_data;
  i2c_error_code = I2C_SUCCESS;
  i2c_addr = address & ~0x01;
  I2C_DEV->CMD = I2C_CMD_START;

  while (i2c_state != i2c_idle) continue;
  return i2c_error_code;
}

t_i2c_status i2c_rd(uint8_t address, unsigned char * data, unsigned int length) {

  if (i2c_state != i2c_idle) return I2C_BUSY;
#ifdef DEBUG_I2C
  GPIO_PinOutSet(gpioPortD, 1);
#endif
    data_ptr = data;
    data_count = length;
    i2c_state = rcv_data;
    i2c_addr = address |= 0x01;
    i2c_error_code = I2C_SUCCESS;
    I2C_DEV->CMD = I2C_CMD_START;

    while (i2c_state != i2c_idle) continue;
#ifdef DEBUG_I2C
  GPIO_PinOutClear(gpioPortD, 1);
#endif
    return i2c_error_code;
}

t_i2c_status i2c_wr_reg(uint8_t address, uint8_t reg_addr, unsigned char * data, unsigned int length) {

  if (i2c_state != i2c_idle) return I2C_BUSY;

  data_ptr = data;
  data_count = length;
  i2c_register = reg_addr;
  i2c_state = send_reg_lo;
  i2c_error_code = I2C_SUCCESS;
  i2c_addr = address & ~0x01;
  I2C_DEV->CMD = I2C_CMD_START;

  while (i2c_state != i2c_idle) continue;
  return i2c_error_code;
}

t_i2c_status i2c_rd_reg(uint8_t address, uint8_t reg_addr, unsigned char * data, unsigned int length) {

  if (i2c_state != i2c_idle) return I2C_BUSY;

  data_ptr = data;
  data_count = length;
  i2c_register = reg_addr;
  i2c_state = rcv_reg_lo;
  i2c_addr = address & ~0x01;
  i2c_error_code = I2C_SUCCESS;
  I2C_DEV->CMD = I2C_CMD_START;

  while (i2c_state != i2c_idle) continue;
  return i2c_error_code;
}

t_i2c_status i2c_wr_reg16(uint8_t address, uint16_t reg_addr, unsigned char * data, unsigned int length) {

  if (i2c_state != i2c_idle) return I2C_BUSY;

  data_ptr = data;
  data_count = length;
  i2c_register = ((reg_addr & 0x00FF) << 8) | ((reg_addr & 0xff00) >> 8);
  i2c_state = send_reg_hi;
  i2c_error_code = I2C_SUCCESS;
  i2c_addr = address & ~0x01;
  I2C_DEV->CMD = I2C_CMD_START;

  while (i2c_state != i2c_idle) continue;
  return i2c_error_code;
}

t_i2c_status i2c_rd_reg16(uint8_t address, uint16_t reg_addr, unsigned char * data, unsigned int length) {

  if (i2c_state != i2c_idle) return I2C_BUSY;

  data_ptr = data;
  data_count = length;
  i2c_register = ((reg_addr & 0x00FF) << 8) | ((reg_addr & 0xff00) >> 8);
  i2c_state = rcv_reg_hi;
  i2c_error_code = I2C_SUCCESS;
  i2c_addr = address & ~0x01;
  I2C_DEV->CMD = I2C_CMD_START;

  while (i2c_state != i2c_idle) continue;
  return i2c_error_code;
}


#ifdef OLED_I2C
#ifdef SSD1306
#include "Adafruit_SSD1306.h"
#define OLED_I2C_ADDR SSD1306_I2C_ADDRESS
#elif defined SH1106
#include "Adafruit_SH1106.h"
#define OLED_I2C_ADDR SH1106_I2C_ADDRESS
#endif

void lcdwrite(unsigned char* data_ptr, unsigned int count, lcddatacommand dc){
    i2c_wr_reg(OLED_I2C_ADDR, (dc << 6), data_ptr, count);
}
#endif
