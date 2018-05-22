/*
HW#4 intFloatConvert.c
Written by: Cihan Sebzeci and Alex Cohen
Purpose: To define the functions that were declared in the header file 
	intFloatConvert.h (to convert an integer to a number in floating
	point representation and vice versa)
*/

#include <assert.h>
#include "intFloatConvert.h"
#include "rgbYConvert.h"
#include "pnm.h"
#include "a2methods.h"
#include "mem.h"

/* Declare constants so no hardcoded values need be used */
#define INT_MAX 255

/*
int2float(int col, int row, A2 pixels, void *pixel, void *extraInfo)
Purpose: To convert pixel values from integers to floating point
	representation 
Arguments: A specific pixel, the whole image array of pixels, the row and 
	column location of that pixel in the whole image array of pixels, 
	and a closure pointer to a struct holding any additional information
	that may be needed (a new image array of pixels in floating point
	representation, a methods suite of functions that can be used
	on said image array, and the denominator value of the original 
	image)
Returns: N/A (void function)
*/
void int2float(int col, int row, A2 pixels, void *pixel, void *extraInfo)
{
	(void) pixels;
	assert(pixel != NULL); 	/* Check arguments for validity */
	assert(extraInfo != NULL);

	newCl helpfulStuff = extraInfo; /* Declare what were void types */

	/* Check for reaching outside of array bounds */
	if (col < helpfulStuff -> methods -> width(helpfulStuff -> oldArray)
	   && row < helpfulStuff -> methods -> 
					height(helpfulStuff -> oldArray)){
  		
		A2 floatPixels = helpfulStuff -> newArray;

		Float_rgb *newPixel = helpfulStuff -> methods -> 
						at(floatPixels, col, row);   

		Pnm_rgb oldPixel = pixel;

		/* Convert each pixel value from int form to float form */ 
		(*newPixel) -> red = ((float) oldPixel -> red) / 
				     (helpfulStuff -> denom);
		(*newPixel) -> blue = ((float) oldPixel -> blue) / 
				     (helpfulStuff -> denom);
		(*newPixel) -> green = ((float) oldPixel -> green) / 
				     (helpfulStuff -> denom);

	}
}

/*
float2int(Float_rgb toRgb, Pnm_rgb newPixel, unsigned denom)
Purpose: To convert pixel values from floating point representation to
	integers
Arguments: A specific pixel in floating point representation that needs to
	be converted, a pixel in integer representation that will be
	accessible by the modules that called this function, and an integer
	representing the denominator of the original image
Returns: N/A (void function)
*/
void float2int(Float_rgb *toRgb, Pnm_rgb newPixel, unsigned denom)
{
	/* Check if pixel values are in the right range, if yes convert
	   floats to ints, otherwise set to maximum int value */
	if (((*toRgb) -> red) > 1)
		newPixel -> red = INT_MAX;
	else
		newPixel -> red = (unsigned int)(((*toRgb) -> red) * denom);
	if (((*toRgb) -> blue) > 1)
		newPixel -> blue = INT_MAX;
	else
    		newPixel -> blue = (unsigned int)(((*toRgb) -> blue) * denom);
	if (((*toRgb) -> green) > 1)
		newPixel -> green = INT_MAX;
	else
		newPixel -> green = (unsigned int)(((*toRgb) -> green) 
								     * denom);
}
