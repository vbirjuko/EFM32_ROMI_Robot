/*
 * UART0.c
 *
 *  Created on: 11 мар. 2021 г.
 *      Author: wl
 */
#include "em_device.h"
#include "em_gpio.h"
#include "em_cmu.h"
#include "resources.h"

#define RXBUFBITSIZE 8

#define RXBUFSIZE (1u << RXBUFBITSIZE)
static volatile uint8_t rxBuffer[RXBUFSIZE], txBuffer[RXBUFSIZE];
static volatile unsigned int rxWriteIndex=0, rxReadIndex=0;
static volatile unsigned int txWriteIndex=0, txReadIndex=0;

void UART0_Init(void) {

  GPIO_PinModeSet(gpioPortE, 6, gpioModeInputPull, 1);  // RX
  GPIO_PinModeSet(gpioPortE, 7, gpioModePushPull, 1);   // TX
  GPIO_PinModeSet(gpioPortB, 6, gpioModeInputPull, 0);  // CTS
  GPIO_PinModeSet(gpioPortB, 5, gpioModePushPull, 1);   // RTS

  GPIO_PinModeSet(gpioPortA, 15, gpioModePushPull, 1);  // enable link mcu <-> debug

  CMU_ClockEnable(cmuClock_USART0, 1);

  USART0->CTRL = USART_CTRL_OVS_X16 ;
//  USART0->CLKDIV = 0x1a20;
  {
    unsigned int refFreq, clkdiv;
    const unsigned int oversample = 16, baudrate = 115200;
    refFreq = CMU_ClockFreqGet(cmuClock_HFPER);
    clkdiv  = 32 * refFreq + (oversample * baudrate) / 2;
    clkdiv /= oversample * baudrate;
    clkdiv -= 32;
    clkdiv *= 8;
    USART0->CLKDIV = clkdiv;
  }
  USART0->ROUTEPEN = USART_ROUTEPEN_TXPEN | USART_ROUTEPEN_RXPEN | USART_ROUTEPEN_CTSPEN | USART_ROUTEPEN_RTSPEN;
  USART0->ROUTELOC0 = USART_ROUTELOC0_RXLOC_LOC1 | USART_ROUTELOC0_TXLOC_LOC1;
  USART0->ROUTELOC1 = USART_ROUTELOC1_CTSLOC_LOC4 | USART_ROUTELOC1_RTSLOC_LOC4;
  USART0->FRAME = USART_FRAME_DATABITS_EIGHT | USART_FRAME_STOPBITS_ONE | USART_FRAME_PARITY_NONE;
  USART0->CMD = USART_CMD_TXEN | USART_CMD_RXEN | USART_CMD_CLEARRX | USART_CMD_CLEARTX;
  USART0->IEN = USART_IEN_RXDATAV;
//  USART0->CTRLX  = USART_CTRLX_CTSEN;

  NVIC_EnableIRQ(USART0_TX_IRQn);
  NVIC_SetPriority(USART0_TX_IRQn, USART0_IRQ_PRI);
  NVIC_EnableIRQ(USART0_RX_IRQn);
  NVIC_SetPriority(USART0_RX_IRQn, USART0_IRQ_PRI);
}

unsigned int UART0_Out(uint8_t data) {
  if (((txWriteIndex + 1) & (RXBUFSIZE - 1)) == txReadIndex) return 1;

//  CORE_CriticalDisableIrq();
  txBuffer[txWriteIndex++] = data;
  txWriteIndex &= (RXBUFSIZE - 1);
//  CORE_CriticalEnableIrq();
  BUS_RegMaskedSet(&USART0->IEN, USART_IEN_TXBL);
  return 0;
}

void USART0_TX_IRQHandler(void) {
  if (USART0->IF & USART_IF_TXBL) {
      if (txReadIndex != (txWriteIndex & (RXBUFSIZE - 1))) {
          USART0->TXDATA = txBuffer[txReadIndex++];
          txReadIndex &= (RXBUFSIZE - 1);
          if (txReadIndex == txWriteIndex) BUS_RegMaskedClear(&USART0->IEN, USART_IEN_TXBL);
      }
  }
}

void USART0_RX_IRQHandler(void) {
  if (USART0->STATUS & USART_STATUS_RXDATAV) {
      if (((rxWriteIndex + 1) & (RXBUFSIZE - 1)) == (rxReadIndex & (RXBUFBITSIZE - 1))){
      /* The RX buffer is full so we must wait for the RETARGET_ReadChar()
       * function to make some more room in the buffer. RX interrupts are
       * disabled to let the ISR exit. The RX interrupt will be enabled in
       * read_char() */
          BUS_RegMaskedClear(&USART0->IEN, USART_IEN_RXDATAV);
      } else {
          /* There is room for data in the RX buffer so we store the data. */
          rxBuffer[rxWriteIndex++] = (uint8_t)USART0->RXDATA;
          rxWriteIndex &= (RXBUFSIZE - 1);
      }
  }
}

unsigned int UART0_In(uint8_t * data) {
  if (rxReadIndex != rxWriteIndex) {
//      CORE_CriticalDisableIrq();
      *data = rxBuffer[rxReadIndex++];
      rxReadIndex &= (RXBUFSIZE - 1);
//      CORE_CriticalEnableIrq();
      BUS_RegMaskedSet(&USART0->IEN, USART_IEN_RXDATAV);
      return 0;
  }else {
      return -1;
  }
}


void UART0_OutChar(uint8_t data){
  while (UART0_Out(data)) continue;
}

//------------UART0_OutString------------
// Output String (NULL termination)
// Input: pointer to a NULL-terminated string to be transferred
// Output: none
void UART0_OutString(char *pt){
  while(*pt){
    UART0_OutChar(*pt);
    pt++;
  }
}

//-----------------------UART0_OutUDec-----------------------
// Output a 32-bit number in unsigned decimal format
// Input: 32-bit number to be transferred
// Output: none
// Variable format 1-10 digits with no space before or after
void UART0_OutUDec(uint32_t n){
// This function uses recursion to convert decimal number
//   of unspecified length as an ASCII string
  if(n >= 10){
    UART0_OutUDec(n/10);
    n = n%10;
  }
  UART0_OutChar(n+'0'); /* n is between 0 and 9 */
}

