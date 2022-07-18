#ifndef CPU_H_
#define CPU_H_
#include <stdint.h>


struct CPU{
    uint16_t    AX;
    uint16_t    CX;
    uint16_t    DX;
    uint16_t    BX;
    uint16_t    SP;
    uint16_t    BP;
    uint16_t    SI;
    uint16_t    DI;
    uint8_t     _o;
    uint8_t     _s;
    uint8_t     _z;
    uint8_t     _c;
    uint8_t     mode;
    uint8_t*    memory;
    uint8_t     end_process;
};

#endif