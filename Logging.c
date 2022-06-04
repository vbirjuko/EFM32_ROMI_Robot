//#include "dma.h"
#include "spi_drv.h"
#include "Maze.h"
//#include "main.h"
#include "configure.h"
#include "Logging.h"
#include "Reflectance.h"
#include "LE_timer0.h"

typedef struct  {
	uint8_t 	command[4];
	uint16_t	data[128];
} spi_buffer_t;
// Available
spi_buffer_t spi_buffer[2];
static uint16_t *buffer_ptr;
volatile int16_t add_log[12000][5], add_count;

static uint32_t eeprom_address, current_buffer = 0, log_state = 0, log_size, log_watermark, log_place_available = 0;
static unsigned int buffer_count = 0;

void data_log_init(void) {
    uint8_t erase_buffer[4];

    current_buffer = 0;
    eeprom_address = LOG_ADDRESS;

    while (eeprom_address < LOG_END_ADDRESS) {
        erase_buffer[0] = eeprom_write_enable;
        spi_exchange(erase_buffer, 1);
        while (tx_busy) continue;

        erase_buffer[0] = eeprom_block_erase;
        erase_buffer[1] = ((eeprom_address & 0x0ff0000) >> 16) & 0xFF;
        erase_buffer[2] = ((eeprom_address & 0x000ff00) >>  8) & 0xFF;
        erase_buffer[3] = ((eeprom_address & 0x00000ff) >>  0) & 0xFF;
        spi_exchange(erase_buffer, 4);

        do {
            letimer_delay_ms(50);
            erase_buffer[0] = eeprom_read_status_register;
            spi_exchange(erase_buffer, 2);
            while (tx_busy) continue;
        } while (erase_buffer[1] & EEPROM_STATUS_WIP);
        eeprom_address += 0x10000;
    }
    log_place_available = 1;
    eeprom_address = LOG_ADDRESS;
    log_size = 0;
    current_buffer  = 0;
    buffer_ptr = spi_buffer[current_buffer].data;
    buffer_count = 0;
    log_watermark = 0;
    data_log(0xffff, 1);
    data_log(0xffff, 1); // placeholder for log size
    add_count = 0;
}

void add_data_log(int * element) {
  if (log_place_available && (add_count < 12000)) {
    add_log[add_count][0] = *element++;
    add_log[add_count][1] = *element++;
    add_log[add_count][2] = *element++;
    add_log[add_count][3] = *element++;
    add_log[add_count][4] = *element++;
    add_count++;
  }
}

extern volatile unsigned int tx_busy; // in spi_drv.c
void data_log(uint16_t datatowrite, unsigned int write_enable) {

    if (write_enable && log_place_available) {
        *buffer_ptr++ = datatowrite;
        log_size++;
    }

    if (++buffer_count > 127) {

        current_buffer ^= 1;
        buffer_count = 0;
        buffer_ptr = &spi_buffer[current_buffer].data[0];
        log_state = 1;
    }
    switch (log_state) {
    case 1:
        if (tx_busy) break;
        spi_buffer[current_buffer ^ 1].command[0] = eeprom_write_enable;
        spi_exchange(spi_buffer[current_buffer ^ 1].command, 1);
        log_state = 2;
        break;

    case 2:
        if (tx_busy) break;
        spi_buffer[current_buffer ^ 1].command[0] = eeprom_page_program;
        spi_buffer[current_buffer ^ 1].command[1] = ((eeprom_address & 0x0ff0000) >> 16) & 0xFF;
        spi_buffer[current_buffer ^ 1].command[2] = ((eeprom_address & 0x000ff00) >>  8) & 0xFF;
        spi_buffer[current_buffer ^ 1].command[3] = ((eeprom_address & 0x00000ff) >>  0) & 0xFF;
        spi_exchange(spi_buffer[current_buffer ^ 1].command, 256+4);
        eeprom_address += 256;
        if ((eeprom_address + 256) > LOG_END_ADDRESS) log_place_available = 0;
        log_state = 3;
        break;

    case 3:
        if (tx_busy) break;
        spi_buffer[current_buffer ^ 1].command[0] = eeprom_read_status_register;
        spi_exchange(spi_buffer[current_buffer ^ 1].command, 2);
        if (spi_buffer[current_buffer ^ 1].command[1] & EEPROM_STATUS_WIP) break;
        if (log_watermark < buffer_count) log_watermark = buffer_count;
        log_state = 0;
        break;
    }
}

void data_log_finish(void) {

  // Если запись предыдущего буфера в процессе,
  // то ждём его завершения.
  while (log_state) {
      data_log(0xFFFF, 0);
      letimer_delay_ms(10);
  }
  // Теперь завершаем текущий буфер и ждём
  // завершения уже его записи.
  buffer_count = 255;
  data_log(0xFFFF, 0);
  while (log_state) {
      letimer_delay_ms(10);
      data_log(0xFFFF, 0);
  }

  // записываем в самое начала размер сохранённого журнала
  spi_buffer[0].data[0] = (log_size >>  0) & 0x0000ffff;
  spi_buffer[0].data[1] = (log_size >> 16) & 0x0000ffff;
  spi_buffer[0].command[0] = eeprom_write_enable;
  spi_exchange(spi_buffer[0].command, 1);
  while (tx_busy) continue;

  spi_buffer[0].command[0] = eeprom_page_program;
  spi_buffer[0].command[1] = ((LOG_ADDRESS & 0x0ff0000) >> 16) & 0xFF;
  spi_buffer[0].command[2] = ((LOG_ADDRESS & 0x000ff00) >>  8) & 0xFF;
  spi_buffer[0].command[3] = ((LOG_ADDRESS & 0x00000ff) >>  0) & 0xFF;
  spi_exchange(spi_buffer[0].command, 4+4);
  while (tx_busy) continue;

  do {
      while (!photo_data_ready) continue;
      photo_data_ready = 0;
      spi_buffer[0].command[0] = eeprom_read_status_register;
      spi_exchange(spi_buffer[0].command, 2);
      while (tx_busy) continue;
  } while (spi_buffer[0].command[1] & EEPROM_STATUS_WIP);
  data.log_watermark = log_watermark;
  log_place_available = 0;

  spi_write_eeprom(0x40000, (unsigned char *)add_log, add_count*sizeof(add_log[0]));
}
