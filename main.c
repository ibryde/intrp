#include <stdio.h>
#include <unistd.h> //write() & read() function
#include <err.h> //errx() function
#include <fcntl.h> //open() function
#include <stdint.h> //uint_s variable

#include "parser.h"


/// "./intrp -d file"

int main(int argc, char* argv[]){
    int fd;

    if (argc != 3){
        errx(2, "intrp -d name_of_the_file\n");
        return 1;
    }

    fd = open(argv[2], O_RDONLY);
    if (fd == -1){
        errx(2, "unable to access the file");
        return 1;
    }

    parser(fd);

    return 0;
}