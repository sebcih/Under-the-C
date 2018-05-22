/* 
 * Comp40 hw6
 * test_memory.c
 * by: Cihan Sebzeci, Thomas Hendrickson
 * Purpose: Unit Testing for memory interface
 *
 */


#include <assert.h>
#include <stdio.h>
#include <except.h>
#include "universal_mach_mem.h"
#include "universal_mach_cpu.h"

/* checks the store and load functions
 * stores and then loads a word
 */
void check_store_and_load(Segmem_T memory, uint32_t size, uint32_t word, uint32_t offset);

/* checks the map and unmapping functions
 * with 500 mappings and then unmaps all of them
 */
void check_map_and_unmap(Segmem_T memory, uint32_t size);

/*
 * checks if free works by calling free
 */
void check_free(Segmem_T memory, uint32_t size);

/* 
 * stores word in segment that's not zero
 * then loads segment into segment zero
 * verifies the word is now in segment zero
 */
void check_load_prg(Segmem_T segmem, uint32_t size, uint32_t word);

/* 
 * does a random pattern of mappings/unmappings for 
 * ten times
 */
void check_random_pattern(uint32_t size, FILE *randfp);

int main()
{
        FILE *randfp = fopen("/dev/urandom", "r");
        assert(randfp != NULL);
        uint32_t word = 0;
        uint32_t size = 0;// = 5;
        uint32_t offset = 0;
        Segmem_T segmem = create_memory();

        for (unsigned trial = 0; trial < 20; trial++) {
                size_t rc = fread(&word, sizeof(word), 1, randfp);
                assert(rc == 1);
                rc = fread(&size, 1, 1, randfp);
                assert(rc == 1);
                rc = fread(&offset, 1, 1, randfp);
                assert(rc = 1);
                size = (size % (4096)) + 6;
                offset = offset % (size - 1);
//                fprintf(stderr, "test %d size: %u\n", trial, size);
/*                check_store_and_load(segmem, size, word, offset);*/
//                fprintf(stderr, "%u\n", size);
//                check_map_and_unmap(segmem, size); 
//                check_free(size);
                check_load_prg(segmem, size, word);
//                check_random_pattern(size, randfp);
                free_mem(&segmem);
                segmem = create_memory();
        }
        free_mem(&segmem);
        fclose(randfp);
        fprintf(stderr, "completed\n");
}

void check_store_and_load(Segmem_T memory, uint32_t size, uint32_t word, uint32_t offset)
{
        uint32_t segid = map(memory, size);
        store_word(memory, segid, offset, word);
        TRY
                assert(load_word(memory, segid, offset) == word);
        ELSE
                fprintf(stderr, "ERROR IN LOAD OR STORE");
        END_TRY;
}


void check_map_and_unmap(Segmem_T segmem, uint32_t size)
{
        for(int i = 1; i < 20; i++) {
                map(segmem, size);
//                fprintf(stderr, "mapped  %d\n", i);
                TRY
                        assert(seg_count(segmem) == (uint32_t)i);
                ELSE
                        fprintf(stderr, "ERROR IN MAPPING\n");
                END_TRY;
        }

        for(int i = 18; i >= 0; i--) {
                fprintf(stderr, "trying to unmap\n");
                unmap(segmem, i);
                fprintf(stderr, "UNMAPPED!\n");
                TRY
                        assert(top(segmem) == (uint32_t)i && empty(segmem, i));
                ELSE
                        fprintf(stderr, "ERROR IN UNMAP"); 
                END_TRY;
                }
}

/*
void check_free(Segmem_T segmem, uint32_t size)
{
        for(uint32_t i = 2; i < 1000; i++) {
                uint32_t segid = map(size);
                TRY
                        assert(seg_count(segmem) == (uint32_t)i && segid != i-1);
                ELSE
                        fprintf(stderr, "ERROE IN MAPPING");
                END_TRY;
        }
        free_mem();

        }*/

void check_load_prg(Segmem_T segmem, uint32_t size, uint32_t word)
{
        map(segmem, size);
        for(int i = 0; i < 10; i++) {
                uint32_t segid = map(segmem, size);
                fprintf(stderr, "segid: %u\n", segid);
                store_word(segmem, segid, 0 , word);
                load_program(segmem, segid);
                TRY
                        assert(load_word(segmem, 0,0) == word);
                ELSE
                        fprintf(stderr, "ERROR IN PROGRAM LOAD");
                END_TRY;
        }
}
/*
void check_random_pattern(uint32_t size, FILE *randfp)
{
        for (int repeat = 0; repeat < 10; repeat++) {
                uint32_t add;
                uint32_t remove;
                size_t rc = fread(&add, 10, 1, randfp);
                assert(rc == 1);
                rc = fread(&remove, 10, 1, randfp);
                remove = remove %(add - 1);
                for (uint32_t i = 0; i < add; i++) 
                        map(size);
                for (uint32_t i = 0; i < remove; i++)
                        unmap(i);
        }
}
*/
