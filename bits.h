#ifndef BITS_H_
#define BITS_H

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#define BUFFMAX     4096
#define BITREAD        0
#define BITWRITE       1
#define BITCLOSED      2
#define BITESIZE       8

typedef
    unsigned char
byte_t;

typedef 
    unsigned long long int
bcount_t;

typedef struct bitfile {
    byte_t buffer[BUFFMAX];
    int bit_pos;
    int byte_pos;
    int byte_num;
    int fd;
    int mode;
}BITFILE;

BITFILE *bit_open(char *path, int mode);
byte_t bit_close(BITFILE *bd);
int bit_read(BITFILE *bd);
void bit_rewind(BITFILE *bd);
void bit_write(BITFILE *bd, int bit);

#endif
