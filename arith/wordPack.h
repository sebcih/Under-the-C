/*
HW #4 wordPack.h
Written by: Cihan Sebzeci and Alex Cohen
Purpose: To define the types and declare the functions used by other 
	modules to create a codeword, or take apart a codeword
*/
#ifndef WORDPACK_H
#define WORDPACK_H

#include <stdio.h>
#include <stdlib.h>
#include "a2methods.h"
#include "mem.h"
#include "bitpack.h"

/* Define the struct that will hold the different parts of a "codeword" */
typedef struct wordElements *wordElements;
struct wordElements{
    unsigned int a;
    signed int b;
    signed int c;
    signed int d;
    unsigned int Pbindex;
    unsigned int Prindex;
    float Pbavg;
    float Pravg;
};

unsigned int makeWord(wordElements wordParts);
wordElements takeApartWord(unsigned int);

#endif
