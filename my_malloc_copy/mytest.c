#include <stdlib.h>
#include <stdio.h>
#include "my_malloc.h"

#define FF first_fit
#ifdef FF
#define MALLOC(sz) ff_malloc(sz)
#define FREE(p) ff_free(p)
#endif
#ifdef BF
#define MALLOC(sz) bf_malloc(sz)
#define FREE(p) bf_free(p)
#endif

#define INT_size sizeof(int)
#define DOUBLE_size sizeof(double)
int main(int argc, char *argv[])
{
    const unsigned NUM_ITEMS = 100;

    int *array[NUM_ITEMS];
    for (int i = 0; i < NUM_ITEMS; i++)
    {
        if (i % 2 == 0)
        {
            array[i] = (int *)MALLOC(INT_size);
            // FREE(array[i]);
        }
        else
        {
            array[i] = (double *)MALLOC(DOUBLE_size);
            // FREE(array[i]);
        }
        // PrintFREE_BLOCK();
    }
    for (int i = 0; i < NUM_ITEMS; i++)
    {
        // if (i % 2 == 0)
        //     FREE(array[i]);
        FREE(array[i]);
    }

    PrintFREE_BLOCK();
    int *array2[NUM_ITEMS];
    for (int i = 0; i < NUM_ITEMS; i++)
    {

        if (i % 2 == 0)
        {
            array2[i] = (int *)MALLOC(INT_size);
            // FREE(array[i]);
        }
        else
        {
            array2[i] = (double *)MALLOC(DOUBLE_size);
            // FREE(array[i]);
        }
        // PrintFREE_BLOCK();
        printf("i=%d: \n",i);
        PrintFREE_BLOCK();
    }
    for (int i = 0; i < NUM_ITEMS; i++)
    {
        if (i % 2 == 0)
            FREE(array[i]);
        // FREE(array[i]);
    }
    PrintFREE_BLOCK();

    return 0;
}