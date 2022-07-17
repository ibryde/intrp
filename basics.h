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

void op_add();

void op_or();

void op_adc();

void op_sbb();

void op_and();

void op_sub();

void op_xor();

void op_cmp();

uint8_t op_test(uint8_t* buffer, uint16_t i, struct infos* k);

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

uint8_t mod_switch(uint8_t* buffer, uint16_t i, uint8_t _mod,
                    uint8_t _rm, char **str, uint8_t _reg);

void data_switch(uint8_t* buffer, uint16_t i, uint8_t w,
                    uint8_t disp_nb, struct datas* data);

#endif