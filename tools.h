#ifndef TOOLS
#define TOOLS

struct infos{
    uint8_t w;

    uint8_t d; // s & d & v takes the same spot
    uint8_t s;

    uint8_t _mod;
    uint8_t _reg;
    uint8_t _rm; 
};

struct datas{
    char* src;
    int16_t value;

    uint8_t size;
};

void fill_infos(struct infos* info, uint8_t* buffer, uint16_t i);

char* bracket(char* s);

char* dispstr(char* s, int16_t disp);

char* printnb16(int16_t nb);

char* printnb8(int16_t nb);

void spacebar(size_t nb_bytes);

#endif