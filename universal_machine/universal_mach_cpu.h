/*
 * Comp40
 * universal_mach_cpu.h
 * By: Cihan Sebzeci, Thomas Hendrickson
 * On: 4/12/16 
 * Purpose: interface for the universal_mach_cpu.c 
 */

#ifndef UNIVERSAL_MACH_CPU_H
#define UNIVERSAL_MACH_CPU_H


/* starts the fetch decode execute cycle for the cpu 
 * assumes that the program to start executing is in
 * segment 0 and will start executing the first instruction
 */
extern void fde_cycle(Segmem_T memory);

#endif
















