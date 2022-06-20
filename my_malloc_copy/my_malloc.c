
/***********************************************************
 *This file is the implementation of the my_malloc library.*
 ***********************************************************/

#include <stdlib.h>
#include "my_malloc.h"
#include "assert.h"
#include <time.h>

#define METADATA_SIZE sizeof(FREE_BLOCK)

FREE_BLOCK *block_list_head = NULL;
unsigned long block_list_size = 0;
unsigned long data_segment_size = 0;
unsigned long data_segment_free_space_size = 0;
// very importent simple func
void *malloc_ret_ptr(void *ptr)
{
    return ptr + METADATA_SIZE;
}

FREE_BLOCK *to_block_ptr(void *ptr)
{
    return (FREE_BLOCK *)(ptr - METADATA_SIZE);
}
// very importent simple func
unsigned long get_data_segment_size()
{
    return data_segment_size;
}

unsigned long get_data_segment_free_space_size()
{
    return data_segment_free_space_size;
}

FREE_BLOCK *create_block(size_t size)
{
    // FREE_BLOCK *ptr = (FREE_BLOCK *)sbrk(size + METADATA_SIZE);
    FREE_BLOCK *ptr = sbrk(size + METADATA_SIZE);
    memset(ptr, 0, sizeof(FREE_BLOCK));
    ptr->size = size;
    data_segment_size += (size + METADATA_SIZE);
    return ptr;
}

void emplace_block(FREE_BLOCK *node, FREE_BLOCK *ptr)
{
    // add as head
    if (node == NULL)
    {
        ptr->next = block_list_head;
        ptr->prev = NULL; //!!!
        if (block_list_head)
            block_list_head->prev = ptr;
        block_list_head = ptr;
        block_list_size++;
        data_segment_free_space_size += (ptr->size + METADATA_SIZE);
        //   return;
    }
    else
    {
        // PrintFREE_BLOCK();
        // PrintFREE_BLOCK();
        ptr->next = node->next;
        ptr->prev = node;
        // if (node->next)
        //     node->next->prev = ptr;
        // node->next = ptr;
        node->next = ptr;
        if (ptr->next)
        {
            ptr->next->prev = ptr;
        }
        block_list_size++;
        data_segment_free_space_size += (ptr->size + METADATA_SIZE);
    }
}

void delete_block(FREE_BLOCK *ptr)
{
    if (ptr == NULL)
        return;
    if (ptr == NULL || block_list_size == 0)
        return;
    // delete head
    // delete head

    if (ptr == block_list_head)
    {
        block_list_head = ptr->next;
        // if (ptr->next)
        //     ptr->next->prev = NULL;
        // block_list_size--;
        // data_segment_free_space_size -= (ptr->size + METADATA_SIZE);
        // return;
    }
    if (ptr->prev)
        ptr->prev->next = ptr->next;
    if (ptr->next)
        ptr->next->prev = ptr->prev;
    block_list_size--;
    data_segment_free_space_size -= (ptr->size + METADATA_SIZE);
}

void merge_block(FREE_BLOCK *begin)
{
    FREE_BLOCK *ptr = begin;
    while (ptr)
    {
        if (((unsigned long)ptr + ptr->size + METADATA_SIZE) == (unsigned long)ptr->next && ptr->next != NULL)
        {
            ptr->size += (ptr->next->size + METADATA_SIZE);
            ptr->next = ptr->next->next;
            if (ptr->next)
            {
                ptr->next->prev = ptr;
            }
            // ptr->next->prev = ptr;
            block_list_size--;
        }
        else break;
        // ptr = ptr->next;
    }
}

FREE_BLOCK *split_block(FREE_BLOCK *ptr, size_t size)
{
    FREE_BLOCK *node = ptr->prev;
    delete_block(ptr);
    if (ptr->size - size <= METADATA_SIZE)
    {
        // delete_block(ptr);
        return ptr;
    }
    
        // FREE_BLOCK *new_block = (FREE_BLOCK *)((void *)ptr + ptr->size + METADATA_SIZE);
        FREE_BLOCK *new_block = (FREE_BLOCK *)((void *)ptr + size + METADATA_SIZE);
        new_block->size = ptr->size - size - METADATA_SIZE;
        ptr->size=size; 
        emplace_block(node, new_block);
        return ptr;
    
}

void *ff_malloc(size_t size)
{
    FREE_BLOCK *ptr = block_list_head;
    while (ptr)
    {
        if (ptr->size == size)
        {
            delete_block(ptr);
            return (void *)ptr + METADATA_SIZE;
        }
        if (ptr->size > size)
        {
            FREE_BLOCK *result_block = split_block(ptr, size);
            return (void *)result_block + METADATA_SIZE;
        }
        ptr = ptr->next;
    }
    FREE_BLOCK *new_block = create_block(size);
    // return (void *)new_block + METADATA_SIZE;
    return malloc_ret_ptr(new_block);
}

void ff_free(void *pointer)
{

    // FREE_BLOCK *ptr = (FREE_BLOCK *)(pointer - METADATA_SIZE);
    FREE_BLOCK *ptr=to_block_ptr(pointer);
    // delete_block(ptr);
    FREE_BLOCK *temp = NULL;
    FREE_BLOCK *pre = block_list_head;
    while (pre)
    {
        if ((unsigned long)pre > (unsigned long)ptr)
        {
            break;
        }
        temp = pre;
        pre = pre->next;
    }
    emplace_block(temp, ptr);
    if (temp == NULL)
    {
        merge_block(block_list_head);
    }
    else
    {
        merge_block(temp);
    }
}
void *bf_malloc(size_t size)
{
    size_t bestsize = ULONG_MAX;
    FREE_BLOCK *ptr = block_list_head;
    FREE_BLOCK *best_block = NULL;
    while (ptr)
    {
        if (ptr->size == size)
        {
            delete_block(ptr);
            return malloc_ret_ptr(ptr);
            // return (void *)ptr + METADATA_SIZE;
        }
        if (ptr->size > size && ptr->size < bestsize)
        {
            best_block = ptr;
            bestsize = ptr->size;
        }
        ptr = ptr->next;
    }
    if (best_block)
    {
        // delete_block(best_block); //!!!!
        split_block(best_block,size);
        // return (void *)best_block + METADATA_SIZE;
        return malloc_ret_ptr(best_block);
    }
    FREE_BLOCK *new_block = create_block(size);
    // return (void *)(new_block + METADATA_SIZE);
    return malloc_ret_ptr(new_block);
}
void *wf_malloc(size_t size)
{
    size_t bigsize = 0;
    FREE_BLOCK *ptr = block_list_head;
    FREE_BLOCK *worse_block = NULL;
    while (ptr)
    {
        if (ptr->size == size)
        {
            delete_block(ptr);
            return malloc_ret_ptr(ptr);
            // return (void *)ptr + METADATA_SIZE;
        }
        if (ptr->size > size && ptr->size > bigsize)
        {
            worse_block = ptr;
            bigsize = ptr->size;
        }
        ptr = ptr->next;
    }
    if (worse_block)
    {
        // delete_block(best_block); //!!!!
        split_block(worse_block,size);
        // return (void *)best_block + METADATA_SIZE;
        return malloc_ret_ptr(worse_block);
    }
    FREE_BLOCK *new_block = create_block(size);
    // return (void *)(new_block + METADATA_SIZE);
    return malloc_ret_ptr(new_block);
}

void bf_free(void *pointer)
{
    ff_free(pointer);
}
void wf_free(void *pointer)
{
    ff_free(pointer);
}
void PrintFREE_BLOCK()
{
    printf("---------------------------------------------------------------------------print Block_List---------------------------------------------------------------------\n");
    printf("blocksize=:%d \n",block_list_size);
   
    FREE_BLOCK *ptr = block_list_head;
    unsigned long block_size = block_list_size;
    int num = 1;
    while (ptr && block_size > 0)
    {
        size_t Blocksize = ptr->size + METADATA_SIZE;
        int Blockbegin_malloc = (int)(ptr);
        int Blockend_malloc = (int)((void *)ptr + ptr->size + METADATA_SIZE);
        size_t ActualBlocksize = Blockend_malloc - Blockbegin_malloc;
        size_t Datesize = ptr->size;

        if (Blocksize != ActualBlocksize)
        {
            printf("Error!:Block%d: ", num);
            printf("Blocksize=%d bytes\tActualBlocksize=%d bytes\t\tDatesize=%d bytes\t\t", Blocksize, ActualBlocksize, Datesize);
            printf("Blockbegin_malloc=%d\tBlockend_malloc=%d\n", Blockbegin_malloc, Blockend_malloc);
        }
        else
        {
            printf("Block%d:is Valid  ", num);
            printf("Blocksize=%d bytes\tActualBlocksize=%d bytes\t\tDatesize=%d bytes\t\t", Blocksize, ActualBlocksize, Datesize);
            printf("Blockbegin_malloc=%d\tBlockend_malloc=%d\n", Blockbegin_malloc, Blockend_malloc);
        }

        ptr = ptr->next;
        block_size--;
        num++;
    }
    //  printf("---------------------------------------------------------------------------print Block_List---------------------------------------------------------------------\n");
}
// void PrintFREE_BLOCK(FREE_BLOCK*begin)
// {
//     FREE_BLOCK* ptr=begin;
//     printf("---------------------------------------------------------------------------print List---------------------------------------------------------------------\n");

//     // printf("blocksize=:%d \n",block_list_size);
//     unsigned long block_size = block_list_size;
//     int num = 1;
//     while (ptr)
//     {
//         size_t Blocksize = ptr->size + METADATA_SIZE;
//         int Blockbegin_malloc = (int)(ptr);
//         int Blockend_malloc = (int)((void *)ptr + ptr->size + METADATA_SIZE);
//         size_t ActualBlocksize = Blockend_malloc - Blockbegin_malloc;
//         size_t Datesize = ptr->size;

//         if (Blocksize != ActualBlocksize)
//         {
//             printf("Error!:Block%d: ", num);
//             printf("Blocksize=%d bytes\tActualBlocksize=%d bytes\t\tDatesize=%d bytes\t\t", Blocksize, ActualBlocksize, Datesize);
//             printf("Blockbegin_malloc=%d\tBlockend_malloc=%d\n", Blockbegin_malloc, Blockend_malloc);
//         }
//         else
//         {
//             // printf("Block%d:is Valid  ", num);
//             // printf("Blocksize=%d bytes\tActualBlocksize=%d bytes\t\tDatesize=%d bytes\t\t", Blocksize, ActualBlocksize, Datesize);
//             // printf("Blockbegin_malloc=%d\tBlockend_malloc=%d\n", Blockbegin_malloc, Blockend_malloc);
//         }

//         ptr = ptr->next;
//         block_size--;
//         num++;
//     }
  
//     //  printf("---------------------------------------------------------------------------print Block_List---------------------------------------------------------------------\n");
// }