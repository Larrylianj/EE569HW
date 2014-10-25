
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#define COEF_R 0.21
#define COEF_G 0.72
#define COEF_B 0.07
#define COLOR_RANGE 256
#define WINDOW_SIZE 23
#define K_PARAMETER sqrt(1.6)
#define DEVIATION 1.5
#define GAMA 0.93
#define THRESHOLD 4.1
#define STEEPNESS 2
#define BINARY_1 255
#define BINARY_0 0


using namespace std;

int main(int argc, char *argv[])

{
    
	// Define file pointer and variables
	FILE *file;
	int BytesPerPixel;
	int Size1 = 300;
    int Size2 = 400;
	
	// Check for proper syntax
	if (argc < 3){
		cout << "Syntax Error - Incorrect Parameter Usage:" << endl;
		cout << "program_name input_image.raw output_image.raw [BytesPerPixel = 1] [Size1 = 256] [Size2 = 256]" << endl;
		return 0;
	}
	
	// Check if image is grayscale or color
	if (argc < 4){
		BytesPerPixel = 3; // default is color image
	}
	else {
		BytesPerPixel = atoi(argv[3]);
		// Check if size is specified
		if (argc >= 5)
			Size1 = atoi(argv[4]);
        if (argc >= 6)
            Size2 = atoi(argv[5]);
	}
	
	// Allocate image data array
    int GrayScaleChannel = 1;
	unsigned char Imagedata[Size1][Size2][BytesPerPixel];
    unsigned char OutputImagedata[Size1][Size2][GrayScaleChannel];
    
	// Read image (filename specified by first argument) into image data matrix
	if (!(file=fopen(argv[1],"rb"))) {
		cout << "Cannot open file: " << argv[1] <<endl;
		exit(1);
	}
	fread(Imagedata, sizeof(unsigned char), Size1*Size2*BytesPerPixel, file);
	fclose(file);
    
	///////////////////////// INSERT YOUR PROCESSING CODE HERE /////////////////////////
    // convert to gray scale
    for (int i = 0; i < Size1; i++) {
        for(int j = 0;j < Size2; j++){
            Imagedata[i][j][0] = (unsigned char)round(COEF_B*(double)Imagedata[i][j][0] + COEF_G*(double)Imagedata[i][j][1] + COEF_R*(double)Imagedata[i][j][2]);
            
        }
    }
    
    // apply XDoG filter
    for (int x = 0; x < Size1; x++)
        for (int y = 0; y < Size2; y++)
            for (int color = 0; color < GrayScaleChannel; color++) {
                double sum = 0;
                double XDoG, kXDoG;
                for (int i = x-WINDOW_SIZE/2; i < x+WINDOW_SIZE/2; i++) {
                    for(int j = y-WINDOW_SIZE/2;j < y+WINDOW_SIZE/2; j++){
                        if (i >= 0 && i < Size1 && j >= 0 && j < Size2) {
                            sum += Imagedata[i][j][color]* exp(-(pow((double)(x-i), 2.0)+pow((double)(y-j), 2.0))/(2*pow(DEVIATION, 2.0)));
                        }
                    }
                }
                XDoG = 1/(2*M_PI*pow(DEVIATION, 2.0)) * sum;
                sum = 0;
                for (int i = x-WINDOW_SIZE/2; i < x+WINDOW_SIZE/2; i++) {
                    for(int j = y-WINDOW_SIZE/2;j < y+WINDOW_SIZE/2; j++){
                        if (i >= 0 && i < Size1 && j >= 0 && j < Size2) {
                            sum += Imagedata[i][j][color]* exp(-(pow((double)(x-i), 2.0)+pow((double)(y-j), 2.0))/(2*pow(K_PARAMETER*DEVIATION, 2.0)));
                        }
                    }
                }
                kXDoG = 1/(2*M_PI*pow(K_PARAMETER*DEVIATION, 2.0)) * sum;
                double Dx = XDoG - GAMA * kXDoG;
                //OutputImagedata[x][y][color] = Dx > THRESHOLD? BINARY_1:BINARY_0;
                OutputImagedata[x][y][color] = Dx > THRESHOLD? BINARY_1:(unsigned char)round((1+tanh(STEEPNESS*(Dx-THRESHOLD)))*BINARY_0);
                
            }
    
	// Write image data (filename specified by second argument) from image data matrix
    
	if (!(file=fopen(argv[2],"wb"))) {
		cout << "Cannot open file: " << argv[2] << endl;
		exit(1);
	}
	fwrite(OutputImagedata, sizeof(unsigned char), Size1*Size2*GrayScaleChannel, file);
	fclose(file);
    
	return 0;
}


