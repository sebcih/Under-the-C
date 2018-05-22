/*
HW#4 decompress40.c
Written by: Cihan Sebzeci and Alex Cohen
Purpose: To decompress an image
*/

#include "compress40.h"
#include "a2methods.h"
#include "a2plain.h"
#include "pnm.h"
#include <stdlib.h>
#include "unpack.h"
#include "assert.h"

/* Define constants so hardcoded values need not be used */
#define DENOM 255

/* Function prototypes for this module alone */
static void decompressImage(Pnm_ppm pixmap, FILE *fp);
static void create_pixels(int row, int col, unsigned word, Pnm_ppm image);
static void put_pixel_in_image(int row, int col, Pnm_ppm image, 
			       twobytwo block, int pixNum);
static void freeMemory(twobytwo block);

/*
decompress40(FILE *fp)
Purpose: To decompress an image from codewords read in from a file
Arguments: A pointer to a file stream where the codewords are to be read
	in from
Returns: N/A (void function)
*/
void decompress40(FILE *fp)
{
	/* Read in the header from a file to get the height and width */
	unsigned height;
	unsigned width;
	int read = fscanf(fp, "COMP40 Compressed image format 2\n%u %u", 
                          &width, &height);
	assert(read == 2); /* Make sure that we read in correct things */
	int c = getc(fp);
	assert(c == '\n');
	
	/* Create a methods suite */
	A2Methods_T methods = uarray2_methods_plain;
	assert(methods != NULL);

	/* Create a new image object that will eventually hold the 
	decompressed image, but for now just holds all other necessary 
	info */
	A2 image = methods -> new(width, height, sizeof(struct Pnm_rgb));
	Pnm_ppm pixmap = NEW(pixmap);
	pixmap -> width = width;
	pixmap -> height = height;
	pixmap -> denominator = DENOM;
	pixmap -> pixels = image;
	pixmap -> methods = methods;

	/* Decompress the image */
	decompressImage(pixmap, fp);

	/* Write the newly decompressed image */
	Pnm_ppmwrite(stdout, pixmap);

	/* Free the dynamically allocated memory */
	Pnm_ppmfree(&pixmap);	
}

/*
decompressImage(Pnm_ppm pixmap, FILE *fp)
Purpose: To read in codewords from a file and create pixels from those
	codewords
Arguments: An image array passed by reference so once it is filled up by
	this function appropriately, the function that called this one
	can use it, a methods suite containing functions to apply to
	image arrays, and a pointer to a file stream where the codewords
	will be read in from
Returns: N/A (void function)
*/ 
static void decompressImage(Pnm_ppm pixmap, FILE *fp)
{
	unsigned rowLoc = 0;
	unsigned colLoc = 0;

	/* Calculate how many codewords should have been printed out from
	   the compression */
	int numCodeWords = (pixmap -> height * pixmap -> width)/4;

	/* Until we reach the end of the file */
	while(numCodeWords > 0){
		uint64_t codeword = 0;
		unsigned LSB = 24;
		unsigned width = 8;

		/* Read in bytes in groups of four, and create a codeword
		   from those four bytes */
		for(int i=0; i<4; i++){
			uint64_t byte = getc(fp);
			if((int)byte == EOF) break;
			codeword = Bitpack_newu(codeword, width, LSB, byte);
			LSB -= width;
		}

		/* Unpack the codewords and create pixels in 2x2 blocks */
		create_pixels(rowLoc, colLoc, codeword, pixmap);

		/* Monitoring the location of the top left pixel in the 
		   current 2x2 block with respect to the whole image array */
		colLoc+=2;
		if(colLoc >= pixmap->width){
			colLoc = 0;
			rowLoc += 2;
		}  
		numCodeWords --;
	}
}

/*
create_pixels(int row, int col, unsigned word, Pnm_ppm image)
Purpose: To utilize other modules in this project to create 2x2 blocks
	of pixels from a given codeword
Arguments: The row and column location where the top left pixel of a block
	is supposed to be within the larger image ultimately "decompressed",
	an unsigned integer representing the codeword to unpack, and an image
	array to update
Returns: N/A (void function)
*/
static void create_pixels(int row, int col, unsigned word, Pnm_ppm image)
{
	/* Call the unpack module to create a 2x2 block of pixels from
	   a given codeword */
	twobytwo block = unpack(word);

	put_pixel_in_image(row, col, image, block, 1);
	put_pixel_in_image(row, col+1, image, block, 2);
	put_pixel_in_image(row+1, col, image, block, 3);
	put_pixel_in_image(row+1, col+1, image, block, 4);

	/* Free up the dynamically allocated memory */
	freeMemory(block);
}

/* 
put_pixel_in_image(int row, int col, Pnm_ppm image, twobytwo block,
						int pixNum)
Purpose: To put a pixel from an unpacked block into the new decompressed
	image array
Arguments: The row and column location where a given pixel should be placed
	in a 2D image array, an image array to update, a block of four 
	unpacked pixels, and a number representing which pixel in the block
	of four we want to put in the image array
Returns: N/A (void function)
*/
static void put_pixel_in_image(int row, int col, Pnm_ppm image, 
			       twobytwo block, int pixNum)
{
	/* Create a new float pixel and a new int pixel */
       	Float_rgb toRgb = NEW(toRgb);
       	Pnm_rgb newPixel = NEW(newPixel);
	YPbPr toConvert;

	/* Get the pixel we wish to add to the array out of the block */
	if (pixNum == 1)
		toConvert = block -> pixel1;
	else if (pixNum == 2)
		toConvert = block -> pixel2;
	else if (pixNum == 3)
		toConvert = block -> pixel3;
	else if (pixNum == 4)
		toConvert = block -> pixel4;
	
	/* Convert the pixel from the block to RGB format and then from
	   floating point representation to integer representation */
	Y2rgb(toRgb, toConvert);
	float2int(&toRgb, newPixel, image -> denominator);
	
	/* Put the pixel in the array */
	if((col < (int)(image -> width)) &&
	   (row < (int)(image -> height))){
		Pnm_rgb atLoc = image -> methods -> at(image -> pixels, col,
						       row);
		*atLoc = *newPixel;
		(void) atLoc;
	}
	
	/* Free dynamically allocated memory */
	FREE(toRgb);
	FREE(newPixel);
}

/*
freeMemory(Float_rgb toRgb, twobytwo block, Pnm_rgb newPixel)
Purpose: To free up the dynamically allocated memory
Arguments: A pointer to a pixel in floating point represeentation,
	a pointer to a struct holding four pixels in component video color
	space representation, and a pointer to a pixel in integer form
Returns: N/A (void function)
*/
static void freeMemory(twobytwo block)
{
	/* Free each YPbPr pixel in the block */
       	FREE(block->pixel1);
	FREE(block->pixel2);
	FREE(block->pixel3);
	FREE(block->pixel4);

	/* Free the block struct */
	free(block);
}
