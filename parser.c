#include <stdio.h>
#include <unistd.h> //write() & read() function
#include <err.h> //errx() function
#include <fcntl.h> //open() function
#include <stdint.h> //uint_s variable
#include <string.h> //memset manipulation
#include <stdlib.h>

#include "a.out.h"
#include "operations.h"
#include "cpu.h"
#include "tools.h"

extern int16_t (*op[])(uint8_t* buffer, uint16_t i, struct CPU* cpu __attribute__((unused)));

void parser(int fd, uint8_t mode){
    struct header head;
    read(fd, &head, sizeof(struct header));
    lseek(fd, head.a_hdrlen, SEEK_SET);

    uint8_t buffer[head.a_text];
    read(fd, &buffer, head.a_text);
    lseek(fd, head.a_hdrlen + head.a_text, SEEK_SET);

    struct CPU* cpu = calloc(1, sizeof(struct CPU));

    cpu->mode = mode;
    cpu->memory = calloc(0x10000, sizeof(uint8_t));

    read(fd, cpu->memory, head.a_data);

    /// not implemented yet 
    cpu->SP = 0xffdc;
    cpu->memory[0xffdc] = 1;
    WORD(0xffde) = 0xffe6;

    uint16_t i = 0;
    
    if (cpu->mode == DEBUG) printf(" AX   BX   CX   DX   SP   BP   SI   DI  FLAGS IP\n");

    while(i < head.a_text){
        if (cpu->mode == DEBUG) printf("%04hx %04hx %04hx %04hx %04hx %04hx %04hx %04hx %c%c%c%c ",  
                cpu->AX, cpu->BX, cpu->CX, cpu->DX,
                cpu->SP, cpu->BP, cpu->SI, cpu->DI,
                (cpu->_o == 0)? '-' : 'O',
                (cpu->_s == 0)? '-' : 'S',
                (cpu->_z == 0)? '-' : 'Z',
                (cpu->_c == 0)? '-' : 'C');

        if (i+1 >= head.a_text){
            if (cpu->mode == DISASSEMBLY ||
                cpu->mode == DEBUG) printf("%04x: 00            (undefined)\n", i);
            break;
        }

        if (cpu->mode == DISASSEMBLY) printf("%04x: ", i);
        if (cpu->mode == DEBUG) printf("%04x:", i);

        fflush(stdout);

        i += op[buffer[i]](&buffer, i, cpu);

        if (cpu->mode == DISASSEMBLY ||
            cpu->mode == DEBUG) printf("\n");
        
        // printf("[%04hx]%04hx\n", 0xffb2, WORD(0xffb2));
        
        if (cpu->end_process && (cpu->mode == INTERPRETER || cpu->mode == DEBUG))
            break;
    }

    free(cpu->memory);
    free(cpu);
}