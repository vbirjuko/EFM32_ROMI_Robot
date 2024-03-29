#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

#include "resources.h"

// Функция опроса клавиатуры



#define I2C_PCA_ADDR	0x82
enum pca_reg {
	input_port = 0x00,
	output_port = 0x01,
	polarity_inversion = 0x02,
	configuration = 0x03,
};

typedef enum {
    DEBOUNCE_DELAY = (SCANPERSECOND*3/100), // 20 ms - подавление дребезга
    REPEAT_DELAY   = (SCANPERSECOND*3/4),   // 0.75c - задержка перед повтором
    REPEAT_PERIOD  = (SCANPERSECOND/3)      // 0.33c - период автоповтора
} KEY_DELAYS;

#if defined(RSLK_MAX)
#ifdef PCA
#define KEY_UP        	(1u << 0)
#define KEY_DOWN        (1u << 2)
#define KEY_SET         (1u << 1)
#elif defined(KEYB_PORT)
#define KEY_UP        (1u << 0)
#define KEY_DOWN        (1u << 2)
#define KEY_SET         (1u << 3)
#define KEY_PORT        P10
#endif
#else
#define KEY_UP        (1u << 0)
#define KEY_DOWN       (1u << 2)
#define KEY_SET         (1u << 1)
#define KEY_PORT        P5
#endif

#define KEY_LEFT			(1u << 8)
#define KEY_RIGHT			(1u << 9)

#define KEY_MASK        (KEY_UP | KEY_DOWN | KEY_SET)
#define HOLDED          (1u << 7)
#define RELEASE         (1u << 6)

unsigned int kbdread(void);
void kbd_init(void);
extern volatile unsigned int kbddelay;

#endif
