/*
 * spi_drv.c
 *
 *  Created on: 10 мар. 2021 г.
 *      Author: wl
 */
#include "em_device.h"
#include "em_gpio.h"
#include "em_cmu.h"
#include "em_ldma.h"
#include "spi_drv.h"
#include "LaunchPad.h"
#include "Reflectance.h"
#include "LE_timer0.h"
#include "resources.h"

/***************************************************************************//**
 * @brief
 *    LDMA Configuration Definitions.
 ******************************************************************************/

//volatile unsigned char *buffer_ptr, *rx_buffer_ptr;
volatile unsigned int /*buffer_count = 0, */ tx_busy = 0;
static volatile uint8_t exchange_buffer[256+4];

#ifdef OLED_SPI
LDMA_TransferCfg_t OLED_txferCfg = LDMA_TRANSFER_CFG_PERIPHERAL(ldmaPeripheralSignal_USART3_TXBL);

LDMA_Descriptor_t OLED_tx_descriptor[] =
    {
      LDMA_DESCRIPTOR_LINKREL_SYNC(0, 0x80, 0x00, 0x00, 1),
//      LDMA_DESCRIPTOR_LINKREL_WRITE(0, &GPIO->P[gpioPortA].DOUT, 1),
      LDMA_DESCRIPTOR_LINKREL_WRITE(USART_CMD_RXDIS, &USART3->CMD, 1),
      LDMA_DESCRIPTOR_LINKREL_WRITE(USART_ROUTELOC0_CLKLOC_LOC0 | USART_ROUTELOC0_CSLOC_LOC0 |
                                    USART_ROUTELOC0_RXLOC_LOC0 | USART_ROUTELOC0_TXLOC_LOC0,
                                    &USART3->ROUTELOC0, 1),
      LDMA_DESCRIPTOR_LINKREL_M2P_BYTE(0, &USART3->TXDATA, 0, 1),
      LDMA_DESCRIPTOR_SINGLE_WRITE(0, &tx_busy)
    };

void lcdwrite(unsigned char* data_ptr, unsigned int count, lcddatacommand dc){

  while (tx_busy) continue;
  tx_busy = 1;

//  OLED_tx_descriptor[1].wri.immVal = (dc << 5)|(1 << 4)|(1 << 3)|launchpad_LED_state|(1 << 15);

  OLED_tx_descriptor[3].xfer.xferCnt = count - 1;
  OLED_tx_descriptor[3].xfer.srcAddr = (uint32_t) data_ptr;
  OLED_tx_descriptor[3].xfer.doneIfs = 0; // это прерывание не нужно.
  OLED_tx_descriptor[4].xfer.doneIfs = 0; // это прерывание не нужно.
  BUS_RegBitWrite(&GPIO->P[gpioPortA].DOUT, 5, dc);

  LDMA_StartTransfer(LDMA_USART_OLED_CHANNEL, &OLED_txferCfg, (void *)&OLED_tx_descriptor);
}

#include "keyboard.h"
#include "i2c_drv.h"

void spi_oled_init(void) {
  uint8_t reset_state = 0;

    CMU_ClockEnable(cmuClock_USART3, 1);
    USART3->CTRL = USART_CTRL_SYNC | USART_CTRL_AUTOCS | USART_CTRL_CLKPOL_IDLELOW | USART_CTRL_MSBF ;
    USART3->CLKDIV = (CPU_FREQ/1000000) << _USART_CLKDIV_DIV_SHIFT;  // 1 MBit transfer rate
    USART3->ROUTELOC0 = USART_ROUTELOC0_CLKLOC_LOC0 | USART_ROUTELOC0_CSLOC_LOC0 |
        USART_ROUTELOC0_RXLOC_LOC0 | USART_ROUTELOC0_TXLOC_LOC0;
    USART3->ROUTEPEN = USART_ROUTEPEN_CLKPEN | USART_ROUTEPEN_TXPEN | USART_ROUTEPEN_CSPEN | USART_ROUTEPEN_RXPEN;
    USART3->CMD = USART_CMD_MASTEREN | USART_CMD_TXEN | USART_CMD_RXDIS;
    USART3->IEN = USART_IEN_TXC;

    GPIO_PinModeSet(gpioPortA, 5, gpioModePushPull, 0); // D/C
//    GPIO_PinModeSet(gpioPortA, 4, gpioModePushPull, 1); // RES
    GPIO_PinModeSet(gpioPortE, 4, gpioModePushPull, 1); // CS EEPROM
    GPIO_PinModeSet(gpioPortA, 3, gpioModePushPull, 1); // CS OLED
    GPIO_PinModeSet(gpioPortA, 2, gpioModePushPull, 0);
    GPIO_PinModeSet(gpioPortA, 1, gpioModeInputPull, 1);
    GPIO_PinModeSet(gpioPortA, 0, gpioModePushPull, 0);

    // reset the LCD to a known state, RESET low
//    BUS_RegBitWrite((volatile uint32_t *)&GPIO->P[gpioPortA].DOUT,  4, 0);
    i2c_wr_reg(I2C_PCA_ADDR, output_port, &reset_state, sizeof(reset_state));
//    delay = 10000000; // delay minimum 100 ns
//    while (delay--) continue;
    letimer_delay_ms(10);
    reset_state = 1 << 3;
//    BUS_RegBitWrite((volatile uint32_t *)&GPIO->P[gpioPortA].DOUT,  4, 1);
    i2c_wr_reg(I2C_PCA_ADDR, output_port, &reset_state, sizeof(reset_state));
    // Enable LDMA clock
    CMU_ClockEnable(cmuClock_LDMA, true);
    LDMA_Init_t init = LDMA_INIT_DEFAULT;

    // Initialize LDMA
    LDMA_Init(&init);
    // Enable LDMA Interrupt
    NVIC_ClearPendingIRQ(LDMA_IRQn);
    NVIC_SetPriority(LDMA_IRQn, LDMA_IRQ_PRI);
    NVIC_EnableIRQ(LDMA_IRQn);

    NVIC_DisableIRQ(USART3_RX_IRQn);
    NVIC_EnableIRQ(USART3_TX_IRQn);
    NVIC_SetPriority(USART3_TX_IRQn, USART3_TX_IRQ_PRI);
    BUS_RegMaskedSet(&LDMA->SYNC, 0x80);
}
#endif

void USART3_TX_IRQHandler(void) {
  if(USART3->IF & USART_IF_TXC) {
      USART3->IFC = USART_IFC_TXC;
//      tx_busy = 0;
      BUS_RegMaskedSet(&LDMA->SYNC, 0x80);
  }
}


LDMA_TransferCfg_t  EEPROM_rxferCfg = LDMA_TRANSFER_CFG_PERIPHERAL(ldmaPeripheralSignal_USART3_RXDATAV),
                    EEPROM_txferCfg = LDMA_TRANSFER_CFG_PERIPHERAL(ldmaPeripheralSignal_USART3_TXBL);

LDMA_Descriptor_t
  EEPROM_rx_descriptor[] = {
      LDMA_DESCRIPTOR_LINKREL_P2M_BYTE(&(USART3->RXDATA), (void*) 0, 0, 1),
      LDMA_DESCRIPTOR_SINGLE_WRITE(0, &tx_busy)
  },
  EEPROM_tx_descriptor[] = {
      LDMA_DESCRIPTOR_LINKREL_SYNC(0, 0x80, 0x00, 0x00, 1),
      LDMA_DESCRIPTOR_LINKREL_WRITE(USART_ROUTELOC0_CLKLOC_LOC0 | USART_ROUTELOC0_CSLOC_LOC1 |
                                    USART_ROUTELOC0_RXLOC_LOC0 | USART_ROUTELOC0_TXLOC_LOC0,
                                    &USART3->ROUTELOC0, 1),
      LDMA_DESCRIPTOR_LINKREL_WRITE(USART_CMD_RXEN | USART_CMD_CLEARRX, &USART3->CMD, 1),
      LDMA_DESCRIPTOR_SINGLE_M2P_BYTE(0, &(USART3->TXDATA), 0)
  };

void  spi_exchange(unsigned char * send_data_ptr, uint16_t count) {

  while ((BUS_RegBitRead(&LDMA->SYNC, 7) == 0) || tx_busy) continue;
    tx_busy = 1;

    EEPROM_rx_descriptor[0].xfer.dstAddr = (uint32_t)send_data_ptr;
    EEPROM_rx_descriptor[0].xfer.xferCnt = count - 1;
    EEPROM_rx_descriptor[0].xfer.doneIfs   = 0;
    EEPROM_rx_descriptor[1].xfer.doneIfs   = 0;

    EEPROM_tx_descriptor[3].xfer.srcAddr = (uint32_t)send_data_ptr;
    EEPROM_tx_descriptor[3].xfer.xferCnt = count - 1;
    EEPROM_tx_descriptor[3].xfer.doneIfs   = 0;
    LDMA_StartTransfer(LDMA_USART3_RX_CHANNEL, &EEPROM_rxferCfg, (void *)&EEPROM_rx_descriptor);
    LDMA_StartTransfer(LDMA_USART3_TX_CHANNEL, &EEPROM_txferCfg, (void *)&EEPROM_tx_descriptor);

//    while (tx_busy) continue;
}

LDMA_Descriptor_t EEdescLinktx[5], EEdescLinkrx[3];
LDMA_TransferCfg_t EExchfertx = LDMA_TRANSFER_CFG_PERIPHERAL(ldmaPeripheralSignal_USART3_TXBL),
                   EExchferrx = LDMA_TRANSFER_CFG_PERIPHERAL(ldmaPeripheralSignal_USART3_RXDATAV);


void spi_eeprom_exchange(uint8_t * cmd_ptr, unsigned int cmd_count, unsigned char * send_data_ptr, uint16_t count) {
  while ((BUS_RegBitRead(&LDMA->SYNC, 7) == 0) || tx_busy) continue;
  tx_busy = 1;

  EEdescLinkrx[0] = (LDMA_Descriptor_t)LDMA_DESCRIPTOR_LINKREL_P2M_BYTE(&USART3->RXDATA, cmd_ptr, cmd_count, 1);
  EEdescLinkrx[1] = (LDMA_Descriptor_t)LDMA_DESCRIPTOR_LINKREL_P2M_BYTE(&USART3->RXDATA, send_data_ptr, count, 1);
  EEdescLinkrx[2] = (LDMA_Descriptor_t)LDMA_DESCRIPTOR_SINGLE_WRITE(0, &tx_busy);
  EEdescLinkrx[1].xfer.doneIfs = 0;
  EEdescLinkrx[2].xfer.doneIfs = 0;

  EEdescLinktx[0] = (LDMA_Descriptor_t)LDMA_DESCRIPTOR_LINKREL_SYNC(0, 0x80, 0x00, 0x00, 1);
  EEdescLinktx[1] = (LDMA_Descriptor_t)LDMA_DESCRIPTOR_LINKREL_WRITE(USART_ROUTELOC0_CLKLOC_LOC0 | USART_ROUTELOC0_CSLOC_LOC1 |
                                                                     USART_ROUTELOC0_RXLOC_LOC0 | USART_ROUTELOC0_TXLOC_LOC0,
                                                                     &USART3->ROUTELOC0, 1);
  EEdescLinktx[2] = (LDMA_Descriptor_t)LDMA_DESCRIPTOR_LINKREL_WRITE(USART_CMD_RXEN | USART_CMD_CLEARRX, &USART3->CMD, 1);
  EEdescLinktx[3] = (LDMA_Descriptor_t)LDMA_DESCRIPTOR_LINKREL_M2P_BYTE(cmd_ptr, &USART3->TXDATA, cmd_count, 1);
  EEdescLinktx[4] = (LDMA_Descriptor_t)LDMA_DESCRIPTOR_SINGLE_M2P_BYTE(send_data_ptr, &USART3->TXDATA, count);
  EEdescLinktx[3].xfer.doneIfs = 0;
  EEdescLinktx[4].xfer.doneIfs = 0;

  LDMA_StartTransfer(LDMA_USART3_RX_CHANNEL, &EExchferrx, (void *)&EEdescLinkrx);
  LDMA_StartTransfer(LDMA_USART3_TX_CHANNEL, &EExchfertx, (void *)&EEdescLinktx);
}

void spi_eeprom_init(void) {

  CMU_ClockEnable(cmuClock_USART3, 1);
  USART3->CTRL = USART_CTRL_SYNC | USART_CTRL_AUTOCS | USART_CTRL_CLKPOL_IDLELOW | USART_CTRL_MSBF ;
  USART3->CLKDIV = (CPU_FREQ/1000000) << _USART_CLKDIV_DIV_SHIFT;
  USART3->ROUTELOC0 = USART_ROUTELOC0_CLKLOC_LOC0 | USART_ROUTELOC0_CSLOC_LOC1 |
      USART_ROUTELOC0_RXLOC_LOC0 | USART_ROUTELOC0_TXLOC_LOC0;
  USART3->ROUTEPEN = USART_ROUTEPEN_CLKPEN | USART_ROUTEPEN_TXPEN | USART_ROUTEPEN_RXPEN | USART_ROUTEPEN_CSPEN;
  USART3->CMD = USART_CMD_MASTEREN | USART_CMD_TXEN | USART_CMD_RXEN ;
  USART3->IEN = USART_IEN_TXC ;

  GPIO_PinModeSet(gpioPortA, 5, gpioModePushPull, 0); // D/C
  GPIO_PinModeSet(gpioPortA, 4, gpioModePushPull, 1); // RES
  GPIO_PinModeSet(gpioPortE, 4, gpioModePushPull, 1); // CS EEPROM
  GPIO_PinModeSet(gpioPortA, 3, gpioModePushPull, 1); // CS OLED
  GPIO_PinModeSet(gpioPortA, 2, gpioModePushPull, 0);
  GPIO_PinModeSet(gpioPortA, 1, gpioModeInputPull, 1);
  GPIO_PinModeSet(gpioPortA, 0, gpioModePushPull, 0);

// Enable LDMA clock
  CMU_ClockEnable(cmuClock_LDMA, true);
  LDMA_Init_t init = LDMA_INIT_DEFAULT;

  // Initialize LDMA
  LDMA_Init(&init);
  // Enable LDMA Interrupt
  NVIC_ClearPendingIRQ(LDMA_IRQn);
  NVIC_EnableIRQ(LDMA_IRQn);

  NVIC_DisableIRQ(USART3_RX_IRQn);
  NVIC_EnableIRQ(USART3_TX_IRQn);
  NVIC_SetPriority(USART3_TX_IRQn, USART3_TX_IRQ_PRI);
}

volatile unsigned int dma_flag = 0;

void LDMA_IRQHandler(void) {
  uint32_t ch;
  /* Get all pending and enabled interrupts. */
  uint32_t pending = LDMA_IntGetEnabled();

  /* Loop on an LDMA error to enable debugging. */
  while (pending & LDMA_IF_ERROR) {
      LaunchPad_Output(7);
      LaunchPad_Output1(7);
  }

  /* Iterate over all LDMA channels. */
  for (ch = 0; ch < DMA_CHAN_COUNT; ch++) {
    uint32_t mask = 0x1 << ch;
    if (pending & mask) {
      /* Clear the interrupt flag. */
      LDMA->IFC = mask;
      if (ch == LDMA_MEM2MEM_CHANNEL) {
          dma_flag = 0;
      } else if (ch == LDMA_ADC_CHANNEL) {
          ldma_adc_eoc();
      }
    }
  }
}

unsigned int spi_read_eeprom(unsigned int ee_address, unsigned char * ram_address, unsigned int ee_size) {
    unsigned int bytes_to_read;

    while (ee_size) {
        if (ee_size > 2048) bytes_to_read = 2048;
        else     bytes_to_read = ee_size;
        ee_size -= bytes_to_read;

        while (tx_busy) continue;

        exchange_buffer[0] = eeprom_read_data;
        exchange_buffer[1] = ((ee_address & 0x0ff0000) >> 16) & 0xFF;
        exchange_buffer[2] = ((ee_address & 0x000FF00) >>  8) & 0xFF;
        exchange_buffer[3] = ((ee_address & 0x00000ff) >>  0) & 0xFF;

        spi_eeprom_exchange((unsigned char *)exchange_buffer, 4, ram_address, bytes_to_read);

        ee_address += bytes_to_read;
        ram_address += bytes_to_read;
    }
    while (tx_busy) continue;
    return 0;
}

void spi_write_eeprom(unsigned int ee_address, unsigned char * ram_address, unsigned int ee_size) {
//    const uint8_t enable_command_string[] = {eeprom_write_enable};
          uint8_t *data_ptr;
    unsigned int i, eeprom_ptr, bytes_to_copy;

    eeprom_ptr = ee_address;
    for (i = 0; i < (ee_size / 4096)+1; i++) {

        exchange_buffer[0] = eeprom_write_enable;
        spi_exchange((unsigned char *)exchange_buffer, 1);  // Write enable
        while (tx_busy) continue;

        exchange_buffer[0] = eeprom_sector_erase;
        exchange_buffer[1] = ((eeprom_ptr & 0x0ff0000) >> 16) & 0xFF;
        exchange_buffer[2] = ((eeprom_ptr & 0x000FF00) >>  8) & 0xFF;
        exchange_buffer[3] = ((eeprom_ptr & 0x00000ff) >>  0) & 0xFF;
        spi_exchange((unsigned char *)exchange_buffer, 4);      // Erase sector

        // Wait for complete
        do {
            letimer_delay_ms(50);
            exchange_buffer[0] = eeprom_read_status_register;
            spi_exchange((unsigned char *)exchange_buffer, 2);
            while (tx_busy) continue;
        } while (exchange_buffer[1] & EEPROM_STATUS_WIP);
        eeprom_ptr += 4096;
    }

    data_ptr = ram_address;
    eeprom_ptr = ee_address;
    while (ee_size) {
        bytes_to_copy = (ee_size > 256) ? 256 : ee_size;
        copy_data_dma(data_ptr, (uint8_t *)&exchange_buffer[4], bytes_to_copy);
        data_ptr += bytes_to_copy;
        ee_size -= bytes_to_copy;
        exchange_buffer[0] = eeprom_write_enable;
        spi_exchange((unsigned char *)exchange_buffer, 1);  // Write enable
        while (tx_busy) continue;

        exchange_buffer[0] = eeprom_page_program;
        exchange_buffer[1] = ((eeprom_ptr & 0x0ff0000) >> 16) & 0xFF;
        exchange_buffer[2] = ((eeprom_ptr & 0x000FF00) >>  8) & 0xFF;
        exchange_buffer[3] = ((eeprom_ptr & 0x00000ff) >>  0) & 0xFF;
        while (dma_flag) continue;
        spi_exchange((unsigned char *)exchange_buffer, bytes_to_copy + 4);   // Write page

        eeprom_ptr += 256;

        do {
            letimer_delay_ms(2);
            while (tx_busy) continue;
            exchange_buffer[0] = eeprom_read_status_register;
            spi_exchange((unsigned char *)exchange_buffer, 2);
            while (tx_busy) continue;
        } while (exchange_buffer[1] & EEPROM_STATUS_WIP);
    }
}


void copy_data_dma(uint8_t* source, uint8_t* dest, uint16_t count) {
  if ((count - 1) < 2048) {
      while (dma_flag) continue;
      LDMA_TransferCfg_t xferCfg = LDMA_TRANSFER_CFG_MEMORY();
      LDMA_Descriptor_t descriptor = LDMA_DESCRIPTOR_SINGLE_M2M_BYTE(source, dest, count);
      dma_flag = 1;
      LDMA_StartTransfer(LDMA_MEM2MEM_CHANNEL, &xferCfg, &descriptor);
  }
}

void copy_data32_dma(uint32_t* source, uint32_t* dest, uint16_t count) {
  if ((count - 1) < 2048) {
      while (dma_flag) continue;
      LDMA_TransferCfg_t xferCfg = LDMA_TRANSFER_CFG_MEMORY();
      LDMA_Descriptor_t descriptor = LDMA_DESCRIPTOR_SINGLE_M2M_WORD(source, dest, count);
      dma_flag = 1;
      LDMA_StartTransfer(LDMA_MEM2MEM_CHANNEL, &xferCfg, &descriptor);
  }
}

//void fill_data32_dma(uint32_t data, uint32_t* dest, uint16_t count) {
//  static uint32_t source[1];
//
//  if ((count - 1) < 2048) {
//      while (dma_flag) continue;
//      LDMA_TransferCfg_t xferCfg = LDMA_TRANSFER_CFG_MEMORY();
//      LDMA_Descriptor_t descriptor = LDMA_DESCRIPTOR_SINGLE_M2M_WORD(source, dest, count);
//      source[0] = data;
//      descriptor.xfer.srcInc = ldmaCtrlSrcIncNone;
//      dma_flag = 1;
//      LDMA_StartTransfer(LDMA_MEM2MEM_CHANNEL, &xferCfg, &descriptor);
//  }
//}

LDMA_TransferCfg_t xferCfg;
LDMA_Descriptor_t descriptor[2];

void fill_data32_dma(uint32_t data, uint32_t* dest, uint16_t count) {
  static uint32_t source[1];

    if ((count - 1) < 2048) {
        while (dma_flag) continue;
        xferCfg = (LDMA_TransferCfg_t) LDMA_TRANSFER_CFG_MEMORY();
        descriptor[0] = (LDMA_Descriptor_t) LDMA_DESCRIPTOR_SINGLE_M2M_WORD(source, dest, count);
        source[0] = data;
        descriptor[0].xfer.srcInc = ldmaCtrlSrcIncNone;
        dma_flag = 1;
        LDMA_StartTransfer(LDMA_MEM2MEM_CHANNEL, &xferCfg, &descriptor[0]);
    } else {
        while (dma_flag) continue;
        xferCfg      = (LDMA_TransferCfg_t) LDMA_TRANSFER_CFG_MEMORY_LOOP((count / 2048)-1);
        descriptor[0] = (LDMA_Descriptor_t) LDMA_DESCRIPTOR_LINKREL_M2M_WORD(source, dest, ((count-1) % 2048) + 1, 1);
        descriptor[1] = (LDMA_Descriptor_t) LDMA_DESCRIPTOR_LINKREL_M2M_WORD(source,    0, 2048, 0);

        source[0] = data;
        descriptor[0].xfer.srcInc = ldmaCtrlSrcIncNone;
        descriptor[1].xfer.srcInc = ldmaCtrlSrcIncNone;
        descriptor[1].xfer.dstAddrMode = ldmaCtrlDstAddrModeRel;
        descriptor[1].xfer.decLoopCnt = 1;
        descriptor[1].xfer.link = 0;          // Stop after looping
        descriptor[1].xfer.doneIfs = 1;

        dma_flag = 1;
        LDMA_StartTransfer(LDMA_MEM2MEM_CHANNEL, &xferCfg, &descriptor[0]);
    }
}

void fill_data16_dma(uint16_t data, uint16_t* dest, uint16_t count) {
  static uint32_t source[1];

    if ((count - 1) < 2048) {
        while (dma_flag) continue;
        xferCfg = (LDMA_TransferCfg_t) LDMA_TRANSFER_CFG_MEMORY();
        descriptor[0] = (LDMA_Descriptor_t) LDMA_DESCRIPTOR_SINGLE_M2M_HALF(source, dest, count);
        source[0] = data;
        descriptor[0].xfer.srcInc = ldmaCtrlSrcIncNone;
        dma_flag = 1;
        LDMA_StartTransfer(LDMA_MEM2MEM_CHANNEL, &xferCfg, &descriptor[0]);
    } else {
        while (dma_flag) continue;
        xferCfg = (LDMA_TransferCfg_t) LDMA_TRANSFER_CFG_MEMORY_LOOP((count / 2048)-1);
        descriptor[0] = (LDMA_Descriptor_t) LDMA_DESCRIPTOR_LINKREL_M2M_HALF(source, dest, ((count-1) % 2048) + 1, 1),
        descriptor[1] = (LDMA_Descriptor_t) LDMA_DESCRIPTOR_LINKREL_M2M_HALF(source,    0, 2048, 0);

        source[0] = data;
        descriptor[0].xfer.srcInc = ldmaCtrlSrcIncNone;
        descriptor[1].xfer.srcInc = ldmaCtrlSrcIncNone;
        descriptor[1].xfer.dstAddrMode = ldmaCtrlDstAddrModeRel;
        descriptor[1].xfer.decLoopCnt = 1;
        descriptor[1].xfer.link = 0;          // Stop after looping
        descriptor[1].xfer.doneIfs = 1;

        dma_flag = 1;
        LDMA_StartTransfer(LDMA_MEM2MEM_CHANNEL, &xferCfg, &descriptor[0]);
    }
}
