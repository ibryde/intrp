#include <stdio.h>
#include <unistd.h> //write() & read() function
#include <err.h> //errx() function
#include <fcntl.h> //open() function
#include <stdint.h> //uint_s variable
#include <string.h> //string manipulation
#include <stdlib.h>

#include "./tools.h"
#include "./type.h"

char* reg[] = {
    "al", "cl", "dl", "bl", "ah", "ch", "dh", "bh",
    "ax", "cx", "dx", "bx", "sp", "bp", "si", "di"
};

char* ea[] = {
    "bx+si", "bx+di", "bp+si", "bp+di",
    "si", "di", "bp", "bx"
};

char hex[] = {'0', '1', '2', '3',
                '4', '5', '6', '7',
                '8', '9', 'a', 'b',
                'c', 'd', 'e', 'f'};

void fill_infos(struct infos* info, uint8_t* buffer, uint16_t i){
    info->w = buffer[i] & 1;
    info->d = (buffer[i] >> 1) & 1;
    info->s = (buffer[i] >> 1) & 1;
    
    info->_mod = (buffer[i+1] >> 6) & 3; 
    info->_reg = (buffer[i+1] >> 3) & 7;
    info->_rm = buffer[i+1] & 7; 
}


/// Puts brackets before and after a given string.
/// Ease the process of disassembler
char* bracket(char* s){
    size_t l = strlen(s);
    char* res = calloc(l+3, sizeof(char));
    res[0] = '[';
    for(size_t i = 0; i<l; i++)
        res[i+1] = s[i];
    res[l+1] = ']';
    return res;
}

/// Adds the displacement number on a given string.
/// Ease the process of disassembler
char* dispstr(char* s, int16_t disp){
    uint16_t disp_abs = (disp < 0)? (disp * -1) : disp;
    size_t l = strlen(s);
    size_t i = 1;
    // i = 0 because if d < 10 it has a length of 1

    for (int16_t temp = disp_abs; temp >= 16; i++)
        temp /= 16;
    // printf("\ndisp = %x ; i = %u", disp, i);
    char* res = calloc(l + 2 + i, sizeof(char));
    // 1 + ... cause we count the length of the sign
    for (size_t j = 0; j < l; j++)
        res[j] = s[j];
    res[l] = (disp < 0)? '-' : '+'; 

    do {
        res[l + i] = hex[disp_abs%16];
        disp_abs /= 16;
        i--;
    } while(i > 0);
    
    return res;
}

char* printnb16(int16_t nb){
    char* res = calloc(5, sizeof(char));
    for (size_t i = 0; i < 4; i++){
        res[3 - i] = hex[nb%16];
        nb /= 16;
    }
    return res;
}

char* printnb8(int16_t nb){
    char* res = calloc(3, sizeof(char));
    for (size_t i = 0; i < 2; i++){
        res[1 - i] = hex[nb%16];
        nb /= 16;
    }
    return res;

}

/// Estimate how much from the opcode to the function
/// Ease the process of disassembler
void spacebar(size_t nb_bytes){
    size_t nb_spaces = 7 - (nb_bytes);
    // x2 : A byte is represented by 2 hex number
    for (size_t i = 0; i < nb_spaces; i++){
        printf("  ");
    }
}

void spacebar_debug(size_t nb_bytes){
    size_t nb_spaces = 13 - (nb_bytes*2);
    for (size_t i = 0; i < nb_spaces; i++){
        printf(" ");
    }
}

/// Get the reg
void* get_reg(struct CPU* cpu, uint8_t _reg){
    uint8_t* ptr;
    switch(_reg){
        case 0: // w = 0 and reg = 000;
            return (void*) &(cpu->AX);
        case 1: // w = 0 and reg = 001;
            return (void*) &(cpu->CX);
        case 2: // w = 0 and reg = 010;
            return (void*) &(cpu->DX);
        case 3: // w = 0 and reg = 011;
            return (void*) &(cpu->BX);
        case 4: // w = 0 and reg = 100;
            ptr = (uint8_t*) &(cpu->AX);
            ptr++;
            return (void *) ptr;
        case 5: // w = 0 and reg = 101;
            ptr = (uint8_t*) &(cpu->CX);
            ptr++;
            return (void *) ptr;
        case 6: // w = 0 and reg = 110;
            ptr = (uint8_t*) &(cpu->DX);
            ptr++;
            return (void *) ptr;
        case 7: // w = 0 and reg = 111;
            ptr = (uint8_t*) &(cpu->BX);
            ptr++;
            return (void *) ptr;
        case 8: // w = 1 and reg = 000;
            return (void*) &(cpu->AX);
        case 9: // w = 1 and reg = 001;
            return (void*) &(cpu->CX);
        case 10: // w = 1 and reg = 01;
            return (void*) &(cpu->DX);
        case 11: // w = 1 and reg = 01;
            return (void*) &(cpu->BX);
        case 12: // w = 1 and reg = 10;
            return (void*) &(cpu->SP);
        case 13: // w = 1 and reg = 10;
            return (void*) &(cpu->BP);
        case 14: // w = 1 and reg = 11;
            return (void*) &(cpu->SI);
        default: // w = 1 and reg = 11;
            return (void*) &(cpu->DI);
    }
}


/// Get the effective adress with the rm
uint32_t get_memory(struct CPU* cpu, uint8_t _rm, int16_t disp){
    uint32_t ptr;
    switch(_rm){
        case 0: //bx+si
            ptr = cpu->BX + cpu->SI + disp;
            break;
        case 1: //bx+di
            ptr = cpu->BX + cpu->DI + disp;
            break;
        case 2: //bp+si
            ptr = cpu->BP + cpu->SI + disp;
            break;
        case 3: //bp+di
            ptr = cpu->BP + cpu->DI + disp;
            break;
        case 4: //si
            ptr = cpu->SI + disp;
            break;
        case 5: //di
            ptr = cpu->DI + disp;
            break;
        case 6: //bp
            ptr = cpu->BP + disp;
            break;
        case 7: //bx
            ptr = cpu->BX + disp;
            break;
        default: // ea = disp
            ptr = disp;
            break;
    }
    return ptr;
}

/// Get the effective adress with the rm
void* get_ea(struct CPU* cpu, uint8_t _rm, int16_t disp){
    return (void*) &(cpu->memory[get_memory(cpu, _rm, disp)]);
}

uint16_t get_value(uint8_t* buffer, uint16_t i, struct CPU* cpu, uint8_t _mod, uint8_t _rm){
    if (_mod == 3)
        return 0;
        
    int16_t disp = 0;
    switch(_mod){
        case 0: // (_rm = 6)? disp = 0 : ea = disp
            if (_rm == 6){
                disp = (((int16_t) buffer[i+3]) << 8) + ((int16_t) buffer[i+2]);
                _rm = 8;
            }
            break;
        case 1:
            disp = (int8_t) buffer[i+2];
            break;
        default:
            disp = (((int16_t) buffer[i+3]) << 8) + ((int16_t) buffer[i+2]);
            break;
    }

    return get_memory(cpu, _rm, disp);
}

/// Memory update for the debug mod
void memory_update(uint8_t* buffer, uint16_t i, struct CPU* cpu, uint8_t _mod, uint8_t _rm){
    if (_mod == 3)
        return;
        
    int16_t disp = 0;
    switch(_mod){
        case 0: // (_rm = 6)? disp = 0 : ea = disp
            if (_rm == 6){
                disp = (((int16_t) buffer[i+3]) << 8) + ((int16_t) buffer[i+2]);
                _rm = 8;
            }
            break;
        case 1:
            disp = (int8_t) buffer[i+2];
            break;
        default:
            disp = (((int16_t) buffer[i+3]) << 8) + ((int16_t) buffer[i+2]);
            break;
    }

    uint16_t value = get_memory(cpu, _rm, disp);
    printf(" ;[%04hx]%04hx", value, WORD(value));
}