#include "basics.h"

extern char* reg[]; // arrays of registers
extern char* ea[]; // arrays of effectives adresses


void op_add(){
    printf("add");
}

void op_or(){
    printf("or");
}

void op_adc(){
    printf("adc");
}

void op_sbb(){
    printf("sbb");
}

void op_and(){
    printf("and");
}

void op_sub(){
    printf("sub");
}

void op_xor(){
    printf("xor");
}

void op_cmp(){
    printf("cmp");
}


uint8_t op_test(uint8_t* buffer, uint16_t i, struct infos* k){
    char* des = ea[k->_rm];
    uint8_t disp_nb = mod_switch(buffer, i, k->_mod, k->_rm, &des, (k->w << 3) + k->_rm);
    if (k->w == 1){
        printf("%02x%02x", buffer[i+2+disp_nb], buffer[i+3+disp_nb]);
        spacebar(disp_nb + 4);
        int16_t data_l = (buffer[i+3+disp_nb] << 8) + ((uint8_t) buffer[i+2+disp_nb]);
        printf("test %s, %04x", des, data_l);
        return 2 + disp_nb;
    }
    else{
        printf("%02x", buffer[i+2+disp_nb]);
        spacebar(disp_nb + 3);
        int8_t data_s = (int8_t) buffer[i+2+disp_nb];
        printf("test");
        if (k->_mod != 3)
            printf(" byte");
        printf(" %s, %hhx", des, data_s);
        return 1 + disp_nb;
    }
}

void op_not(){
    printf("not");
}

void op_neg(){
    printf("neg");
}

void op_mul(){
    printf("mul");
}

void op_imul(){
    printf("imul");
}

void op_div(){
    printf("div");
}

void op_idiv(){
    printf("idiv");
}

void op_inc(){
    printf("inc");
}

void op_dec(){
    printf("dec");
}

void op_call(){
    printf("call");
}

void op_jmp(){
    printf("jmp");
}

void op_push(){
    printf("push");
}

void op_rol(){
    printf("rol");
}

void op_ror(){
    printf("ror");
}

void op_rcl(){
    printf("rcl");
}

void op_rcr(){
    printf("rcr");
}

void op_shl(){
    printf("shl");
}

void op_shr(){
    printf("shr");
}

void op_sar(){
    printf("sar");
}

/// if (d==0) then str = des
/// if (d==1) then str = src
uint8_t mod_switch(uint8_t* buffer, uint16_t i, uint8_t _mod,
                    uint8_t _rm, char **str, uint8_t _reg){
    int16_t disp;
    uint8_t disp_nb = 0;
    switch(_mod){
    case 0: // MOD = 00 -> DISP = 0 EXCEPT IF R/M = 110
        if (_rm == 6){
            disp_nb = 2;
            disp = (((int16_t) buffer[i+3]) << 8) + ((int16_t) buffer[i+2]);
            *str = printnb16(disp);
            printf("%02x%02x", buffer[i+2], buffer[i+3]);
        }
        *str = bracket(*str);
        break;
    case 1: // MOD = 01 -> DISP = disp-low in 16bits
        disp_nb = 1;
        disp = (int8_t) buffer[i+2];
        *str = dispstr(*str, disp);
        *str = bracket(*str);
        printf("%02x", buffer[i+2]);
        break;
    case 2: // MOD = 10 -> DISP = disp-high ; disp-low
        disp_nb = 2;
        disp = (((int16_t) buffer[i+3]) << 8) + ((int16_t) buffer[i+2]);
        *str = dispstr(*str, disp);
        *str = bracket(*str);
        printf("%02x%02x", buffer[i+2], buffer[i+3]);
        break;
    default: // MOD = 11 -> R/M is treated as a REG
        *str = reg[_reg];
        break;
    }

    return disp_nb;
}

void data_switch(uint8_t* buffer, uint16_t i, uint8_t w,
                    uint8_t disp_nb, struct datas* data){
    if (w == 0){
        printf("%02x", buffer[i+1+disp_nb]);
        data->value = (int8_t) buffer[i+1+disp_nb];
        data->size = 1;
        data->src = printnb8((int8_t)data->value);
    }
    else{
        printf("%02x%02x", buffer[i+1+disp_nb], buffer[i+2+disp_nb]);
        data->value = (buffer[i+2+disp_nb] << 8) + buffer[i+1+disp_nb];
        data->size = 2;
        data->src = printnb16(data->value);
    }
}


void (*basics[])() = {&op_add, &op_or, &op_adc, &op_sbb,
                        &op_and, &op_sub, &op_xor, &op_cmp};

void (*basics2[])() = {NULL, NULL, &op_not, &op_neg,
                        &op_mul, &op_imul, &op_div, &op_idiv};

void (*basics3[])() = {&op_inc, &op_dec, &op_call, &op_call,
                        &op_jmp, &op_jmp, &op_push, NULL};

void (*basics4[])() = {&op_rol, &op_ror, &op_rcl, &op_rcr,
                        &op_shl, &op_shr, NULL, &op_sar};

char* jmp_on[] = {"jo", "jno", "jb", "jnb",
                    "je", "jne", "jbe", "jnbe",
                    "js", "jns", "jp", "jnp",
                    "jl", "jnl", "jle", "jnle"};