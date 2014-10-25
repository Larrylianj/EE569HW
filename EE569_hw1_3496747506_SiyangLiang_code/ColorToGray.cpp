// This sample code reads in image data from a RAW image file and
// writes it into another file

// NOTE:	The code assumes that the image is of size 256 x 256 and is in the
//			RAW format. You will need to make corresponding changes to
//			accommodate images of different sizes and/or types

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <math.h>
#define COEF_R 0.21
#define COEF_G 0.72
#define COEF_B 0.07

using namespace std;

int main(int argc, char *argv[])

{
    // for debug only
//    char in_file[] = "/Users/liangsiyang/Documents/USC-learning/EE-569/HW1/images/pepper.raw";
//    char out_file[] = "/Users/liangsiyang/Documents/USC-learning/EE-569/HW1/images/hw1_1.output.raw";
//    argc = 5;
//    argv[1] = in_file;
//    argv[2] = out_file;
//    *argv[3] = '3';
//    char BPP[] = "512";
//    argv[4] = BPP;
    
	// Define file pointer and variables
	FILE *file;
	int BytesPerPixel;
	int Size = 256;
	
	// Check for proper syntax
	if (argc < 3){
		cout << "Syntax Error - Incorrect Parameter Usage:" << endl;
		cout << "program_name input_image.raw output_image.raw [BytesPerPixel = 1] [Size = 256]" << endl;
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
    BytesPerPixel = 1;
    unsigned char OutImagedata[Size][Size][BytesPerPixel];
    for (int i = 0; i < Size; i++) {
        for(int j = 0;j < Size; j++){
            OutImagedata[i][j][0] = (unsigned char)round(COEF_B*(double)Imagedata[i][j][0] + COEF_G*(double)Imagedata[i][j][1] + COEF_R*(double)Imagedata[i][j][2]);
            
        }
    }
    
	// Write image data (filename specified by second argument) from image data matrix
    
	if (!(file=fopen(argv[2],"wb"))) {
		cout << "Cannot open file: " << argv[2] << endl;
		exit(1);
	}
	fwrite(OutImagedata, sizeof(unsigned char), Size*Size*BytesPerPixel, file);
	fclose(file);
    
	return 0;
}
