#include "maze.h"
#include "spi_drv.h"
#include "crc.h"

const map_cell_t my_maze[] = {
{{0,0},{1,UNKNOWN,UNKNOWN,UNKNOWN},{2,4,4,4}},
{{0,531},{UNKNOWN,UNKNOWN,0,2},{4,4,2,2}},
{{-161,531},{UNKNOWN,1,6,3},{4,2,2,2}},
{{-329,531},{UNKNOWN,2,4,UNKNOWN},{4,2,2,4}},
{{-329,233},{3,6,5,UNKNOWN},{2,2,2,4}},
{{-329,70},{4,UNKNOWN,UNKNOWN,UNKNOWN},{2,4,4,4}},
{{-177,233},{2,UNKNOWN,7,4},{2,4,2,2}},
{{-177,-105},{6,UNKNOWN,UNKNOWN,8},{2,4,4,2}},
{{-327,-105},{UNKNOWN,7,UNKNOWN,UNKNOWN},{4,2,4,4}}
};

const map_cell_t youtube_data[] = {
{{0,0},{11,1,UNKNOWN,UNKNOWN},{0,0,4,4}},
{{2,0},{13,2,UNKNOWN,0},{0,0,4,0}},
{{3,0},{14,3,UNKNOWN,1},{0,0,4,0}},
{{4,0},{15,4,UNKNOWN,2},{0,0,4,0}},
{{5,0},{16,5,UNKNOWN,3},{0,0,4,0}},
{{6,0},{17,6,UNKNOWN,4},{0,0,4,0}},
{{7,0},{18,UNKNOWN,UNKNOWN,5},{0,4,4,0}},
{{9,0},{20,8,UNKNOWN,UNKNOWN},{0,0,4,4}},
{{11,0},{21,9,UNKNOWN,7},{0,0,4,0}},
{{13,0},{23,10,UNKNOWN,8},{0,0,4,0}},
{{14,0},{UNKNOWN,UNKNOWN,UNKNOWN,9},{4,4,4,0}},
{{0,1},{25,12,0,UNKNOWN},{0,0,0,4}},
{{1,1},{26,UNKNOWN,UNKNOWN,11},{0,4,4,0}},
{{2,1},{27,14,1,UNKNOWN},{0,0,0,4}},
{{3,1},{28,UNKNOWN,2,13},{0,4,0,0}},
{{4,1},{29,16,3,UNKNOWN},{0,0,0,4}},
{{5,1},{30,17,4,15},{0,0,0,0}},
{{6,1},{31,18,5,16},{0,0,0,0}},
{{7,1},{45,19,6,17},{0,0,0,0}},
{{8,1},{32,20,UNKNOWN,18},{0,0,4,0}},
{{9,1},{33,21,7,19},{0,0,0,0}},
{{11,1},{UNKNOWN,UNKNOWN,8,20},{4,4,0,0}},
{{12,1},{36,23,UNKNOWN,UNKNOWN},{0,0,4,4}},
{{13,1},{UNKNOWN,UNKNOWN,9,22},{4,4,0,0}},
{{14,1},{38,UNKNOWN,UNKNOWN,UNKNOWN},{0,4,4,4}},
{{0,2},{UNKNOWN,26,11,UNKNOWN},{4,0,0,4}},
{{1,2},{40,27,12,25},{0,0,0,0}},
{{2,2},{41,28,13,26},{0,0,0,0}},
{{3,2},{42,29,14,27},{0,0,0,0}},
{{4,2},{UNKNOWN,30,15,28},{4,0,0,0}},
{{5,2},{44,31,16,29},{0,0,0,0}},
{{6,2},{UNKNOWN,UNKNOWN,17,30},{4,4,0,0}},
{{8,2},{UNKNOWN,33,19,UNKNOWN},{4,0,0,4}},
{{9,2},{47,34,20,32},{0,0,0,0}},
{{10,2},{UNKNOWN,UNKNOWN,UNKNOWN,33},{4,4,4,0}},
{{11,2},{UNKNOWN,36,UNKNOWN,UNKNOWN},{4,0,4,4}},
{{12,2},{49,UNKNOWN,22,35},{0,4,0,0}},
{{13,2},{50,38,UNKNOWN,UNKNOWN},{0,0,4,4}},
{{14,2},{89,UNKNOWN,24,37},{0,4,0,0}},
{{0,3},{64,40,UNKNOWN,UNKNOWN},{0,0,4,4}},
{{1,3},{UNKNOWN,41,26,39},{4,0,0,0}},
{{2,3},{52,UNKNOWN,27,40},{0,4,0,0}},
{{3,3},{53,43,28,UNKNOWN},{0,0,0,4}},
{{4,3},{54,44,UNKNOWN,42},{0,0,4,0}},
{{5,3},{55,45,30,43},{0,0,0,0}},
{{7,3},{57,46,18,44},{0,0,0,0}},
{{8,3},{UNKNOWN,UNKNOWN,UNKNOWN,45},{4,4,4,0}},
{{9,3},{UNKNOWN,48,33,UNKNOWN},{4,0,0,4}},
{{10,3},{60,UNKNOWN,UNKNOWN,47},{0,4,4,0}},
{{12,3},{UNKNOWN,50,36,UNKNOWN},{4,0,0,4}},
{{13,3},{62,UNKNOWN,37,49},{0,4,0,0}},
{{1,4},{65,UNKNOWN,UNKNOWN,UNKNOWN},{0,4,4,4}},
{{2,4},{UNKNOWN,53,41,UNKNOWN},{4,0,0,4}},
{{3,4},{67,54,42,52},{0,0,0,0}},
{{4,4},{68,55,43,53},{0,0,0,0}},
{{5,4},{UNKNOWN,56,44,54},{4,0,0,0}},
{{6,4},{70,UNKNOWN,UNKNOWN,55},{0,4,4,0}},
{{7,4},{71,58,45,UNKNOWN},{0,0,0,4}},
{{8,4},{72,59,UNKNOWN,57},{0,0,4,0}},
{{9,4},{73,60,UNKNOWN,58},{0,0,4,0}},
{{10,4},{UNKNOWN,UNKNOWN,48,59},{4,4,0,0}},
{{11,4},{85,62,UNKNOWN,UNKNOWN},{0,0,4,4}},
{{13,4},{UNKNOWN,63,50,61},{4,0,0,0}},
{{14,4},{88,UNKNOWN,UNKNOWN,62},{0,4,4,0}},
{{0,5},{77,65,39,UNKNOWN},{0,0,0,4}},
{{1,5},{78,UNKNOWN,51,64},{0,4,0,0}},
{{2,5},{79,67,UNKNOWN,UNKNOWN},{0,0,4,4}},
{{3,5},{80,68,53,66},{0,0,0,0}},
{{4,5},{81,69,54,67},{0,0,0,0}},
{{5,5},{UNKNOWN,UNKNOWN,UNKNOWN,68},{4,4,4,0}},
{{6,5},{82,71,56,UNKNOWN},{0,0,0,4}},
{{7,5},{UNKNOWN,UNKNOWN,57,70},{4,4,0,0}},
{{8,5},{83,73,58,UNKNOWN},{0,0,0,4}},
{{9,5},{84,74,59,72},{0,0,0,0}},
{{10,5},{UNKNOWN,UNKNOWN,UNKNOWN,73},{4,4,4,0}},
{{12,5},{86,UNKNOWN,UNKNOWN,UNKNOWN},{0,4,4,4}},
{{13,5},{87,UNKNOWN,UNKNOWN,UNKNOWN},{0,4,4,4}},
{{0,6},{UNKNOWN,UNKNOWN,64,UNKNOWN},{4,4,0,4}},
{{1,6},{UNKNOWN,79,65,UNKNOWN},{4,0,0,4}},
{{2,6},{UNKNOWN,80,66,78},{4,0,0,0}},
{{3,6},{UNKNOWN,UNKNOWN,67,79},{4,4,0,0}},
{{4,6},{UNKNOWN,82,68,UNKNOWN},{4,0,0,4}},
{{6,6},{UNKNOWN,83,70,81},{4,0,0,0}},
{{8,6},{UNKNOWN,UNKNOWN,72,82},{4,4,0,0}},
{{9,6},{UNKNOWN,UNKNOWN,73,UNKNOWN},{4,4,0,4}},
{{11,6},{UNKNOWN,86,61,UNKNOWN},{4,0,0,4}},
{{12,6},{UNKNOWN,87,75,85},{4,0,0,0}},
{{13,6},{UNKNOWN,88,76,86},{4,0,0,0}},
{{14,6},{UNKNOWN,UNKNOWN,63,87},{4,4,0,0}},
{{14,3},{UNKNOWN,UNKNOWN,38,UNKNOWN},{4,4,0,4}}
};

const map_cell_t sigulda_data[] = {
{{0, 0},{ 1, UNKNOWN, UNKNOWN, UNKNOWN},{ 0, 4, 4, 4}},
{{0, 1},{ UNKNOWN, UNKNOWN, 0, 12},{ 4, 4, 0, 0}},
{{0, 2},{ 3, UNKNOWN, UNKNOWN, 13},{ 0, 4, 4, 0}},
{{0, 3},{ UNKNOWN, UNKNOWN, 2, UNKNOWN},{ 4, 4, 0, 4}},
{{0, 4},{ 5, UNKNOWN, UNKNOWN, 15},{ 0, 4, 4, 0}},
{{0, 6},{ UNKNOWN, UNKNOWN, 4, 17},{ 4, 4, 0, 0}},
{{0, 7},{ UNKNOWN, UNKNOWN, UNKNOWN, 18},{ 4, 4, 4, 0}},
{{0, 8},{ UNKNOWN, UNKNOWN, UNKNOWN, 19},{ 4, 4, 4, 0}},
{{0, 9},{ 9, UNKNOWN, UNKNOWN, UNKNOWN},{ 0, 4, 4, 4}},
{{0, 10},{ 10, UNKNOWN, 8, 21},{ 0, 4, 0, 0}},
{{0, 12},{ UNKNOWN, UNKNOWN, 9, 23},{ 4, 4, 0, 0}},
{{0, 13},{ UNKNOWN, UNKNOWN, UNKNOWN, 31},{ 4, 4, 4, 0}},
{{-1, 1},{ 13, 1, UNKNOWN, 24},{ 0, 0, 4, 0}},
{{-1, 2},{ UNKNOWN, 2, 12, UNKNOWN},{ 4, 0, 0, 4}},
{{-1, 3},{ 15, UNKNOWN, UNKNOWN, UNKNOWN},{ 0, 4, 4, 4}},
{{-1, 4},{ UNKNOWN, 4, 14, 25},{ 4, 0, 0, 0}},
{{-1, 5},{ 17, UNKNOWN, UNKNOWN, UNKNOWN},{ 0, 4, 4, 4}},
{{-1, 6},{ 18, 5, 16, 27},{ 0, 0, 0, 0}},
{{-1, 7},{ 19, 6, 17, UNKNOWN},{ 0, 0, 0, 4}},
{{-1, 8},{ UNKNOWN, 7, 18, UNKNOWN},{ 4, 0, 0, 4}},
{{-1, 9},{ 21, UNKNOWN, UNKNOWN, UNKNOWN},{ 0, 4, 4, 4}},
{{-1, 10},{ UNKNOWN, 9, 20, 28},{ 4, 0, 0, 0}},
{{-1, 11},{ 23, UNKNOWN, UNKNOWN, 29},{ 0, 4, 4, 0}},
{{-1, 12},{ UNKNOWN, 10, 22, 30},{ 4, 0, 0, 0}},
{{-2, 1},{ 25, 12, UNKNOWN, UNKNOWN},{ 0, 0, 4, 4}},
{{-2, 4},{ 26, 15, 24, UNKNOWN},{ 0, 0, 0, 4}},
{{-2, 5},{ UNKNOWN, UNKNOWN, 25, UNKNOWN},{ 4, 4, 0, 4}},
{{-2, 6},{ 28, 17, UNKNOWN, UNKNOWN},{ 0, 0, 4, 4}},
{{-2, 10},{ UNKNOWN, 21, 27, UNKNOWN},{ 4, 0, 0, 4}},
{{-2, 11},{ UNKNOWN, 22, UNKNOWN, UNKNOWN},{ 4, 0, 4, 4}},
{{-2, 12},{ 31, 23, UNKNOWN, UNKNOWN},{ 0, 0, 4, 4}},
{{-2, 13},{ UNKNOWN, 11, 30, 34},{ 4, 0, 0, 0}},
{{-2, 0},{ UNKNOWN, UNKNOWN, UNKNOWN, 33},{ 4, 4, 4, 0}},
{{-3, 0},{ 34, 32, UNKNOWN, 35},{ 0, 0, 4, 0}},
{{-3, 13},{ UNKNOWN, 31, 33, UNKNOWN},{ 4, 0, 0, 4}},
{{-4, 0},{ 36, 33, UNKNOWN, UNKNOWN},{ 0, 0, 4, 4}},
{{-4, 1},{ 37, UNKNOWN, 35, 48},{ 0, 4, 0, 0}},
{{-4, 3},{ UNKNOWN, UNKNOWN, 36, UNKNOWN},{ 4, 4, 0, 4}},
{{-4, 4},{ UNKNOWN, UNKNOWN, UNKNOWN, 51},{ 4, 4, 4, 0}},
{{-4, 5},{ 40, UNKNOWN, UNKNOWN, UNKNOWN},{ 0, 4, 4, 4}},
{{-4, 6},{ 41, UNKNOWN, 39, 53},{ 0, 4, 0, 0}},
{{-4, 8},{ UNKNOWN, UNKNOWN, 40, 55},{ 4, 4, 0, 0}},
{{-4, 9},{ UNKNOWN, UNKNOWN, UNKNOWN, 56},{ 4, 4, 4, 0}},
{{-4, 10},{ 44, UNKNOWN, UNKNOWN, 57},{ 0, 4, 4, 0}},
{{-4, 11},{ UNKNOWN, UNKNOWN, 43, UNKNOWN},{ 4, 4, 0, 4}},
{{-4, 12},{ 46, UNKNOWN, UNKNOWN, 59},{ 0, 4, 4, 0}},
{{-4, 13},{ UNKNOWN, UNKNOWN, 45, UNKNOWN},{ 4, 4, 0, 4}},
{{-5, 0},{ 48, UNKNOWN, UNKNOWN, UNKNOWN},{ 0, 4, 4, 4}},
{{-5, 1},{ 49, 36, 47, 61},{ 0, 0, 0, 0}},
{{-5, 2},{ 50, UNKNOWN, 48, 62},{ 0, 4, 0, 0}},
{{-5, 3},{ 51, UNKNOWN, 49, 63},{ 0, 4, 0, 0}},
{{-5, 4},{ 52, 38, 50, 64},{ 0, 0, 0, 0}},
{{-5, 5},{ 53, UNKNOWN, 51, 65},{ 0, 4, 0, 0}},
{{-5, 6},{ UNKNOWN, 40, 52, 66},{ 4, 0, 0, 0}},
{{-5, 7},{ UNKNOWN, UNKNOWN, UNKNOWN, 67},{ 4, 4, 4, 0}},
{{-5, 8},{ 56, 41, UNKNOWN, 68},{ 0, 0, 4, 0}},
{{-5, 9},{ 57, 42, 55, 69},{ 0, 0, 0, 0}},
{{-5, 10},{ 58, 43, 56, UNKNOWN},{ 0, 0, 0, 4}},
{{-5, 11},{ 59, UNKNOWN, 57, 71},{ 0, 4, 0, 0}},
{{-5, 12},{ 60, 45, 58, UNKNOWN},{ 0, 0, 0, 4}},
{{-5, 13},{ UNKNOWN, UNKNOWN, 59, 73},{ 4, 4, 0, 0}},
{{-6, 1},{ UNKNOWN, 48, UNKNOWN, UNKNOWN},{ 4, 0, 4, 4}},
{{-6, 2},{ UNKNOWN, 49, UNKNOWN, UNKNOWN},{ 4, 0, 4, 4}},
{{-6, 3},{ UNKNOWN, 50, UNKNOWN, UNKNOWN},{ 4, 0, 4, 4}},
{{-6, 4},{ UNKNOWN, 51, UNKNOWN, UNKNOWN},{ 4, 0, 4, 4}},
{{-6, 5},{ UNKNOWN, 52, UNKNOWN, UNKNOWN},{ 4, 0, 4, 4}},
{{-6, 6},{ 67, 53, UNKNOWN, UNKNOWN},{ 0, 0, 4, 4}},
{{-6, 7},{ UNKNOWN, 54, 66, UNKNOWN},{ 4, 0, 0, 4}},
{{-6, 8},{ UNKNOWN, 55, UNKNOWN, UNKNOWN},{ 4, 0, 4, 4}},
{{-6, 9},{ 70, 56, UNKNOWN, UNKNOWN},{ 0, 0, 4, 4}},
{{-6, 10},{ UNKNOWN, UNKNOWN, 69, UNKNOWN},{ 4, 4, 0, 4}},
{{-6, 11},{ 72, 58, UNKNOWN, UNKNOWN},{ 0, 0, 4, 4}},
{{-6, 12},{ UNKNOWN, UNKNOWN, 71, UNKNOWN},{ 4, 4, 0, 4}},
{{-6, 13},{ UNKNOWN, 60, UNKNOWN, UNKNOWN},{ 4, 0, 4, 4}},
};

typedef struct {
    const map_cell_t *    pre_map_data;
    int             sq_data;
    int             map_size;
    int             map_start_cell;
    int             map_end_cell;
} alt_data_t;

alt_data_t  inject_map[] = {
    {sigulda_data, 0x00100d6, sizeof(sigulda_data)/sizeof(sigulda_data[0]),      0, 73},
    {youtube_data, 0x0000c5e6, sizeof(youtube_data)/sizeof(youtube_data[0]),    75, 51},
    {my_maze,      0x00000000, sizeof(my_maze)/sizeof(my_maze[0]),              0, 8}
};

unsigned int inject(unsigned int select) {
    int i;
    if (data.cell_step == 0) {
        data.cell_step = 300;
    }
    for (i = 0; i < inject_map[select].map_size; i++) {
        map[i] = inject_map[select].pre_map_data[i];
        map[i].coordinate.east *= data.cell_step;
        map[i].coordinate.north *= data.cell_step;
    }
    spi_write_eeprom(ROM_map_addr, (unsigned char *) &map, inject_map[select].map_size*sizeof(map_cell_t));
    data.map_size       = inject_map[select].map_size;
    data.red_cell_nr    = inject_map[select].map_end_cell;
    data.green_cell_nr  = inject_map[select].map_start_cell;
    data.pathlength     = 0;
    data.sq_init        = inject_map[select].sq_data;
    return 0;
}

// Функция конвертирует данные из программы winmaze в формат лабиринта из линий
// Каждой клетке в соответствие ставится узел, кроме "туннельных" клеток (0x0A и 0x05).
// Для приведения соответствия используется массив linkarray. Но так как он заполняется
// по мере обработки клеточек (с юга на север и с запада на восток), в нем нет данных
// о будущих узлах. Поэтому линки с запада на восток заполняеются по достижении восточной
// клетки. клетка с севера всегда будет на 1 больше текущей. Для обеспечения такого
// функционала "туннельным" клеткам присваивается индекс клетки находящейся южнее, если
// стенки с запада и востока (0x0A) или западной, если стенки с севера и юга.
//
//   | 2   8 | 12| 20|
//   +   +---+   +---+
//   | 1   1   12  19|
//   |   +-------+   |
//   | 0 | 7   7   18|
//   |   +   +---+   |
//   | 0 | 6 | 11  17|
//   +---+---+---+---+---

unsigned int  convert_maze(uint8_t * mazfile) {
    unsigned int ii, jj, index = 0;
    uint8_t linkarray[16*16];
    map_cell_t *map_ptr;
    map_ptr = map;
    if (data.cell_step == 0) return 1;
    for (jj = 0; jj < 16; jj++) {
        for (ii = 0; ii < 16; ii++){
            if (((*mazfile & 0x0F) == 0x0a) && ii) {
                linkarray[ii+(jj*16)] = linkarray[ii-1 + (jj*16)];
                mazfile++;
                continue;
            }
            if (((*mazfile & 0x0F) == 0x05) && jj) {
                linkarray[ii+(jj*16)] = linkarray[ii + ((jj-1)*16)];
                mazfile++;
                continue;
            }

            linkarray[ii+(jj*16)] = index;
            map_ptr->coordinate.north = data.cell_step * ii;
            map_ptr->coordinate.east  = data.cell_step * jj;
// north
            if (*mazfile & 0x01) {
                map_ptr->node_link[0] = UNKNOWN;
                map_ptr->pass_count[0] = 4;
            } else {
                map_ptr->node_link[0] = index+1;
                map_ptr->pass_count[0] = 0;
            }
// east
            if (*mazfile & 0x02) {
                map_ptr->node_link[1] = UNKNOWN;
                map_ptr->pass_count[1] = 4;
            } else {
                map_ptr->node_link[1] = linkarray[ii + ((jj+1) * 16)]; // error
                map_ptr->pass_count[1] = 0;
            }
//south
            if (*mazfile & 0x04) {
                map_ptr->node_link[2] = UNKNOWN;
                map_ptr->pass_count[2] = 4;
            } else {
                map_ptr->node_link[2] = linkarray[ii-1 + (jj * 16)];
                map_ptr->pass_count[2] = 0;
                if (ii) {
                    map[linkarray[ii-1 + (jj*16)]].node_link[0] = index;
                }
            }
// west
            if (*mazfile & 0x08) {
                map_ptr->node_link[3] = UNKNOWN;
                map_ptr->pass_count[3] = 4;
            } else {
                map_ptr->node_link[3] = linkarray[ii + ((jj-1) * 16)];
                map_ptr->pass_count[3] = 0;
                if (jj) {
                    map[linkarray[ii + ((jj-1)*16)]].node_link[1] = index;
                }
            }
            mazfile++;
            map_ptr++;
            if (++index > MAX_MAP_SIZE) { // лабиринт не влезает в отведенную память
                data.map_size = 0;
                data.pathlength = 0;
                return 1;
            }
        }
    }
    spi_write_eeprom(ROM_map_addr, (unsigned char *) map, sizeof(map_cell_t)*index);
    data.map_size       = index;
    data.red_cell_nr    = 0;
    data.green_cell_nr  = 0;
    data.pathlength     = 0;
    data.sq_init        = 0x000000FF;
    return 0;
}

// array size is 256
const uint8_t APEC88[]  = {
  0x0e, 0x0a, 0x08, 0x08, 0x0a, 0x0a, 0x0a, 0x08, 0x0a, 0x08, 0x08, 0x0a, 0x0a, 0x0a, 0x08, 0x09,
  0x0c, 0x0a, 0x03, 0x06, 0x09, 0x0e, 0x08, 0x02, 0x0a, 0x01, 0x05, 0x0e, 0x0a, 0x0a, 0x03, 0x05,
  0x06, 0x0a, 0x09, 0x0c, 0x03, 0x0c, 0x02, 0x09, 0x0c, 0x03, 0x07, 0x0c, 0x0a, 0x0a, 0x08, 0x03,
  0x0c, 0x08, 0x01, 0x06, 0x09, 0x06, 0x09, 0x06, 0x03, 0x0c, 0x0a, 0x00, 0x0a, 0x0a, 0x02, 0x09,
  0x05, 0x05, 0x05, 0x0d, 0x04, 0x0b, 0x06, 0x09, 0x0e, 0x01, 0x0d, 0x06, 0x08, 0x08, 0x09, 0x05,
  0x06, 0x03, 0x04, 0x02, 0x02, 0x09, 0x0e, 0x02, 0x0a, 0x03, 0x04, 0x09, 0x05, 0x05, 0x07, 0x05,
  0x0c, 0x09, 0x06, 0x0a, 0x08, 0x03, 0x0c, 0x08, 0x0a, 0x09, 0x05, 0x05, 0x05, 0x06, 0x09, 0x05,
  0x05, 0x05, 0x0c, 0x09, 0x04, 0x09, 0x05, 0x04, 0x09, 0x05, 0x05, 0x04, 0x03, 0x0c, 0x03, 0x05,
  0x04, 0x02, 0x03, 0x06, 0x03, 0x07, 0x05, 0x06, 0x03, 0x05, 0x05, 0x05, 0x0d, 0x05, 0x0e, 0x03,
  0x04, 0x09, 0x0c, 0x0a, 0x0a, 0x0b, 0x06, 0x08, 0x0a, 0x01, 0x05, 0x07, 0x05, 0x06, 0x0a, 0x09,
  0x07, 0x05, 0x04, 0x0b, 0x0c, 0x0a, 0x09, 0x05, 0x0d, 0x05, 0x06, 0x0a, 0x00, 0x0a, 0x0a, 0x03,
  0x0d, 0x04, 0x03, 0x0c, 0x03, 0x0d, 0x06, 0x03, 0x05, 0x04, 0x09, 0x0d, 0x06, 0x0a, 0x0a, 0x09,
  0x04, 0x03, 0x0c, 0x03, 0x0c, 0x01, 0x0d, 0x0d, 0x06, 0x01, 0x05, 0x04, 0x09, 0x0c, 0x08, 0x03,
  0x04, 0x09, 0x04, 0x0a, 0x03, 0x04, 0x00, 0x02, 0x0b, 0x05, 0x05, 0x05, 0x05, 0x05, 0x04, 0x09,
  0x05, 0x05, 0x06, 0x0a, 0x0a, 0x03, 0x06, 0x0a, 0x09, 0x06, 0x00, 0x03, 0x06, 0x02, 0x03, 0x05,
  0x06, 0x02, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x02, 0x0b, 0x06, 0x0a, 0x0a, 0x0a, 0x0a, 0x03
};

const unsigned char world1st[256] ={
    0x0e,0x0a,0x09,0x0c,0x0a,0x0a,0x0a,0x0a,0x0a,0x0a,0x08,0x0a,0x0a,0x0a,0x08,0x09,
    0x0c,0x09,0x05,0x06,0x08,0x0a,0x0a,0x0a,0x0a,0x0b,0x06,0x0a,0x0a,0x0a,0x03,0x05,
    0x05,0x05,0x05,0x0c,0x02,0x0b,0x0e,0x08,0x0a,0x0a,0x08,0x0a,0x08,0x08,0x09,0x05,
    0x05,0x04,0x01,0x06,0x08,0x0a,0x09,0x04,0x0a,0x0a,0x00,0x0a,0x03,0x05,0x05,0x05,
    0x05,0x05,0x04,0x09,0x06,0x09,0x05,0x04,0x0a,0x0a,0x02,0x0a,0x0b,0x05,0x05,0x05,
    0x05,0x04,0x03,0x06,0x0a,0x02,0x03,0x06,0x0a,0x0a,0x0a,0x0a,0x09,0x05,0x05,0x05,
    0x05,0x05,0x0d,0x0d,0x0d,0x0c,0x08,0x0a,0x0a,0x0a,0x0a,0x09,0x05,0x05,0x05,0x05,
    0x06,0x03,0x04,0x01,0x04,0x01,0x05,0x0c,0x09,0x0c,0x08,0x01,0x05,0x05,0x05,0x05,
    0x0c,0x08,0x01,0x06,0x01,0x05,0x04,0x02,0x03,0x05,0x05,0x05,0x05,0x05,0x05,0x05,
    0x05,0x05,0x05,0x0d,0x06,0x01,0x05,0x0c,0x0a,0x01,0x05,0x05,0x05,0x05,0x05,0x05,
    0x05,0x05,0x05,0x04,0x09,0x06,0x03,0x06,0x0a,0x02,0x00,0x03,0x05,0x04,0x03,0x05,
    0x05,0x04,0x03,0x05,0x05,0x0c,0x0a,0x0a,0x08,0x09,0x04,0x0a,0x01,0x05,0x0d,0x05,
    0x05,0x05,0x0d,0x05,0x05,0x04,0x0a,0x08,0x03,0x05,0x06,0x0a,0x03,0x05,0x04,0x01,
    0x05,0x05,0x04,0x01,0x04,0x03,0x0c,0x02,0x0b,0x06,0x08,0x0a,0x0a,0x03,0x05,0x05,
    0x05,0x06,0x01,0x07,0x06,0x08,0x02,0x0a,0x0a,0x0b,0x06,0x08,0x0a,0x0a,0x00,0x01,
    0x06,0x0a,0x02,0x0a,0x0a,0x02,0x0b,0x0e,0x0a,0x0a,0x0a,0x02,0x0a,0x0a,0x03,0x07
};

const unsigned char kankou2003[256] ={
    0x0e,0x0a,0x0a,0x08,0x0a,0x0a,0x0a,0x08,0x0a,0x0a,0x08,0x0a,0x08,0x0a,0x08,0x09,
    0x0c,0x0a,0x08,0x02,0x0a,0x0b,0x0f,0x05,0x0e,0x0b,0x06,0x09,0x06,0x09,0x05,0x05,
    0x04,0x09,0x05,0x0c,0x0b,0x0e,0x0a,0x02,0x08,0x0a,0x0b,0x04,0x0b,0x04,0x01,0x05,
    0x05,0x05,0x05,0x04,0x0a,0x0a,0x0a,0x09,0x05,0x0c,0x0a,0x03,0x0f,0x05,0x06,0x03,
    0x05,0x05,0x05,0x06,0x0b,0x0f,0x0d,0x07,0x04,0x03,0x0e,0x08,0x0a,0x03,0x0f,0x0d,
    0x05,0x05,0x06,0x0a,0x08,0x0a,0x02,0x0a,0x03,0x0e,0x0b,0x06,0x0a,0x0a,0x0a,0x01,
    0x05,0x06,0x0a,0x09,0x06,0x0a,0x08,0x0b,0x0c,0x0a,0x09,0x0c,0x0a,0x0a,0x09,0x05,
    0x04,0x08,0x09,0x06,0x0a,0x09,0x05,0x0c,0x01,0x0f,0x05,0x07,0x0e,0x09,0x05,0x05,
    0x05,0x05,0x04,0x0a,0x09,0x05,0x05,0x06,0x03,0x0f,0x05,0x0d,0x0e,0x03,0x05,0x05,
    0x05,0x05,0x05,0x0f,0x05,0x05,0x06,0x09,0x0f,0x0c,0x03,0x06,0x0a,0x0a,0x03,0x05,
    0x05,0x07,0x04,0x0a,0x03,0x06,0x08,0x02,0x09,0x06,0x08,0x0b,0x0e,0x0a,0x0a,0x01,
    0x04,0x0b,0x05,0x0c,0x0b,0x0f,0x07,0x0d,0x04,0x09,0x06,0x0a,0x0a,0x09,0x0d,0x05,
    0x06,0x09,0x05,0x04,0x0a,0x0a,0x0a,0x03,0x07,0x06,0x0a,0x09,0x0f,0x05,0x06,0x01,
    0x0f,0x05,0x05,0x06,0x0b,0x0e,0x0a,0x0a,0x0a,0x0a,0x0b,0x04,0x0b,0x04,0x09,0x05,
    0x0c,0x03,0x06,0x0a,0x09,0x0d,0x0c,0x0a,0x0a,0x0a,0x0a,0x03,0x0c,0x03,0x06,0x01,
    0x06,0x0a,0x0a,0x0a,0x02,0x02,0x02,0x0a,0x0a,0x0a,0x0a,0x0a,0x02,0x0a,0x0a,0x03
};

const unsigned char russian2004[256] ={
    0x0e,0x0a,0x0a,0x0a,0x09,0x0c,0x0a,0x0a,0x09,0x0c,0x0a,0x0a,0x0a,0x0a,0x0a,0x09,
    0x0c,0x0a,0x0a,0x0a,0x02,0x02,0x0a,0x09,0x04,0x02,0x08,0x0a,0x09,0x0c,0x09,0x05,
    0x05,0x0c,0x0a,0x0a,0x0a,0x0a,0x09,0x05,0x05,0x0c,0x02,0x09,0x06,0x03,0x06,0x01,
    0x05,0x06,0x08,0x0a,0x0a,0x09,0x05,0x05,0x05,0x07,0x0c,0x03,0x0c,0x0a,0x08,0x01,
    0x06,0x09,0x05,0x0c,0x09,0x05,0x05,0x05,0x05,0x0c,0x01,0x0c,0x02,0x09,0x05,0x05,
    0x0c,0x01,0x05,0x05,0x05,0x05,0x06,0x03,0x06,0x01,0x05,0x07,0x0e,0x02,0x01,0x07,
    0x05,0x05,0x05,0x05,0x05,0x04,0x09,0x0e,0x0a,0x03,0x06,0x08,0x09,0x0c,0x02,0x09,
    0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x0c,0x08,0x0a,0x09,0x05,0x05,0x05,0x0e,0x01,
    0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x06,0x03,0x0d,0x05,0x05,0x05,0x06,0x08,0x01,
    0x05,0x06,0x03,0x05,0x05,0x05,0x04,0x0a,0x0a,0x03,0x05,0x05,0x04,0x09,0x05,0x05,
    0x04,0x0a,0x0a,0x03,0x04,0x03,0x05,0x0c,0x0a,0x08,0x01,0x05,0x05,0x05,0x05,0x05,
    0x04,0x09,0x0c,0x09,0x05,0x0c,0x01,0x05,0x0c,0x03,0x05,0x05,0x05,0x05,0x05,0x05,
    0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x06,0x09,0x05,0x05,0x05,0x05,0x05,0x05,
    0x05,0x05,0x05,0x05,0x05,0x05,0x06,0x03,0x0c,0x03,0x05,0x05,0x05,0x05,0x05,0x05,
    0x05,0x06,0x03,0x04,0x02,0x02,0x0a,0x0a,0x02,0x09,0x05,0x06,0x03,0x06,0x03,0x05,
    0x06,0x0a,0x0a,0x02,0x0a,0x0a,0x0a,0x0a,0x0a,0x03,0x06,0x0a,0x0a,0x0a,0x0a,0x03
};

const unsigned char EURO_V59[256] ={
     0x0e,0x0a,0x0a,0x09,0x0c,0x0a,0x0a,0x0a,0x0a,0x0a,0x0a,0x0a,0x0a,0x09,0x0c,0x09,
     0x0c,0x0a,0x0a,0x01,0x05,0x0c,0x09,0x0c,0x0a,0x0a,0x0a,0x0a,0x09,0x05,0x05,0x05,
     0x05,0x0c,0x09,0x05,0x06,0x03,0x05,0x05,0x0c,0x09,0x0c,0x0a,0x03,0x06,0x03,0x05,
     0x05,0x05,0x05,0x06,0x0a,0x0a,0x03,0x05,0x05,0x05,0x06,0x0a,0x0a,0x09,0x0c,0x03,
     0x06,0x03,0x05,0x0c,0x0a,0x0a,0x0a,0x02,0x01,0x06,0x0a,0x08,0x09,0x05,0x06,0x09,
     0x0c,0x0a,0x03,0x05,0x0c,0x0a,0x0a,0x0a,0x03,0x0c,0x09,0x05,0x05,0x06,0x09,0x05,
     0x06,0x09,0x0c,0x03,0x05,0x0c,0x08,0x0a,0x0a,0x01,0x05,0x06,0x02,0x09,0x05,0x05,
     0x0c,0x03,0x05,0x0c,0x03,0x05,0x05,0x0c,0x09,0x05,0x04,0x08,0x0a,0x03,0x05,0x05,
     0x05,0x0c,0x01,0x05,0x0c,0x03,0x05,0x04,0x03,0x05,0x05,0x05,0x0c,0x09,0x05,0x05,
     0x05,0x05,0x05,0x05,0x06,0x09,0x06,0x02,0x0a,0x03,0x05,0x05,0x05,0x06,0x02,0x01,
     0x05,0x05,0x05,0x06,0x08,0x02,0x0a,0x0a,0x0a,0x0a,0x03,0x05,0x07,0x0c,0x0a,0x03,
     0x05,0x05,0x06,0x0b,0x06,0x0a,0x0a,0x0a,0x0a,0x0a,0x0a,0x02,0x0a,0x01,0x0c,0x09,
     0x05,0x06,0x0a,0x0a,0x0a,0x09,0x0c,0x09,0x0c,0x0a,0x0a,0x09,0x0d,0x05,0x05,0x05,
     0x06,0x09,0x0c,0x0a,0x08,0x01,0x05,0x05,0x06,0x08,0x0a,0x03,0x05,0x06,0x03,0x05,
     0x0c,0x03,0x06,0x09,0x05,0x06,0x03,0x06,0x0a,0x02,0x0a,0x0a,0x02,0x08,0x09,0x05,
     0x06,0x0a,0x0a,0x03,0x06,0x0a,0x0a,0x0a,0x0a,0x0a,0x0a,0x0a,0x0a,0x03,0x06,0x03
};

unsigned char minos2008[256] ={
0x0e,0x0a,0x08,0x08,0x0a,0x0a,0x0a,0x0a,0x0a,0x0a,0x0a,0x08,0x0a,0x09,0x0d,0x0d,
0x0e,0x08,0x03,0x05,0x0d,0x0c,0x0b,0x0c,0x09,0x0c,0x09,0x06,0x0a,0x01,0x06,0x01,
0x0e,0x02,0x0b,0x05,0x05,0x06,0x0a,0x01,0x06,0x01,0x06,0x09,0x0c,0x02,0x08,0x01,
0x0c,0x08,0x0b,0x05,0x05,0x0d,0x0d,0x06,0x08,0x02,0x09,0x06,0x02,0x08,0x03,0x05,
0x05,0x05,0x0c,0x00,0x00,0x00,0x03,0x0c,0x01,0x0c,0x02,0x08,0x0b,0x04,0x0a,0x01,
0x05,0x04,0x01,0x05,0x05,0x05,0x0c,0x03,0x06,0x01,0x0c,0x01,0x0e,0x01,0x0c,0x03,
0x05,0x07,0x05,0x05,0x04,0x03,0x04,0x0a,0x08,0x01,0x05,0x04,0x09,0x06,0x02,0x09,
0x05,0x0d,0x05,0x05,0x05,0x0c,0x03,0x0c,0x01,0x06,0x01,0x05,0x06,0x09,0x0c,0x01,
0x07,0x05,0x06,0x00,0x03,0x04,0x09,0x04,0x03,0x0e,0x01,0x06,0x09,0x06,0x01,0x07,
0x0e,0x00,0x08,0x03,0x0e,0x01,0x05,0x06,0x09,0x0d,0x05,0x0d,0x04,0x09,0x06,0x09,
0x0f,0x05,0x05,0x0c,0x0a,0x03,0x04,0x0a,0x02,0x03,0x05,0x05,0x07,0x04,0x0b,0x05,
0x0c,0x00,0x03,0x05,0x0c,0x08,0x03,0x0c,0x0a,0x0a,0x00,0x02,0x08,0x02,0x0b,0x05,
0x05,0x05,0x0e,0x02,0x01,0x04,0x08,0x00,0x0a,0x08,0x00,0x09,0x04,0x0a,0x09,0x05,
0x04,0x02,0x08,0x0b,0x05,0x05,0x05,0x07,0x0d,0x05,0x05,0x04,0x01,0x0d,0x05,0x05,
0x05,0x0e,0x02,0x0a,0x01,0x05,0x06,0x08,0x03,0x05,0x06,0x03,0x05,0x04,0x02,0x03,
0x07,0x0e,0x0a,0x0a,0x02,0x02,0x0a,0x02,0x0b,0x06,0x0a,0x0a,0x02,0x02,0x0a,0x0b
};

unsigned char robotic2008[256] ={
0x0e,0x0a,0x0a,0x0a,0x08,0x0a,0x0a,0x0a,0x0a,0x0a,0x08,0x0a,0x0a,0x08,0x0a,0x09,
0x0c,0x0a,0x0a,0x09,0x05,0x0c,0x0a,0x0a,0x08,0x09,0x07,0x0e,0x08,0x03,0x0d,0x05,
0x06,0x0a,0x09,0x05,0x04,0x02,0x0b,0x0c,0x01,0x04,0x0a,0x08,0x02,0x0b,0x05,0x05,
0x0c,0x09,0x06,0x02,0x00,0x08,0x0a,0x03,0x05,0x07,0x0e,0x02,0x09,0x0d,0x06,0x01,
0x05,0x06,0x0a,0x09,0x05,0x04,0x0a,0x0a,0x02,0x0b,0x0c,0x0a,0x02,0x02,0x09,0x05,
0x05,0x0d,0x0d,0x05,0x07,0x05,0x0d,0x0c,0x0b,0x0c,0x01,0x0e,0x0a,0x0a,0x01,0x05,
0x05,0x04,0x03,0x04,0x08,0x02,0x01,0x06,0x0a,0x01,0x07,0x0c,0x0a,0x08,0x01,0x05,
0x05,0x06,0x09,0x07,0x07,0x0d,0x07,0x0c,0x09,0x07,0x0c,0x03,0x0e,0x01,0x07,0x05,
0x05,0x0d,0x06,0x0a,0x08,0x02,0x0b,0x04,0x03,0x0c,0x03,0x0c,0x0a,0x03,0x0d,0x05,
0x05,0x05,0x0e,0x09,0x06,0x09,0x0d,0x05,0x0e,0x01,0x0c,0x02,0x0a,0x0a,0x01,0x05,
0x05,0x04,0x09,0x06,0x09,0x04,0x03,0x05,0x0c,0x03,0x06,0x0a,0x0b,0x0c,0x01,0x05,
0x05,0x05,0x04,0x09,0x05,0x05,0x0c,0x03,0x05,0x0c,0x0a,0x08,0x0b,0x05,0x06,0x01,
0x04,0x01,0x05,0x06,0x01,0x06,0x02,0x09,0x05,0x06,0x09,0x06,0x09,0x06,0x09,0x05,
0x05,0x05,0x06,0x0b,0x05,0x0c,0x09,0x05,0x04,0x09,0x06,0x09,0x05,0x0c,0x01,0x05,
0x05,0x04,0x0a,0x0a,0x02,0x03,0x05,0x06,0x03,0x06,0x0a,0x03,0x06,0x03,0x07,0x05,
0x06,0x02,0x0a,0x0a,0x0a,0x0a,0x02,0x0a,0x0a,0x0a,0x0a,0x0a,0x0a,0x0a,0x0a,0x03
};

unsigned char robotex[256] ={
    0x0e,0x0a,0x08,0x09,0x0c,0x09,0x0e,0x0a,0x0a,0x0a,0x09,0x0c,0x0a,0x08,0x08,0x0b,
    0x0c,0x0a,0x01,0x06,0x01,0x06,0x0a,0x0a,0x0a,0x0a,0x00,0x03,0x0e,0x01,0x06,0x09,
    0x07,0x0d,0x06,0x0a,0x02,0x08,0x0a,0x0b,0x0c,0x0b,0x04,0x0a,0x08,0x02,0x09,0x05,
    0x0c,0x00,0x0a,0x08,0x0b,0x05,0x0c,0x0b,0x04,0x0a,0x03,0x0c,0x02,0x0b,0x05,0x05,
    0x05,0x06,0x09,0x05,0x0e,0x02,0x00,0x08,0x02,0x0b,0x0e,0x01,0x0c,0x08,0x02,0x03,
    0x05,0x0d,0x07,0x06,0x09,0x0e,0x01,0x06,0x08,0x08,0x09,0x07,0x05,0x06,0x09,0x0d,
    0x06,0x01,0x0c,0x09,0x06,0x09,0x06,0x0a,0x03,0x07,0x05,0x0e,0x02,0x09,0x06,0x01,
    0x0d,0x06,0x01,0x06,0x09,0x06,0x09,0x0c,0x09,0x0e,0x00,0x0b,0x0e,0x00,0x09,0x05,
    0x05,0x0d,0x06,0x08,0x03,0x0c,0x03,0x06,0x01,0x0e,0x00,0x0a,0x0b,0x07,0x05,0x05,
    0x06,0x00,0x0b,0x06,0x09,0x06,0x0a,0x08,0x03,0x0c,0x02,0x0b,0x0c,0x0a,0x01,0x05,
    0x0d,0x06,0x09,0x0c,0x02,0x08,0x0b,0x05,0x0e,0x02,0x09,0x0e,0x01,0x0c,0x03,0x05,
    0x05,0x0d,0x06,0x02,0x09,0x06,0x09,0x06,0x0b,0x0c,0x00,0x0a,0x03,0x05,0x0c,0x01,
    0x04,0x01,0x0e,0x08,0x03,0x0d,0x06,0x09,0x0c,0x03,0x05,0x0f,0x0d,0x05,0x05,0x05,
    0x05,0x06,0x08,0x03,0x0e,0x01,0x0f,0x07,0x06,0x09,0x06,0x09,0x06,0x01,0x07,0x05,
    0x05,0x0c,0x02,0x08,0x0b,0x04,0x0a,0x0a,0x0a,0x03,0x0d,0x06,0x09,0x05,0x0c,0x03,
    0x06,0x02,0x0a,0x02,0x0a,0x02,0x0a,0x0a,0x0a,0x0a,0x02,0x0a,0x03,0x06,0x02,0x0b
};

void map_select(void) {
    const menuitem_t map_menu_items[] = {
        {"Erase map       ", 0,  none}, // 0
        {"My home maze    ", 0,  none}, // 1
        {"1st World       ", 0,  none}, // 2
        {"2003 Kankou     ", 0,  none}, // 3
        {"2004 Russia     ", 0,  none}, // 4
        {"Euro-V59        ", 0,  none}, // 5
        {"Sigulda         ", 0,  none}, // 6
        {"YouTube         ", 0,  none}, // 7
        {"Minos 2008      ", 0,  none}, // 8
        {"RoboTIC 2008    ", 0,  none}, // 9
        {"Robotex         ", 0,  none}, // 10
        {"APEC 88         ", 0,  none}  // 11
    };
    switch (do_menu((menuitem_t*) map_menu_items, ((sizeof(map_menu_items)/sizeof(map_menu_items[0]))-1))) {
    case 0:
        data.map_size       = 0;
        data.pathlength     = 0;
        data.green_cell_nr  = 0;
        data.red_cell_nr    = 0;
        data.cell_step      = 0;
        break;

    case 1:
        data.cell_step = 1;
        inject(2);
        data.cell_step = 0;
        break;

    case 11:
        convert_maze((uint8_t *)APEC88);
        break;

    case 2:
        convert_maze((uint8_t *) world1st);
        break;

    case 3:
        convert_maze((uint8_t *)kankou2003);
        break;

    case 4:
        convert_maze((uint8_t *)russian2004);
        break;

    case 5:
        convert_maze((uint8_t *)EURO_V59);
        break;

    case 6:
        inject(0);
        break;

    case 7:
        inject(1);
        break;

    case 8:
        convert_maze((uint8_t *)minos2008);
        break;

    case 9:
        convert_maze((uint8_t *)robotic2008);
        break;

    case 10:
        convert_maze((uint8_t *)robotex);
        break;

    default:
        break;
    }
    data.crc32 = calc_crc32((uint8_t*)&data, sizeof(data)-4);
    spi_write_eeprom(EEPROM_COPY_ADDRESS, (uint8_t*) &data, sizeof(data));  // сначала записываем резерв
    spi_write_eeprom(EEPROM_CONFIG_ADDRESS, (uint8_t*) &data, sizeof(data));    // теперь, нормальный вариант.
}
