#ifndef BWT_H_
#define BWT_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef
    unsigned char
byte_t;

typedef struct suffix {
    byte_t *suffix;
    int index;
    int n;
} SUFFIX;

int bwt_encode(byte_t buffer[], int n);
void bwt_decode(byte_t buffer[], int n, int primary);

#endif

