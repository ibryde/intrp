#include "syscall.h"

void intrpexit(uint16_t status, struct CPU* cpu){
    if(cpu->mode == DEBUG) fprintf(stderr, "<exit(%d)>\n", status);        
    // if (!currentRuntime->getIsFork()) 
    exit(status);
}

int intrpwrite(uint16_t fd, uint16_t addr, uint16_t len, struct CPU* cpu){
    if(cpu->mode == DEBUG) fprintf(stderr, "<write(%d, 0x%04x, %d)", fd, addr, len);  
    int ret = write(fd, &(cpu->memory[addr]), len);
    if(cpu->mode == DEBUG) fprintf(stderr, " => %d>", ret);    
    cpu->AX = 0;

    return ret;
}

void intrpioctl(uint16_t fd, uint16_t req, uint16_t addr, struct CPU* cpu){
    if(cpu->mode == DEBUG) fprintf(stderr, "<ioctl(%d, 0x%04x, 0x%04x)>", fd, req, addr);
    errno = EINVAL;
    cpu->AX = 0;
}

int intrpbrk(uint16_t addr, struct CPU* cpu, uint16_t* brksize){
    cpu->AX = 0;
    
    if(cpu->mode == DEBUG) fprintf(stderr, "<brk(0x%04x) => ", addr);
    
    if (addr >= ((cpu->SP & ~0x3ff) - 0x400)){
        errno = ENOMEM;
        if(cpu->mode == DEBUG) fprintf(stderr, "ENOMEM>");
        return -1;
    } else {
        *brksize = addr;
        if(cpu->mode == DEBUG) fprintf(stderr, "0>");
        return 0;
    }
}


int intrp_syscall(struct CPU* cpu) {
    int result = -1;

    message *m;
    m = (message*)(cpu->memory + cpu->BX);

    int16_t ret;
    switch(m->m_type) {
        case 1: { // exit
            intrpexit(m->m1_i1, cpu);
            result = m->m1_i1;
            break;
        }
        case 4: { // write
            ret = intrpwrite(m->m1_i1, m->m1_p1, m->m1_i2, cpu);
            (ret == -1)? (m->m_type = -errno) : (m->m_type = ret);
            break;
        }
        case 17: { // brk
            uint16_t addr = WORD(cpu->BX + 10);
            uint16_t brksize = 0;
            if (intrpbrk(addr, cpu, &brksize) == -1){
                m->m_type = -errno;
            }
            else {
                m->m_type = 0;
                WORD(cpu->BX + 18) = brksize;
            }
            break;
        }
        case 54: { // ioctl
            uint16_t fd = WORD(cpu->BX + 4);
            uint16_t request = WORD(cpu->BX + 8);
            uint16_t address = WORD(cpu->BX + 18);
            intrpioctl(fd, request, address, cpu);
            m->m_type = -errno;
            break;
        }
        default: {
            fprintf(stderr, "%d: unsupported system call in intrp\n", m->m_type);
            break;
        }
    }

    return result;
}