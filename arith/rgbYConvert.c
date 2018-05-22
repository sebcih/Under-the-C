/*
HW#4 rgbYConvert.c
Written by: Cihan Sebzeci and Alex Cohen
Purpose: To define the functions that were declared in the header file
	rgbYConvert.h (to convert a pixel from RGB color space to component
	video space and vice versa)
*/

#include <assert.h>
#include "a2plain.h"
#include "rgbYConvert.h"
#include "pnm.h"

/* Declare constants so no hardcoded values need be used */
#define RGB_MAX 255
#define RGB_MIN 0

/*
rgb2Y(int col, int row, A2 pixels, void *pixel, void *extraInfo)
Purpose: To convert a pixel from RGB color space to component video color
	space
Arguments: A specific pixel, the whole image array of pixels, the row and 
	column location of that pixel in the whole image array of pixels,
	and a closure pointer to a struct holding any additional information
	that may be needed (a new image array of pixels in component video
	color space and a methods suite of functions that can be used on
	said image array)
Returns: N/A (void function)
*/
void rgb2Y(int col, int row, A2 pixels, void *pixel, void *extraInfo)
{
	(void) pixels;
	assert(pixel != NULL); /* Check arguments for validity */
	assert(extraInfo != NULL);

	/* Declare what were formerly void types */
	newCl helpfulStuff = extraInfo;
	A2Methods_UArray2 changedPixels = helpfulStuff -> newArray;
	
	YPbPr *newPixel = helpfulStuff -> methods -> at(changedPixels, 
								  col, row); 
	Float_rgb *oldPixel = pixel;

	/* Convert each pixel value from RGB values to Y, Pb, and Pr values */
	(*newPixel) -> Y = (0.299 * ((*oldPixel) -> red)) 
                + (0.587 * ((*oldPixel) -> green))
		+ (0.114 * ((*oldPixel) -> blue));

	(*newPixel) -> Pb = (-0.168736 * ((*oldPixel) -> red))
		+ (-0.331264 * ((*oldPixel) -> green))
		+ (0.5 * ((*oldPixel) -> blue));
	
	(*newPixel) -> Pr = (0.5 * ((*oldPixel) -> red))
		+ (-0.418688 * ((*oldPixel) -> green))
		+ (-0.081312 * ((*oldPixel) -> blue));
}

/*
Y2rgb(Float_rgb, toRgb, YPbPr pixel)
Purpose: To convert a pixel from component video color space to RGB color
	space
Arguments: A pixel in component video color space that needs to be changed,
	and a pixel in floating point representation RGB color space that
	will be updated and then accessible by the module that called this
	function
Returns: N/A (void function)
*/
void Y2rgb(Float_rgb toRgb, YPbPr oldPixel)
{
	/* Convert Y, Pb, Pr values to red, green, blue values. If the pixel
	values are outside of the range of a pixel (0-255), adjust the value
	accordingly */ 
	toRgb -> red = (1.0 * (oldPixel -> Y)) 
                + (0.0 * (oldPixel -> Pb))
		+ (1.402 * (oldPixel -> Pr));
	if ((toRgb -> red) > RGB_MAX) toRgb -> red = RGB_MAX;
	else if ((toRgb -> red) < RGB_MIN) toRgb -> red = RGB_MIN;

	toRgb -> green = (1.0 * (oldPixel -> Y))
		+ (-0.344136 * (oldPixel -> Pb))
		+ (-0.714136 * (oldPixel -> Pr));
	if ((toRgb -> green) > RGB_MAX) toRgb -> green = RGB_MAX;
	else if ((toRgb -> green) < RGB_MIN) toRgb -> green = RGB_MIN;

	toRgb -> blue = (1.0 * (oldPixel -> Y))
		+ (1.772 * (oldPixel -> Pb))
		+ (0.0 * (oldPixel -> Pr));
	if ((toRgb -> blue) > RGB_MAX) toRgb -> blue = RGB_MAX;
	else if ((toRgb -> blue) < RGB_MIN) toRgb -> blue = RGB_MIN;
}
