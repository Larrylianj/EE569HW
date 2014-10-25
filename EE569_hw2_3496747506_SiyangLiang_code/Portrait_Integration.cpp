
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#define COLOR_RANGE 256


using namespace std;

int main(int argc, char *argv[])

{
    
	// Define file pointer and variables
	FILE *file;
	int BytesPerPixel;
	int Size1 = 300;
    int Size2 = 400;
	
	// Check for proper syntax
	if (argc < 4){
		cout << "Syntax Error - Incorrect Parameter Usage:" << endl;
		cout << "program_name background.raw contour.raw output_image.raw [BytesPerPixel = 1] [Size1 = 256] [Size2 = 256]" << endl;
		return 0;
	}
	
	// Check if image is grayscale or color
	if (argc < 5){
		BytesPerPixel = 3; // default is color image
	}
	else {
		BytesPerPixel = atoi(argv[4]);
		// Check if size is specified
		if (argc >= 6)
			Size1 = atoi(argv[5]);
        if (argc >= 7)
            Size2 = atoi(argv[6]);
	}
	
	// Allocate image data array
    int GrayScaleChannel = 1;
	unsigned char Background[Size1][Size2][BytesPerPixel];
    unsigned char Contour[Size1][Size2][GrayScaleChannel];
    unsigned char OutputImagedata[Size1][Size2][BytesPerPixel];
    
	// Read image (filename specified by first argument) into image data matrix
	if (!(file=fopen(argv[1],"rb"))) {
		cout << "Cannot open file: " << argv[1] <<endl;
		exit(1);
	}
	fread(Background, sizeof(unsigned char), Size1*Size2*BytesPerPixel, file);
	fclose(file);
    if (!(file=fopen(argv[2],"rb"))) {
		cout << "Cannot open file: " << argv[2] <<endl;
		exit(1);
	}
	fread(Contour, sizeof(unsigned char), Size1*Size2*GrayScaleChannel, file);
	fclose(file);
    
	///////////////////////// INSERT YOUR PROCESSING CODE HERE /////////////////////////
    for (int x = 0; x < Size1; x++) {
        for(int y = 0; y < Size2; y++){
            for(int color = 0; color < BytesPerPixel; color++){
                OutputImagedata[x][y][color] = (unsigned char)round((double)Background[x][y][color] * ((double)Contour[x][y][0]/(double)(COLOR_RANGE-1)));
            }
        }
    }
    
    
	// Write image data (filename specified by second argument) from image data matrix
    
	if (!(file=fopen(argv[3],"wb"))) {
		cout << "Cannot open file: " << argv[3] << endl;
		exit(1);
	}
	fwrite(OutputImagedata, sizeof(unsigned char), Size1*Size2*BytesPerPixel, file);
	fclose(file);
    
	return 0;
}



