#include "mtf.h"

#define ASCIISZ      255
#define BUFFSIZE    8192
#define INDEXBUFF      2
#define MAXNAME     1024

// #define DEBUG_BWT

static byte_t mtf[ASCIISZ];
static byte_t where[ASCIISZ];

void mtf_init()
{
    int i;

    for(i = 0; i < ASCIISZ; i++) {
        mtf[i] = where[i] = i;
    }
}

void mtf_encode(char *fname, char *target)
{
    int fdread, fdwrite;
    int i, j;
    int nbytes;
    int index;
    byte_t buffer[BUFFSIZE];

    if((fdread = open(fname, O_RDONLY)) == -1) {
        fprintf(stderr, "%s: could not open file\n", fname);
        exit(1);
    }

    if((fdwrite = open(target, O_WRONLY | O_CREAT, 0644)) == -1) {
        fprintf(stderr, "%s: could not open file\n", target);
        exit(1);
    }

    mtf_init();

    for(;;) {
        nbytes = read(fdread, buffer, BUFFSIZE);

        if(nbytes <= 0)
            break;
    
        index = bwt_encode(buffer, nbytes);

        for(i = 0; i < nbytes; i++) {
            for(j = 0; j < ASCIISZ; j++) {
                where[mtf[j]] = j;
            }
            write(fdwrite, &where[buffer[i]], 1);
            mtf_move(buffer[i], (int)where[buffer[i]]);
        }
        write(fdwrite, &index, sizeof(int));
    }

    close(fdwrite);
    close(fdread);
}

void mtf_move(byte_t c, int index)
{
    for(; index >= 1; index--) 
        mtf[index] = mtf[index - 1];
    mtf[0] = c;
}

void mtf_decode(char *fname, char *target)
{
    int fdread, fdwrite;
    int i;
    int nbytes;
    int index;
    byte_t *ibuff;
    byte_t curr;
    byte_t buffer[BUFFSIZE];

    if((fdread = open(fname, O_RDONLY)) == -1) {
        fprintf(stderr, "%s: could not open file\n", fname);
        exit(1);
    }

    if((fdwrite = open(target, O_WRONLY | O_CREAT, 0644)) == -1) {
        fprintf(stderr, "%s: could not open file\n", target);
        exit(1);
    }

    mtf_init();

    for(;;) {
        nbytes = read(fdread, buffer, BUFFSIZE);

        if(nbytes <= 0) {
            break;
        } if(nbytes == BUFFSIZE) {
            read(fdread, &index, sizeof(int));
        } else {
            ibuff = (byte_t *)malloc(sizeof(int) * sizeof(byte_t));
            for(i = sizeof(int) - 1; i >= 0; --i)
                ibuff[i] = buffer[--nbytes];
            memcpy(&index, ibuff, sizeof(int));
            free(ibuff);
        }

        for(i = 0; i < nbytes; i++) {
//             write(fdwrite, &mtf[buffer[i]], 1);
            curr = mtf[buffer[i]];
            mtf_move(mtf[buffer[i]], (int)buffer[i]);
            buffer[i] = curr;
        }
        
        bwt_decode(buffer, nbytes, index);
        write(fdwrite, buffer, nbytes);
    }
    
    close(fdwrite);
    close(fdread);
}
