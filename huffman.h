#ifndef HUFFMAN_H_
#define HUFFMAN_H_

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "bits.h"

#define ASCIIMAX    256
#define INNERLEAF    -1
#define NUMBYTES     16
#define BYTESIZE      8

typedef
    unsigned long int
code_t;

typedef 
    unsigned char
byte_t;

typedef struct byte {
   byte_t value;
   code_t code;
   int count;
   int length;
   struct byte *left, *right;
}BYTE;

BYTE *huffmanTree(BYTE **byteList, int numBytes);
BYTE *innerNode(BYTE *lchild, BYTE *rchild);
byte_t encodeFile(BYTE byteList[], int numBytes, char *fileName, char *target);
byte_t huffmanEncode(char *fileName, char *target, byte_t lengths[]);
int standardSort(const void *a, const void *b);
void byteLengths(BYTE *tree, int depth, byte_t lengths[]);
void countFile(BYTE byteList[], char *fileName);
void downHeap(BYTE **byteList, int numBytes, int index);
void freeHuffTree(BYTE *tree);
void huffmanDecode(char *fname, char *target, byte_t *lengths, byte_t lastBits);
void huffmanStandard(BYTE **byteList, int numBytes);
void makeList(BYTE byteList[]);
void minHeap(BYTE **byteList, int numBytes);
void printCodeBinary(code_t code, int length);
void printHeap(BYTE **byteList, int numBytes);
void printLengths(BYTE *tree);
void writeCode(BITFILE *fd, code_t code, int length);

#endif

