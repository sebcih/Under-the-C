/*
HW#4 wordPack.c
Written by: Cihan Sebzeci and Alex Cohen
Purpose: To define the functions that were declared in the header file
	wordPack.h (to create a "codeword," or take one apart)
*/

#include <assert.h>
#include "rgbYConvert.h"
#include "wordPack.h"

/* Declare constants so no hardcoded values need be used */
#define RGB_MAX 255
#define aWidth 9
#define aLSB 23
#define bWidth 5
#define bLSB 18
#define cWidth 5
#define cLSB 13
#define dWidth 5
#define dLSB 8
#define pbWidth 4
#define pbLSB 4
#define prWidth 4
#define prLSB 0

/*
makeWord(wordElements wordParts)
Purpose: To create a "codeword" given separate parts of a word
Arguments: A pointer to a struct containing all of the elements that are
	needed to make up a word
Returns: An unsigned int representing the created codeword
*/
unsigned int makeWord(wordElements wordParts)
{
	unsigned int codeword = 0;

	/* Update the codeword to include each additional part of a 
	   word, in big-endian order (use the bitpack module) */
	codeword = Bitpack_newu(codeword, aWidth, aLSB, wordParts -> a);
	codeword = Bitpack_news(codeword, bWidth, bLSB, wordParts -> b);
	codeword = Bitpack_news(codeword, cWidth, cLSB, wordParts -> c);
	codeword = Bitpack_news(codeword, dWidth, dLSB, wordParts -> d);
	codeword = Bitpack_newu(codeword, pbWidth, pbLSB, 
							wordParts -> Pbindex);
	codeword = Bitpack_newu(codeword, prWidth, prLSB, 
							wordParts -> Prindex);
	return codeword;
}

/*
takeApartWord(unsigned int codeword)
Purpose: To take apart a given "codeword," and separate the values into
	distinct elements
Arguments: An unsigned int representing the codeword
Returns: A pointer to a struct that holds all of the separate values taken 
	from the codeword
*/
wordElements takeApartWord(unsigned int codeword)
{
	wordElements newElements = NEW(newElements);

	/* Separate a codeword into each distinct part, in big-endian order
	   (use the bitpack module) */
	newElements -> a = Bitpack_getu(codeword, aWidth, aLSB);
	newElements -> b = Bitpack_gets(codeword, bWidth, bLSB);
	newElements -> c = Bitpack_gets(codeword, cWidth, cLSB);
	newElements -> d = Bitpack_gets(codeword, dWidth, dLSB);
	newElements -> Pbindex = Bitpack_getu(codeword, pbWidth, pbLSB);
	newElements -> Prindex = Bitpack_getu(codeword, prWidth, prLSB);

	return newElements;
}
