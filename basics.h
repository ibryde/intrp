#ifndef BASICS_H_
#define BASICS_H_
#include <stdio.h>
#include <unistd.h> //write() & read() function
#include <err.h> //errx() function
#include <fcntl.h> //open() function
#include <stdint.h> //uint_s variable
#include <string.h> //string manipulation
#include <stdlib.h>

#include "./tools.h"
#include "cpu.h"

void op_add(struct CPU* cpu, void* src, void* des, uint8_t size);

void op_or(struct CPU* cpu, void* src, void* des, uint8_t size);

void op_adc(struct CPU* cpu, void* src, void* des, uint8_t size);

void op_sbb(struct CPU* cpu, void* src, void* des, uint8_t size);

void op_and(struct CPU* cpu, void* src, void* des, uint8_t size);

void op_sub(struct CPU* cpu, void* src, void* des, uint8_t size);

void op_xor(struct CPU* cpu, void* src, void* des, uint8_t size);

void op_cmp(struct CPU* cpu, void* src, void* des, uint8_t size);


void op_mov(struct CPU* cpu, void* src, void* des, uint8_t size);


uint8_t op_test(struct CPU* cpu, uint8_t* buffer, uint16_t i, struct infos* k);

void op_not();

void op_neg();

void op_mul();

void op_imul();

void op_div();

void op_idiv();

void op_call();

void op_jmp();

void op_push();

void op_rol();

void op_ror();

void op_rcl();

void op_rcr();

void op_shl();

void op_shr();

void op_sar();

uint8_t mod_switch(uint8_t* buffer, uint16_t i, struct CPU* cpu, uint8_t _mod,
                    uint8_t _rm, char **str, uint8_t _reg, void** value);

/// CONDITIONNAL JUMP
int op_jo(struct CPU* cpu);

int op_jno(struct CPU* cpu);

int op_jb(struct CPU* cpu);

int op_jnb(struct CPU* cpu);

int op_je(struct CPU* cpu);

int op_jne(struct CPU* cpu);

int op_jbe(struct CPU* cpu);

int op_jnbe(struct CPU* cpu);

int op_js(struct CPU* cpu);

int op_jns(struct CPU* cpu);

int op_jp(struct CPU* cpu);

int op_jnp(struct CPU* cpu);

int op_jl(struct CPU* cpu);

int op_jnl(struct CPU* cpu);

int op_jle(struct CPU* cpu);

int op_jnle(struct CPU* cpu);

#endif