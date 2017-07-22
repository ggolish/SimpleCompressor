#include "huffman.h"

// #define DEBUG
// #define DEBUG_HEAP
// #define DEBUG_BINCODE

byte_t huffmanEncode(char *fileName, char *target, byte_t lengths[])
{
    int i, numBytes;
    byte_t bits;
    BYTE byteList[ASCIIMAX];
    BYTE *countList[ASCIIMAX];
    BYTE *root;
    
    // initialize list of bytes and read in frequencies
    makeList(byteList);
    countFile(byteList, fileName);
    numBytes = 0;

    // make list of pointers to bytes with nonzero frequencies
    for(i = 0; i < ASCIIMAX; i++) {
        if(byteList[i].count) {
            countList[numBytes++] = &byteList[i];
        }
    }
    
    #ifdef DEBUG
        for(i = 0; i < numBytes; i++) 
            printf("value: %3d, count: %6d\n", countList[i]->value, countList[i]->count);
    #endif

    // build huffman tree
    root = huffmanTree(countList, numBytes);

    #ifdef DEBUG
        printf("root value: %d, root count: %d\n", root->value, root->count);
    #endif

    // get lengths for each byte in huffman tree
    byteLengths(root, 0, lengths);

    #ifdef DEBUG
        printLengths(root);
    #endif

    // rebuild list of pointers
    for(i = numBytes = 0; i < ASCIIMAX; i++) {
        if(byteList[i].count) {
            countList[numBytes++] = &byteList[i];
        }
    }
    
    // sort list of pointers by: (1) code length (2) ascii value
    qsort(countList, numBytes, sizeof(BYTE *), standardSort);

    // calculate and store the standard huffman code for each byte
    huffmanStandard(countList, numBytes);

    // encode file
    bits = encodeFile(byteList, numBytes, fileName, target);

    // free huffman tree
    freeHuffTree(root);

    return bits;
}

void huffmanDecode(char *fname, char *target, byte_t *lengths, byte_t lastBits)
{
    BITFILE *fd_read;
    BYTE *bytePointers[ASCIIMAX];
    BYTE byteList[ASCIIMAX];
    bcount_t bit_count, curr_bit;
    code_t code;
    int length;
    int bit;
    int fd_write;
    int i, numBytes;
    struct stat statbuff;

    // build list of bytes from header information
    makeList(byteList);
    for(i = numBytes = 0; i < ASCIIMAX; i++) {
        byteList[i].length = lengths[i];
        if(byteList[i].length)
            bytePointers[numBytes++] = &byteList[i];
    }

    qsort(bytePointers, numBytes, sizeof(BYTE *), standardSort);
    huffmanStandard(bytePointers, numBytes);

    if((fd_read = bit_open(fname, BITREAD)) == NULL) {
        fprintf(stderr, "%s: could not open file\n", fname);
        exit(0);
    }

    if((fd_write = open(target, O_WRONLY | O_CREAT, 0644)) == -1) {
        fprintf(stderr, "%s: could not open file\n", target);
        exit(0);
    }

    fstat(fd_read->fd, &statbuff);

    bit_count = (statbuff.st_size * 8) - lastBits;

    length = 1;
    code = 0;
    for(curr_bit = 0; (bit = bit_read(fd_read)) != -1 && curr_bit < bit_count; curr_bit++) {
        code |= bit;
        for(i = 0; i < numBytes; i++) {
            if(bytePointers[i]->code == code) {
                if(bytePointers[i]->length == length) {
                    code = 0;
                    length = 0;
                    write(fd_write, &bytePointers[i]->value, 1);
                }
            }
        }
        code <<= 1;
        length++;
    }

    bit_close(fd_read);
    free(fd_read);
    close(fd_write);
}

byte_t encodeFile(BYTE byteList[], int numBytes, char *fileName, char *target)
{
    byte_t buffer[NUMBYTES];
    byte_t bits;
    int fdread;
    BITFILE *fdwrite;
    int n, i;

    if((fdread = open(fileName, O_RDONLY)) == -1) {
        fprintf(stderr, "huffman read error\n");
        exit(0);
    }
    
    fdwrite = bit_open(target, BITWRITE);

    for(;;) {
        if((n = read(fdread, buffer, NUMBYTES)) <= 0)
            break;
        for(i = 0; i < n; i++) {
            writeCode(fdwrite, byteList[buffer[i]].code, byteList[buffer[i]].length);         
        }
    }

    #ifdef DEBUG_BINCODE
        printf("\n");
    #endif

    close(fdread);
    bits = bit_close(fdwrite);
    free(fdwrite);
    return bits;
}

void writeCode(BITFILE *fd, code_t code, int length)
{
    code_t mask;
    
    for(mask = 1 << (length - 1); mask; mask >>= 1) {
        if((code & mask)) {
            bit_write(fd, 1);
            #ifdef DEBUG_BINCODE
                printf("1");
            #endif
        } else {
            bit_write(fd, 0);
            #ifdef DEBUG_BINCODE
                printf("0");
            #endif
        }
    }
}

void countFile(BYTE byteList[], char *fileName)
{
    int fd, numBytes, i;
    byte_t buffer[NUMBYTES];

    if((fd = open(fileName, O_RDONLY)) == -1) {
        fprintf(stderr, "%s: cannot open file\n", fileName);
        exit(0);
    }

    for(;;) {
        if((numBytes = read(fd, buffer, NUMBYTES)) <= 0) {
            break;
        }
        for(i = 0; i < numBytes; i++) {
            byteList[buffer[i]].count++;
        }
    }

    close(fd);
}

void makeList(BYTE byteList[])
{
    int i;

    for(i = 0; i < ASCIIMAX; i++) {
        byteList[i].value = i;
        byteList[i].count = 0;
        byteList[i].length = 0;
        byteList[i].code = 0;
        byteList[i].left = byteList[i].right = NULL;
    }
}

BYTE *huffmanTree(BYTE **byteList, int numBytes)
{
    BYTE *a, *b;

    minHeap(byteList, numBytes);
    #ifdef DEBUG_HEAP
        printHeap(byteList, numBytes);
    #endif

    while(numBytes > 1) {
        a = byteList[0];
        byteList[0] = byteList[--numBytes];
        downHeap(byteList, numBytes, 0);
        #ifdef DEBUG_HEAP
            printHeap(byteList, numBytes);
        #endif
        b = innerNode(a, byteList[0]);
        byteList[0] = b;
        downHeap(byteList, numBytes, 0);
        #ifdef DEBUG_HEAP
            printHeap(byteList, numBytes);
        #endif
    }

    return b;
}

void printHeap(BYTE **byteList, int numBytes)
{
    int i;

    printf("--------------\n");
    for(i = 0; i < numBytes; i++) {
        printf("value: %3d, count: %4d, length: %3d\n", 
                byteList[i]->value, byteList[i]->count, byteList[i]->length);
    }
    printf("--------------\n");
}

void minHeap(BYTE **byteList, int numBytes)
{
    int i;

    for(i = (numBytes - 2) / 2; i >= 0; i--) {
        downHeap(byteList, numBytes, i);
    }
}

void downHeap(BYTE **byteList, int numBytes, int index)
{
    int max, lchild, rchild;
    BYTE *tmp;

    max = index;
    lchild = 2 * index + 1;
    rchild = 2 * index + 2;

    if(lchild < numBytes && byteList[lchild]->count < byteList[max]->count) {
        max = lchild;
    }

    if(rchild < numBytes && byteList[rchild]->count < byteList[max]->count) {
        max = rchild;
    }

    if(max != index) {
        tmp = byteList[index];
        byteList[index] = byteList[max];
        byteList[max] = tmp;
        downHeap(byteList, numBytes, max);
    }
}

void freeHuffTree(BYTE *tree)
{
    if(!tree) return;

    freeHuffTree(tree->left);
    freeHuffTree(tree->right);
    if(tree->left != NULL || tree->right != NULL)
        free(tree);
}

void printLengths(BYTE *tree)
{
    if(!tree) return;

    printLengths(tree->left);

    if(tree->left == NULL && tree->right == NULL)
        printf("value: %3d, length: %3d, count: %6d\n", 
                tree->value, tree->length, tree->count);

    printLengths(tree->right);
}

void byteLengths(BYTE *tree, int depth, byte_t lengths[])
{
    if(!tree) return;
    if(!tree->left && !tree->right) {
        lengths[tree->value] = depth;
        tree->length = depth;
    } else {
        byteLengths(tree->left, depth + 1, lengths);
        byteLengths(tree->right, depth + 1, lengths);
    }
}

int standardSort(const void *a, const void *b)
{
    BYTE *x = *(BYTE **)a;
    BYTE *y = *(BYTE **)b;
    int ldiff = x->length - y->length;
    int vdiff = x->value - y->value;

    if(ldiff != 0)
        return ldiff;
    else if(vdiff != 0)
        return vdiff;
    else 
        return 0;
}

void huffmanStandard(BYTE **byteList, int numBytes)
{
    int i;
    int last;
    code_t value;

    last = byteList[0]->length;
    value = 0;

    for(i = 0; i < numBytes; i++) {
        if(byteList[i]->length != last) {
            value <<= (byteList[i]->length - last);
            last = byteList[i]->length;
        }
        byteList[i]->code = value++;
        #ifdef DEBUG
            printf("%5d %016lx ", byteList[i]->value, byteList[i]->code);
            printCodeBinary(byteList[i]->code, byteList[i]->length);
            printf("\n");
        #endif
    }
}

void printCodeBinary(code_t code, int length)
{
    code_t mask = 1 << (length - 1);

    while(mask) {
        if((code & mask))
            printf("1");
        else
            printf("0");
        mask >>= 1;
    }
}

BYTE *innerNode(BYTE *lchild, BYTE *rchild)
{
    BYTE *tmp;

    tmp = (BYTE *)malloc(sizeof(BYTE));
    tmp->value = INNERLEAF;
    tmp->length = INNERLEAF;
    tmp->code = INNERLEAF;
    tmp->count = lchild->count + rchild->count;
    tmp->left = lchild;
    tmp->right = rchild;

    return tmp;
}
