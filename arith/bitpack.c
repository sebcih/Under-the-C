/*
Bitpack.c
By: Cihan Sebzeci (csebze01) and Alex Cohen (acohen05)
Purpose: Defines an interface to mainpulate bit fields
*/

#include "bitpack.h"
#include <stdio.h>
#include "assert.h"

Except_T Bitpack_Overflow = { "Overflow packing bits" };

/* Declare constants to replace real numbers */
const uint64_t MAX_BITS = sizeof(uint64_t) * 8; /* 8 bits per byte */
const uint64_t ONE = 1;
const uint64_t ZERO = 0;
const int64_t  NEG_ONE= -1;

/* Function prototypes for functions within this module alone */
static uint64_t create_mask(unsigned width, unsigned lsb, bool type);
static inline uint64_t right_shiftu(uint64_t n, unsigned width);
static inline int64_t right_shifts(int64_t n,  unsigned width);
static inline uint64_t left_shift(uint64_t n,  unsigned num_bits);
static uint64_t exponent(int exp);


/*
right_shiftu(uint64_t n, unsigned num_bits)
Purpose: To shift a number of bits to the right within an unsigned pack of
	bits
Arguments: An unsigned pack of bits and the bits to be shifted to the right
Returns: An updated unsigned pack of bits
*/
static inline uint64_t right_shiftu(uint64_t n, unsigned num_bits)
{
         if (num_bits >= MAX_BITS)
                return ZERO;

	 return n >> num_bits;
}

/*
right_shifts(int64_t n, unsigned num_bits)
Purpose: To shift a number of bits to the right within a signed pack of
	bits
Arguments: A signed pack of bits and the bits to be shifted to the right
Returns: An updated signed pack of bits
*/
static inline int64_t right_shifts(int64_t n, unsigned num_bits)
{
        if (num_bits >= MAX_BITS) {
                if (n >= 0)
                        return ZERO;
                return NEG_ONE;
        }

        return n >> num_bits;
}

/*
left_shift(uint64_t n, unsigned num_bits)
Purpose: To shift a number of bits to the left within an unsigned pack of
	bits
Arguments: An unsigned pack of bits and the bits to be shifted to the left
Returns: An updated unsigned pack of bits 
*/
static inline uint64_t left_shift(uint64_t n, unsigned num_bits)
{
        if (num_bits > MAX_BITS)
                return ZERO; 

        return n << num_bits;
}

/*
exponent(int exp)
Purpose: To manipulate bits to raise 2 to an exponent
Arguments: An integer representing the exponent
Returns: An integer representing the number (2^exp)
*/
static uint64_t exponent(int exp)
{
        return left_shift(ONE, exp);
}

/*
Bitpack_fitsu(uint64_t n, unsigned width)
Purpose: To check to see if an unsigned width of bits would fit within a
	specific unsigned pack of bits
Arguments: An unsigned pack of bits to fit inside and a number representing
	the number of bits we would like to fit inside the first pack
Returns: A boolean value representing whether or not the given width would
	fit within the given unsigned pack of bits
*/
bool Bitpack_fitsu(uint64_t n, unsigned width)
{
        assert (width <= 64);

        if (width == 64)
                return true;

        uint64_t max = exponent(width) - 1;
        if (n > max)
                return false;

	return true;
}

/*
Bitpack_fitss(int64_t n, unsigned width)
Purpose: To check to see if an unsigned width of bits would fit within a
	specific signed pack of bits
Arguments: A signed pack of bits to fit inside and a number representing
	the number of bits we would like to fit inside the first pack
Returns: A boolean value representing whether or not the given width would
	fit within the given signed pack of bits
*/
bool Bitpack_fitss(int64_t n, unsigned width)
{
        assert(width <= 64);
	
	if (width == 0 && n == 0)
                return true;

        if (width == 0)
                return false;

        if (width == 64) 
                return true;

        int64_t max = exponent(width - 1) - 1;
        int64_t min = NEG_ONE * exponent(width - 1);

        if (n > max || n < min)
                return false;

        return true;
}

/*
Bitpack_getu(uint64_t word, unsigned width, unsigned lsb)
Purpose: To return a grouping of bits of size width within a larger
	pack of bits
Arguments: An unsigned pack of bits containing the smaller pack to be 
	returned, a width representing how many bits are in the desired
	smaller pack, and an offset value representing how far from the 
	least significant bit position into the pack of bits the desired
	smaller pack of bits is located
Returns: An unsigned pack of bits containing only the desired bits
*/
uint64_t Bitpack_getu(uint64_t word, unsigned width, unsigned lsb)
{
        assert(lsb + width <= MAX_BITS);

        uint64_t mask = create_mask(width, lsb, true);
	word &= mask;
        return right_shiftu(word, lsb);
}

/*
Bitpack_gets(int64_t word, unsigned width, unsigned lsb)
Purpose: To return a grouping of bits of size width within a larger
	pack of bits
Arguments: A signed pack of bits containing the smaller pack to be 
	returned, a width representing how many bits are in the desired
	smaller pack, and an offset value representing how far from the 
	least significant bit position into the pack of bits the desired
	smaller pack of bits is located
Returns: A signed pack of bits containing only the desired bits
*/
int64_t Bitpack_gets(uint64_t word, unsigned width, unsigned lsb)
{
        int64_t new_word = (int64_t)Bitpack_getu(word, width, lsb);
	new_word = left_shift(new_word, MAX_BITS - width);
        new_word = right_shifts(new_word, MAX_BITS - width);

        return new_word;
}

/*
Bitpack_newu(uint64_t word, unsigned width, unsigned lsb, uint64_t value)
Purpose: To create a new unsigned pack of bits
Arguments: An unsigned pack of bits, a width representing the number of bits
	within that original pack of bits we wish to replace, a number 
	representing how far from the least significant bit position into the
	pack of bits the desired smaller pack of bits to replace is located,
	and the value that we wish to replace those bits with
Returns: A new unsigned pack of bits
*/
uint64_t Bitpack_newu(uint64_t word, unsigned width, unsigned lsb, 
							uint64_t value)
{
        if (!Bitpack_fitsu(value, width))
                RAISE(Bitpack_Overflow);
        assert(width + lsb <=  MAX_BITS);

        uint64_t mask = create_mask(width, lsb, false);
	word &= mask;
        word |= (value << lsb);

	return word;
}

/*
Bitpack_news(sint64_t word, unsigned width, unsigned lsb, int64_t value)
Purpose: To create a new signed pack of bits
Arguments: A signed pack of bits, a width representing the number of bits
	within that original pack of bits we wish to replace, a number 
	representing how far from the least significant bit position into the
	pack of bits the desired smaller pack of bits to replace is located,
	and the value that we wish to replace those bits with
Returns: A new signed pack of bits
*/
uint64_t Bitpack_news(uint64_t word, unsigned width, unsigned lsb,  
							int64_t value)
{
        if (!Bitpack_fitss(value, width))
                RAISE(Bitpack_Overflow);

        uint64_t new_value = Bitpack_getu(value, width, 0);
	return Bitpack_newu(word, width, lsb, new_value);
}

/*
create_mask(unsigned width, unsigned lsb, bool type)
Purpose: To create a mask to be used by other functions
Arguments: A width representing the number of ones needed in the mask,
	a value representing where in the pack of bits those ones should
	be located, and a boolean type representing whether we want a mask
	of all zeros and the desired section in ones or a mask of all ones
	and the desired section in zeros
Returns: An unsigned pack of bits to use as a "mask"
*/
static uint64_t create_mask(unsigned width, unsigned lsb, bool type)
{
	uint64_t mask = (uint64_t) NEG_ONE;
        mask = right_shiftu(mask, MAX_BITS - width);
	mask = left_shift(mask, lsb);

        if (!type)
                mask =~mask; 
	return mask;
}

