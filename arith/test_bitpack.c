/*
 * An inclass exercise from a former midterm exam.
 *
 * Print the binary representation of a number.
 *
 * 1) Look at main(), and figure out the binary representations
 *    of the numbers given by hand.
 * 2) Complete the implementation of the BIT macro
 * 3) Complete the implementation of the printbits() function.
 *
 * If the BIT macro is confusing, then just start working on
 * printbits().  You'll have to achieve its functionality somehow.
 *
 */

#include <stdio.h>

#define TYPE int
#define BITS_IN_TYPE (sizeof(TYPE) * 8)

/*
 * Replace 0 with your solution.
 * You should use BITS_IN_TYPE above
 */
const unsigned int ONES = 1;

void printbits(TYPE value)
{
	unsigned int i;
	printf("\"0b");

        for(i = 0; i < BITS_IN_TYPE; i++){
        	if(i%4 == 0 && i != 0)
                        printf(" ");
                printf("%u",((value & (ONES << (BITS_IN_TYPE -(1+i)))) >> (BITS_IN_TYPE -(1+i))));
        }
	printf("\"");
}

void print_all_fmt(TYPE value)
{
	printf("Number %11d (dec) is: 0x%08X OR ", value, value);
	printbits(*((TYPE *)((void *)&value)));
	printf("\n");
}


int main(int argc, char *argv[])
{
	(void) argc;
	(void) argv;

	TYPE number;

	number = 5;
	print_all_fmt(number);
	
	number = 257;
	print_all_fmt(number);
	
	number = (-3);
	print_all_fmt(number);
}

