#include "compress.h"

// #define DEBUG_HEADER

static char *split_suffix(char *s);
static HEADER *make_header();
static void read_header(HEADER *hdr, char *f1, char *f2);

void compress_456(char *file_name)
{
    HEADER *hdr;
    byte_t buffer[BUFFSIZE];
    char target[BUFFSIZE];
    char temp2[BUFFSIZE];
    char temp[BUFFSIZE];
    int fd, td, num_bytes;

    hdr = make_header();
    
    sprintf(target, "%s.%s", file_name, "456");
    sprintf(temp, "%s.%s", file_name, "mtf");
    sprintf(temp2, "%s.%s", temp, "huff");
    
    mtf_encode(file_name, temp);
    hdr->lastbits = huffmanEncode(temp, temp2, hdr->lengths);

    fd = open(target, O_WRONLY | O_CREAT, 0644);
    write(fd, hdr, sizeof(HEADER));

    td = open(temp2, O_RDONLY);
    for(;;) {
        if((num_bytes = read(td, buffer, BUFFSIZE)) <= 0)
            break;
        write(fd, buffer, num_bytes);
    }

    close(fd);
    close(td);

    unlink(temp);
    unlink(temp2);
    unlink(file_name);

    free(hdr);
}

void decompress_456(char *file_name)
{
    HEADER *hdr;
    char *target;
    char temp[BUFFSIZE];
    char temp2[BUFFSIZE];
    byte_t magic[] = {4, 5, 6};

    target = split_suffix(file_name);
    sprintf(temp, "%s.%s", target, "huff");
    sprintf(temp2, "%s.%s", target, "mtf");

    hdr = (HEADER *)malloc(sizeof(HEADER));
    read_header(hdr, file_name, temp);

    if(memcmp(magic, hdr->magic, MAGICNUM) != 0) {
       fprintf(stderr, "%s: file not compress456 encoded!\n", file_name);
       free(hdr);
       free(target);
       unlink(temp);
       exit(0);
    }

    #ifdef DEBUG_HEADER
        int i;

        printf("Magic number: ");
        for(i = 0; i < MAGICNUM; i++)
            printf("%d ", hdr->magic[i]);
        printf("\n");
        printf("Last bits: %d\n", hdr->lastbits);
        printf("Lengths:\n");
        for(i = 0; i < ASCIIMAX; i++)
            printf("%3d: %d\n", i, hdr->lengths[i]);
    #endif
    
    huffmanDecode(temp, temp2, hdr->lengths, hdr->lastbits);
    free(hdr);
    mtf_decode(temp2, target);
    free(target);
    
    unlink(temp);
    unlink(temp2);
    unlink(file_name);
}

static void read_header(HEADER *hdr, char *f1, char *f2)
{
    byte_t buffer[BUFFSIZE];
    int fd_read, fd_write, n;

    fd_read = open(f1, O_RDONLY);
    fd_write = open(f2, O_WRONLY | O_CREAT, 0644);

    read(fd_read, hdr, sizeof(HEADER));
    for(;;) {
        n = read(fd_read, buffer, BUFFSIZE);
        if(n <= 0)
            break;
        write(fd_write, buffer, n);
    }
    
    close(fd_read);
    close(fd_write);

}

static char *split_suffix(char *s)
{
    char *t, *r;

    r = strdup(s);
    if((t = strrchr(r, '.')) != NULL)
        *t = 0;
    return r;
}

static HEADER *make_header()
{
    HEADER *tmp;
    int i;

    tmp = (HEADER *)malloc(sizeof(HEADER));
    tmp->magic[0] = 0x4;
    tmp->magic[1] = 0x5;
    tmp->magic[2] = 0x6;
    tmp->lastbits = 0;
    for(i = 0; i < ASCIIMAX; i++)
        tmp->lengths[i] = 0;

    return tmp;
}
