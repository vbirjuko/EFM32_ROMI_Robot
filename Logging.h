/*
 * Logging.h
 *
 *  Created on: 29 нояб. 2020 г.
 *      Author: wl
 */

#ifndef LOGGING_H_
#define LOGGING_H_

void data_log(uint16_t datatowrite, unsigned int write_enable);
void data_log_init(void);
void data_log_finish(void);
void add_data_log(int * element);
extern volatile int16_t add_log[12000][5];

enum where {
    node_idx = 0x00,
    Solve = 0x10,               // frame related
    Segment = 0x20,             // frame related
    Turn = 0x30,                // frame related
    Entrance = 0x40,
    coord_east_lsb = 0x1,
    coord_east_msb = 0x2,
    coord_north_lsb = 0x3,
    coord_north_msb = 0x4,
    segm_length_lsb = 0x5,
    segm_length_msb = 0x6,
};

#endif /* LOGGING_H_ */
