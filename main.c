#include <stdio.h>
#include <unistd.h> //write() & read() function
#include <err.h> //errx() function
#include <fcntl.h> //open() function
#include <stdint.h> //uint_s variable

#include "parser.h"
#include "tools.h"

/// "./intrp -d file"

int main(int argc, char* argv[]){
    int fd;
    uint8_t mode;

    if (argc > 3 || argc < 2){
        errx(2, "intrp -d name_of_the_file\n");
        return 1;
    }

    fd = open(argv[argc-1], O_RDONLY);
    if (fd == -1){
        errx(2, "unable to access the file");
        return 1;
    }

    if (argc == 3){
        if (argv[1][1] == 'd')
            mode = DISASSEMBLY;
        else if (argv[1][1] == 'm')
            mode = DEBUG;
        else{
            errx(2, "unknown command : (\"%s\")\n\t-d = disassembly mode\n\t-m = debug mode", argv[1]);
            return 1;
        }
    }
    else
        mode = INTERPRETER;


    parser(fd, mode);

    return 0;
}