/*
 * Comp40
 * universal_mach_cpu.h
 * By: Cihan Sebzeci, Thomas Hendrickson
 * On: 4/12/16 
 * Purpose: implementation of um cpu
 */

#ifndef UNIVERSAL_MACH_CPU_H
#define UNIVERSAL_MACH_CPU_H

#include "universal_mach_mem.h"
#include "universal_mach_cpu.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <bitpack.h>
#include <mem.h>

#define OP_BIT_LEN 4
#define OP_LSB (32 - OP_BIT_LEN)
#define REG_BIT_LEN 3
#define A_LSB (REG_BIT_LEN * 2)
#define B_LSB (REG_BIT_LEN)
#define C_LSB 0


typedef enum Um_opcode {
        CMOV = 0, SLOAD, SSTORE, ADD, MUL, DIV,
        NAND, HALT, ACTIVATE, INACTIVATE, OUT, IN, LOADP, LV
} Um_opcode;

/****************um instruction declarations*****************************/

static inline void con_move(uint32_t *registers, uint32_t reg_a, 
			    uint32_t reg_b, uint32_t reg_c);
static inline void seg_load(Segmem_T memory, uint32_t *registers, 
		            uint32_t reg_a, uint32_t reg_b, uint32_t reg_c);
static inline void seg_store(Segmem_T memory, uint32_t *registers, 
                            uint32_t reg_a, uint32_t reg_b, uint32_t reg_c);
static inline void add(uint32_t *registers, uint32_t reg_a, uint32_t reg_b, 
                       uint32_t reg_c);
static inline void mult(uint32_t *registers, uint32_t reg_a, uint32_t reg_b, 
                        uint32_t reg_c);
static inline void divide(uint32_t *registers, uint32_t reg_a,
                          uint32_t reg_b, uint32_t reg_c);
static inline void nand(uint32_t *registers, uint32_t reg_a, 
                        uint32_t reg_b, uint32_t reg_c);
static inline void halt();
static inline void map_seg(Segmem_T memory, uint32_t *registers, 
                           uint32_t reg_b, uint32_t reg_c);
static inline void unmap_seg(Segmem_T memory, uint32_t *registers, 
                             uint32_t reg_c);
static inline void output(uint32_t *registers, uint32_t reg_c);
static inline void input(uint32_t *registers, uint32_t reg_c);
static inline uint32_t load_prg(Segmem_T memory, uint32_t *registers, 
                                uint32_t reg_b, uint32_t reg_c);
static inline void load_val(uint32_t *registers, uint32_t word,
     			    uint32_t reg_a);




/* starts the fetch decode execute cycle for the cpu 
 * assumes that the program to start executing is in
 * segment 0 and will start executing the first instruction
 */
extern void fde_cycle(Segmem_T memory)
{
	uint32_t reg_array[8] = {0, 0, 0, 0, 0, 0, 0, 0};
        uint32_t prog_counter = 0;
        Um_opcode opcode;
        uint32_t reg_a, reg_b, reg_c;
        uint32_t instruction;

	while (1) {
                instruction = load_word(memory, 0, prog_counter);
                prog_counter++;
                opcode = Bitpack_getu(instruction, OP_BIT_LEN, OP_LSB);
                reg_a = Bitpack_getu(instruction, REG_BIT_LEN, A_LSB);
                reg_b = Bitpack_getu(instruction, REG_BIT_LEN, B_LSB);
                reg_c = Bitpack_getu(instruction, REG_BIT_LEN, C_LSB);

		switch(opcode) {
	                case CMOV:
                                con_move(reg_array, reg_a, reg_b, reg_c);
                                break;
	                case SLOAD:
				seg_load(memory, reg_array, reg_a, reg_b, 
                                         reg_c);
                                break;
	                case SSTORE:
                                seg_store(memory, reg_array, reg_a, reg_b, 
                                          reg_c);
                                break;
	                case ADD:
                                add(reg_array, reg_a, reg_b, reg_c);
                                break;
	                case MUL:
                                mult(reg_array, reg_a, reg_b, reg_c);
                                break;
	                case DIV:
				divide(reg_array, reg_a, reg_b, reg_c);
                                break;
	                case NAND:
                                nand(reg_array, reg_a, reg_b, reg_c);
                                break;
	                case HALT: 
                                halt(memory);
                                return;
	                case ACTIVATE:
				map_seg(memory, reg_array, reg_b, reg_c);
                                break;
	                case INACTIVATE:
				unmap_seg(memory, reg_array, reg_c);
                                break;
	                case OUT:
                                output(reg_array, reg_c);
                                break;
	                case IN:
                                input(reg_array, reg_c);
                                break;
	                case LOADP:
                                prog_counter = load_prg(memory, reg_array, 
                                                        reg_b, reg_c);
                                break;
	                case LV:
				reg_a = Bitpack_getu(instruction, REG_BIT_LEN, 
                                                     (32 - OP_BIT_LEN - 
                                                      REG_BIT_LEN));
                                load_val(reg_array, instruction, reg_a);
                                break;
	                default:
                                exit(EXIT_FAILURE);
                }
        }
}


/* if $r[reg_a] != 0 then $r[reg_b] := $r[reg_c]*/
static inline void con_move(uint32_t *registers, uint32_t reg_a, 
                            uint32_t reg_b, uint32_t reg_c)
{
        if (registers[reg_c] != 0) {
                registers[reg_a] = registers[reg_b];
        }
}

/* $r[reg_a] := $m[$r[reg_b]][$r[reg_c]]*/
static inline void seg_load(Segmem_T memory, uint32_t *registers, 
                            uint32_t reg_a, uint32_t reg_b, uint32_t reg_c)
{
        registers[reg_a] = load_word(memory, registers[reg_b], 
                                     registers[reg_c]);
}

/* $m[$r[reg_a]][$r[reg_b]] := $r[reg_c]*/
static inline void seg_store(Segmem_T memory, uint32_t *registers, 
                             uint32_t reg_a, uint32_t reg_b, uint32_t reg_c)
{
        store_word(memory, registers[reg_a], registers[reg_b], 
                   registers[reg_c]);
}

/* $r[reg_a] := ($r[reg_b] + $r[reg_c]) mod 2^32*/
static inline void add(uint32_t *registers, uint32_t reg_a, 
                       uint32_t reg_b, uint32_t reg_c)
{
        registers[reg_a] = registers[reg_b] + registers[reg_c];
}

/* $r[reg_a] := ($r[reg_b] x $r[reg_c]) mod 2^32*/
static inline void mult(uint32_t *registers, uint32_t reg_a,
                        uint32_t reg_b, uint32_t reg_c)
{
        registers[reg_a] = registers[reg_b] * registers[reg_c];
}

/* $r[reg_a] := floor($r[reg_b] / $r[reg_c])*/
static inline void divide(uint32_t *registers, uint32_t reg_a,
                          uint32_t reg_b, uint32_t reg_c)
{
        registers[reg_a] = registers[reg_b] / registers[reg_c];
}

/* $r[reg_a] := ~($r[reg_b] and $r[reg_c])*/
static inline void nand(uint32_t *registers, uint32_t reg_a,
                        uint32_t reg_b, uint32_t reg_c)
{
        registers[reg_a] = ~(registers[reg_b] & registers[reg_c]);
}

/* halts computation*/
static inline void halt(Segmem_T memory)
{
	free_mem(&memory);
        FREE(memory);
}

/* a new segment is created with a number
 * of words equal to the value in $r[reg_c].
 * Each word in the new segment is initialized
 * to 0. A bit pattern that is not all zeroes 
 * and does not identify any currently mapped
 * segment is placed in $r[reg_b] (the returned
 * segment ID from the memory). The new segment
 * is mapped as $m[$r[reg_b]]
 */
static inline void map_seg(Segmem_T memory, uint32_t *registers, uint32_t reg_b,
                           uint32_t reg_c)
{
        registers[reg_b] = map(memory, registers[reg_c]);
}


/* the segment $m[$r[reg_c]] is unmapped
 * future segment instructions may reuse
 * the identifier $r[reg_c]
 */
static inline void unmap_seg(Segmem_T memory, uint32_t *registers, 
                             uint32_t reg_c)
{
        unmap(memory, registers[reg_c]);
}


/* The value in $r[reg_c] is displayed on
 * the I/O device immediatly. Only values 
 * from 0 to 255 are allowed
 */
static inline void output(uint32_t *registers, uint32_t reg_c)
{
        putc(registers[reg_c], stdout);
}

/* The um waits for input on the I/O device
 * when input arrives, $r[reg_c] is loaded
 * with the input, which must be a value from
 * 0 to 255. If the end of input has been 
 * signaled, then $r[reg_c] is loaded with
 * a full 32-bit word in which every bit is
 * 1
 */
static inline void input(uint32_t *registers, uint32_t reg_c)
{
        registers[reg_c] = getc(stdin);
}


/* Segment $m[$r[reg_b]] is duplicated, and the
 * duplicate replaces $m[0], which is abandoned
 * the program counter is set to pont to 
 * $m[0][$r[reg_c]]. If $r[reg_b] = 0, this
 * is extremely quick
 */
static inline uint32_t load_prg(Segmem_T memory, uint32_t *registers, 
                                uint32_t reg_b, uint32_t reg_c)
{
        if (registers[reg_b] != 0) 
                load_program(memory, registers[reg_b]);
        
        return registers[reg_c];
}



/* Loads a value into $r[reg_a], the value
 *   is the lower 25 bits of the word
 */
static inline void load_val(uint32_t *registers, uint32_t word,
                            uint32_t reg_a)
{
        registers[reg_a] = (word << 7) >> 7;
}


#undef OP_BIT_LEN
#undef OP_LSB
#undef REG_BIT_LEN
#undef A_LSB
#undef B_LSB
#undef C_LSB


#endif
