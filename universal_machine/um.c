/*
 * Comp40
 * um.c
 * By: Cihan Sebzeci, Thomas Hendrickson
 * On: 4/6/16 
 * Purpose: Implementation of the Universal Machine
 *          This code combines the cpu and the memory,
 *          loads a program, and starts execution
 */

#include "universal_mach_mem.h"
#include "universal_mach_cpu.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <mem.h>
#include <bitpack.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <inttypes.h>



Segmem_T init_um();

int main(int argc, char *argv[])
{
        FILE *fp;
        Segmem_T memory;

        if (argc != 2) {
                fprintf(stderr, "usage: ./um [input_file]\n");
                return EXIT_FAILURE;
        }
        else {
                fp = fopen(argv[1], "rb");
                if (fp == NULL) {
                        fprintf(stderr, "invalid file input\n");
                        return EXIT_FAILURE;
                }
        }
        memory = init_um(fp, argv[1]);
        fclose(fp);

        fde_cycle(memory);
}


Segmem_T init_um(FILE *input, char *f_name)
{
        Segmem_T memory;
	struct stat fileStats;
        uint64_t prog_size, i, j, lsb, instruction_num;
        uint32_t *input_program, instruction = 0;
        int byte, file = 0;

        if ((file = open(f_name, O_RDONLY)) < -1)
                exit(EXIT_FAILURE);
        if (fstat(file, &fileStats) < 0) 
                exit(EXIT_FAILURE);

        prog_size = fileStats.st_size;

        input_program = ALLOC(prog_size);

        instruction_num = prog_size / 4;
        for (i = 0; i < instruction_num; ++i) {
                instruction = 0;
                lsb = 24;
                for (j = 0; j < 4; ++j) {
                        if((byte = getc(input)) == EOF) {
                                fprintf(stderr, "INVALID INPUT PROGRAM\n");
                                exit(EXIT_FAILURE);
                        }
                        instruction = Bitpack_newu(instruction, 8, lsb, byte);
                        lsb -= 8;
                }
                input_program[i] = instruction;
        }
        memory = bootstrap_load(input_program, instruction_num);
        FREE(input_program);
        return memory;
}



