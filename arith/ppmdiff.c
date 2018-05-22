#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "assert.h"
#include "a2methods.h"
#include "a2plain.h"
#include "pnm.h"
#include "mem.h"

void calculateE(Pnm_ppm image1, Pnm_ppm image2, int w, int h,
		A2Methods_T methods);

int main(int argc, char *argv[])
{
	(void) argc;
        A2Methods_T methods = uarray2_methods_plain;
	assert(methods);

	FILE *fp1 = fopen(argv[1], "r");
	FILE *fp2 = fopen(argv[2], "r");	

        Pnm_ppm image1 = Pnm_ppmread(fp1, methods);
	assert(image1);
        Pnm_ppm image2 = Pnm_ppmread(fp2, methods);
	assert(image2);
	
	fclose(fp1);
	fclose(fp2);

	if((abs((image1 -> width) - (image2 -> width))) > 1 ||
	   (abs((image1 -> height) - (image2 -> height))) > 1){
		fprintf(stderr, "Dimensions of images are too different\n");
		fprintf(stdout, "%1.1f\n", 1.0);
		exit(EXIT_FAILURE);
	}

	int smallerW = image1 -> width;
	if(smallerW > (int) image2 -> width) smallerW = image2 -> width;
	int smallerH = image1 -> height;
	if(smallerH > (int) image2 -> height) smallerH = image2 -> height;

	calculateE(image1, image2, smallerW, smallerH, methods);

	Pnm_ppmfree(&image1);
	Pnm_ppmfree(&image2); 
}

void calculateE(Pnm_ppm image1, Pnm_ppm image2, int w, int h,
				A2Methods_T methods)
{
	double totalSum = 0;
	for(int c=0; c<w; c++){
		for(int r=0; r<h; r++){
			
			Pnm_rgb	pixel = methods -> at(image1->pixels, c, r);

			double red1 = (pixel -> red) / 
				(double)(image1->denominator);
			double green1 = (pixel -> green) / 
				(double)(image1->denominator);
			double blue1 = (pixel -> blue) / 
				(double)(image1->denominator);


			pixel = methods -> at(image2->pixels, c, r);
			
			double red2 = (pixel -> red) / 
				(double)(image2->denominator);
			double green2 = (pixel -> green) / 
				(double)(image2->denominator);
			double blue2 = (pixel -> blue) / 
				(double)(image2->denominator);

			double redVal = (red1 - red2) * (red1 - red2);

			double greenVal = (green1 - green2) * (green1 - green2);

			double blueVal = (blue1 - blue2) * (blue1 - blue2);

			double singleSum = redVal + greenVal + blueVal;
		
			totalSum += singleSum;	
		}
	}
	double beforeRoot = totalSum / (3 * w * h);
	double E = sqrt(beforeRoot);

	fprintf(stdout, "%1.4f\n", E);
}









