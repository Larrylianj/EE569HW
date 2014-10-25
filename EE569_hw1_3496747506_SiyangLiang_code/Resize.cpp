// This sample code reads in image data from a RAW image file and
// writes it into another file

// NOTE:	The code assumes that the image is of size 512 x 512 and is in the
//			RAW format. You will need to make corresponding changes to
//			accommodate images of different sizes and/or types

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <math.h>

using namespace std;

int main(int argc, char *argv[])

{
    //for debug only
//    char in_file[] = "/Users/liangsiyang/Documents/USC-learning/EE-569/HW1/images/mandril.raw";
//    char out_file[] = "/Users/liangsiyang/Documents/USC-learning/EE-569/HW1/images/hw1_2.output.raw";
//    argc = 5;
//    argv[1] = in_file;
//    argv[2] = out_file;
//    *argv[3] = '3';
//    char BPP[] = "512";
//    argv[4] = BPP;
    
	// Define file pointer and variables
	FILE *file;
	int BytesPerPixel;
	int Size = 512;
    int NewSize = 700;
	
	// Check for proper syntax
	if (argc < 3){
		cout << "Syntax Error - Incorrect Parameter Usage:" << endl;
		cout << "program_name input_image.raw output_image.raw [BytesPerPixel = 1] [Size = 512] [Size = 700]" << endl;
		return 0;
	}
	
	// Check if image is grayscale or color
	if (argc < 4){
		BytesPerPixel = 1; // default is grey image
	}
	else {
		BytesPerPixel = atoi(argv[3]);
		// Check if size is specified
		if (argc >= 5){
			Size = atoi(argv[4]);
		}
        if  (argc >= 6){
            NewSize = atoi(argv[5]);
        }
	}
	
	// Allocate image data array
	unsigned char Imagedata[Size][Size][BytesPerPixel];
    
	// Read image (filename specified by first argument) into image data matrix
	if (!(file=fopen(argv[1],"rb"))) {
		cout << "Cannot open file: " << argv[1] <<endl;
		exit(1);
	}
	fread(Imagedata, sizeof(unsigned char), Size*Size*BytesPerPixel, file);
	fclose(file);
    
	///////////////////////// INSERT YOUR PROCESSING CODE HERE /////////////////////////
    double pos_x, pos_y;
    double coef_a, coef_b;
    unsigned char OutImagedata[NewSize][NewSize][BytesPerPixel];
    
    // Apply Bi-linear Interpolation
    for (int x = 0; x < NewSize; x++) {
        for (int y = 0; y < NewSize; y++) {
            for(int channel = 0; channel < BytesPerPixel; channel++){
                pos_x = (double)(Size-1)/(double)(NewSize-1) * ((double)x-1.0)+1.0;
                pos_y = (double)(Size-1)/(double)(NewSize-1) * ((double)y-1.0)+1.0;
                coef_a = pos_x - floor(pos_x);
                coef_b = pos_y - floor(pos_y);
                OutImagedata[x][y][channel] = (unsigned char)round(
                (1.0-coef_a)*(1.0-coef_b)*Imagedata[(int)floor(pos_x)][(int)floor(pos_y)][channel]+
                coef_a*(1.0-coef_b)*Imagedata[(int)floor(pos_x)+1][(int)floor(pos_y)][channel]+
                (1.0-coef_a)*coef_b*Imagedata[(int)floor(pos_x)][(int)floor(pos_y)+1][channel]+
                coef_a*coef_b*Imagedata[(int)floor(pos_x)+1][(int)floor(pos_y)+1][channel]);
            }
        }
    }
    
    
    
	// Write image data (filename specified by second argument) from image data matrix
	if (!(file=fopen(argv[2],"wb"))) {
		cout << "Cannot open file: " << argv[2] << endl;
		exit(1);
	}
	fwrite(OutImagedata, sizeof(unsigned char), NewSize*NewSize*BytesPerPixel, file);
	fclose(file);
    
	return 0;
}


