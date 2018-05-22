/*                                                                        
 HW #4 rgbYConvert.h                                                 
 Written by: Cihan Sebzeci and Alex Cohen                                 
 Purpose: To define the types and declare the functions used by other
	modules to convert from RGB color space to component video
	color space or vice versa           
*/

#ifndef RGBYCONVERT_H
#define RGBYCONVERT_H
 
#include <stdio.h>
#include <stdlib.h>
#include "mem.h"
#include "intFloatConvert.h"
#include "pnm.h"

typedef struct YPbPr *YPbPr;
typedef struct newCl *newCl;

/* Define the struct that will hold the component video color space values */
struct YPbPr{
	float Y;
	float Pb;
	float Pr;
};

/* Define a new closure struct that will allow us to pass more information */
struct newCl{
	A2Methods_UArray2 newArray;
	A2Methods_T methods;
	A2Methods_UArray2 oldArray;
	unsigned int denom;
};

void rgb2Y(int col, int row, A2 pixels, void *pixel, void *extraInfo);
void Y2rgb(Float_rgb toRgb, YPbPr oldPixel);

#endif
