/*
 * crc.c
 *
 *  Created on: 29 апр. 2021 г.
 *      Author: wl
 */

#include "em_device.h"
#include "em_cmu.h"

#define CRC32_INIT  0xFFFFFFFF

volatile unsigned int dma_crc_busy = 0;

void crc32_init(void) {
  CMU_ClockEnable(cmuClock_GPCRC, 1);
  GPCRC->CTRL = GPCRC_CTRL_BYTEMODE | GPCRC_CTRL_EN;
}

uint32_t calc_crc32(uint8_t* ptr, uint32_t size) {

    uint32_t transferbytes;
    GPCRC->INIT = CRC32_INIT;
    GPCRC->CMD =  GPCRC_CMD_INIT;

    for (transferbytes = 0; transferbytes < size; transferbytes++) {
        GPCRC->INPUTDATABYTE = *ptr++;
    }
    return GPCRC->DATA;
}
