/*
 * Comp40
 * universal_mach_mem.h
 * By: Cihan Sebzeci, Thomas Hendrickson
 * On: 4/6/16 
 * Purpose: interface for the universal_mach_mem.c 
 */

#ifndef UNIVERSAL_MACH_MEM_H
#define UNIVERsAL_MACH_MEM_H

#include <stdbool.h>
#include <stdint.h>

#define T Segmem_T 
typedef struct T *T;


/* Creates a new segment of size = word_size * size
 * Returns the new segment ID
 * it is an Unchecked Runtime Error to call when
 * all the memory has been used
 */
extern uint32_t map(T segmem, uint32_t size);

/* Frees the segment with seg_id
 * It is an Unchecked Runtime Error for seg_id to
 * reference an unmapped segment
 * It is an Unchecked Runtime Error to unmap 
 * segment 0
 */
extern void unmap(T segmem, uintptr_t seg_id);


/* Frees all memory associated with all mapped
 * segment IDs
 */
extern void free_mem(T *segmem);

/* Returns the word refenced by seg_id and offset
 * It is an URE to reference an unmapped segment,
 * or an out of bounds segment offset
 */
extern uint32_t load_word(T segmem, uint32_t seg_id, uint32_t offset);

/* Writes the uint32_t word to the memory
 * referenced by seg_id and offset
 * It is an URE to reference an unmapped segment,
 * or an out of bounds segment offset
 */
extern void store_word(T segmem, uint32_t seg_id, uint32_t offset, 
                       uint32_t word);


/* Copies the segment referenced by seg_id into 
 * segment 0 after freeing the memory in segment
 * 0
 */
extern void load_program(T segmem, uint32_t seg_id);


/* initializes segment zero to be the pointer
 * 'segment_zero'. This is used for initializing
 * the universal machine memory before program 
 * execution has begun.
 */
extern T bootstrap_load(uint32_t *segment_zero, uint32_t seg_size);



extern T create_memory();


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
extern uint32_t top(T segmem);

/* returns true if the value at index is == NULL
 *         false if not
 */
extern bool empty(T segmem, uint32_t index);

/* 
 * returns the size of the Uarray
 */
extern uint32_t seg_count(T segmem);


#undef T

#endif
