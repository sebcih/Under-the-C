/*
HW #4 YABCDCONVERT.h
Written by: Cihan Sebzeci and Alex Cohen
Purpose: To define the types and declare the functions used by other modules
	to transform luminance values of pixels into cosine coefficients and
	vice versa
*/
#ifndef YABCDCONVERT_H
#define YABCDCONVERT_H

#include <stdio.h>
#include <stdlib.h>
#include "a2methods.h"
#include "mem.h"
#include "bitpack.h"
#include "wordPack.h"
#include "rgbYConvert.h"

/* Define the struct that will hold the four pixels in component video color
   space where the luma values will be taken from or added to */
typedef struct twobytwo *twobytwo;
struct twobytwo{
	YPbPr pixel1;
	YPbPr pixel2;
	YPbPr pixel3;
	YPbPr pixel4;
};

/* Define the struct that will hold the four cosine coefficients in floating
   point representation */
typedef struct coeffs *coeffs;
struct coeffs{
	float a;
	float b;
	float c;
	float d;
};

void transformY(coeffs coeffs, twobytwo block);
void transformABCD(coeffs coeffs, twobytwo block);

#endif
