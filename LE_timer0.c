/*
 * LE_timer0.c
 *
 *  Created on: 7 янв. 2022 г.
 *      Author: wl
 */
#include "em_device.h"
#include "em_letimer.h"
#include "em_cmu.h"
#include "resources.h"

volatile unsigned int letimer_run = 0;

void le_timer_Init(void) {
    CMU_ClockEnable(cmuClock_LETIMER0, 1);

    LETIMER0->CTRL = LETIMER_CTRL_DEBUGRUN | LETIMER_CTRL_REPMODE_ONESHOT;
    LETIMER0->REP0 = 0;
    LETIMER0->IEN = LETIMER_IEN_REP0;
    NVIC_SetPriority(LETIMER0_IRQn, LETIMER0_IRQ_Priority);
    NVIC_EnableIRQ(LETIMER0_IRQn);
}

void LETIMER0_IRQHandler(void) {
    LETIMER0->IFC = LETIMER_IFC_REP0;
    letimer_run = 0;
}

unsigned int letimer_delay_ms(unsigned int delay_ms) {
    unsigned int count = (delay_ms * CMU_ClockFreqGet(cmuClock_LETIMER0)+500)/1000;
    unsigned int reminder;

    LETIMER0->CNT = reminder = count % 65536;
    LETIMER0->REP0 = (count / 65536) + ((reminder) ? 1 : 0);

    letimer_run = 1;
    LETIMER0->CMD = LETIMER_CMD_START;

    while (letimer_run)    continue;
    return 0;
}

#include "Motor.h"

void TestLeTimer(void) {
    do {
        Motor_Enable();
        letimer_delay_ms(950);
        Motor_Disable();
        for (volatile int ii = 0; ii <1000000; ii++)continue;
    } while(1);
}
