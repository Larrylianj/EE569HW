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
#define GX {{-1,0,1},{-2,0,2},{-1,0,1}}
#define GY {{1,2,1},{0,0,0},{-1,-2,-1}}

using namespace std;

int main(int argc, char *argv[])

{
    
	// Define file pointer and variables
	FILE *file;
	int BytesPerPixel;
	int Size1 = 256;
    int Size2 = 256;
	
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
    double Magnitude[Size1][Size2][BytesPerPixel];
    double X_gradient[Size1][Size2][BytesPerPixel];
    double Y_gradient[Size1][Size2][BytesPerPixel];
    unsigned char OutputImagedata[Size1][Size2][BytesPerPixel];
    
	// Read image (filename specified by first argument) into image data matrix
	if (!(file=fopen(argv[1],"rb"))) {
		cout << "Cannot open file: " << argv[1] <<endl;
		exit(1);
	}
	fread(Imagedata, sizeof(unsigned char), Size1*Size2*BytesPerPixel, file);
	fclose(file);
    
	///////////////////////// INSERT YOUR PROCESSING CODE HERE /////////////////////////
    int gx[3][3] = GX;
    int gy[3][3] = GY;
    double max[BytesPerPixel];
    int max_X[BytesPerPixel], min_X[BytesPerPixel], max_Y[BytesPerPixel], min_Y[BytesPerPixel];
    for (int color = 0; color < BytesPerPixel; color++) {
        max[color] = 0;
        max_X[color] = 0;min_X[color] = 0;
        max_Y[color] = 0;min_Y[color] = 0;
        for (int x = 0; x < Size1; x++){
            for (int y = 0; y < Size2; y++){
                int sumX = 0,sumY = 0;
                bool edge = false;
                for (int i = -1; i < 2; i++) {
                    for (int j = -1; j < 2; j++) {
                        if (x+i >= 0 && x+i < Size1 && y+j >= 0 && y+j < Size2) {
                            sumX += gx[i+1][j+1] * (int)Imagedata[x+i][y+j][color];
                            sumY += gy[i+1][j+1] * (int)Imagedata[x+i][y+j][color];
                        }else{
                            edge = true;
                        }
                    }
                }
                
                if (sumX > max_X[color])
                    max_X[color] = sumX;
                if (sumY > max_Y[color])
                    max_Y[color] = sumY;
                if (sumX < min_X[color])
                    min_X[color] = sumX;
                if (sumY < min_Y[color])
                    min_Y[color] = sumY;
                X_gradient[x][y][color] = sumX;
                Y_gradient[x][y][color] = sumY;
            }
        }
    }
    
    // normalize
    for (int x = 0; x < Size1; x++)
        for (int y = 0; y < Size2; y++)
            for (int color = 0; color < BytesPerPixel; color++){
                X_gradient[x][y][color] = round((double)(X_gradient[x][y][color] - min_X[color])/(double)(max_X[color] - min_X[color]) * (COLOR_RANGE-1));
                Y_gradient[x][y][color] = round((double)(Y_gradient[x][y][color] - min_Y[color])/(double)(max_Y[color] - min_Y[color]) * (COLOR_RANGE-1));
                Magnitude[x][y][color] = sqrt(pow(X_gradient[x][y][color], 2.0)+pow(Y_gradient[x][y][color], 2.0));
                if (Magnitude[x][y][color] > max[color])
                    max[color] = Magnitude[x][y][color];
            }
    for (int x = 0; x < Size1; x++)
        for (int y = 0; y < Size2; y++)
            for (int color = 0; color < BytesPerPixel; color++)
                //OutputImagedata[x][y][color] = (unsigned char)Y_gradient[x][y][color];
                OutputImagedata[x][y][color] = (unsigned char)round((double)Magnitude[x][y][color]/(double)max[color]*(COLOR_RANGE-1));
    
    
	// Write image data (filename specified by second argument) from image data matrix
    
	if (!(file=fopen(argv[2],"wb"))) {
		cout << "Cannot open file: " << argv[2] << endl;
		exit(1);
	}
	fwrite(OutputImagedata, sizeof(unsigned char), Size1*Size2*BytesPerPixel, file);
	fclose(file);
    
	return 0;
}



