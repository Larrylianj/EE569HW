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

using namespace std;


int main(int argc, char *argv[])

{
    // for debug only
//    char in_file_origin[] = "/Users/liangsiyang/Documents/USC-learning/EE-569/HW1/images/Lena.raw";
//    char in_file_mix[] = "/Users/liangsiyang/Documents/USC-learning/EE-569/HW1/images/Lena_linear.raw";
//    char out_file[] = "/Users/liangsiyang/Documents/USC-learning/EE-569/HW1/images/hw3_a.output.raw";
//    argc = 5;
//    argv[1] = in_file_origin;
//    argv[2] = in_file_mix;
//    argv[3] = out_file;
//    *argv[4] = '3';
    
	// Define file pointer and variables
	FILE *file;
	int BytesPerPixel;
	int Size1 = 512;
    int Size2 = 512;
	
	// Check for proper syntax
	if (argc < 4){
		cout << "Syntax Error - Incorrect Parameter Usage:" << endl;
		cout << "program_name original_image.raw mixed_image,raw output_image.raw [BytesPerPixel = 3] [Size1 = 256] [Size2 = 256]" << endl;
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
        if(argc >= 7)
            Size2 = atoi(argv[6]);
	}
	
	// Allocate image data array
	unsigned char Imagedata[Size1][Size2][BytesPerPixel];
    unsigned char Mixeddata[Size1][Size2][BytesPerPixel];
    unsigned char Output[Size1][Size2][BytesPerPixel];
    
	// Read image (filename specified by first argument) into image data matrix
	if (!(file=fopen(argv[1],"rb"))) {
		cout << "Cannot open file: " << argv[1] <<endl;
		exit(1);
	}
	fread(Imagedata, sizeof(unsigned char), Size1*Size2*BytesPerPixel, file);
	fclose(file);
    if (!(file=fopen(argv[2],"rb"))) {
		cout << "Cannot open file: " << argv[2] <<endl;
		exit(1);
	}
	fread(Mixeddata, sizeof(unsigned char), Size1*Size2*BytesPerPixel, file);
	fclose(file);
    
    
	///////////////////////// INSERT YOUR PROCESSING CODE HERE /////////////////////////
    double sum[BytesPerPixel];
    double psnr[BytesPerPixel];
    for (int i = 0; i < BytesPerPixel; i++) {
        sum[i] = 0.0;
    }
    for (int x = 0; x < Size1; x++)
        for(int y = 0;y < Size2; y++)
            for (int color = 0; color < BytesPerPixel; color++){
                Output[x][y][color] = (Mixeddata[x][y][color]-Imagedata[x][y][color]+COLOR_RANGE-1)%(COLOR_RANGE-1);
                sum[color] += pow((double)(Mixeddata[x][y][color]-Imagedata[x][y][color]),2);
            }
    for (int color = 0; color < BytesPerPixel; color++) {
        psnr[color] = 10.0*log10((double)((COLOR_RANGE-1)*(COLOR_RANGE-1))/(sum[color]/(double)(Size1*Size2)));
        printf("PSNR = %f\n",psnr[color]);
    }
    
	// Write image data (filename specified by second argument) from image data matrix
    
	if (!(file=fopen(argv[3],"wb"))) {
		cout << "Cannot open file: " << argv[3] << endl;
		exit(1);
	}
	fwrite(Output, sizeof(unsigned char), Size1*Size2*BytesPerPixel, file);
	fclose(file);
    
	return 0;
}


