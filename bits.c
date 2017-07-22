#include "bits.h"

static int read_bytes(BITFILE *bd);

BITFILE *bit_open(char *path, int mode)
{
    BITFILE *tmp;

    tmp = (BITFILE *)malloc(sizeof(BITFILE));
    tmp->bit_pos = 0;
    tmp->byte_pos = 0;
    tmp->mode = mode;

    if(mode == BITWRITE) {
        if((tmp->fd = open(path, O_WRONLY | O_CREAT, 0644)) == -1) {
            free(tmp);
            return NULL;
        } else {
            memset(tmp->buffer, 0, BUFFMAX * sizeof(byte_t));
            return tmp;
        }
    } else if(mode == BITREAD) {
        if((tmp->fd = open(path, O_RDONLY)) == -1) {
            free(tmp);
            return NULL;
        } else {
            if(read_bytes(tmp) == -1) {
                free(tmp);
                return NULL;
            }
            return tmp;
        }
    }

    return NULL;
}

static int read_bytes(BITFILE *bd)
{
    int num_bytes;

    memset(bd->buffer, 0, BUFFMAX * sizeof(byte_t));
    num_bytes = read(bd->fd, bd->buffer, BUFFMAX);
    bd->byte_num = num_bytes;
    return num_bytes;
}

void bit_rewind(BITFILE *bd)
{
    if(bd->mode == BITCLOSED) return;

    bd->byte_pos = 0;
    bd->bit_pos = 0;
    if(bd->mode == BITWRITE) {
        memset(bd->buffer, 0, BUFFMAX * sizeof(byte_t));
    } else {
        read_bytes(bd);
    }
}

int bit_read(BITFILE *bd)
{
    if(bd->mode != BITREAD) return -1;

    int bit;

    if(bd->bit_pos == BITESIZE) {
        bd->byte_pos++;
        bd->bit_pos = 0;
        if(bd->byte_pos == bd->byte_num) {
            bd->byte_pos = 0;
            if(read_bytes(bd) == 0) {
                return -1;
            }
        }
    }

    if (bd->buffer[bd->byte_pos] & (1 << (BITESIZE - bd->bit_pos - 1))) {
        bit = 1;
    } else {
        bit = 0;
    }

    bd->bit_pos++;
    return bit;
}

void bit_write(BITFILE *bd, int bit)
{
    if(bd->mode != BITWRITE) return;

    if(bit) {
        bd->buffer[bd->byte_pos] |= (1 << (BITESIZE - bd->bit_pos - 1));
    }

    bd->bit_pos++;
    if(bd->bit_pos == BITESIZE) {
        bd->bit_pos = 0;
        bd->byte_pos++;
        if(bd->byte_pos == BUFFMAX) {
            write(bd->fd, bd->buffer, BUFFMAX);
            bd->byte_pos = 0;
            memset(bd->buffer, 0, BUFFMAX * sizeof(byte_t));
        }
    }
}

byte_t bit_close(BITFILE *bd)
{
    if(bd->mode == BITCLOSED) return 0;

    if(bd->mode == BITREAD) {
        close(bd->fd);
        bd->mode = BITCLOSED;
        return 0;
    }

    if(bd->bit_pos > 0) {
        bd->byte_pos++;
    }

    write(bd->fd, bd->buffer, bd->byte_pos);
    close(bd->fd);
    bd->mode = BITCLOSED;

    if(bd->bit_pos)
        return BITESIZE - bd->bit_pos;
    else
        return 0;
}
