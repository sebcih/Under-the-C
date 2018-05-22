/*
HW#4 compress40.c
Written by: Cihan Sebzeci and Alex Cohen
Purpose: To compress an image (compress each 2x2 block of pixels in an image
	array into 32-bit codewords)
*/

#include "compress40.h"
#include "a2methods.h"
#include "a2plain.h"
#include "pnm.h"
#include <stdlib.h>
#include "pack.h"
#include "assert.h"

#define firstLSB 24
#define byteSize 8

/* Function prototypes for this module alone */
static void compressImage(A2Methods_T methods, Pnm_ppm image);
static void trim_image(Pnm_ppm image);
static void from_ints_to_floats(Pnm_ppm image, newCl extraInfo);
static void newFloat(A2 floatPixels, A2Methods_T methods);
static void toVideoSpace(A2 changedPixels, newCl extraInfo);
static void packBlocks(A2 changedPixels, A2Methods_T methods);
static void create_codewords(int outerC, int outerR, A2Methods_T methods, 
			A2 changedPixels);
static void setPixels(twobytwo block, int cellInCol, int cellInRow, 
		      A2 changedPixels, A2Methods_T methods);
static void freeMemory(newCl extraInfo);
static void deleteFloats(A2 floatPixels, A2Methods_T methods);
static void deleteColorSpace(A2 changedPixels, A2Methods_T methods);

/*
compress40(FILE *fp)
Purpose: To compress a read-in image
Arguments: A pointer to a file stream where the image will be read in
Returns: N/A (void function)
*/
void compress40(FILE *fp)
{
	/* Create a methods suite */
	A2Methods_T methods = uarray2_methods_plain;
        assert(methods != NULL);

	/* Read in the image from the file given */
        Pnm_ppm image = Pnm_ppmread(fp, methods);
        assert(image != NULL);

	/* Compress the image */
	compressImage(methods, image);	

	/* Free the dynamically allocated memory */
	Pnm_ppmfree(&image);
}

/*
compressImage(A2Methods_T methods, Pnm_ppm image)
Purpose: To utilize helper functions and other modules created for
	this project to compress an image
Arguments: A pointer to a struct containing an image array and a methods
	suite of functions to apply on that image array
Returns: N/A (void function)
*/
static void compressImage(A2Methods_T methods, Pnm_ppm image)
{
	/* Make sure the image has even dimensions */
    	trim_image(image);

	/* Create a new closure struct to hold all of the necessary info */
	newCl extraInfo = NEW(extraInfo);
	extraInfo -> methods = methods;
	extraInfo -> denom = image -> denominator;
	extraInfo -> oldArray = image;
	
	/* Convert the pixels in the image from ints to floats */
	from_ints_to_floats(image, extraInfo);

	/* Create a new array that can hold pixels in component video
	   color space form, and convert the float pixels to this form */
	A2 changedPixels = methods->new(image->width, image->height, 
                                                      sizeof(struct YPbPr));
	toVideoSpace(changedPixels, extraInfo);

	/* Pack the image and generate the codewords */
	packBlocks(changedPixels, methods);
	
	/* Free up the dynamically allocated memory */
	freeMemory(extraInfo);
}

/*
trim_image(Pnm_ppm image)
Purpose: If the number of rows or the number of columns is odd (height or
	width of the image is odd), cut the last row or column so all 
	dimensions are even
Arguments: A pointer to an image object
Returns: N/A (void function)
*/
static void trim_image(Pnm_ppm image)
{
        if ((image->height % 2) != 0) /* If odd number of rows */
                image->height -= 1;

        if ((image->width % 2) != 0) /* If odd number of cols */
                image->width -= 1;
}

/*
from_ints_to_floats(Pnm_ppm image, newCl extraInfo)
Purpose: To convert the pixels in an image from ints to floats
Arguments: A pointer to a struct holding an original image array and
	a pointer to a struct holding additional information
	(a new array to pass back, a methods suite holding functions to
	use on the image arrays, etc).
Returns: N/A (void function)
*/
static void from_ints_to_floats(Pnm_ppm image, newCl extraInfo)
{
	/* Create a new array that can hold pixels in float form */
	A2 floatPixels = extraInfo -> methods -> new(image->width, 
				image->height, sizeof(struct Float_rgb));
	newFloat(floatPixels, extraInfo -> methods);

	/* Update the extraInfo struct to hold the new array of floats */
	extraInfo -> newArray = floatPixels;

	/* Convert each pixel from ints to floats */
	extraInfo -> methods -> map_row_major(image -> pixels, int2float, 
								extraInfo);
}

/*
newFloat(A2 floatPixels, A2Methods_T methods)
Purpose: To create a new array of pixels where each "pixel" is a struct
	holding values in floating point representation
Arguments: A pointer to an array of pixels in floating point form that is
	accessible by the function that called this helper and a pointer
	to a methods suite containing functions we can apply on that 
	array
Return: N/A (void function)
*/
static void newFloat(A2 floatPixels, A2Methods_T methods)
{
	/* For each spot in the image array "floatPixels," set each
	pointer to be pointing to an empty RGB pixel that will ultimately
	store values in floating point representation */
	for (int row = 0; row < methods -> height(floatPixels); row++) {
		for (int col = 0; col < methods -> width(floatPixels); 
								   col++){
			Float_rgb floats = NEW(floats);
			*((Float_rgb *)(methods -> 
					     at(floatPixels, col, row))) 
						= floats;
		}
	}
}

/*
toVideoSpace(A2 changedPixels, newCl extraInfo)
Purpose: To utilize other modules to convert an image array with pixels
	in floating point representation to an image array with pixels
	in component video color space 
Arguments: A pointer to an image array of pixels in component video color
	space (passed by reference so the function that called this one can
	use it after it's been updated), and a pointer to a special
	type of closure struct containing any additional information needed
Returns: N/A (void function)
*/ 
static void toVideoSpace(A2 changedPixels, newCl extraInfo)
{
       	/* For each spot in the image array "changedPixels," set each
	pointer to be pointing to an empty pixel that will ultimately
	store values in component video color space representation */
	for (int row = 0; row < extraInfo -> methods -> 
					height(changedPixels); row++) {
		for (int col = 0; col < extraInfo -> methods -> 
					width(changedPixels); col++) {
			YPbPr converted = NEW(converted);
			converted -> Y = 0;
			converted -> Pb = 0;
			converted -> Pr = 0;
			*((YPbPr *)(extraInfo -> methods -> 
				at(changedPixels, col, row))) = converted;
		}
	}

	/* Update the oldArray and newArray elements of the struct */
	extraInfo -> oldArray = extraInfo -> newArray;
	extraInfo -> newArray = changedPixels;

	/* Convert every pixel from RGB color space to component video
	   color space */
	extraInfo -> methods -> map_row_major(extraInfo -> oldArray, rgb2Y, 
								extraInfo);
}

/*
packBlocks(A2 changedPixels, A2Methods_T methods)
Purpose: To utilize other modules to pack together blocks of pixels together
	into code words and then print out those code words 
Arguments: An image array of pixels in component video color space format
	and a methods suite containing functions that can be applied on
	said image array
Returns: N/A (void function)
*/
static void packBlocks(A2 changedPixels, A2Methods_T methods)
{
	/* Print the appropriate header */
	unsigned width = methods -> width(changedPixels);
	unsigned height = methods -> height(changedPixels);
	printf("COMP40 Compressed image format 2\n%u %u", width, height);
	printf("\n");

	/* For each block of 2x2 pixels, call the helper to pack and print
	   each codeword */
	for (int outerR = 0; outerR < (int)height/2; outerR++) {
		for (int outerC = 0; outerC < (int)width/2; outerC++) {
			create_codewords(outerC, outerR, methods, 
							changedPixels);
		}
	}
}

/*
create_codewords(int outerC, int outerR, A2Methods_T methods, 
							A2 changedPixels)
Purpose: Helper function to pack together blocks of pixels together into
	codewords and print out said code words
Arguments: A row and a column location of a specific block of four pixels
	in a larger image array, a methods suite of functions that can
	be applied to that larger image array, and the larger image array
Returns: N/A (void pointer)
*/
static void create_codewords(int outerC, int outerR, A2Methods_T methods, 
			A2 changedPixels)
{
	twobytwo block = NEW(block);

	/* Find the location of the top left pixel in this 2x2 block
	   in terms of the full image array */
	int cellInCol = outerC*2;
	int cellInRow = outerR*2;

	/* Set the four pixels in the block to the appropriate pixels
	   from the image array */
	setPixels(block, cellInCol, cellInRow, changedPixels,
				  methods);

	/* Call the pack module to create a codeword */
	unsigned int codeword = pack(block);

	/* Print out the codewords byte by byte */
	unsigned byte = 0;
	for (int lsb = firstLSB; lsb >= 0; lsb -= byteSize) {
		byte = Bitpack_getu(codeword, byteSize, lsb);
		putchar(byte);
	}

	/* Free dynamically allocated memory */
       	FREE(block);
}

/*
setPixels(twobytwo block, int cellInCol, int cellInRow, A2 changedPixels, 
		A2Methods_T methods)
Purpose: Helper function to set pixels within a block
Arguments: A 2x2 block of pixels, a row and a column location of said
	block in a larger image array, a methods suite of functions that can
	be applied to that larger image array, and the larger image array
Returns: N/A (void pointer)
*/
static void setPixels(twobytwo block, int cellInCol, int cellInRow, 
		      A2 changedPixels, A2Methods_T methods)
{
	/* Check if we're reaching out of the image bounds, and set each
	   pixel in "block" to the appropriate pixel in the image array */

	if (cellInCol >= methods -> width(changedPixels) ||
     		   cellInRow >= methods -> height(changedPixels)) 
		return;		       
	block -> pixel1 = *(YPbPr *)(methods -> at(changedPixels,
						      cellInCol, cellInRow));

	if (cellInRow >= methods -> height(changedPixels)-1) 
		return;

	block -> pixel3 = *(YPbPr *)(methods -> at(changedPixels,
						    cellInCol, cellInRow+1));

	if (cellInCol >= methods -> width(changedPixels)-1) 
		return;

        block -> pixel2 = *(YPbPr *)(methods -> at(changedPixels,
						    cellInCol+1, cellInRow));
        block -> pixel4 = *(YPbPr *)(methods -> at(changedPixels,
						  cellInCol+1, cellInRow+1));
}

/* 
freeMemory(A2 floatPixels, A2Methods_T methods, A2 changedPixels,
						newCl extraInfo)
Purpose: To free up the dynamically allocated memory 
Arguments: An array of pixels in float form, an array of pixels
	in component video color space form, a methods suite of functions
	to apply to those image arrays, and a closure struct holding any
	additional information
Returns: N/A (void function)
*/
static void freeMemory(newCl extraInfo)
{
	/* Free each float pixel */
	deleteFloats(extraInfo -> oldArray, extraInfo -> methods); 

	/* Free array of floats */
	extraInfo -> methods -> free(&(extraInfo -> oldArray)); 

	/* Free each YPbPr pixel */
	deleteColorSpace(extraInfo -> newArray, extraInfo -> methods); 

	/* Free array of YPbPr pixels */
	extraInfo -> methods -> free(&(extraInfo -> newArray));

	FREE(extraInfo); 
}

/*
deleteFloats(A2 floatPixels, A2Methods_T methods)
Purpose: To free up the memory used to store all of the image pixels
	with RGB values in floating point representation
Arguments: An image array of pixels with RGB values in floating point
	representation and a methods suite of functions to apply to that
	array
Returns: N/A (void function)
*/
static void deleteFloats(A2 floatPixels, A2Methods_T methods)
{
	/* Free each pixel in floating point representation */

	for (int row = 0; row < methods -> height(floatPixels); row++) {
		for (int col = 0; col < methods -> width(floatPixels); 
								     col++){
			FREE(*((Float_rgb *)(methods -> at(floatPixels, 
							   col, row))));
		}
	}
}

/*
deleteColorSpace(A2 changedPixels, A2Methods_T methods)
Purpose: To free up the memory used to store all of the image pixels
	in component video color space representation
Arguments: An image array of pixels in component video color space 
	representation and a methods suite of functions to apply to that
	array
Returns: N/A (void function)
*/
static void deleteColorSpace(A2 changedPixels, A2Methods_T methods)
{
	/* Free each pixel in component video color space form */

	for (int row = 0; row < methods -> height(changedPixels); row++) {
		for (int col = 0; col < methods -> width(changedPixels); 
								     col++){
			FREE(*((YPbPr *)(methods -> at(changedPixels,
						       col, row))));
		}
	}
}
