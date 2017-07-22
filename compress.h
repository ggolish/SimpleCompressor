#ifndef COMPRESS_H_
#define COMPRESS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mtf.h"
#include "huffman.h"

#define BUFFSIZE    1024
#define MAGICNUM       3
#define ASCIIMAX     256

typedef
    unsigned char
byte_t;

typedef struct __attribute__((packed)) {
    byte_t magic[MAGICNUM];
    byte_t lastbits;
    byte_t lengths[ASCIIMAX];
}HEADER;

void compress_456(char *file_name);
void decompress_456(char *file_name);

#endif
