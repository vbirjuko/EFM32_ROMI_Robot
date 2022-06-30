#ifndef TACHOMETR_H
#define TACHOMETR_H

#include "em_device.h"

//#define LeftSteps   ((int32_t)(WTIMER0->CNT))
#define LeftSteps   ((int16_t)(PCNT1->CNT))
#define RightSteps  ((int16_t)(PCNT2->CNT))

#include <stdint.h>
void tachometer_init(void);
//extern volatile int LeftSteps, RightSteps;

enum TachDirection{
  STOPPED, /**< Wheel is stopped */
  FORWARD, /**< Wheel is making robot move forward */
  REVERSE  /**< Wheel is making robot move backward */
};

typedef struct {
    uint32_t Period;
    enum  		TachDirection  Dir;
} Tach_stru_t;

void Tachometer_Get(Tach_stru_t *leftTach, Tach_stru_t *rightTach);
void reset_steps(void);

void TestTachom(void);

#endif
