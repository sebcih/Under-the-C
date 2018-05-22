/*
HW#4 YABCDConvert.c
Written by: Cihan Sebzeci and Alex Cohen
Purpose: To define the functions that were declared in the header file
	YABCDConvert.h (to transform luminance Y values to cosine coefficients
	and vice versa)
*/

#include <assert.h>
#include "YABCDConvert.h"

/*
transformABCD(coeffs coeffs, twobytwo block)
Purpose: To transform luma Y values from four different pixels to four
	cosine coefficients
Arguments: A struct containing the four pixels containing the Y values to 
	convert, and a struct containing the four coefficients that will be
	updated and accesssible by whatever module calls this function
Returns: N/A (void function)
*/
void transformABCD(coeffs coeffs, twobytwo block)
{
	float Y1 = 0;
	float Y2 = 0;
	float Y3 = 0;
	float Y4 = 0;

	/* If the pixels exist, get their Y values */
	if(block -> pixel1 != NULL)
		Y1 = block -> pixel1 -> Y;
	if(block -> pixel2 != NULL)
 		Y2 = block -> pixel2 -> Y;
	if(block -> pixel3 != NULL)
		Y3 = block -> pixel3 -> Y;
	if(block -> pixel4 != NULL)
		Y4 = block -> pixel4 -> Y;

	/* Use the Y values to compute the cosine coefficients */
	coeffs -> a = (Y4 + Y3 + Y2 + Y1) / 4.0;
        coeffs -> b = (Y4 + Y3 - Y2 - Y1) / 4.0;
	coeffs -> c = (Y4 - Y3 + Y2 - Y1) / 4.0;
	coeffs -> d = (Y4 - Y3 - Y2 + Y1) / 4.0;
}

/*
transformY(coeffs coeffs, twobytwo block)
Purpose: To transform coefficient values a, b, c, and d to luminescence Y
	values for four different pixels
Arguments: A struct containing the four coefficients to convert, and a struct
	containing the four pixels containing Y values that need to be
	updated (this struct is passed by reference, so it's accessible by
	whatever module calls this function)
Returns: N/A (void function)
*/
void transformY(coeffs coeffs, twobytwo block)
{

	/* Use the cosine coefficients to calculate the appropriate Y
	   values for each of the four pixels in "block" */
	block -> pixel1 -> Y = (coeffs -> a) - (coeffs -> b)
                - (coeffs -> c) + (coeffs -> d);
	block -> pixel2 -> Y = (coeffs -> a) - (coeffs -> b)
                + (coeffs -> c) - (coeffs -> d);
	block -> pixel3 -> Y = (coeffs -> a) + (coeffs -> b)
                - (coeffs -> c) - (coeffs -> d);
	block -> pixel4 -> Y = (coeffs -> a) + (coeffs -> b)
                + (coeffs -> c) + (coeffs -> d);
}
