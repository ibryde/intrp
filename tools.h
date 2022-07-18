#ifndef TOOLS
#define TOOLS

#define INTERPRETER 0
#define DISASSEMBLY 1
#define DEBUG 2
#define MEM_SIZE 0x10000 * sizeof(uint8_t)
#define WORD(a)(*(uint16_t*)(cpu->memory + a))
#define MEMORY(a) (void*) &(cpu->memory[a])


#include "cpu.h"
#include "syscall.h"

struct infos{
    uint8_t w;

    uint8_t d; // s & d & v takes the same spot
    uint8_t s;

    uint8_t _mod;
    uint8_t _reg;
    uint8_t _rm; 
    uint8_t _ea;
};

/*
struct datas{
    char* src;
    int16_t value;

    uint8_t size;
};
*/

void fill_infos(struct infos* info, uint8_t* buffer, uint16_t i);

char* bracket(char* s);

char* dispstr(char* s, int16_t disp);

char* printnb16(int16_t nb);

char* printnb8(int16_t nb);

void spacebar(size_t nb_bytes);

void spacebar_debug(size_t nb_bytes);

void* get_reg(struct CPU* cpu, uint8_t _reg);

uint32_t get_memory(struct CPU* cpu, uint8_t _rm, int16_t disp);

void* get_ea(struct CPU* cpu, uint8_t _rm, int16_t disp);

uint16_t get_value(uint8_t* buffer, uint16_t i, struct CPU* cpu, uint8_t _mod, uint8_t _rm);

void memory_update(uint8_t* buffer, uint16_t i, struct CPU* cpu, uint8_t _mod, uint8_t _rm);

#endif