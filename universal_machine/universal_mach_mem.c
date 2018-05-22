/*
 * Comp40
 * universal_mach_mem.c
 * By: Cihan Sebzeci, Thomas Hendrickson
 * On: 4/11/16 
 * Purpose: implementation of the um memory
 */

#include <stack.h>
#include <stdint.h>
#include <mem.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <except.h>
#include "universal_mach_mem.h"
#include <seq.h>
#include <stdint.h>

#define T Segmem_T

struct T {
        uint32_t mem_size;
        Stack_T stack;
        Seq_T memory;
};

/* Creates a new segment of size = word_size * size
 * Returns the new segment ID
 * it is an Unchecked Runtime Error to call when
 * all the memory has been used
 */
extern uint32_t map(T segmem, uint32_t size) 
{
        uint32_t seg_id, i;
        uint32_t *seg_p;

        seg_p = ALLOC((size + 1) * sizeof(uint32_t));

        seg_p[0] = size;
        for (i = 1; i <= size; ++i) 
                seg_p[i] = 0;

        if (Stack_empty(segmem->stack)) {
                seg_id = segmem->mem_size;
                (segmem->mem_size)++;
                Seq_addhi(segmem->memory, seg_p);
        } 
        else {
                seg_id = (uint64_t)(Stack_pop(segmem->stack));
                Seq_put(segmem->memory, seg_id, seg_p);
        }

        return seg_id;
}

/* Frees the segment with seg_id
 * It is an Unchecked Runtime Error for seg_id to
 * reference an unmapped segment
 * It is an Unchecked Runtime Error to unmap 
 * segment 0
 */
extern void unmap(T segmem, uint64_t seg_id)
{
        uint32_t *seg_pp;

        seg_pp = Seq_get(segmem->memory, seg_id);
	FREE(seg_pp);
	Seq_put(segmem->memory, seg_id, NULL);
        Stack_push(segmem->stack, (void *)seg_id);
}

/* Frees all memory associated with all mapped
 * segment IDs
 */
extern void free_mem(T *segmem)
{
        int i, size;
        uint32_t *seg_pp;
        size = (*segmem)->mem_size;

	for (i = 0; i < size; ++i) {
                seg_pp = Seq_get((*(segmem))->memory, i);
                if (seg_pp != NULL)
	                FREE(seg_pp);
        }

        Seq_free(&((*(segmem))->memory));
        Stack_free(&((*(segmem))->stack));
        FREE(*segmem);
        *segmem = NULL;
}

/* Returns the word refenced by seg_id and offset
 * It is an URE to reference an unmapped segment,
 * or an out of bounds segment offset
 */
extern uint32_t load_word(T segmem, uint32_t seg_id, uint32_t offset)
{
        uint32_t *seg_pp;

        seg_pp = Seq_get(segmem->memory, seg_id);
        return seg_pp[offset + 1];

}


/* Writes the uint32_t word to the memory
 * referenced by seg_id and offset
 * It is an URE to reference an unmapped segment,
 * or an out of bounds segment offset
 */
extern void store_word(T segmem, uint32_t seg_id, uint32_t offset, 
                       uint32_t word)
{
        uint32_t *seg_pp;

        seg_pp = Seq_get(segmem->memory, seg_id);

        seg_pp[offset + 1] = word;
}

/* Copies the segment referenced by seg_id into 
 * segment 0 after freeing the memory in segment
 * 0
 */
extern void load_program(T segmem, uint32_t seg_id)
{
        uint32_t size, i;
        uint32_t *seg_pp, *seg_zero_pp;
	
        seg_pp = Seq_get(segmem->memory, seg_id);
        size = seg_pp[0];

        unmap(segmem, 0);
        map(segmem, size);

        seg_zero_pp = Seq_get(segmem->memory, 0);

        for (i = 0; i <= size; ++i)
                seg_zero_pp[i] = seg_pp[i];
}

/* initializes segment zero to be the pointer
 * 'segment_zero'. This is used for initializing
 * the universal machine memory before program 
 * execution has bepgun.
 */
extern T bootstrap_load(uint32_t *segment_zero, uint32_t seg_size)
{
        T segmem = create_memory();
        uint32_t *seg_zero_p, i;

        seg_zero_p = ALLOC((seg_size + 1) * sizeof(uint32_t));

        seg_zero_p[0] = seg_size;
        for (i = 1; i <= seg_size; ++i) {
                seg_zero_p[i] = segment_zero[i - 1];
        }

        Seq_addhi(segmem->memory, seg_zero_p);

        (segmem->mem_size)++;

        return segmem;
}

extern T create_memory()
{
        T segmem;

        NEW(segmem);
        segmem->stack = Stack_new();
        segmem->mem_size = 0;
        segmem->memory = Seq_new(32);

        return segmem;
}


/****************************************************************/
/*******************TESTING FUNCTIONS****************************/
/***************THIS IS THE END OF THE MEMORY INTERFACE**********/
/*
 * The following are functions for testing purposes only
 * They are used with our test_memory.c file, however
 * they are not outlined in the design doc as they will 
 * not be part of the completed interface 
 */

/* returns the ID at the top of the stack */
extern uint32_t top(T segmem) 
{
        uint64_t temp;
        temp = (uint64_t)Stack_pop(segmem->stack);
        Stack_push(segmem->stack, (void *)temp);
        return temp;
}

/* returns true if the value at index is == NULL
 *         false if not
 */
extern bool empty(T segmem, uint32_t index)
{
        return Seq_get(segmem->memory, index) == NULL;

}
/* 
 * returns the size of the Uarray
 */
extern uint32_t seg_count(T segmem) 
{
        return segmem->mem_size;

}
