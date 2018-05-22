/*
HW #4 intFloatConvert.h
Written by: Cihan Sebzeci and Alex Cohen
Purpose: To define the types and declare the functions used by other
	modules to convert an integer to floating point representation
	or vice versa
*/

#ifndef INTFLOATCONVERT_H
#define INTFLOATCONVERT_H

#include <stdio.h>
#include <stdlib.h>
#include "pnm.h"

typedef A2Methods_UArray2 A2;

/* Define the struct that will hold the red, green, and blue values
   in float form */
typedef struct Float_rgb *Float_rgb;
struct Float_rgb{
	float red;
	float green;
	float blue;
};

void int2float(int col, int row, A2 pixels, void *pixel, void *extraInfo);
void float2int(Float_rgb *toRgb, Pnm_rgb newPixel, unsigned denom);

#endif


