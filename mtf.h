#ifndef MTF_H_
#define MTF_H_

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "bwt.h"

typedef
    unsigned char
byte_t;

void mtf_decode(char *fname, char *target);
void mtf_encode(char *fname, char *target);
void mtf_extension(char *fname, char *ext);
void mtf_init();
void mtf_move(byte_t c, int index);

#endif
