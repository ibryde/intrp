#include <stdio.h>
#include <unistd.h> //write() & read() function
#include <err.h> //errx() function
#include <fcntl.h> //open() function
#include <stdint.h> //uint_s variable
#include <string.h> //string manipulation

#include "./tools.h"
#include "./basics.h"
#include "./type.h"

extern char* reg[]; // arrays of registers
extern char* ea[]; // arrays of effectives adresses
extern char* jmp_on[]; // arrays of jmp name conditions
extern int (*op_jmp_on[])(struct CPU* cpu);
    // arrays of jmp conditions
extern void (*basics[]) //arrays of the basics op
    (struct CPU* cpu, void* src, void* des, uint8_t size);
extern char* basics_str[]; // array of the basics string
extern void (*basics2[]) //arrays of the basics op
    (struct CPU* cpu, void* des, uint8_t size); //arrays of another basics op
extern char* basics2_str[]; //arrays of another basics op
extern char* basics3[]; //arrays of another basics op
extern void (*basics4[])(struct CPU* cpu, void* src, void* des, uint8_t size); //arrays of another basics op
extern char* basics4_str[];

int16_t op_XX(uint8_t* buffer, uint16_t i, struct CPU* cpu __attribute__((unused))){
    fprintf(stderr, "%02x                ", buffer[i]);
    return -1;
}


/// BASICS - Reg./Memory with Register to Either
int16_t op_00(uint8_t* buffer, uint16_t i, struct CPU* cpu __attribute__((unused))){
    if (cpu->mode == DISASSEMBLY ||
        cpu->mode == DEBUG) fprintf(stderr, "%02x%02x", buffer[i], buffer[i+1]);

    struct infos* k = malloc(sizeof(struct infos));
    fill_infos(k, buffer, i);

    uint8_t op_nb = buffer[i] >> 3; // Gets the value of the operations
    int8_t disp_nb;

    if (k->d == 0){ //if d == 0, then 'from' reg
        char* src_0 = reg[(k->w << 3) + k->_reg];
        void* src_ptr_0 = get_reg(cpu, (k->w << 3) + k->_reg);

        char* des_0 = ea[k->_rm];
        void* des_ptr_0 = NULL;

        disp_nb = mod_switch(buffer, i, cpu, k->_mod, k->_rm, &des_0, (k->w << 3) + k->_rm, &des_ptr_0);

        if (cpu->mode == DISASSEMBLY) spacebar(2 + disp_nb);
        if (cpu->mode == DEBUG) spacebar_debug(2 + disp_nb);

        if (cpu->mode == DISASSEMBLY ||
            cpu->mode == DEBUG) fprintf(stderr, "%s %s, %s", basics_str[op_nb], des_0, src_0); 

        if (cpu->mode == DEBUG) memory_update(buffer, i, cpu, k);
        
        basics[op_nb](cpu, src_ptr_0, des_ptr_0, 1 + k->w);
        
    }
    else{ //if d == 1, then 'to' reg
        char* src = ea[k->_rm];
        void* src_ptr = NULL;

        char* des = reg[(k->w << 3) + k->_reg];
        void* des_ptr = get_reg(cpu, (k->w << 3) + k->_reg);

        disp_nb = mod_switch(buffer, i, cpu, k->_mod, k->_rm, &src, (k->w << 3) + k->_rm, &src_ptr);

        if (cpu->mode == DISASSEMBLY) spacebar(2 + disp_nb);
        if (cpu->mode == DEBUG) spacebar_debug(2 + disp_nb);

        if (cpu->mode == DISASSEMBLY ||
            cpu->mode == DEBUG) fprintf(stderr, "%s %s, %s", basics_str[op_nb], des, src); 
        
        if (cpu->mode == DEBUG) memory_update(buffer, i, cpu, k);

        basics[op_nb](cpu, src_ptr, des_ptr, 1 + k->w);

    }

    free(k);

    return 2 + disp_nb;
}

/// BASICS - Immediate to Accumulator
int16_t op_01(uint8_t* buffer, uint16_t i, struct CPU* cpu __attribute__((unused))){
    if (cpu->mode == DISASSEMBLY ||
        cpu->mode == DEBUG) fprintf(stderr, "%02x", buffer[i]);

    struct infos* k = malloc(sizeof(struct infos));
    fill_infos(k, buffer, i);

    uint8_t op_nb = buffer[i] >> 3; // Gets the value of the operations

    char* des = (k->w == 0)? "al" : "ax"; // if w = 0 then byte
    void* des_ptr = (void*) &(cpu->AX);

    uint8_t data_size = 1;

    if (k->w == 0){
        if (cpu->mode == DISASSEMBLY ||
            cpu->mode == DEBUG) fprintf(stderr, "%02x", buffer[i+1]);

        if (cpu->mode == DISASSEMBLY) spacebar(2);
        if (cpu->mode == DEBUG) spacebar_debug(2);

        int8_t data_l = (int8_t) buffer[i+1];

        if (cpu->mode == DISASSEMBLY ||
            cpu->mode == DEBUG) fprintf(stderr, "%s %s, %02hhx", basics_str[op_nb], des, data_l);
        
        // fprintf(stderr, "\ndata = %04hx; src = %04hx\n", data_l, *((int8_t *)des_ptr));

        basics[op_nb](cpu, &data_l, des_ptr, 1);
    }
    else{
        fprintf(stderr, "%02x%02x", buffer[i+1], buffer[i+2]);

        if (cpu->mode == DISASSEMBLY) spacebar(3);
        if (cpu->mode == DEBUG) spacebar_debug(3);

        int16_t data_x = (buffer[i+2] << 8) + buffer[i+1];
        data_size++;

        if (cpu->mode == DISASSEMBLY ||
            cpu->mode == DEBUG) fprintf(stderr, "%s %s, %04hx", basics_str[op_nb], des, data_x);

        // fprintf(stderr, "\ndata = %04hx; src = %04hx\n", data_x, *((int16_t *)des_ptr));

        basics[op_nb](cpu, &data_x, des_ptr, 2);
    }
    
    free(k);

    return 1 + data_size;
}

/// INC - Register
/// Increment
int16_t op_40(uint8_t* buffer, uint16_t i, struct CPU* cpu __attribute__((unused))){
    if (cpu->mode == DISASSEMBLY ||
        cpu->mode == DEBUG) fprintf(stderr, "%02x", buffer[i]);

    if (cpu->mode == DISASSEMBLY) spacebar(1);
    if (cpu->mode == DEBUG) spacebar_debug(1);

    uint8_t _reg = buffer[i] & 7;
    if (cpu->mode == DISASSEMBLY ||
        cpu->mode == DEBUG) fprintf(stderr, "inc %s", reg[8 + _reg]);
    
    void* reg_ptr = get_reg(cpu, 8 + _reg);
    int16_t scr = 1;

    // Since C flags are not affected
    // we keep it in memory
    uint8_t temp = cpu->_c;
    op_add(cpu, &scr, reg_ptr, 2);
    cpu->_c = temp;

    return 1;
}

/// DEC - Register
/// Decrement
int16_t op_48(uint8_t* buffer, uint16_t i, struct CPU* cpu __attribute__((unused))){
    if (cpu->mode == DISASSEMBLY ||
        cpu->mode == DEBUG) fprintf(stderr, "%02x", buffer[i]);

    if (cpu->mode == DISASSEMBLY) spacebar(1);
    if (cpu->mode == DEBUG) spacebar_debug(1);

    uint8_t _reg = buffer[i] & 7;
    if (cpu->mode == DISASSEMBLY ||
        cpu->mode == DEBUG) fprintf(stderr, "dec %s", reg[8 + _reg]);

    void* reg_ptr = get_reg(cpu, 8 + _reg);
    int16_t scr = 1;

    // Since C flags are not affected
    // we keep it in memory
    uint8_t temp = cpu->_c;
    op_sub(cpu, &scr, reg_ptr, 2);
    cpu->_c = temp;

    return 1;
}

/// PUSH - Register
/// PUSH REG to SP
int16_t op_50(uint8_t* buffer, uint16_t i, struct CPU* cpu __attribute__((unused))){
    if (cpu->mode == DISASSEMBLY ||
        cpu->mode == DEBUG) fprintf(stderr, "%02x", buffer[i]);

    if (cpu->mode == DISASSEMBLY) spacebar(1);
    if (cpu->mode == DEBUG) spacebar_debug(1);

    uint8_t _reg = buffer[i] & 7;
    void* reg_ptr = get_reg(cpu, 8 + _reg);

    cpu->SP -= 2;
    void* des_ptr = MEMORY(cpu->SP);

    op_mov(cpu, reg_ptr, des_ptr, 2);

    if (cpu->mode == DISASSEMBLY ||
        cpu->mode == DEBUG) fprintf(stderr, "push %s", reg[8 + _reg]);

    return 1;
}

/// POP - Register
/// POP SP to REG
int16_t op_58(uint8_t* buffer, uint16_t i, struct CPU* cpu __attribute__((unused))){
    if (cpu->mode == DISASSEMBLY ||
        cpu->mode == DEBUG) fprintf(stderr, "%02x", buffer[i]);

    if (cpu->mode == DISASSEMBLY) spacebar(1);
    if (cpu->mode == DEBUG) spacebar_debug(1);

    uint8_t _reg = buffer[i] & 7;
    void* reg_ptr = get_reg(cpu, 8 + _reg);

    void* src_ptr = MEMORY(cpu->SP);   
    cpu->SP += 2; 

    op_mov(cpu, src_ptr, reg_ptr, 2);

    if (cpu->mode == DISASSEMBLY ||
        cpu->mode == DEBUG) fprintf(stderr, "pop %s", reg[8 + _reg]);
    
    return 1;
}

/// JMP ON
int16_t op_7X(uint8_t* buffer, uint16_t i, struct CPU* cpu __attribute__((unused))){
    //printf("tf?\n");
    if (cpu->mode == DISASSEMBLY ||
        cpu->mode == DEBUG) fprintf(stderr, "%02x%02x", buffer[i], buffer[i+1]);

    if (cpu->mode == DISASSEMBLY) spacebar(2);
    if (cpu->mode == DEBUG) spacebar_debug(2);

    uint8_t jmp_nb = buffer[i] & 15;
    int16_t disp = (int8_t) buffer[i+1];
    int jmp = op_jmp_on[jmp_nb](cpu);
    //printf("\nis? : %i ; isn't? : %i\n", jmp, op_jmp_on[jmp_nb-1](cpu));
    //printf("c : %i\n", cpu->_c);
    disp += i+2;

    if (cpu->mode == DISASSEMBLY ||
        cpu->mode == DEBUG) fprintf(stderr, "%s %04x", jmp_on[jmp_nb], disp);

    if (jmp && (cpu->mode == INTERPRETER ||
    cpu->mode == DEBUG)) return disp-i;

    return 2;
}

/// BASICS - Immediate to Register/Memory
int16_t op_80(uint8_t* buffer, uint16_t i, struct CPU* cpu __attribute__((unused))){
    if (cpu->mode == DISASSEMBLY ||
        cpu->mode == DEBUG) fprintf(stderr, "%02x%02x", buffer[i], buffer[i+1]);
    struct infos* k = malloc(sizeof(struct infos));
    fill_infos(k, buffer, i);
    uint8_t op_nb = (buffer[i+1] >> 3) & 7;
    char* des = ea[k->_rm];
    void* des_ptr = NULL;

    uint8_t disp_nb = mod_switch(buffer, i, cpu, k->_mod, k->_rm, &des, (k->w << 3) + k->_rm, &des_ptr);
    uint8_t data_size = 1 + ((k->s == 0 && k->w == 1)? 1 : 0);

    if (k->w == 0){
        int8_t data_l = (int8_t) buffer[i+2+disp_nb];
        if (cpu->mode == DISASSEMBLY ||
            cpu->mode == DEBUG) fprintf(stderr, "%02x", buffer[i+2+disp_nb]);
        
        if (cpu->mode == DISASSEMBLY) spacebar(3 + disp_nb);
        if (cpu->mode == DEBUG) spacebar_debug(3 + disp_nb);

        if (k->_mod != 3 &&
        (cpu->mode == DISASSEMBLY || cpu->mode == DEBUG))
            fprintf(stderr, "%s byte", basics_str[op_nb]);
        if (cpu->mode == DISASSEMBLY || cpu->mode == DEBUG){
            if (data_l < 0)
                fprintf(stderr, " %s, -%hhx", des, -1 * data_l);
            else
                fprintf(stderr, " %s, %hhx", des, data_l);
        }

        if (cpu->mode == DEBUG) memory_update(buffer, i, cpu, k);
        
        basics[op_nb](cpu, &data_l, des_ptr, 1);
    }
    else{
        int16_t data_x;
        if (k->s == 0){
            data_x = ((uint16_t)buffer[i+3+disp_nb]<<8) + buffer[i+2+disp_nb];
            if (cpu->mode == DISASSEMBLY ||
                cpu->mode == DEBUG) fprintf(stderr, "%02x%02x", buffer[i+2+disp_nb], buffer[i+3+disp_nb]);
                
            if (cpu->mode == DISASSEMBLY) spacebar(2 + disp_nb + data_size);
            if (cpu->mode == DEBUG) spacebar_debug(2 + disp_nb + data_size);

            if (cpu->mode == DISASSEMBLY ||
                cpu->mode == DEBUG) fprintf(stderr, "%s %s, %04hx", basics_str[op_nb], des, data_x);
            
            if (cpu->mode == DEBUG) memory_update(buffer, i, cpu, k);

            basics[op_nb](cpu, &data_x, des_ptr, 2);
        }
        else{
            //printf("\nHERE 1\n");
            data_x = (int8_t) buffer[i+2+disp_nb];
            if (cpu->mode == DISASSEMBLY ||
                cpu->mode == DEBUG) fprintf(stderr, "%02x", buffer[i+2+disp_nb]);

            if (cpu->mode == DISASSEMBLY) spacebar(2 + disp_nb + data_size);
            if (cpu->mode == DEBUG) spacebar_debug(2 + disp_nb + data_size);

            if (data_x < 0){
                if (cpu->mode == DISASSEMBLY || cpu->mode == DEBUG)
                    fprintf(stderr, "%s %s, -%hx", basics_str[op_nb], des, -1 * data_x);
            }
            else{
                if (cpu->mode == DISASSEMBLY || cpu->mode == DEBUG)
                    fprintf(stderr, "%s %s, %hx", basics_str[op_nb], des, data_x);
            }

            if (cpu->mode == DEBUG) memory_update(buffer, i, cpu, k);

            basics[op_nb](cpu, &data_x, des_ptr, 2);
        }        
    }

    free(k);
    return 2 + disp_nb + data_size;
}

/// TEST - Register/Memory and Register
///         And Function to Flags, No Result:
int16_t op_84(uint8_t* buffer, uint16_t i, struct CPU* cpu __attribute__((unused))){
    if (cpu->mode == DISASSEMBLY ||
        cpu->mode == DEBUG) fprintf(stderr, "%02x%02x", buffer[i], buffer[i+1]);

    struct infos* k = malloc(sizeof(struct infos));
    fill_infos(k, buffer, i);

    uint8_t op_nb = buffer[i] >> 3; // Gets the value of the operations
    int8_t disp_nb;
    int8_t data_nb = 0;

    if (k->d == 0){
        char* src_0 = reg[(k->w << 3) + k->_reg];
        void* src_ptr_0 = get_reg(cpu, (k->w << 3) + k->_reg);

        char* des_0 = ea[k->_rm];
        void* des_ptr_0 = NULL;

        disp_nb = mod_switch(buffer, i, cpu, k->_mod, k->_rm, &des_0, (k->w << 3) + k->_rm, &des_ptr_0);

        if (cpu->mode == DISASSEMBLY) spacebar(2 + disp_nb);
        if (cpu->mode == DEBUG) spacebar_debug(2 + disp_nb);

        if (cpu->mode == DISASSEMBLY ||
            cpu->mode == DEBUG) fprintf(stderr, "test %s, %s", des_0, src_0); 

        if (cpu->mode == DEBUG) memory_update(buffer, i, cpu, k);
        
        op_testadd(cpu, src_ptr_0, des_ptr_0, 1 + k->w);
    }
    else{
        char* src = ea[k->_rm];
        void* src_ptr = NULL;

        char* des = reg[(k->w << 3) + k->_reg];
        void* des_ptr = get_reg(cpu, (k->w << 3) + k->_reg);

        disp_nb = mod_switch(buffer, i, cpu, k->_mod, k->_rm, &src, (k->w << 3) + k->_rm, &src_ptr);

        if (cpu->mode == DISASSEMBLY) spacebar(2 + disp_nb);
        if (cpu->mode == DEBUG) spacebar_debug(2 + disp_nb);

        if (cpu->mode == DISASSEMBLY ||
            cpu->mode == DEBUG) fprintf(stderr, "test %s, %s", des, src); 
        
        if (cpu->mode == DEBUG) memory_update(buffer, i, cpu, k);

        op_testadd(cpu, src_ptr, des_ptr, 1 + k->w);
    }

    free(k);
    
    return 2 + disp_nb;
}

/// XCHG - Exchange
///         Register/Memory with Register
int16_t op_86(uint8_t* buffer, uint16_t i, struct CPU* cpu __attribute__((unused))){
    fprintf(stderr, "%02x%02x", buffer[i], buffer[i+1]);
    struct infos* k = malloc(sizeof(struct infos));
    fill_infos(k, buffer, i);
    
    char* src = reg[(k->w << 3) + k->_reg]; 
    char* des = ea[k->_rm]; 
    void* temp; 

    int8_t disp_nb = mod_switch(buffer, i, cpu, k->_mod, k->_rm, &des, (k->w << 3) + k->_rm, &temp);

    spacebar(2 + disp_nb);

    fprintf(stderr, "xchg");
    fprintf(stderr, " %s, %s", des, src);
    free(k);

    return 2 + disp_nb;
}

/// MOV - Register/Memory to/from Register
int16_t op_88(uint8_t* buffer, uint16_t i, struct CPU* cpu __attribute__((unused))){
    if (cpu->mode == DISASSEMBLY ||
        cpu->mode == DEBUG) fprintf(stderr, "%02x%02x", buffer[i], buffer[i+1]);

    struct infos* k = malloc(sizeof(struct infos));
    fill_infos(k, buffer, i);

    int8_t disp_nb;

    if (k->d == 0){ //if d == 0, then 'from' reg
        char* src_0 = reg[(k->w << 3) + k->_reg];
        void* src_ptr_0 = get_reg(cpu, (k->w << 3) + k->_reg);

        char* des_0 = ea[k->_rm];
        void* des_ptr_0 = NULL;

        disp_nb = mod_switch(buffer, i, cpu, k->_mod, k->_rm, &des_0, (k->w << 3) + k->_rm, &des_ptr_0);

        if (cpu->mode == DISASSEMBLY) spacebar(2 + disp_nb);
        if (cpu->mode == DEBUG) spacebar_debug(2 + disp_nb);

        if (cpu->mode == DISASSEMBLY ||
            cpu->mode == DEBUG) fprintf(stderr, "mov %s, %s", des_0, src_0); //yep
        
        if (cpu->mode == DEBUG) memory_update(buffer, i, cpu, k);

        if (cpu->mode == INTERPRETER ||
            cpu->mode == DEBUG) op_mov(cpu, src_ptr_0, des_ptr_0, 1 + k->w);
    }
    else{ //if d == 1, then 'to' reg
        char* src = ea[k->_rm];
        void* src_ptr = NULL;

        char* des = reg[(k->w << 3) + k->_reg];
        void* des_ptr = get_reg(cpu, (k->w << 3) + k->_reg);
        
        disp_nb = mod_switch(buffer, i, cpu, k->_mod, k->_rm, &src, (k->w << 3) + k->_rm, &src_ptr);

        if (cpu->mode == DISASSEMBLY) spacebar(2 + disp_nb);
        if (cpu->mode == DEBUG) spacebar_debug(2 + disp_nb);

        if (cpu->mode == DISASSEMBLY ||
            cpu->mode == DEBUG) fprintf(stderr, "mov %s, %s", des, src); //yep
        
        if (cpu->mode == DEBUG) memory_update(buffer, i, cpu, k);

        if (cpu->mode == INTERPRETER ||
            cpu->mode == DEBUG) op_mov(cpu, src_ptr, des_ptr, 1 + k->w);
    }

    free(k);

    return 2 + disp_nb;
}

/// LEA - Load EA to Register
int16_t op_8D(uint8_t* buffer, uint16_t i, struct CPU* cpu __attribute__((unused))){
    if (cpu->mode == DISASSEMBLY ||
        cpu->mode == DEBUG) fprintf(stderr, "%02x%02x", buffer[i], buffer[i+1]);
    struct infos* k = malloc(sizeof(struct infos));
    fill_infos(k, buffer, i);
    
    char* src = ea[k->_rm];
    void* src_ptr = NULL;
    char* des = reg[(k->w << 3) + k->_reg];
    void* des_ptr = get_reg(cpu, (k->w << 3) + k->_reg);

    int8_t disp_nb = mod_switch(buffer, i, cpu, k->_mod, k->_rm, &src, (k->w << 3) + k->_rm, &src_ptr);

    if (cpu->mode == DISASSEMBLY) spacebar(2 + disp_nb);
    if (cpu->mode == DEBUG) spacebar_debug(2 + disp_nb);

    if (cpu->mode == DISASSEMBLY ||
        cpu->mode == DEBUG) fprintf(stderr, "lea %s, %s", des, src);

    if (cpu->mode == DEBUG) memory_update(buffer, i, cpu, k);

    int16_t* des16 = (uint16_t*) des_ptr;
    *des16 = get_value(buffer, i, cpu, k->_mod, k->_rm);

    free(k);

    return 2 + disp_nb;
}

/// XCHG - Exchange
///         Register with Accumulator
int16_t op_90(uint8_t* buffer, uint16_t i, struct CPU* cpu __attribute__((unused))){
    if (cpu->mode == DISASSEMBLY ||
        cpu->mode == DEBUG) fprintf(stderr, "%02x", buffer[i]);

    if (cpu->mode == DISASSEMBLY) spacebar(1);
    if (cpu->mode == DEBUG) spacebar_debug(1);

    uint8_t _reg = buffer[i] & 7;
    uint16_t* des_ptr = (uint16_t*) get_reg(cpu, 8 + _reg);

    if (cpu->mode == DISASSEMBLY ||
        cpu->mode == DEBUG) fprintf(stderr, "xchg %s, ax", reg[8 + _reg]);

    uint16_t temp = *des_ptr;
    *des_ptr = cpu->AX;
    cpu->AX = temp;

    return 1;
}

/// CBW - Convert byte to word
int16_t op_98(uint8_t* buffer, uint16_t i, struct CPU* cpu __attribute__((unused))){
    if (cpu->mode == DISASSEMBLY ||
        cpu->mode == DEBUG) fprintf(stderr, "%02x", buffer[i]);

    if (cpu->mode == DISASSEMBLY) spacebar(1);
    if (cpu->mode == DEBUG) spacebar_debug(1);

    if (cpu->mode == DISASSEMBLY ||
        cpu->mode == DEBUG) fprintf(stderr, "cbw");

    uint16_t al = (uint8_t) cpu->AX;
    cpu->AX = al;

    return 1;
}

/// CWD - Confert word to double word
int16_t op_99(uint8_t* buffer, uint16_t i, struct CPU* cpu __attribute__((unused))){
    if (cpu->mode == DISASSEMBLY ||
        cpu->mode == DEBUG) fprintf(stderr, "%02x", buffer[i]);

    if (cpu->mode == DISASSEMBLY) spacebar(1);
    if (cpu->mode == DEBUG) spacebar_debug(1);

    if (cpu->mode == DISASSEMBLY ||
        cpu->mode == DEBUG) fprintf(stderr, "cwd");
    
    uint32_t ax = (uint32_t) cpu->AX;
    uint16_t* ax_l = (uint16_t*) &ax;
    uint16_t* dx = ax_l+1;

    cpu->AX = *ax_l;
    cpu->DX = *dx;

    return 1;
}


/// TEST - Immediate Data and Accumulator
///            And Function to Flags, No Result:
int16_t op_A8(uint8_t* buffer, uint16_t i, struct CPU* cpu __attribute__((unused))){
    fprintf(stderr, "%02x", buffer[i]);
    struct infos* k = malloc(sizeof(struct infos));
    fill_infos(k, buffer, i);
    uint8_t data_size = 0;

    if (k->w == 0){
        fprintf(stderr, "%02x", buffer[i+1]);
        spacebar(2);
        int8_t data_l = (int8_t) buffer[i+1];
        fprintf(stderr, "test al, %x", data_l);
        data_size = 1;
    }
    else{
        fprintf(stderr, "%02x%02x", buffer[i+1], buffer[i+2]);
        spacebar(3);
        int8_t data_x = (buffer[i+2] << 8) + buffer[i+1];
        fprintf(stderr, "test ax, %x", data_x);
        data_size = 2;
    }    

    free(k);
    return 1 + data_size;
}

/// MOV - Immediate to Register
int16_t op_BX(uint8_t* buffer, uint16_t i, struct CPU* cpu __attribute__((unused))){
    int8_t _reg = buffer[i] & 15; // GET THE LAST 4 BITS
    void* des = get_reg(cpu, buffer[i] & 15);

    if (cpu->mode == DISASSEMBLY) fprintf(stderr, "%02x%02x%02x        ", buffer[i], buffer[i+1], buffer[i+2]);
    if (cpu->mode == DEBUG) fprintf(stderr, "%02x%02x%02x       ", buffer[i], buffer[i+1], buffer[i+2]);

    if (cpu->mode == DISASSEMBLY ||
        cpu->mode == DEBUG) fprintf(stderr, "mov %s, %02x%02x", reg[_reg], buffer[i+2], buffer[i+1]);
    int16_t data = (buffer[i+2] << 8) + buffer[i+1];
    op_mov(cpu, &data, des, 2);
    return 3;
}

/// BASICS4 - THIS ONE
int16_t op_D0(uint8_t* buffer, uint16_t i, struct CPU* cpu __attribute__((unused))){
    if (cpu->mode == DISASSEMBLY ||
        cpu->mode == DEBUG) fprintf(stderr, "%02x%02x", buffer[i], buffer[i+1]);
    struct infos* k = malloc(sizeof(struct infos));
    fill_infos(k, buffer, i);

    uint8_t op_nb = k->_reg;

    char* des = ea[k->_rm];
    void* des_ptr = 0;

    char* src = (k->d == 0)? "1" : "cl";
    uint8_t one = 1;
    void* src_ptr = (k->d == 0)? &one : get_reg(cpu, 9);
    
    uint8_t disp_nb = mod_switch(buffer, i, cpu, k->_mod, k->_rm, &des, (k->w << 3) + k->_rm, &des_ptr);

    if (cpu->mode == DISASSEMBLY) spacebar(2 + disp_nb);
    if (cpu->mode == DEBUG) spacebar_debug(2 + disp_nb);

    if (cpu->mode == DISASSEMBLY ||
        cpu->mode == DEBUG) fprintf(stderr, "%s %s, %s", basics4_str[op_nb],des, src);

    basics4[op_nb](cpu, src_ptr, des_ptr, 1 + k->w);

    free(k);

    return 2 + disp_nb;
}

/// RET - Return from CALL
///         Within Seg Adding Immed to SP
int16_t op_C2(uint8_t* buffer, uint16_t i, struct CPU* cpu __attribute__((unused))){
    if (cpu->mode == DISASSEMBLY ||
        cpu->mode == DEBUG) fprintf(stderr, "%02x%02x%02x", buffer[i], buffer[i+1], buffer[i+2]);

    if (cpu->mode == DISASSEMBLY) spacebar(3);
    if (cpu->mode == DEBUG) spacebar_debug(3);

    if (cpu->mode == DISASSEMBLY ||
        cpu->mode == DEBUG) fprintf(stderr, "ret %02x%02x", buffer[i+2], buffer[i+1]);

    int16_t data = (buffer[i+2] << 8) + buffer[i+1];
    int16_t disp = WORD(cpu->SP);
    cpu->SP += 2;
    cpu->SP += data;

    if (cpu->mode == INTERPRETER ||
        cpu->mode == DEBUG) return disp - i;
    return 3;
}

/// RET - Return from CALL
///         Within Segment
int16_t op_C3(uint8_t* buffer, uint16_t i, struct CPU* cpu __attribute__((unused))){
    if (cpu->mode == DISASSEMBLY ||
        cpu->mode == DEBUG) fprintf(stderr, "%02x", buffer[i]);

    if (cpu->mode == DISASSEMBLY) spacebar(1);
    if (cpu->mode == DEBUG) spacebar_debug(1);

    int16_t disp = WORD(cpu->SP);
    cpu->SP += 2;

    fprintf(stderr, "ret");

    if (cpu->mode == INTERPRETER ||
        cpu->mode == DEBUG) return disp - i;
    
    return 1;
}

/// MOV - Immediate to Register/Memory
int16_t op_C6(uint8_t* buffer, uint16_t i, struct CPU* cpu __attribute__((unused))){
    if (cpu->mode == DISASSEMBLY ||
        cpu->mode == DEBUG) fprintf(stderr, "%02x%02x", buffer[i], buffer[i+1]);
    struct infos* k = malloc(sizeof(struct infos));
    fill_infos(k, buffer, i);

    char* des = ea[k->_rm];
    void* des_ptr = 0; 
    
    int8_t disp_nb = mod_switch(buffer, i, cpu, k->_mod, k->_rm, &des, (k->w << 3) + k->_rm, &des_ptr);
    uint8_t data_size = 0;

    if (k->w == 0){
        if (cpu->mode == DISASSEMBLY ||
            cpu->mode == DEBUG) fprintf(stderr, "%02x", buffer[i+2+disp_nb]);
        data_size = 1;

        if (cpu->mode == DISASSEMBLY) spacebar(2 + disp_nb + data_size);
        if (cpu->mode == DEBUG) spacebar_debug(2 + disp_nb + data_size);

        int8_t data_l = (int8_t) buffer[i+2+disp_nb];

        if (cpu->mode == DISASSEMBLY ||
            cpu->mode == DEBUG) fprintf(stderr, "mov");

        if (k->_mod != 3 && (cpu->mode == DISASSEMBLY || cpu->mode == DEBUG))
            fprintf(stderr, " byte");

        if (cpu->mode == DISASSEMBLY ||
            cpu->mode == DEBUG) fprintf(stderr, " %s, %hhx", des, data_l);

        if (cpu->mode == DEBUG) memory_update(buffer, i, cpu, k);

        if (cpu->mode == INTERPRETER ||
            cpu->mode == DEBUG) op_mov(cpu, &data_l, des_ptr, 1);
    }
    else{
        if (cpu->mode == DISASSEMBLY ||
            cpu->mode == DEBUG) fprintf(stderr, "%02x%02x", buffer[i+2+disp_nb], buffer[i+3+disp_nb]);
        data_size = 2;

        if (cpu->mode == DISASSEMBLY) spacebar(2 + disp_nb + data_size);
        if (cpu->mode == DEBUG) spacebar_debug(2 + disp_nb + data_size);

        int16_t data_x = ((uint16_t)buffer[i+3+disp_nb]<<8) + buffer[i+2+disp_nb];

        if (cpu->mode == DISASSEMBLY ||
            cpu->mode == DEBUG) fprintf(stderr, "mov %s, %04hx", des, data_x);

        if (cpu->mode == DEBUG) memory_update(buffer, i, cpu, k);

        if (cpu->mode == INTERPRETER ||
            cpu->mode == DEBUG) op_mov(cpu, &data_x, des_ptr, 2);
    }

    free(k);

    return 2 + disp_nb + data_size;
}

/// INT - Type Specified
int16_t op_CD(uint8_t* buffer, uint16_t i, struct CPU* cpu __attribute__((unused))){
    if (cpu->mode == DISASSEMBLY ||
        cpu->mode == DEBUG) fprintf(stderr, "%02x%02x", buffer[i], buffer[i+1]);
    
    if (cpu->mode == DISASSEMBLY) spacebar(2);
    if (cpu->mode == DEBUG) spacebar_debug(2);

    if (cpu->mode == DISASSEMBLY ||
        cpu->mode == DEBUG) fprintf(stderr, "int %02x", buffer[i+1]);

    if (cpu->mode == DEBUG) fprintf(stderr, "\n");

    /// SYS_CALL

    if (cpu->mode == INTERPRETER ||
        cpu->mode == DEBUG) intrp_syscall(cpu);

    return 2;
}

/// BASICS2
int16_t op_F6(uint8_t* buffer, uint16_t i, struct CPU* cpu __attribute__((unused))){
    // fprintf(stderr, "\n[%04hx]%04hx\n", 0x0010, WORD(0x0010));
    if (cpu->mode == DISASSEMBLY ||
        cpu->mode == DEBUG) fprintf(stderr, "%02x%02x", buffer[i], buffer[i+1]);

    struct infos* k = malloc(sizeof(struct infos));
    fill_infos(k, buffer, i);

    uint8_t op_nb = k->_reg;
    uint8_t disp_nb = 0;

    if (op_nb >= 2){
        char* des = ea[k->_rm];
        void* des_ptr = 0; 

        disp_nb += mod_switch(buffer, i, cpu, k->_mod, k->_rm, &des, (k->w << 3) + k->_rm, &des_ptr);

        if (cpu->mode == DISASSEMBLY) spacebar(2 + disp_nb);
        if (cpu->mode == DEBUG) spacebar_debug(2 + disp_nb);

        if (cpu->mode == DISASSEMBLY ||
            cpu->mode == DEBUG) fprintf(stderr, "%s", basics2_str[op_nb]);

        if ((k->w == 0) && (cpu->mode == DISASSEMBLY || cpu->mode == DEBUG))
            fprintf(stderr, " byte");

        if (cpu->mode == DISASSEMBLY ||
            cpu->mode == DEBUG) fprintf(stderr, " %s", des);
        
        if (cpu->mode == INTERPRETER ||
                cpu->mode == DEBUG) basics2[op_nb](cpu, des_ptr, 1 + k->w);
    }
    else
        disp_nb += op_test(cpu, buffer, i, k);
    free(k);

    // fprintf(stderr, "\n[%04hx]%04hx\n", 0x0010, WORD(0x0010));
    return 2 + disp_nb;
}

/// 
int16_t op_E2(uint8_t* buffer, uint16_t i, struct CPU* cpu __attribute__((unused))){
    fprintf(stderr, "%02x%02x", buffer[i], buffer[i+1]);
    spacebar(2);
    int16_t value = (i+2) + ((int8_t) buffer[i+1]);
    fprintf(stderr, "loop %x", value);
    return 2;
}

/// IN - Fixed Port
/// w = 0;
int16_t op_E4(uint8_t* buffer, uint16_t i, struct CPU* cpu __attribute__((unused))){
    fprintf(stderr, "%02x%02x", buffer[i], buffer[i+1]);
    spacebar(2);
    fprintf(stderr, "in al, %x", buffer[i+1]);
    return 2;
}

/// IN - Fixed Port
/// w = 1;
int16_t op_E5(uint8_t* buffer, uint16_t i, struct CPU* cpu __attribute__((unused))){
    fprintf(stderr, "%02x%02x", buffer[i], buffer[i+1]);
    spacebar(2);
    fprintf(stderr, "in ax, %x", buffer[i+1]);
    return 2;
}

/// w = 0;
int16_t op_EC(uint8_t* buffer, uint16_t i, struct CPU* cpu __attribute__((unused))){
    fprintf(stderr, "%02x", buffer[i]);
    spacebar(1);
    fprintf(stderr, "in al, dx");
    return 1;
}

/// w = 1;
int16_t op_ED(uint8_t* buffer, uint16_t i, struct CPU* cpu __attribute__((unused))){
    fprintf(stderr, "%02x", buffer[i]);
    spacebar(1);
    fprintf(stderr, "in ax, dl");
    return 1;
}

/// CALL - Direct within Segment
int16_t op_E8(uint8_t* buffer, uint16_t i, struct CPU* cpu __attribute__((unused))){
    if (cpu->mode == DISASSEMBLY ||
        cpu->mode == DEBUG) fprintf(stderr, "%02x%02x%02x", buffer[i], buffer[i+1], buffer[i+2]);

    if (cpu->mode == DISASSEMBLY) spacebar(3);
    if (cpu->mode == DEBUG) spacebar_debug(3);

    cpu->SP -= 2;
    void* des_ptr = MEMORY(cpu->SP);

    int16_t disp = (buffer[i+2] << 8) + buffer[i+1] + (i+3);
    uint16_t addr = i + 3;

    if (cpu->mode == DISASSEMBLY ||
        cpu->mode == DEBUG) fprintf(stderr, "call %04x", disp);

    op_mov(cpu, &addr, des_ptr, 2);

    if (cpu->mode == INTERPRETER ||
            cpu->mode == DEBUG) return disp-i;

    return 3;
}

/// JMP - Direct within Segment
int16_t op_E9(uint8_t* buffer, uint16_t i, struct CPU* cpu __attribute__((unused))){
    if (cpu->mode == DISASSEMBLY ||
        cpu->mode == DEBUG) fprintf(stderr, "%02x%02x%02x", buffer[i], buffer[i+1], buffer[i+2]);

    if (cpu->mode == DISASSEMBLY) spacebar(3);
    if (cpu->mode == DEBUG) spacebar_debug(3);

    int16_t disp = (buffer[i+2] << 8) + buffer[i+1] + (i+3);

    if (cpu->mode == DISASSEMBLY ||
        cpu->mode == DEBUG) fprintf(stderr, "jmp %04x", disp);

    if (cpu->mode == INTERPRETER ||
            cpu->mode == DEBUG) return disp-i;

    return 3;
}

/// JMP - Direct within Segment-Short
int16_t op_EB(uint8_t* buffer, uint16_t i, struct CPU* cpu __attribute__((unused))){
    if (cpu->mode == DISASSEMBLY ||
        cpu->mode == DEBUG) fprintf(stderr, "%02x%02x", buffer[i], buffer[i+1]);

    if (cpu->mode == DISASSEMBLY) spacebar(2);
    if (cpu->mode == DEBUG) spacebar_debug(2);

    int16_t disp = ((int8_t) buffer[i+1]) + (i+2);

    if (cpu->mode == DISASSEMBLY ||
        cpu->mode == DEBUG) fprintf(stderr, "jmp short %04x", disp);

    if (cpu->mode == INTERPRETER ||
            cpu->mode == DEBUG) return disp-i;

    return 2;
}

/// REP : Repeat
int16_t op_F2(uint8_t* buffer, uint16_t i, struct CPU* cpu __attribute__((unused))){
    fprintf(stderr, "%02x%02x", buffer[i], buffer[i+1]);
    spacebar(2);
    if ((buffer[i+1] & 7) == 4)
        fprintf(stderr, "rep movsb");
    else
        fprintf(stderr, "rep movsw");
    return 2;
}

/// HLT : Halt
int16_t op_F4(uint8_t* buffer, uint16_t i, struct CPU* cpu __attribute__((unused))){
    fprintf(stderr, "%02x", buffer[i]);
    spacebar(1);
    fprintf(stderr, "hlt");
    return 1;
}

/// CLD - Clear Direction
int16_t op_FC(uint8_t* buffer, uint16_t i, struct CPU* cpu __attribute__((unused))){
    fprintf(stderr, "%02x", buffer[i]);
    spacebar(1);
    fprintf(stderr, "cld");
    return 1;
}

/// STD - Set Direction 
int16_t op_FD(uint8_t* buffer, uint16_t i, struct CPU* cpu __attribute__((unused))){
    fprintf(stderr, "%02x", buffer[i]);
    spacebar(1);
    fprintf(stderr, "std");
    return 1;
}

/// INC + DEC : Register/Memory with (w = 0)!
/// Increment or Decrement r/m by 1
int16_t op_FE(uint8_t* buffer, uint16_t i, struct CPU* cpu __attribute__((unused))){
    if (cpu->mode == DISASSEMBLY ||
        cpu->mode == DEBUG) fprintf(stderr, "%02x%02x", buffer[i], buffer[i+1]);
    struct infos* k = malloc(sizeof(struct infos));
    fill_infos(k, buffer, i);

    uint8_t op_nb = k->_reg;

    char* des = ea[k->_rm];
    void* des_ptr = 0; 

    int16_t scr = 1;
    
    int8_t disp_nb = mod_switch(buffer, i, cpu, k->_mod, k->_rm, &des, k->_rm, &des_ptr);

    if (cpu->mode == DISASSEMBLY) spacebar(2 + disp_nb);
    if (cpu->mode == DEBUG) spacebar_debug(2 + disp_nb);

    // fprintf(stderr, "%x\n", op_nb);
    if (cpu->mode == DISASSEMBLY ||
        cpu->mode == DEBUG) fprintf(stderr, "%s %s", basics3[op_nb], des);

    if (cpu->mode == DEBUG) memory_update(buffer, i, cpu, k);

    if (cpu->mode == INTERPRETER || cpu->mode == DEBUG){
        uint8_t temp = cpu->_c;
        if (op_nb == 0) // INC : Increment
            op_add(cpu, &scr, des_ptr, 1);
        else // DEC : Decrement
            op_sub(cpu, &scr, des_ptr, 1);
        cpu->_c = temp;
    }

    free(k);

    return 2 + disp_nb;
}


/// BASICS3 + (INC + DEC with (w = 1)!)
int16_t op_FF(uint8_t* buffer, uint16_t i, struct CPU* cpu __attribute__((unused))){
    if (cpu->mode == DISASSEMBLY ||
        cpu->mode == DEBUG) fprintf(stderr, "%02x%02x", buffer[i], buffer[i+1]);
    struct infos* k = malloc(sizeof(struct infos));
    fill_infos(k, buffer, i);

    uint8_t op_nb = k->_reg;
    char* src = ea[k->_rm];
    void* src_ptr = 0; 
    int16_t one = 1;
    int16_t dest;

    int8_t disp_nb = mod_switch(buffer, i, cpu, k->_mod, k->_rm, &src, (1 << 3) + k->_rm, &src_ptr);

    if (cpu->mode == DISASSEMBLY) spacebar(2 + disp_nb);
    if (cpu->mode == DEBUG) spacebar_debug(2 + disp_nb);

    if (cpu->mode == DISASSEMBLY ||
        cpu->mode == DEBUG) fprintf(stderr, "%s %s", basics3[op_nb], src);

    if (cpu->mode == DEBUG) memory_update(buffer, i, cpu, k);

    if (cpu->mode == INTERPRETER || cpu->mode == DEBUG){
        uint8_t temp = cpu->_c;
        switch(op_nb){
            case 0: // INC : Increment
                op_add(cpu, &one, src_ptr, 2);
                cpu->_c = temp;
                break;
            case 1: // DEC : Decrement
                op_sub(cpu, &one, src_ptr, 2);
                cpu->_c = temp;
                break;
            case 2: // CALL
            case 3: 
                cpu->SP -= 2;
                void* stack_ptr = MEMORY(cpu->SP);
                uint16_t addr = i + 2 + disp_nb;
                dest = *((int16_t *)src_ptr);

                op_mov(cpu, &addr, stack_ptr, 2);

                if (cpu->mode == INTERPRETER ||
                    cpu->mode == DEBUG) return dest - i;
                break;
            case 4: // JMP
            case 5:
                dest = *((int16_t *)src_ptr);

                if (cpu->mode == INTERPRETER ||
                    cpu->mode == DEBUG) return dest - i;
                break;
            case 6: // PUSH
                cpu->SP -= 2;
                void* des_ptr = MEMORY(cpu->SP);

                op_mov(cpu, src_ptr, des_ptr, 2);
                break;
            default:
                break;
        }
    }

    free(k);

    return 2 + disp_nb;
}

int16_t (*op[])(uint8_t* buffer, uint16_t i, struct CPU* cpu __attribute__((unused))) = {
    &op_00, &op_00, &op_00, &op_00, &op_01, &op_01, NULL  , NULL  , &op_00, &op_00, &op_00, &op_00, &op_01, &op_01, NULL  , NULL  , // 0
    &op_00, &op_00, &op_00, &op_00, &op_01, &op_01, NULL  , NULL  , &op_00, &op_00, &op_00, &op_00, &op_01, &op_01, NULL  , NULL  , // 1
    &op_00, &op_00, &op_00, &op_00, &op_01, &op_01, NULL  , &op_XX, &op_00, &op_00, &op_00, &op_00, &op_01, &op_01, NULL  , &op_XX, // 2
    &op_00, &op_00, &op_00, &op_00, &op_01, &op_01, NULL  , &op_XX, &op_00, &op_00, &op_00, &op_00, &op_01, &op_01, NULL  , &op_XX, // 3
    &op_40, &op_40, &op_40, &op_40, &op_40, &op_40, &op_40, &op_40, &op_48, &op_48, &op_48, &op_48, &op_48, &op_48, &op_48, &op_48, // 4
    &op_50, &op_50, &op_50, &op_50, &op_50, &op_50, &op_50, &op_50, &op_58, &op_58, &op_58, &op_58, &op_58, &op_58, &op_58, &op_58, // 5
    NULL  , NULL  , NULL  , NULL  , NULL  , NULL  , NULL  , NULL  , NULL  , NULL  , NULL  , NULL  , NULL  , NULL  , NULL  , NULL  , // 6
    &op_7X, &op_7X, &op_7X, &op_7X, &op_7X, &op_7X, &op_7X, &op_7X, &op_7X, &op_7X, &op_7X, &op_7X, &op_7X, &op_7X, &op_7X, &op_7X, // 7
    &op_80, &op_80, &op_80, &op_80, &op_84, &op_84, &op_86, &op_86, &op_88, &op_88, &op_88, &op_88, &op_XX, &op_8D, &op_XX, &op_XX, // 8
    &op_90, &op_90, &op_90, &op_90, &op_90, &op_90, &op_90, &op_90, &op_98, &op_99, &op_XX, &op_XX, &op_XX, &op_XX, &op_XX, &op_XX, // 9
    &op_XX, &op_XX, &op_XX, &op_XX, &op_XX, &op_XX, &op_XX, &op_XX, &op_A8, &op_A8, &op_XX, &op_XX, &op_XX, &op_XX, &op_XX, &op_XX, // A
    &op_BX, &op_BX, &op_BX, &op_BX, &op_BX, &op_BX, &op_BX, &op_BX, &op_BX, &op_BX, &op_BX, &op_BX, &op_BX, &op_BX, &op_BX, &op_BX, // B
    NULL  , NULL  , &op_C2, &op_C3, &op_XX, &op_XX, &op_C6, &op_C6, NULL  , NULL  , &op_XX, &op_XX, &op_XX, &op_CD, &op_XX, &op_XX, // C
    &op_D0, &op_D0, &op_D0, &op_D0, &op_XX, &op_XX, &op_XX, &op_XX, &op_XX, &op_XX, &op_XX, &op_XX, &op_XX, &op_XX, &op_XX, &op_XX, // D
    &op_XX, &op_XX, &op_E2, &op_XX, &op_E4, &op_E5, &op_XX, &op_XX, &op_E8, &op_E9, &op_XX, &op_EB, &op_EC, &op_ED, &op_XX, &op_XX, // E
    &op_XX, &op_XX, &op_F2, &op_F2, &op_F4, &op_XX, &op_F6, &op_F6, &op_XX, &op_XX, &op_XX, &op_XX, &op_FC, &op_FD, &op_FE, &op_FF  // F
    //   0       1       2       3       4       5       6       7       8       9       A       B       C       D       E       F
};