/*
HW#4 pack.c
Written by: Cihan Sebzeci and Alex Cohen
Purpose: To define the functions that were declared in the header file
        pack.h (to pack together four pixels into a 32-bit "codeword"
*/

#include <assert.h>
#include "pack.h"
#include "wordPack.h"
#include "YABCDConvert.h"
#include <arith40.h>

/* Declare constants so no hardcoded values need be used */
#define PBavg 0
#define PRavg 1
#define chromaMax 0.5
#define chromaMin -0.5
#define coeffMax 0.3
#define coeffMin -0.3
#define quantA 511
#define quantB 50
#define quantC 50
#define quantD 50

/* Function prototypes for this module alone */
static float takeAverage(twobytwo block, int returnType);
static void setPbPr(wordElements wordParts, twobytwo block);
static void setCoeffs(wordElements wordParts, twobytwo block);

/*
pack(twobytwo block)
Purpose: To utilize other modules to pack together four pixels into a 
	single codeword
Arguments: A struct passed by reference containing the four pixels to
	pack together
Returns: An unsigned integer representing the codeword
*/
unsigned int pack(twobytwo block)
{
	/* Create a new struct to hold different elements to make a 
	codeword */
	wordElements wordParts = NEW(wordParts);

	/* Set the values for the different parts of the wordElements struct
	   based on the pixel values in "block" */
	setPbPr(wordParts, block);
	setCoeffs(wordParts, block);

	/* Call the wordPack module to create a codeword */
	int codeword = makeWord(wordParts);

	/* Free the dynamically allocated memory */
	FREE(wordParts);
	return codeword;
}

/*
takeAverage(twobytwo block, int returnType)
Purpose: To take the average of values contained in four different pixels
Arguments: A struct passed by reference holding the four pixels and an
	integer representing which of the two possible values we want to
	get the average of
Returns: A floating point number representing the average
*/
static float takeAverage(twobytwo block, int returnType)
{
	float Pb1 = 0;
	float Pr1 = 0;
	float Pb2 = 0;
	float Pr2 = 0;
	float Pb3 = 0;
	float Pr3 = 0;
	float Pb4 = 0;
	float Pr4 = 0;
	
	/* If the pixels exist, get their Pb and Pr values */
	if (block -> pixel1 != NULL) {
		Pb1 = block -> pixel1 -> Pb;
		Pr1 = block -> pixel1 -> Pr;
	} if (block -> pixel2 != NULL) {
 		Pb2 = block -> pixel2 -> Pb;
		Pr2 = block -> pixel2 -> Pr;
	} if (block -> pixel3 != NULL) {
		Pb3 = block -> pixel3 -> Pb;
		Pr3 = block -> pixel3 -> Pr;
	} if (block -> pixel4 != NULL) {
		Pb4 = block -> pixel4 -> Pb;
		Pr4 = block -> pixel4 -> Pr;
	}

	/* Compute the average of the four Pb values and Pr values */
	float average = 0.0;
	if (returnType == PBavg)
		average	= (Pb1 + Pb2 + Pb3 + Pb4) / 4.0;
	else if (returnType == PRavg)
		average = (Pr1 + Pr2 + Pr3 + Pr4) / 4.0;
	
	return average;
}

/*
setPbPr(wordElements wordParts, twobytwo block)
Purpose: To set the values for the Pbavg and Pravg of the wordElements 
	struct based on the Pb and Pr values in "block"
Arguments: A pointer to a struct holding the different elements needed to
	make a word and a pointer to a struct holding four pixels in 
	component video color space form
Returns: N/A (void function)
*/
static void setPbPr(wordElements wordParts, twobytwo block)
{
	/* Get the averages of the Pb and Pr values contained in the pixels
	   in the "block" */
	float Pbavg = takeAverage(block, PBavg);
	float Pravg = takeAverage(block, PRavg);

	/* Make sure the averages are in the correct range */
	if (Pbavg > chromaMax) 
		Pbavg = chromaMax;
	if (Pbavg < chromaMin) 
		Pbavg = chromaMin;
	if (Pravg > chromaMax)
		Pravg = chromaMax;
	if (Pbavg < chromaMin)
		Pravg = chromaMin;

	/* Set the appropriate elements of the struct holding the different
	   parts to make up a code word */
	wordParts -> Pbindex = Arith40_index_of_chroma(Pbavg);
	wordParts -> Prindex = Arith40_index_of_chroma(Pravg);
}

/*
setCoeffs(wordElements wordParts, twobytwo block)
Purpose: To set the values for the a, b, c, and d of the wordElements 
	struct based on the Y values in "block"
Arguments: A pointer to a struct holding the different elements needed to
	make a word and a pointer to a struct holding four pixels in 
	component video color space form
Returns: N/A (void function)
*/
static void setCoeffs(wordElements wordParts, twobytwo block)
{
	coeffs coefficients = NEW(coefficients);

	/* Call the YABCDConvert module to convert the Y values contained
	   in each of the four pixels in "block" to cosine coefficients */
	transformABCD(coefficients, block);

	/* Make sure the values are in the correct range */
	if (coefficients -> b > 0.3) coefficients -> b = coeffMax;
	if (coefficients -> b < -0.3) coefficients -> b = coeffMin;
	if (coefficients -> c > 0.3 ) coefficients -> c = coeffMax;
	if (coefficients -> c < -0.3) coefficients -> c = coeffMin;
	if (coefficients -> d > 0.3) coefficients -> d = coeffMax;
	if (coefficients -> d < -0.3) coefficients -> d = coeffMin; 

	/* Quantize the cosine coefficients, and store them in the struct
	   holding the various parts needed to make a word */
	wordParts -> a = (unsigned int) ((coefficients -> a) * quantA);
	wordParts -> b = (signed int) ((coefficients -> b) * quantB);
	wordParts -> c = (signed int) ((coefficients -> c) * quantC);
	wordParts -> d = (signed int) ((coefficients -> d) * quantD);

	/* Free the dynamically allocated memory */
	FREE(coefficients);
}
