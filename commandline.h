/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#ifndef COMMANDLINE_H
#define COMMANDLINE_H

#define FORTH_STACK_SIZE (16)

struct instance_t;

typedef struct {
    char * CmdName; // name of command
    uint32_t (*fnctPt)(struct instance_t *, int *); // to execute this command
    int *param;
}Cmd_t;


typedef struct instance_t {
        unsigned char input_string[64];
        unsigned int base;
        uint32_t stack[FORTH_STACK_SIZE];
        uint32_t stack_idx;
        unsigned int str_index;
        unsigned int esc_sequence;
        Cmd_t current_vocabulary;
        void (*UART_OutString)(char *);
        void (*UART_OutChar)(uint8_t);
        void (*UART_OutUDec)(uint32_t);
} instance_t;



extern unsigned char input_string[64];

void BlueTooth_parse(void);
unsigned int input_command(instance_t *instance, char inbyte);
void parse_string(instance_t *instance);
    
#endif
/* [] END OF FILE */
