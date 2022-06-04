#ifndef TACHOMETR_H
#define TACHOMETR_H

#include "em_device.h"

//#define LeftSteps   ((int32_t)(WTIMER0->CNT))
#define LeftSteps   ((int16_t)(PCNT1->CNT))
#define RightSteps  ((int32_t)(WTIMER1->CNT))

#include <stdint.h>
void tachometer_init(void);
//extern volatile int LeftSteps, RightSteps;

enum TachDirection{
  FORWARD, /**< Wheel is making robot move forward */
  STOPPED, /**< Wheel is stopped */
  REVERSE  /**< Wheel is making robot move backward */
};

typedef struct {
    uint32_t Period;
    enum  		TachDirection  Dir;
} Tach_stru_t;

/**
 * Get the most recent tachometer measurements.
 * @param leftTach is pointer to store last measured tachometer period of left wheel (units of 0.083 usec)
 * @param leftDir is pointer to store enumerated direction of last movement of left wheel
 * @param leftSteps is pointer to store total number of forward steps measured for left wheel (360 steps per ~220 mm circumference)
 * @param rightTach is pointer to store last measured tachometer period of right wheel (units of 0.083 usec)
 * @param rightDir is pointer to store enumerated direction of last movement of right wheel
 * @param rightSteps is pointer to store total number of forward steps measured for right wheel (360 steps per ~220 mm circumference)
 * @return none
 * @note Assumes Tachometer_Init() has been called<br>
 * @note Assumes Clock_Init48MHz() has been called
 * @brief Get the most recent tachometer measurement
 */
/*void Tachometer_Get(uint16_t *leftTach, enum TachDirection *leftDir, int32_t *leftSteps,
                    uint16_t *rightTach, enum TachDirection *rightDir, int32_t *rightSteps); */
void Tachometer_Get(Tach_stru_t *leftTach, Tach_stru_t *rightTach);

void reset_steps(void);

#endif
