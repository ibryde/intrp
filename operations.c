#include <stdio.h>
#include <unistd.h> //write() & read() function
#include <err.h> //errx() function
#include <fcntl.h> //open() function
#include <stdint.h> //uint_s variable
#include <string.h> //string manipulation

#include "./tools.h"
#include "./basics.h"

extern char* reg[]; // arrays of registers
extern char* ea[]; // arrays of effectives adresses
extern char* jmp_on[]; // arrays of jmp name conditions
extern void (*basics[])(); //arrays of the basics op
extern void (*basics2[])(); //arrays of another basics op
extern void (*basics3[])(); //arrays of another basics op
extern void (*basics4[])(); //arrays of another basics op

int16_t op_XX(uint8_t* buffer, uint16_t i){
    printf("%02x                ", buffer[i]);
    return -1;
}


/// BASICS - Reg./Memory with Register to Either
int16_t op_00(uint8_t* buffer, uint16_t i){
    printf("%02x%02x", buffer[i], buffer[i+1]);
    struct infos* k = malloc(sizeof(struct infos));
    fill_infos(k, buffer, i);

    uint8_t op_nb = buffer[i] >> 3; // Gets the value of the operations
    char* src = (k->d == 0)? reg[(k->w << 3) + k->_reg] : ea[k->_rm]; // if d == 0, then 'from' reg
    char* des = (k->d == 0)? ea[k->_rm] : reg[(k->w << 3) + k->_reg]; // if d == 1, then 'to' reg

    int8_t disp_nb = mod_switch(buffer, i, k->_mod, k->_rm, (k->d == 0)? &des : &src, (k->w << 3) + k->_rm);

    spacebar(2 + disp_nb);

    basics[op_nb]();
    printf(" %s, %s", des, src); //yep
    free(k);
    

    return 2 + disp_nb;
}

/// BASICS - Immediate to Accumulator
int16_t op_01(uint8_t* buffer, uint16_t i){
    printf("%02x", buffer[i]);
    struct infos* k = malloc(sizeof(struct infos));
    fill_infos(k, buffer, i);

    uint8_t op_nb = buffer[i] >> 3; // Gets the value of the operations
    char* des = (k->w == 0)? "al" : "ax"; // if w = 0 then byte
    uint8_t data_size = 1;

    if (k->w == 0){
        printf("%02x", buffer[i+1]);
        spacebar(2);
        int8_t data_l = (int8_t) buffer[i+1];
        basics[op_nb]();
        printf(" %s, %02hhx", des, data_l);
    }
    else{
        printf("%02x%02x", buffer[i+1], buffer[i+2]);
        spacebar(3);
        int16_t data_x = (buffer[i+2] << 8) + buffer[i+1];
        basics[op_nb]();
        printf(" %s, %04hx", des, data_x);
        data_size++;
    }
    
    free(k);

    return 1 + data_size;
}

/// INC - Register
/// Increment
int16_t op_40(uint8_t* buffer, uint16_t i){
    printf("%02x", buffer[i]);
    spacebar(1);

    uint8_t _reg = buffer[i] & 7;
    printf("inc %s", reg[8 + _reg]);
    
    return 1;
}

/// DEC - Register
/// Decrement
int16_t op_48(uint8_t* buffer, uint16_t i){
    printf("%02x", buffer[i]);
    spacebar(1);

    uint8_t _reg = buffer[i] & 7;
    printf("dec %s", reg[8 + _reg]);
    
    return 1;
}

/// PUSH - Register
/// PUSH REG to SP
int16_t op_50(uint8_t* buffer, uint16_t i){
    printf("%02x", buffer[i]);
    spacebar(1);

    uint8_t _reg = buffer[i] & 7;
    printf("push %s", reg[8 + _reg]);

    return 1;
}

/// POP - Register
/// POP SP to REG
int16_t op_58(uint8_t* buffer, uint16_t i){
    printf("%02x", buffer[i]);
    spacebar(1);

    uint8_t _reg = buffer[i] & 7;
    printf("pop %s", reg[8 + _reg]);
    
    return 1;
}

/// JMP ON
int16_t op_7X(uint8_t* buffer, uint16_t i){
    printf("%02x%02x", buffer[i], buffer[i+1]);
    spacebar(2);

    uint8_t jmp_nb = buffer[i] & 15;
    int16_t disp = (int8_t) buffer[i+1];
    disp += i+2;

    printf("%s %04x", jmp_on[jmp_nb], disp);
    return 2;
}

/// BASICS - Immediate to Register/Memory
int16_t op_80(uint8_t* buffer, uint16_t i){
    printf("%02x%02x", buffer[i], buffer[i+1]);
    struct infos* k = malloc(sizeof(struct infos));
    fill_infos(k, buffer, i);
    
    uint8_t op_nb = (buffer[i+1] >> 3) & 7;
    char* des = ea[k->_rm];

    uint8_t disp_nb = mod_switch(buffer, i, k->_mod, k->_rm, &des, (k->w << 3) + k->_rm);
    uint8_t data_size = 1 + ((k->s == 0 && k->w == 1)? 1 : 0);

    if (k->w == 0){
        int8_t data_l = (int8_t) buffer[i+2+disp_nb];
        printf("%02x", buffer[i+2+disp_nb]);
        spacebar(3 + disp_nb);
        basics[op_nb]();
        if (k->_mod != 3)
            printf(" byte");
        if (data_l < 0)
            printf(" %s, -%hhx", des, -1 * data_l);
        else
            printf(" %s, %hhx", des, data_l);
    }
    else{
        int16_t data_x;
        if (k->s == 0){
            data_x = ((uint16_t)buffer[i+3+disp_nb]<<8) + buffer[i+2+disp_nb];
            printf("%02x%02x", buffer[i+2+disp_nb], buffer[i+3+disp_nb]);
            spacebar(2 + disp_nb + data_size);
            basics[op_nb]();
            printf(" %s, %04hx", des, data_x);
        }
        else{
            data_x = (int8_t) buffer[i+2+disp_nb];
            printf("%02x", buffer[i+2+disp_nb]);
            spacebar(2 + disp_nb + data_size);
            basics[op_nb]();
            if (data_x < 0)
                printf(" %s, -%hx", des, -1 * data_x);
            else
                printf(" %s, %hx", des, data_x);
        }        
    }

    free(k);
    return 2 + disp_nb + data_size;
}

/// TEST - Register/Memory and Register
///         And Function to Flags, No Result:
int16_t op_84(uint8_t* buffer, uint16_t i){
    printf("%02x%02x", buffer[i], buffer[i+1]);
    struct infos* k = malloc(sizeof(struct infos));
    fill_infos(k, buffer, i);

    uint8_t op_nb = buffer[i] >> 3; // Gets the value of the operations
    char* src = (k->d == 0)? reg[(k->w << 3) + k->_reg] : ea[k->_rm]; // if d == 0, then 'from' reg
    char* des = (k->d == 0)? ea[k->_rm] : reg[(k->w << 3) + k->_reg]; // if d == 1, then 'to' reg

    int8_t disp_nb = mod_switch(buffer, i, k->_mod, k->_rm, (k->d == 0)? &des : &src, (k->w << 3) + k->_rm);

    spacebar(2 + disp_nb);

    printf("test %s, %s", des, src); //yep
    free(k);
    
    return 2 + disp_nb;
}

/// XCHG - Exchange
///         Register/Memory with Register
int16_t op_86(uint8_t* buffer, uint16_t i){
    printf("%02x%02x", buffer[i], buffer[i+1]);
    struct infos* k = malloc(sizeof(struct infos));
    fill_infos(k, buffer, i);
    
    char* src = reg[(k->w << 3) + k->_reg]; 
    char* des = ea[k->_rm]; 

    int8_t disp_nb = mod_switch(buffer, i, k->_mod, k->_rm, &des, (k->w << 3) + k->_rm);

    spacebar(2 + disp_nb);

    printf("xchg");
    printf(" %s, %s", des, src);
    free(k);

    return 2 + disp_nb;
}

/// MOV - Register/Memory to/from Register
int16_t op_88(uint8_t* buffer, uint16_t i){
    printf("%02x%02x", buffer[i], buffer[i+1]);
    struct infos* k = malloc(sizeof(struct infos));
    fill_infos(k, buffer, i);
    
    char* src = (k->d == 0)? reg[(k->w << 3) + k->_reg] : ea[k->_rm]; // if d == 0, then 'from' reg
    char* des = (k->d == 0)? ea[k->_rm] : reg[(k->w << 3) + k->_reg]; // if d == 1, then 'to' reg

    int8_t disp_nb = mod_switch(buffer, i, k->_mod, k->_rm, (k->d == 0)? &des : &src, (k->w << 3) + k->_rm);

    spacebar(2 + disp_nb);

    printf("mov");
    printf(" %s, %s", des, src);
    free(k);

    return 2 + disp_nb;
}

/// LEA - Load EA to Register
int16_t op_8D(uint8_t* buffer, uint16_t i){
    printf("%02x%02x", buffer[i], buffer[i+1]);
    struct infos* k = malloc(sizeof(struct infos));
    fill_infos(k, buffer, i);
    
    char* src = ea[k->_rm];
    char* des = reg[(k->w << 3) + k->_reg];

    int8_t disp_nb = mod_switch(buffer, i, k->_mod, k->_rm, &src, (k->w << 3) + k->_rm);

    spacebar(2 + disp_nb);

    printf("lea");
    printf(" %s, %s", des, src);
    free(k);

    return 2 + disp_nb;
}

/// XCHG - Exchange
///         Register with Accumulator
int16_t op_90(uint8_t* buffer, uint16_t i){
    printf("%02x", buffer[i]);
    spacebar(1);
    uint8_t _reg = buffer[i] & 7;

    printf("xchg %s, ax", reg[8 + _reg]);
    return 1;
}

/// CBW -
int16_t op_98(uint8_t* buffer, uint16_t i){
    printf("%02x", buffer[i]);
    spacebar(1);
    printf("cbw");
    return 1;
}

/// CWD -
int16_t op_99(uint8_t* buffer, uint16_t i){
    printf("%02x", buffer[i]);
    spacebar(1);
    printf("cwd");
    return 1;
}


/// TEST - Immediate Data and Accumulator
///            And Function to Flags, No Result:
int16_t op_A8(uint8_t* buffer, uint16_t i){
    printf("%02x", buffer[i]);
    struct infos* k = malloc(sizeof(struct infos));
    fill_infos(k, buffer, i);
    uint8_t data_size = 0;

    if (k->w == 0){
        printf("%02x", buffer[i+1]);
        spacebar(2);
        int8_t data_l = (int8_t) buffer[i+1];
        printf("test al, %x", data_l);
        data_size = 1;
    }
    else{
        printf("%02x%02x", buffer[i+1], buffer[i+2]);
        spacebar(3);
        int8_t data_x = (buffer[i+2] << 8) + buffer[i+1];
        printf("test ax, %x", data_x);
        data_size = 2;
    }    

    free(k);
    return 1 + data_size;
}

/// MOV - Immediate to Register
int16_t op_BX(uint8_t* buffer, uint16_t i){
    int8_t _reg = buffer[i] & 15; // GET THE LAST 4 BITS
    printf("%02x%02x%02x        ", buffer[i], buffer[i+1], buffer[i+2]);
    printf("mov %s, %02x%02x", reg[_reg], buffer[i+2], buffer[i+1]);
    return 3;
}

/// BASICS4 - THIS ONE
int16_t op_D0(uint8_t* buffer, uint16_t i){
    printf("%02x%02x", buffer[i], buffer[i+1]);
    struct infos* k = malloc(sizeof(struct infos));
    fill_infos(k, buffer, i);

    uint8_t op_nb = k->_reg;
    char* des = ea[k->_rm];
    char* src = (k->d == 0)? "1" : "cl";
    uint8_t disp_nb = mod_switch(buffer, i, k->_mod, k->_rm, &des, (k->w << 3) + k->_rm);

    spacebar(2 + disp_nb);

    basics4[op_nb]();
    printf(" %s, %s", des, src);
    free(k);

    return 2 + disp_nb;
}

/// RET - Return from CALL
///         Within Seg Adding Immed to SP
int16_t op_C2(uint8_t* buffer, uint16_t i){
    printf("%02x%02x%02x", buffer[i], buffer[i+1], buffer[i+2]);
    spacebar(3);
    printf("ret %02x%02x", buffer[i+2], buffer[i+1]);
    return 3;
}

/// RET - Return from CALL
///         Within Segment
int16_t op_C3(uint8_t* buffer, uint16_t i){
    printf("%02x", buffer[i]);
    spacebar(1);
    printf("ret");
    return 1;
}

/// MOV - Immediate to Register/Memory
int16_t op_C6(uint8_t* buffer, uint16_t i){
    printf("%02x%02x", buffer[i], buffer[i+1]);
    struct infos* k = malloc(sizeof(struct infos));
    fill_infos(k, buffer, i);

    char* des = ea[k->_rm];
    int8_t disp_nb = mod_switch(buffer, i, k->_mod, k->_rm, &des, (k->w << 3) + k->_rm);
    uint8_t data_size = 0;

    if (k->w == 0){
        printf("%02x", buffer[i+2+disp_nb]);
        data_size = 1;
        spacebar(2 + disp_nb + data_size);
        int8_t data_l = (int8_t) buffer[i+2+disp_nb];
        printf("mov");
        if (k->_mod != 3)
            printf(" byte");
        printf(" %s, %hhx", des, data_l);
    }
    else{
        printf("%02x%02x", buffer[i+2+disp_nb], buffer[i+3+disp_nb]);
        data_size = 2;
        spacebar(2 + disp_nb + data_size);
        int16_t data_x = ((uint16_t)buffer[i+3+disp_nb]<<8) + buffer[i+2+disp_nb];
        printf("mov");
        printf(" %s, %04hx", des, data_x);
    }


    free(k);

    return 2 + disp_nb + data_size;
}

/// INT - Type Specified
int16_t op_CD(uint8_t* buffer, uint16_t i){
    printf("%02x%02x          ", buffer[i], buffer[i+1]);
    printf("int %02x", buffer[i+1]);
    return 2;
}

/// BASICS2
int16_t op_F6(uint8_t* buffer, uint16_t i){
    printf("%02x%02x", buffer[i], buffer[i+1]);
    struct infos* k = malloc(sizeof(struct infos));
    fill_infos(k, buffer, i);

    uint8_t data_nb = 0;
    uint8_t op_nb = k->_reg;
    uint8_t disp_nb = 0;

    if (op_nb >= 2){
        char* des = ea[k->_rm];
        disp_nb += mod_switch(buffer, i, k->_mod, k->_rm, &des, (k->w << 3) + k->_rm);
        spacebar(2 + disp_nb);
        basics2[op_nb]();
        if (k->w == 0)
            printf(" byte");
        printf(" %s", des);
    }
    else
        disp_nb += op_test(buffer, i, k);
    free(k);

    return 2 + disp_nb + data_nb;
}

/// 
int16_t op_E2(uint8_t* buffer, uint16_t i){
    printf("%02x%02x", buffer[i], buffer[i+1]);
    spacebar(2);
    int16_t value = (i+2) + ((int8_t) buffer[i+1]);
    printf("loop %x", value);
    return 2;
}

/// IN - Fixed Port
/// w = 0;
int16_t op_E4(uint8_t* buffer, uint16_t i){
    printf("%02x%02x", buffer[i], buffer[i+1]);
    spacebar(2);
    printf("in al, %x", buffer[i+1]);
    return 2;
}

/// IN - Fixed Port
/// w = 1;
int16_t op_E5(uint8_t* buffer, uint16_t i){
    printf("%02x%02x", buffer[i], buffer[i+1]);
    spacebar(2);
    printf("in ax, %x", buffer[i+1]);
    return 2;
}

/// w = 0;
int16_t op_EC(uint8_t* buffer, uint16_t i){
    printf("%02x", buffer[i]);
    spacebar(1);
    printf("in al, dx");
    return 1;
}

/// w = 1;
int16_t op_ED(uint8_t* buffer, uint16_t i){
    printf("%02x", buffer[i]);
    spacebar(1);
    printf("in ax, dl");
    return 1;
}

/// CALL - Direct within Segment
int16_t op_E8(uint8_t* buffer, uint16_t i){
    printf("%02x%02x%02x", buffer[i], buffer[i+1], buffer[i+2]);
    spacebar(3);

    int16_t disp = (buffer[i+2] << 8) + buffer[i+1] + (i+3);
    printf("call %04x", disp);

    return 3;
}

/// JMP - Direct within Segment
int16_t op_E9(uint8_t* buffer, uint16_t i){
    printf("%02x%02x%02x", buffer[i], buffer[i+1], buffer[i+2]);
    spacebar(3);

    int16_t disp = (buffer[i+2] << 8) + buffer[i+1] + (i+3);
    printf("jmp %04x", disp);

    return 3;
}

/// JMP - Direct within Segment-Short
int16_t op_EB(uint8_t* buffer, uint16_t i){
    printf("%02x%02x", buffer[i], buffer[i+1]);
    spacebar(2);

    int16_t disp = ((int8_t) buffer[i+1]) + (i+2);
    printf("jmp short %04x", disp);

    return 2;
}

/// REP : Repeat
int16_t op_F2(uint8_t* buffer, uint16_t i){
    printf("%02x%02x", buffer[i], buffer[i+1]);
    spacebar(2);
    if ((buffer[i+1] & 7) == 4)
        printf("rep movsb");
    else
        printf("rep movsw");
    return 2;
}

/// HLT : Halt
int16_t op_F4(uint8_t* buffer, uint16_t i){
    printf("%02x", buffer[i]);
    spacebar(1);
    printf("hlt");
    return 1;
}

/// CLD - Clear Direction
int16_t op_FC(uint8_t* buffer, uint16_t i){
    printf("%02x", buffer[i]);
    spacebar(1);
    printf("cld");
    return 1;
}

/// STD - Set Direction 
int16_t op_FD(uint8_t* buffer, uint16_t i){
    printf("%02x", buffer[i]);
    spacebar(1);
    printf("std");
    return 1;
}

/// INC + DEC : Register/Memory
int16_t op_FE(uint8_t* buffer, uint16_t i){
    printf("%02x%02x", buffer[i], buffer[i+1]);
    struct infos* k = malloc(sizeof(struct infos));
    fill_infos(k, buffer, i);

    uint8_t op_nb = k->_reg;
    char* des = ea[k->_rm];
    int8_t disp_nb = mod_switch(buffer, i, k->_mod, k->_rm, &des, k->_rm);

    spacebar(2 + disp_nb);
    printf("%x\n", op_nb);

    basics3[op_nb]();

    printf(" %s", des);
    free(k);

    return 2 + disp_nb;
}


/// BASICS3 + INC + DEC
int16_t op_FF(uint8_t* buffer, uint16_t i){
    printf("%02x%02x", buffer[i], buffer[i+1]);
    struct infos* k = malloc(sizeof(struct infos));
    fill_infos(k, buffer, i);

    uint8_t op_nb = k->_reg;
    char* des = ea[k->_rm];
    int8_t disp_nb = mod_switch(buffer, i, k->_mod, k->_rm, &des, (1 << 3) + k->_rm);

    spacebar(2 + disp_nb);
    basics3[op_nb]();
    printf(" %s", des);
    free(k);

    return 2 + disp_nb;
}

int16_t (*op[])(uint8_t* buffer, uint16_t i) = {
    &op_00, &op_00, &op_00, &op_00, &op_01, &op_01, NULL  , NULL  , &op_00, &op_00, &op_00, &op_00, &op_01, &op_01, NULL  , NULL  ,
    &op_00, &op_00, &op_00, &op_00, &op_01, &op_01, NULL  , NULL  , &op_00, &op_00, &op_00, &op_00, &op_01, &op_01, NULL  , NULL  ,
    &op_00, &op_00, &op_00, &op_00, &op_01, &op_01, NULL  , &op_XX, &op_00, &op_00, &op_00, &op_00, &op_01, &op_01, NULL  , &op_XX,
    &op_00, &op_00, &op_00, &op_00, &op_01, &op_01, NULL  , &op_XX, &op_00, &op_00, &op_00, &op_00, &op_01, &op_01, NULL  , &op_XX,
    &op_40, &op_40, &op_40, &op_40, &op_40, &op_40, &op_40, &op_40, &op_48, &op_48, &op_48, &op_48, &op_48, &op_48, &op_48, &op_48,
    &op_50, &op_50, &op_50, &op_50, &op_50, &op_50, &op_50, &op_50, &op_58, &op_58, &op_58, &op_58, &op_58, &op_58, &op_58, &op_58,
    NULL  , NULL  , NULL  , NULL  , NULL  , NULL  , NULL  , NULL  , NULL  , NULL  , NULL  , NULL  , NULL  , NULL  , NULL  , NULL  ,
    &op_7X, &op_7X, &op_7X, &op_7X, &op_7X, &op_7X, &op_7X, &op_7X, &op_7X, &op_7X, &op_7X, &op_7X, &op_7X, &op_7X, &op_7X, &op_7X,
    &op_80, &op_80, &op_80, &op_80, &op_84, &op_84, &op_86, &op_86, &op_88, &op_88, &op_88, &op_88, &op_XX, &op_8D, &op_XX, &op_XX,
    &op_90, &op_90, &op_90, &op_90, &op_90, &op_90, &op_90, &op_90, &op_98, &op_99, &op_XX, &op_XX, &op_XX, &op_XX, &op_XX, &op_XX,
    &op_XX, &op_XX, &op_XX, &op_XX, &op_XX, &op_XX, &op_XX, &op_XX, &op_A8, &op_A8, &op_XX, &op_XX, &op_XX, &op_XX, &op_XX, &op_XX,
    &op_BX, &op_BX, &op_BX, &op_BX, &op_BX, &op_BX, &op_BX, &op_BX, &op_BX, &op_BX, &op_BX, &op_BX, &op_BX, &op_BX, &op_BX, &op_BX,
    NULL  , NULL  , &op_C2, &op_C3, &op_XX, &op_XX, &op_C6, &op_C6, NULL  , NULL  , &op_XX, &op_XX, &op_XX, &op_CD, &op_XX, &op_XX,
    &op_D0, &op_D0, &op_D0, &op_D0, &op_XX, &op_XX, &op_XX, &op_XX, &op_XX, &op_XX, &op_XX, &op_XX, &op_XX, &op_XX, &op_XX, &op_XX,
    &op_XX, &op_XX, &op_E2, &op_XX, &op_E4, &op_E5, &op_XX, &op_XX, &op_E8, &op_E9, &op_XX, &op_EB, &op_EC, &op_ED, &op_XX, &op_XX,
    &op_XX, &op_XX, &op_F2, &op_F2, &op_F4, &op_XX, &op_F6, &op_F6, &op_XX, &op_XX, &op_XX, &op_XX, &op_FC, &op_FD, &op_FE, &op_FF
};