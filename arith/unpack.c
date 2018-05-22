/*
HW#4 unpack.c
Written by: Cihan Sebzeci and Alex Cohen
Purpose: To define the functions that were declared in the header file
	unpack.h (to unpack a codeword into it's separate components)
*/

#include <assert.h>
#include "unpack.h"
#include "wordPack.h"
#include "YABCDConvert.h"
#include <arith40.h>

#define quantA 511
#define quantB 50
#define quantC 50
#define quantD 50

/* Function prototypes for this module alone */
static void setPixels(wordElements wordParts, twobytwo block);
static void setYValues(wordElements wordParts, twobytwo block);

/*
unpack(unsigned int codeword)
Purpose: To utilize other modules to separate a codeword into separate
	components 
Arguments: An unsigned integer representing the codeword
Returns: A pointer to a struct containing four different pixels with values
	depending on the separate components taken from the code word
*/
twobytwo unpack(unsigned int codeword)
{
	/* Call the wordPack module to take apart a codeword */
	wordElements wordParts = takeApartWord(codeword);

	/* Get the float Pb and Pr average values from the indices contained
	   in the wordElements struct from taking apart the codeword */
	wordParts -> Pbavg = Arith40_chroma_of_index(wordParts -> Pbindex);
	wordParts -> Pravg = Arith40_chroma_of_index(wordParts -> Prindex);

	/* Create and set values in a new struct holding four pixels */
	twobytwo block = NEW(block);
	setPixels(wordParts, block);
	setYValues(wordParts, block);

	/* Free the dynamically allocated memory */
	FREE(wordParts);

	return block;
}

/*
setPixels(wordElements wordParts, twobytwo block)
Purpose: To create and set some of the elements of the pixels within 
	the "block"
Arguments: A pointer to a struct containing the separate parts of a word
	and a pointer to a struct holding four different pixels in component
	video color space form
Returns: N/A (void function)
*/
static void setPixels(wordElements wordParts, twobytwo block)
{
	/* Create each pixel and set the Pb, Pr values from the
	averages contained within the wordElements object holding
	the deconstructed word parts */
	block -> pixel1 = ALLOC(sizeof(struct YPbPr));
	block -> pixel1 -> Pb = wordParts -> Pbavg;
	block -> pixel1 -> Pr = wordParts -> Pravg;

	block -> pixel2 = ALLOC(sizeof(struct YPbPr));
	block -> pixel2 -> Pb = wordParts -> Pbavg;
	block -> pixel2 -> Pr = wordParts -> Pravg;

	block -> pixel3 = ALLOC(sizeof(struct YPbPr));
	block -> pixel3 -> Pb = wordParts -> Pbavg;
	block -> pixel3 -> Pr = wordParts -> Pravg;

	block -> pixel4 = ALLOC(sizeof(struct YPbPr));
	block -> pixel4 -> Pb = wordParts -> Pbavg;
	block -> pixel4 -> Pr = wordParts -> Pravg;
}

/*
setYValues(wordElements wordParts, twobytwo block)
Purpose: To set the Y values of the pixels within the "block"
Arguments: A pointer to a struct containing the separate parts of a word
	and a pointer to a struct holding four different pixels in component
	video color space form
Returns: N/A (void function)
*/
static void setYValues(wordElements wordParts, twobytwo block)
{
	/* Create and set values within a new struct holding cosine 
		coefficient values */
	coeffs coefficients = NEW(coefficients);

	coefficients -> a = ((float) wordParts -> a) / quantA;
	coefficients -> b = ((float) wordParts -> b) / quantB;
	coefficients -> c = ((float) wordParts -> c) / quantC;
	coefficients -> d = ((float) wordParts -> d) / quantD;

	/* Call the YABCDConvert module to convert the cosine coefficients to
	   the Y values contained in each of the four pixels in "block" */
	transformY(coefficients, block);

	/* Free dynamically allocated memory */
	FREE(coefficients);
}

