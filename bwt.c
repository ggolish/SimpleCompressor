#include "bwt.h"

// #define DEBUG_SUFFIX

static SUFFIX *make_suffix(int n, int i);
static int suffix_cmp(const void *p1, const void *p2);
static int byte_cmp(const void *p1, const void *p2);

int bwt_encode(byte_t buffer[], int n)
{
    SUFFIX **table;
    byte_t *buff_cpy;
    int i, j, k;
    int primary;

    table = (SUFFIX **)malloc(n * sizeof(SUFFIX *));
    buff_cpy = (byte_t *)malloc(2 * n * sizeof(byte_t));

    for(i = k = 0; i < 2; i++)
        for(j = 0; j < n; j++)
            buff_cpy[k++] = buffer[j];

    for(i = 0; i < n; i++) {
        table[i] = make_suffix(n, i);
        for(j = i, k = 0; j < n + i; j++, k++) {
            table[i]->suffix[k] = buff_cpy[j];
        }
    }
    
    qsort(table, n, sizeof(SUFFIX *), suffix_cmp);

    primary = 0;

    #ifdef DEBUG_SUFFIX
        printf("BEFORE: ");
        for(i = 0; i < n; i++)
            printf("%d ", buffer[i]);
        printf(", %d\n", primary);
    #endif

    for(i = 0; i < n; i++) {
        buffer[i] = table[i]->suffix[n - 1];
        if(table[i]->index == 0)
            primary = i;
        #ifdef DEBUG_SUFFIX
                printf("%d: ", table[i]->index);
            for(j = 0; j < n; j++)
                printf("%d ", table[i]->suffix[j]);
            printf("\n");
        #endif
        free(table[i]->suffix);
        free(table[i]);
    }

    #ifdef DEBUG_SUFFIX
        printf("BWT: ");
        for(i = 0; i < n; i++)
            printf("%d ", buffer[i]);
        printf(", %d\n", primary);
    #endif

    free(table);
    free(buff_cpy);

    return primary;
}

// int bwt_encode(byte_t buffer[], int n)
// {
//     SUFFIX **table;
//     byte_t *buffcpy;
//     int i, j, k;
//     int primary;
// 
//     printf("Before: ");
//     for(i = 0; i < n; i++)
//         printf("%d ", buffer[i]);
//     printf("\n");
// 
//     table = (SUFFIX **)malloc(n * sizeof(SUFFIX *));
//     buffcpy = (byte_t *)malloc(n * sizeof(byte_t));
//     for(i = 0; i < n; i++) {
//         buffcpy[i] = buffer[i];
//         table[i] = make_suffix(n, i);
//         for(j = i, k = 0; j < n; j++, k++) {
//             table[i]->suffix[k] = buffer[j];
//         }
//     }
// 
//     qsort(table, n, sizeof(SUFFIX *), suffix_cmp);
// 
//     for(i = 0; i < n; i++) {
//         if(table[i]->index == 0) {
//             buffer[i] = buffcpy[n - 1];
//             primary = i;
//         } else {
//             buffer[i] = buffcpy[table[i]->index - 1];
//         }
//         #ifdef DEBUG_SUFFIX
//             printf("%d: ", table[i]->index);
//             for(j = 0; j < n - table[i]->index; j++) {
//                 printf("%d ", table[i]->suffix[j]);
//             }
//             printf("\n");
//         #endif
//         free(table[i]->suffix);
//         free(table[i]);
//     }
// 
//     printf("primary index: %d\n", primary);
// 
//     free(table);
//     free(buffcpy);
// 
//     printf("BWT: ");
//     for(i = 0; i < n; i++)
//         printf("%d ", buffer[i]);
//     printf("\n");
// 
//     return primary;
// }

void bwt_decode(byte_t buffer[], int n, int primary)
{
    byte_t *buffcpy;
    byte_t curr;
    int *index;
    int curr_pos, cpy_count, buff_count;
    int i, j;

    buffcpy = (byte_t *)malloc(n * sizeof(byte_t));
    index = (int *)malloc(n * sizeof(int));
    for(i = 0; i < n; i++) {
        buffcpy[i] = buffer[i];
    }
    qsort(buffcpy, n, sizeof(byte_t), byte_cmp);
    #ifdef DEBUG_SUFFIX
        printf("Sorted: ");
    #endif
    for(i = 0; i < n; i++) {
        curr = buffcpy[i];
        #ifdef DEBUG_SUFFIX
            printf("%d ", curr);
        #endif
        for(j = cpy_count = 0; j <= i; j++) {
            if(buffcpy[j] == curr) {
                cpy_count++;
            }
        }
        for(j = buff_count = 0; j < n; j++) {
            if(buffer[j] == curr) {
                buff_count++;
                if(buff_count == cpy_count) {
                    index[i] = j;
                }
            }
        }
    }

    #ifdef DEBUG_SUFFIX
        printf("\n");

        printf("Before:\n");
        for(i = 0; i < n; i++)
            printf("%d\n", index[i]);

        printf("After:\n");
    #endif
    for(i = 0, curr_pos = primary; i < n; i++) {
        curr_pos = index[curr_pos];
        #ifdef DEBUG_SUFFIX
            printf("%d\n", curr_pos);
        #endif
        buffcpy[i] = buffer[curr_pos];
    }

    memcpy(buffer, buffcpy, n);

    #ifdef DEBUG_SUFFIX
        printf("AFTER: ");
        for(i = 0; i < n; i++)
            printf("%d ", buffer[i]);
        printf("\n");
    #endif

    free(buffcpy);
    free(index);
}

static SUFFIX *make_suffix(int n, int i)
{
    SUFFIX *tmp;

    tmp = (SUFFIX *)malloc(sizeof(SUFFIX));
    tmp->suffix = (byte_t *)malloc(n * sizeof(byte_t));
    tmp->index = i;
    return tmp;
}

static int suffix_cmp(const void *p1, const void *p2)
{
    int i;
    SUFFIX *a = *(SUFFIX * const *)p1;
    SUFFIX *b = *(SUFFIX * const *)p2;
    
    for(i = 0;;) {
        if(a->suffix[i] != b->suffix[i]) {
            return a->suffix[i] - b->suffix[i];
        } else {
            i++;
        }
    }
}

static int byte_cmp(const void *p1, const void *p2)
{
    return *(byte_t const *)p1 - *(byte_t const *)p2;
}
