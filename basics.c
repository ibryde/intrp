#include "basics.h"

extern char* reg[]; // arrays of registers
extern char* ea[]; // arrays of effectives adresses


void op_add(struct CPU* cpu, void* src, void* des, uint8_t size){
    if(cpu->mode == INTERPRETER ||
        cpu->mode == DEBUG){
        if (size == 2){
            int16_t* src16 = (uint16_t*) src;
            int16_t* des16 = (uint16_t*) des;
            int32_t res16 =  *des16 + *src16;
            uint32_t carry16 = *((uint16_t*) src) + *((uint16_t*) des);
            //printf("carry = %08x\n", carry16);

            // Set the overflow flag
            if (*src16 < 0 && *des16 < 0)
                cpu->_o = res16 > 0;
            else if (*src16 > 0 && *des16 > 0)
                cpu->_o = res16 < 0;
            // Set the carry flag
            cpu->_c = ((carry16 >> 16) > 0)? 1 : 0;
            // Set the signed flag
            cpu->_s = (res16 < 0);
            // Set the zero flag
            cpu->_z = (res16 == 0);

            *des16 += *src16;
        }
        else{
            int8_t* src8 = (uint8_t*) src;
            int8_t* des8 = (uint8_t*) des;
            int16_t res = *des8 + *src8;
            uint16_t carry = *((uint8_t*) src) + *((uint8_t*) des);
            //printf("carry = %08hx\n", carry);

            // Set the overflow flag
            if (*src8 < 0 && *des8 < 0)
                cpu->_o = res > 0;
            else if (*src8 > 0 && *des8 > 0)
                cpu->_o = res < 0;
            // Set the carry flag
            cpu->_c = ((carry >> 8) > 0)? 1 : 0;
            // Set the signed flag
            cpu->_s = (res < 0);
            // Set the zero flag
            cpu->_z = (res == 0);

            *des8 += *src8;
        }
    }
    // if (cpu->mode == DISASSEMBLY || cpu->mode == DEBUG) printf("add");
    
}

void op_or(struct CPU* cpu, void* src, void* des, uint8_t size){
    if(cpu->mode == INTERPRETER ||
        cpu->mode == DEBUG){
        if (size == 2){
            int16_t* src16 = (uint16_t*) src;
            int16_t* des16 = (uint16_t*) des;

            *des16 |= *src16;

            // Set the signed flag
            cpu->_s = (*des16 < 0);
            // Set the zero flag
            cpu->_z = (*des16 == 0);

        }
        else{
            int8_t* src8 = (uint8_t*) src;
            int8_t* des8 = (uint8_t*) des;
            
            *des8 |= *src8;

            // Set the signed flag
            cpu->_s = (*des8 < 0);
            // Set the zero flag
            cpu->_z = (*des8 == 0);
        }
    }
    // if (cpu->mode == DISASSEMBLY || cpu->mode == DEBUG) printf("or");
}

void op_adc(struct CPU* cpu, void* src, void* des, uint8_t size){
    if(cpu->mode == INTERPRETER ||
        cpu->mode == DEBUG){
        if (size == 2){
            int16_t* src16 = (uint16_t*) src;
            int16_t* des16 = (uint16_t*) des;
            uint8_t c = cpu->_c; // carry
            int32_t res16 =  *des16 + *src16 + c;
            uint32_t carry16 = *((uint16_t*) src) + *((uint16_t*) des) + c;

            // Set the overflow flag
            if (*src16 < 0 && *des16 < 0)
                cpu->_o = res16 > 0;
            else if (*src16 > 0 && *des16 > 0)
                cpu->_o = res16 < 0;
            // Set the carry flag
            cpu->_c = ((carry16 >> 16) > 0)? 1 : 0;
            // Set the signed flag
            cpu->_s = (res16 < 0);
            // Set the zero flag
            cpu->_z = (res16 == 0);

            *des16 += *src16 + c;
        }
        else{
            int8_t* src8 = (uint8_t*) src;
            int8_t* des8 = (uint8_t*) des;
            uint8_t c = cpu->_c; // carry
            int16_t res = *des8 + *src8 + c;
            uint16_t carry = *((uint8_t*) src) + *((uint8_t*) des) + c;

            // Set the overflow flag
            if (*src8 < 0 && *des8 < 0)
                cpu->_o = res > 0;
            else if (*src8 > 0 && *des8 > 0)
                cpu->_o = res < 0;
            // Set the carry flag
            cpu->_c = ((carry >> 8) > 0)? 1 : 0;
            // Set the signed flag
            cpu->_s = (res < 0);
            // Set the zero flag
            cpu->_z = (res == 0);

            *des8 += *src8 + c;
        }
    }
    // if (cpu->mode == DISASSEMBLY || cpu->mode == DEBUG) printf("adc");
}

void op_sbb(struct CPU* cpu, void* src, void* des, uint8_t size){
    if(cpu->mode == INTERPRETER ||
        cpu->mode == DEBUG){
        if (size == 2){
            int16_t* src16 = (uint16_t*) src;
            int16_t* des16 = (uint16_t*) des;
            uint8_t c = cpu->_c; // carry
            int32_t res16 = *des16 - *src16 - c;
            uint32_t carry16 = *((uint16_t*) des) - *((uint16_t*) src) - c;

            // Set the overflow flag
            if (*src16 < 0 && *des16 > 0)
                cpu->_o = res16 < 0;
            else if (*src16 > 0 && *des16 < 0)
                cpu->_o = res16 > 0;
            // Set the carry flag
            cpu->_c = ((carry16 >> 16) > 0)? 1 : 0;
            // Set the signed flag
            cpu->_s = (res16 < 0);
            // Set the zero flag
            cpu->_z = (res16 == 0);

            *des16 -= *src16;
            *des16 -= c;
        }
        else{
            int8_t* src8 = (uint8_t*) src;
            int8_t* des8 = (uint8_t*) des;
            uint8_t c = cpu->_c; // carry
            int16_t res = *des8 - *src8 - c;
            uint16_t carry = *((uint8_t*) src) - *((uint8_t*) des) - c;

            // Set the overflow flag
            if (*src8 < 0 && *des8 > 0)
                cpu->_o = res > 0;
            else if (*src8 > 0 && *des8 < 0)
                cpu->_o = res < 0;
            // Set the carry flag
            cpu->_c = ((carry >> 8) > 0)? 1 : 0;
            // Set the signed flag
            cpu->_s = (res < 0);
            // Set the zero flag
            cpu->_z = (res == 0);

            *des8 -= *src8;
            *des8 -= c;
        }
    }
    // if (cpu->mode == DISASSEMBLY || cpu->mode == DEBUG) printf("sbb");
}

void op_and(struct CPU* cpu, void* src, void* des, uint8_t size){
    if(cpu->mode == INTERPRETER ||
        cpu->mode == DEBUG){
        if (size == 2){
            int16_t* src16 = (uint16_t*) src;
            int16_t* des16 = (uint16_t*) des;

            *des16 &= *src16;

            // Set the flags to zero
            cpu->_c = 0;
            cpu->_o = 0;

            // Set the signed flag
            cpu->_s = (*des16 < 0);
            // Set the zero flag
            cpu->_z = (*des16 == 0);

        }
        else{
            int8_t* src8 = (uint8_t*) src;
            int8_t* des8 = (uint8_t*) des;
            
            *des8 &= *src8;

            // Set the flags to zero
            cpu->_c = 0;
            cpu->_o = 0;

            // Set the signed flag
            cpu->_s = (*des8 < 0);
            // Set the zero flag
            cpu->_z = (*des8 == 0);
        }
    }
    // if (cpu->mode == DISASSEMBLY || cpu->mode == DEBUG) printf("and");
}

void op_testadd(struct CPU* cpu, void* src, void* des, uint8_t size){
    if (size == 2){
        int16_t* src16 = (uint16_t*) src;
        int16_t res16 = *((uint16_t*) des);

        res16 &= *src16;

        // Set the flags to zero
        cpu->_c = 0;
        cpu->_o = 0;

        // Set the signed flag
        cpu->_s = (res16 < 0);
        // Set the zero flag
        cpu->_z = (res16 == 0);

    }
    else{
        int8_t* src8 = (uint8_t*) src;
        int8_t res = *((uint8_t*) des);
        
        res &= *src8;

        // Set the flags to zero
        cpu->_c = 0;
        cpu->_o = 0;

        // Set the signed flag
        cpu->_s = (res < 0);
        // Set the zero flag
        cpu->_z = (res == 0);
    }
}

void op_sub(struct CPU* cpu, void* src, void* des, uint8_t size){
    if(cpu->mode == INTERPRETER ||
        cpu->mode == DEBUG){
        if (size == 2){
            int16_t* src16 = (uint16_t*) src;
            int16_t* des16 = (uint16_t*) des;
            int32_t res16 = *des16 - *src16;
            uint32_t carry16 = *((uint16_t*) des) - *((uint16_t*) src);

            // Set the overflow flag
            if (*src16 < 0 && *des16 > 0)
                cpu->_o = res16 < 0;
            else if (*src16 > 0 && *des16 < 0)
                cpu->_o = res16 > 0;
            // Set the carry flag
            cpu->_c = ((carry16 >> 16) > 0)? 1 : 0;
            // Set the signed flag
            cpu->_s = (res16 < 0);
            // Set the zero flag
            cpu->_z = (res16 == 0);

            *des16 -= *src16;
        }
        else{
            int8_t* src8 = (uint8_t*) src;
            int8_t* des8 = (uint8_t*) des;
            int16_t res = *des8 - *src8;
            uint16_t carry = *((uint8_t*) des) - *((uint8_t*) src);

            // Set the overflow flag
            if (*src8 < 0 && *des8 > 0)
                cpu->_o = res > 0;
            else if (*src8 > 0 && *des8 < 0)
                cpu->_o = res < 0;
            // Set the carry flag
            cpu->_c = ((carry >> 8) > 0)? 1 : 0;
            // Set the signed flag
            cpu->_s = (res < 0);
            // Set the zero flag
            cpu->_z = (res == 0);

            *des8 -= *src8;
        }
    }
    // if (cpu->mode == DISASSEMBLY || cpu->mode == DEBUG) printf("sub");
}

void op_xor(struct CPU* cpu, void* src, void* des, uint8_t size){
    if(cpu->mode == INTERPRETER ||
        cpu->mode == DEBUG){
        if (size == 2){
            int16_t* src16 = (uint16_t*) src;
            int16_t* des16 = (uint16_t*) des;

            *des16 ^= *src16;

            // Set the signed flag
            cpu->_s = (*des16 < 0);
            // Set the zero flag
            cpu->_z = (*des16 == 0);

        }
        else{
            int8_t* src8 = (uint8_t*) src;
            int8_t* des8 = (uint8_t*) des;
            
            *des8 ^= *src8;

            // Set the signed flag
            cpu->_s = (*des8 < 0);
            // Set the zero flag
            cpu->_z = (*des8 == 0);
        }
    }
    // if (cpu->mode == DISASSEMBLY || cpu->mode == DEBUG) printf("xor");
}

void op_cmp(struct CPU* cpu, void* src, void* des, uint8_t size){
    if(cpu->mode == INTERPRETER ||
        cpu->mode == DEBUG){
        if (size == 2){
            int16_t* src16 = (uint16_t*) src;
            int16_t* des16 = (uint16_t*) des;
            int32_t res16 = *des16 - *src16;
            uint32_t carry16 = *((uint16_t*) des) - *((uint16_t*) src);
            //printf("\ncarry = %08x\n", carry16);

            // Set the overflow flag
            if (*src16 < 0 && *des16 > 0)
                cpu->_o = res16 < 0;
            else if (*src16 > 0 && *des16 < 0)
                cpu->_o = res16 > 0;
            // Set the carry flag
            cpu->_c = ((carry16 >> 16) > 0)? 1 : 0;
            // Set the signed flag
            cpu->_s = (res16 < 0);
            // Set the zero flag
            cpu->_z = (res16 == 0);
        }
        else{
            int8_t* src8 = (uint8_t*) src;
            int8_t* des8 = (uint8_t*) des;
            int16_t res = *des8 - *src8;
            uint16_t carry = *((uint8_t*) des) - *((uint8_t*) src);
            //printf("\ncarry = %04hx\n", carry);

            // Set the overflow flag
            if (*src8 < 0 && *des8 > 0)
                cpu->_o = res > 0;
            else if (*src8 > 0 && *des8 < 0)
                cpu->_o = res < 0;
            // Set the carry flag
            cpu->_c = ((carry >> 8) > 0)? 1 : 0;
            // Set the signed flag
            cpu->_s = (res < 0);
            // Set the zero flag
            cpu->_z = (res == 0);
        }
    }
    // if (cpu->mode == DISASSEMBLY || cpu->mode == DEBUG) printf("cmp");
}

void op_mov(struct CPU* cpu, void* src, void* des, uint8_t size){
    if (size == 2){
        int16_t* src16 = (uint16_t*) src;
        int16_t* des16 = (uint16_t*) des;
        *des16 = *src16;
    }
    else{
        int8_t* src8 = (uint8_t*) src;
        int8_t* des8 = (uint8_t*) des;
        *des8 = *src8;
    }
}


uint8_t op_test(struct CPU* cpu, uint8_t* buffer, uint16_t i, struct infos* k){
    char* des = ea[k->_rm];
    void* des_ptr = 0; 
    
    uint8_t disp_nb = mod_switch(buffer, i, cpu, k->_mod, k->_rm, &des, (k->w << 3) + k->_rm, &des_ptr);

    if (k->w == 1){
        if (cpu->mode == DISASSEMBLY ||
            cpu->mode == DEBUG) printf("%02x%02x", buffer[i+2+disp_nb], buffer[i+3+disp_nb]);

        if (cpu->mode == DISASSEMBLY) spacebar(4 + disp_nb);
        if (cpu->mode == DEBUG) spacebar_debug(4 + disp_nb);

        int16_t data_l = (buffer[i+3+disp_nb] << 8) + ((uint8_t) buffer[i+2+disp_nb]);

        // here!
        op_testadd(cpu, &data_l, &des_ptr, 2);

        printf("test %s, %04x", des, data_l);

        if (cpu->mode == DISASSEMBLY ||
            cpu->mode == DEBUG) return 2 + disp_nb;
    }
    else{
        if (cpu->mode == DISASSEMBLY ||
            cpu->mode == DEBUG) printf("%02x", buffer[i+2+disp_nb]);

        if (cpu->mode == DISASSEMBLY) spacebar(3 + disp_nb);
        if (cpu->mode == DEBUG) spacebar_debug(3 + disp_nb);

        int8_t data_s = (int8_t) buffer[i+2+disp_nb];

        // here!
        op_testadd(cpu, &data_s, &des_ptr, 1);

        if (cpu->mode == DISASSEMBLY ||
            cpu->mode == DEBUG) printf("test");

        if (k->_mod != 3 && (cpu->mode == DISASSEMBLY || cpu->mode == DEBUG))
            printf(" byte");

        if (cpu->mode == DISASSEMBLY ||
            cpu->mode == DEBUG) printf(" %s, %hhx", des, data_s);

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

/*
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
*/
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

/// CONDITIONNAL JUMP
int op_jo(struct CPU* cpu){
    return (cpu->_o != 0);
}

int op_jno(struct CPU* cpu){
    return !(op_jo(cpu));
}

int op_jb(struct CPU* cpu){
    return (cpu->_c != 0);
}

int op_jnb(struct CPU* cpu){
    return !(op_jb(cpu));
}

int op_je(struct CPU* cpu){
    return (cpu->_z != 0);
}

int op_jne(struct CPU* cpu){
    return !(op_je(cpu));
}

int op_jbe(struct CPU* cpu){
    return (cpu->_c != 0 || cpu->_z != 0);
}

int op_jnbe(struct CPU* cpu){
    return !(op_jne(cpu));
}

int op_js(struct CPU* cpu){
    return (cpu->_s != 0);
}

int op_jns(struct CPU* cpu){
    return !(op_js(cpu));
}

int op_jp(struct CPU* cpu){
    return 1;
}

int op_jnp(struct CPU* cpu){
    return !(op_jp(cpu));
}

int op_jl(struct CPU* cpu){
    return ((cpu->_s != 0) != (cpu->_o != 0));
}

int op_jnl(struct CPU* cpu){
    return !(op_jl(cpu));
}

int op_jle(struct CPU* cpu){
    return (cpu->_z != 0 ||
            ((cpu->_s != 0) != (cpu->_o != 0)));
}

int op_jnle(struct CPU* cpu){
    return !(op_jle(cpu));
}

/// if (d==0) then str = des
/// if (d==1) then str = src
uint8_t mod_switch(uint8_t* buffer, uint16_t i, struct CPU* cpu, uint8_t _mod,
                    uint8_t _rm, char **str, uint8_t _reg, void** value){
    fflush(stdout);
    int16_t disp;
    uint8_t disp_nb = 0;
    // printf("letme die!\n");
    switch(_mod){
    case 0: // MOD = 00 -> DISP = 0 EXCEPT IF R/M = 110
        // printf("here!\n");
        if (_rm == 6){
            disp_nb = 2;
            disp = (((int16_t) buffer[i+3]) << 8) + ((int16_t) buffer[i+2]);
            if (cpu->mode == DEBUG ||
                cpu->mode == INTERPRETER) *value = get_ea(cpu, 8, disp);
            *str = printnb16(disp);
            if (cpu->mode == DISASSEMBLY ||
                cpu->mode == DEBUG) printf("%02x%02x", buffer[i+2], buffer[i+3]);
        }
        else
            if (cpu->mode == DEBUG ||
                cpu->mode == INTERPRETER) *value = get_ea(cpu, _rm, 0);
        *str = bracket(*str);
        break;
    case 1: // MOD = 01 -> DISP = disp-low in 16bits
        // printf("no here!\n");
        disp_nb = 1;
        disp = (int8_t) buffer[i+2];
        if (cpu->mode == DEBUG ||
            cpu->mode == INTERPRETER) *value = get_ea(cpu, _rm, disp);
        *str = dispstr(*str, disp);
        *str = bracket(*str);
        if (cpu->mode == DISASSEMBLY ||
            cpu->mode == DEBUG) printf("%02x", buffer[i+2]);
        break;
    case 2: // MOD = 10 -> DISP = disp-high ; disp-low
        // printf("heyo??!\n");
        disp_nb = 2;
        disp = (((int16_t) buffer[i+3]) << 8) + ((int16_t) buffer[i+2]);
        if (cpu->mode == DEBUG ||
            cpu->mode == INTERPRETER) *value = get_ea(cpu, _rm, disp);
        *str = dispstr(*str, disp);
        *str = bracket(*str);
        if (cpu->mode == DISASSEMBLY ||
            cpu->mode == DEBUG) printf("%02x%02x", buffer[i+2], buffer[i+3]);
        break;
    default: // MOD = 11 -> R/M is treated as a REG
        // printf("notfunnn!\n");
        // printf("\n_reg = %x\n", _reg);
        *str = reg[_reg];
        // printf("str = %s\n", *str);
        // printf("cpu = %p with cpu->mode = %u\n", cpu, cpu->mode);
        if (cpu->mode == DEBUG || cpu->mode == INTERPRETER){ 
            // printf("value = %p\n", *value);
            *value = get_reg(cpu, _reg);
        }
        break;
    }
    return disp_nb;
}

void (*basics[])(struct CPU* cpu, void* src, void* des, uint8_t size)
         = {&op_add, &op_or, &op_adc, &op_sbb,
            &op_and, &op_sub, &op_xor, &op_cmp};

char* basics_str[] = {"add", "or", "adc", "sbb",
                        "and", "sub", "xor", "cmp"};

void (*basics2[])() = {NULL, NULL, &op_not, &op_neg,
                        &op_mul, &op_imul, &op_div, &op_idiv};

char* basics3[] = {"inc", "dec", "call", "call",
                        "jmp", "jmp", "push", ""};

void (*basics4[])() = {&op_rol, &op_ror, &op_rcl, &op_rcr,
                        &op_shl, &op_shr, NULL, &op_sar};

char* jmp_on[] = {"jo", "jno", "jb", "jnb",
                    "je", "jne", "jbe", "jnbe",
                    "js", "jns", "jp", "jnp",
                    "jl", "jnl", "jle", "jnle"};

int (*op_jmp_on[])(struct CPU* cpu) =
                {&op_jo, &op_jno, &op_jb, &op_jnb,
                    &op_je, &op_jne, &op_jbe, &op_jnbe,
                    &op_js, &op_jns, &op_jp, &op_jnp,
                    &op_jl, &op_jnl, &op_jle, &op_jnle};