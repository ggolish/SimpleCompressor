#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "compress.h"

int main(int argc, char *argv[])
{
    int opt;

    if(argc < 2) {
        fprintf(stderr, "usage: %s [-e | -d] <file>\n", argv[0]);
        exit(0);
    }

    while((opt = getopt(argc, argv, "e:d:")) != -1) {
        switch(opt) {
            case 'e':
                compress_456(optarg);
                break;
            case 'd':
                decompress_456(optarg);
                break;
            default:
                fprintf(stderr, "usage: %s [-e | -d] <file>\n", argv[0]);
                exit(0);
        }
    }

    if(argc == 2) {
        compress_456(argv[1]);
    }

    return 0;
}
