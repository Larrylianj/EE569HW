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
#define WINDOW_SIZE 7
#define SIGMA_C 3
#define SIGMA_S 10

using namespace std;

double gaussian_dis2(int sigma1, int sigma2, int x1, int x2);
double gaussian_dis1(unsigned char sigma, unsigned char x);
int main(int argc, char *argv[])

{
    // for debug only
//    char in_file[] = "/Users/liangsiyang/Documents/USC-learning/EE-569/HW1/images/cartoon9.raw";
//    char out_file[] = "/Users/liangsiyang/Documents/USC-learning/EE-569/HW1/images/cartoon10.raw";
//    argc = 4;
//    argv[1] = in_file;
//    argv[2] = out_file;
//    *argv[3] = '3';
    
	// Define file pointer and variables
	FILE *file;
	int BytesPerPixel;
	int Size1 = 512;
    int Size2 = 512;
	
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
	unsigned char Imagedata[Size1][Size2][BytesPerPixel];
    unsigned char OutputImagedata[Size1][Size2][BytesPerPixel];
    double sum_k,sum_h;
    
	// Read image (filename specified by first argument) into image data matrix
	if (!(file=fopen(argv[1],"rb"))) {
		cout << "Cannot open file: " << argv[1] <<endl;
		exit(1);
	}
	fread(Imagedata, sizeof(unsigned char), Size1*Size2*BytesPerPixel, file);
	fclose(file);
    
	///////////////////////// INSERT YOUR PROCESSING CODE HERE /////////////////////////
    for (int x = 0; x < Size1; x++)
        for (int y = 0; y < Size2; y++)
            for (int color = 0; color < BytesPerPixel; color++) {
                sum_k = 0.0;
                sum_h = 0.0;
                for (int i = x - WINDOW_SIZE/2; i < x + WINDOW_SIZE/2; i++) {
                    for (int j = y - WINDOW_SIZE/2; j < y + WINDOW_SIZE/2; j++) {
                        if (i >= 0 && i < Size1 && j >= 0 && j < Size2) {
                            sum_k += gaussian_dis2(i, j, x, y)*gaussian_dis1(Imagedata[i][j][color], Imagedata[x][y][color]);
                            sum_h += (double)Imagedata[i][j][color]*gaussian_dis2(i, j, x, y)*gaussian_dis1(Imagedata[i][j][color], Imagedata[x][y][color]);
                        }
                    }
                }
                OutputImagedata[x][y][color] = (unsigned char)round(sum_h/sum_k);
            }
    
	// Write image data (filename specified by second argument) from image data matrix
    
	if (!(file=fopen(argv[2],"wb"))) {
		cout << "Cannot open file: " << argv[2] << endl;
		exit(1);
	}
	fwrite(OutputImagedata, sizeof(unsigned char), Size1*Size2*BytesPerPixel, file);
	fclose(file);
    
	return 0;
}

double gaussian_dis2(int sigma1, int sigma2, int x1, int x2)
{
    //printf("dis2 = %f\n",exp(-(pow((double)(sigma1-x1),2.0)+pow((double)(sigma2-x2), 2.0))/(2*pow((double)SIGMA_C, 2))));
    return exp(-(pow((double)(sigma1-x1),2.0)+pow((double)(sigma2-x2), 2.0))/(2*pow((double)SIGMA_C, 2)));
}

double gaussian_dis1(unsigned char sigma, unsigned char x)
{
    //printf("dis1 = %f\n",exp(-(pow((double)(sigma-x),2.0))/(2*pow((double)SIGMA_S, 2))));
    return exp(-(pow((double)(sigma-x),2.0))/(2*pow((double)SIGMA_S, 2)));
}


