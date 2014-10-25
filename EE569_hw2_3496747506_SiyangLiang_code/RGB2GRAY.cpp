// This sample code reads in image data from a RAW image file and
// writes it into another file

// NOTE:	The code assumes that the image is of size 256 x 256 and is in the
//			RAW format. You will need to make corresponding changes to
//			accommodate images of different sizes and/or types

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <math.h>
#define COLOR_RANGE 256
#define COEF_R 0.21
#define COEF_G 0.72
#define COEF_B 0.07

using namespace std;

int main(int argc, char *argv[])

{
    
    
	// Define file pointer and variables
	FILE *file;
	int BytesPerPixel;
    int GrayScale = 1;
    unsigned char threshold;
	int Size1 = 388;
    int Size2 = 374;
	
	// Check for proper syntax
	if (argc < 4){
		cout << "Syntax Error - Incorrect Parameter Usage:" << endl;
		cout << "program_name input_image.raw output_image.raw [Threshold = 128] [BytesPerPixel = 1] [Size1 = 256] [Size2 = 256] " << endl;
		return 0;
	}
    
	// Check if image is grayscale or color
	if (argc < 5){
		BytesPerPixel = 1; // default is gray image
	}
	else {
		BytesPerPixel = atoi(argv[4]);
		// Check if size is specified
		if (argc >= 6)
			Size1 = atoi(argv[5]);
        if (argc >= 7)
            Size2 = atoi(argv[6]);
	}
    
    // set threshold
    threshold = atoi(argv[3]);
	
	// Allocate image data array
	unsigned char Imagedata[Size1][Size2][BytesPerPixel];
    unsigned char GrayImagedata[Size1][Size2][GrayScale];
    
	// Read image (filename specified by first argument) into image data matrix
	if (!(file=fopen(argv[1],"rb"))) {
		cout << "Cannot open file: " << argv[1] <<endl;
		exit(1);
	}
	fread(Imagedata, sizeof(unsigned char), Size1*Size2*BytesPerPixel, file);
	fclose(file);
    
	///////////////////////// INSERT YOUR PROCESSING CODE HERE /////////////////////////
    for (int i = 0; i < Size1; i++) {
        for(int j = 0;j < Size2; j++){
            GrayImagedata[i][j][0] = (unsigned char)round(COEF_B*(double)Imagedata[i][j][0] + COEF_G*(double)Imagedata[i][j][1] + COEF_R*(double)Imagedata[i][j][2]);
            
        }
    }
    for (int x = 0; x < Size1; x++)
        for (int y = 0; y < Size2; y++)
            for (int color = 0; color < GrayScale; color++){
                if (GrayImagedata[x][y][color] >= threshold)
                    GrayImagedata[x][y][color] = COLOR_RANGE-1;
                else
                    GrayImagedata[x][y][color] = 0;
            }
    
    
	// Write image data (filename specified by second argument) from image data matrix
    
	if (!(file=fopen(argv[2],"wb"))) {
		cout << "Cannot open file: " << argv[2] << endl;
		exit(1);
	}
	fwrite(GrayImagedata, sizeof(unsigned char), Size1*Size2*GrayScale, file);
	fclose(file);
    
	return 0;
}






