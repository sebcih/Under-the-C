/*
HW#4 testModules.c
Written by: Cihan Sebzeci and Alex Cohen
Purpose: To test modules one at a time with images before the full compression
	and decompression
*/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "assert.h"
#include "compress40.h"
#include "pnm.h"
#include "a2methods.h"
#include "a2plain.h"
#include "intFloatConvert.h"
#include "pack.h"
#include "rgbYConvert.h"

void testModule(FILE *fp);
static void trim_image(Pnm_ppm image);
static void newFloat(A2Methods_UArray2 floatPixels, A2Methods_T methods);
static void toVideoSpace(A2Methods_T changedPixels, newCl extraInfo);

int main(int argc, char *argv[])
{
	(void) argc;
	FILE *fp = fopen(argv[1], "r");
	assert(fp != NULL);
	testModule(fp);
	fclose(fp);
	return 0;
}

void testModule(FILE *fp)
{
	/* Compression */
	A2Methods_T methods = uarray2_methods_plain;
	assert(methods != NULL);
	Pnm_ppm image = Pnm_ppmread(fp, methods);
	assert(image != NULL);
	trim_image(image);
	A2Methods_UArray2 floatPixels = methods -> new(image->width, image->height,
					sizeof(struct Float_rgb));

	newFloat(floatPixels, methods);
	newCl extraInfo = NEW(extraInfo);
	extraInfo -> newArray = floatPixels;
	extraInfo -> methods = methods;
	extraInfo -> oldArray = image;
	extraInfo -> denom = image -> denominator;
	
	methods -> map_row_major(image -> pixels, int2float, extraInfo);

	A2Methods_T changedPixels = methods -> new(image->width, image->height,
						   sizeof(struct YPbPr));
	toVideoSpace(changedPixels, extraInfo);

	unsigned width = methods -> width(changedPixels);
	unsigned height = methods -> height(changedPixels);
	printf("COMP40 Compressed image format 2\n%u %u", width, height);
	printf("\n");

	/* Decompression */
	A2Methods_UArray2 toWrite = methods -> new(image->width, image->height, 
							  sizeof(struct Pnm_rgb));
	Pnm_ppm pixmap = NEW(pixmap);
	pixmap -> width = image->width;
	pixmap -> height = image->height;
	pixmap -> denominator = 255;
	pixmap -> pixels = toWrite;
	pixmap -> methods = methods;

	for(int r=0; r < (int)pixmap -> height; r++){
		for(int c=0; c<(int)pixmap -> width; c++){
			Float_rgb toRgb = NEW(toRgb);
			Y2rgb(toRgb, *((YPbPr *)methods->at(changedPixels, c, r)));
			Pnm_rgb newPixel = NEW(newPixel);
			float2int(&toRgb, newPixel, pixmap->denominator);
			Pnm_rgb atLoc = methods -> at(pixmap->pixels, c, r);
			*atLoc = *newPixel;
			(void)atLoc;
		}
	}

	Pnm_ppmwrite(stdout, pixmap);
}

static void trim_image(Pnm_ppm image)
{
        if ((image->height % 2) != 0)
                image->height -= 1;

        if ((image->width % 2) != 0)
                image->width -= 1;
}

static void newFloat(A2Methods_UArray2 floatPixels, A2Methods_T methods)
{
	for(int r=0; r< methods -> height(floatPixels); r++){
		for(int c=0; c< methods -> width(floatPixels); c++){
			Float_rgb floats = NEW(floats);
			*((Float_rgb *)(methods -> at(floatPixels, c, r))) = 
								floats;
		}
	}
}

static void toVideoSpace(A2Methods_T changedPixels, newCl extraInfo)
{
	for(int r=0; r< extraInfo -> methods -> height(changedPixels); r++){
		for(int c=0; c< extraInfo -> methods -> width(changedPixels);
									c++){
			YPbPr converted = NEW(converted);
			converted -> Y = 0;
			converted -> Pb = 0;
			converted -> Pr = 0;
			*((YPbPr *)(extraInfo -> methods -> at(changedPixels,
						       c, r))) = converted;
		}
	}
	extraInfo -> oldArray = extraInfo -> newArray;
	extraInfo -> newArray = changedPixels;

	extraInfo -> methods -> map_row_major(extraInfo -> oldArray, rgb2Y, 
								extraInfo);
}
