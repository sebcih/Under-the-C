/*
HW #4 unpack.h
Written by: Cihan Sebzeci and Alex Cohen
Purpose: To declare the functions used by other modules to unpack a 32-bit 
	codeword 
*/

#ifndef UNPACK_H
#define UNPACK_H

#include <stdio.h>
#include <stdlib.h>
#include "pnm.h"
#include "a2methods.h"
#include "mem.h"
#include "YABCDConvert.h"
#include "wordPack.h"
#include "pack.h"

twobytwo unpack(unsigned int codeword);

#endif
