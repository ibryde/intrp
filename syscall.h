#ifndef SYSCALL_H_
#define SYSCALL_H_

#include <stdio.h>
#include <unistd.h> //write() & read() function
#include <err.h> //errx() function
#include <errno.h>
#include <fcntl.h> //open() function
#include <stdint.h> //uint_s variable
#include <string.h> //string manipulation
#include <stdlib.h>

#include "./tools.h"
#include "./type.h"
#include "cpu.h"

void intrpexit(uint16_t status, struct CPU* cpu);

int intrpwrite(uint16_t fd, uint16_t addr, uint16_t len, struct CPU* cpu);

void intrpioctl(uint16_t fd, uint16_t req, uint16_t addr, struct CPU* cpu);

int intrpbrk(uint16_t addr, struct CPU* cpu, uint16_t* brksize);

int intrp_syscall(struct CPU* cpu);

#endif 