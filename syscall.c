#include "syscall.h"

void intrpexit(uint16_t status, struct CPU* cpu) {
    if(cpu->mode == DEBUG) printf("<exit(%d)>\n", status);        
    // if (!currentRuntime->getIsFork()) 
    exit(status);
}

int intrpwrite(uint16_t fd, uint16_t addr, uint16_t len, struct CPU* cpu) {
    if(cpu->mode == DEBUG) printf("<write(%d, 0x%04x, %d)", fd, addr, len);  
    int ret = write(fd, &(cpu->memory[addr]), len);
    if(cpu->mode == DEBUG) printf(" => %d>", ret);    
    cpu->AX = 0;

    return ret;
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

        default: {
            printf("%d: unsupported system call in intrp\n", m->m_type);
            break;
        }
    }

    return result;
}