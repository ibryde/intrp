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

extern int16_t (*op[])(uint8_t* buffer, uint16_t i);

void parser(int fd){
    struct header head;
    read(fd, &head, sizeof(struct header));
    lseek(fd, head.a_hdrlen, SEEK_SET);

    uint8_t buffer[head.a_text];
    read(fd, &buffer, head.a_text);

    // struct CPU* cpu = malloc(sizeof(struct CPU));
    // memset(cpu, 0, sizeof(struct CPU));

    uint16_t i = 0;
    uint16_t temp = i;
    
    //printf(" AX   BX   CX   DX   SP   BP   SI   DI  FLAGS IP\n");

    while(i < head.a_text){
        //printf("%04hx %04hx %04hx %04hx %04hx %04hx %04hx %04hx %s%s%s%s ",  
        //        cpu->AX, cpu->BX, cpu->CX, cpu->DX,
        //        cpu->SP, cpu->BP, cpu->SI, cpu->DI);
        if (i+1 >= head.a_text){
            printf("%04x: 00            (undefined)\n", i);
            break;
        }
        printf("%04x: ", i);
        i += op[buffer[i]](&buffer, i);
        if (i < temp)
            break;
        temp = i;
        printf("\n");
    }
}