/*
HW #4 pack.h
Written by: Cihan Sebzeci and Alex Cohen
Purpose: To declare the functions used by other modules to pack four pixels
	into a 32-bit "codeword"
*/

#ifndef PACK_H
#define PACK_H

#include <stdio.h>
#include <stdlib.h>
#include "pnm.h"
#include "a2methods.h"
#include "mem.h"
#include "YABCDConvert.h"
#include "wordPack.h"
#include "rgbYConvert.h"

unsigned int pack(twobytwo block);

#endif
