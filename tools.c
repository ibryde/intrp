#include <stdio.h>
#include <unistd.h> //write() & read() function
#include <err.h> //errx() function
#include <fcntl.h> //open() function
#include <stdint.h> //uint_s variable
#include <string.h> //string manipulation
#include <stdlib.h>

#include "./tools.h"

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