// This sample code reads in image data from a RAW image file and
// writes it into another file

// NOTE:	The code assumes that the image is of size 256 x 256 and is in the
//			RAW format. You will need to make corresponding changes to
//			accommodate images of different sizes and/or types

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <vector>
#define COLOR_RANGE 256
#define DEFAULT_1ST_THRESHOLD 64
#define DEFAULT_2ND_THRESHOLD 128
#define DEFAULT_3RD_THRESHOLD 192
#define LoG_MASK {{0,0,-1,0,0},{0,-1,-2,-1,0},{-1,-2,16,-2,-1},{0,-1,-2,-1,0},{0,0,-1,0,0}}

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
    int MedianImage[Size1][Size2][BytesPerPixel];
    unsigned char OutputImagedata[Size1][Size2][BytesPerPixel];
    
	// Read image (filename specified by first argument) into image data matrix
	if (!(file=fopen(argv[1],"rb"))) {
		cout << "Cannot open file: " << argv[1] <<endl;
		exit(1);
	}
	fread(Imagedata, sizeof(unsigned char), Size1*Size2*BytesPerPixel, file);
	fclose(file);
    
	///////////////////////// INSERT YOUR PROCESSING CODE HERE /////////////////////////
    int mask[5][5] = LoG_MASK;
    int max[BytesPerPixel], min[BytesPerPixel];
    int hist[BytesPerPixel][COLOR_RANGE];
    
    for (int i = 0; i < BytesPerPixel; i++)
        for (int pixel = 0; pixel < COLOR_RANGE; pixel++)
            hist[i][pixel] = 0;
    
    for (int color = 0; color < BytesPerPixel; color++) {
        max[color] = 0, min[color] = 0;
        for (int x = 0; x < Size1; x++){
            for (int y = 0; y < Size2; y++){
                // LoG Detect
                int sumXY = 0;
                bool edge = false;
                for (int i = -2; i < 3; i++) {
                    for (int j = -2; j < 3; j++) {
                        if (x+i >= 0 && x+i < Size1 && y+j >= 0 && y+j < Size2) {
                            sumXY += mask[i+2][j+2] * (int)Imagedata[x+i][y+j][color];
                        }else{
                            edge = true;
                        }
                    }
                }
                if(!edge)
                    MedianImage[x][y][color] = sumXY;
                else
                    MedianImage[x][y][color] = 0.0;
                //printf("[%d][%d][%d] = %d\n",x,y,color,MedianImage[x][y][color]);
                if (MedianImage[x][y][color] > max[color])
                    max[color] = MedianImage[x][y][color];
                if (MedianImage[x][y][color] < min[color])
                    min[color] = MedianImage[x][y][color];
            }
        }
        //printf("min[%d] = %d\n",color,min[color]);
        //printf("max[%d] = %d\n",color,max[color]);
    }
    
    // normalize
    for (int x = 0; x < Size1; x++)
        for (int y = 0; y < Size2; y++)
            for (int color = 0; color < BytesPerPixel; color++){
                OutputImagedata[x][y][color] = (unsigned char)round(((double)(MedianImage[x][y][color]-min[color]))/((double)(max[color]-min[color])) * (COLOR_RANGE-1));
                // histogram fill in
                for (int pixel = 0; pixel < COLOR_RANGE; pixel++)
                    if(OutputImagedata[x][y][color] == (unsigned char)pixel)
                        hist[color][pixel] ++;
            }

    /*************************************** Ternary Mapping *************************************************/
    
    // get the 1st order derivative
    int first_hist[BytesPerPixel][COLOR_RANGE];
    int second_hist[BytesPerPixel][COLOR_RANGE];
    for(int color = 0; color < BytesPerPixel; color++){
        first_hist[color][0] = 0;
        for (int i = 1; i < COLOR_RANGE; i++) {
            first_hist[color][i] = (int)(hist[i]-hist[i-1]);
        }
    }
    for(int color = 0; color < BytesPerPixel; color++){
        second_hist[color][0] = 0;
        for (int i = 1; i < COLOR_RANGE; i++) {
            second_hist[color][i] = abs((int)(first_hist[i]-first_hist[i-1]));
        }
    }
    
    // get the knee position
    unsigned char knee1[BytesPerPixel], knee2[BytesPerPixel];
    for(int color = 0; color < BytesPerPixel; color++){
        knee1[color] = COLOR_RANGE/3;
        knee2[color] = COLOR_RANGE*2/3;
        unsigned char min1 = second_hist[color][0];
        unsigned char min2 = min1;
        for (int i = 0; i < COLOR_RANGE; i++) {
            if(second_hist[color][i] < min2){
                if (second_hist[color][i] < min1) {
                    min2 = min1;
                    knee2[color] = knee1[color];
                    min1 = second_hist[color][i];
                    knee1[color] = i;
                }else{
                    min2 = second_hist[color][i];
                    knee2[color] = i;
                }
            }
        }
    }
    
    for (int x = 0; x < Size1; x++)
        for (int y = 0; y < Size2; y++)
            for (int color = 0; color < BytesPerPixel; color++){
                if (OutputImagedata[x][y][color] >= knee2[color])
                    OutputImagedata[x][y][color] = DEFAULT_3RD_THRESHOLD;
                else if (OutputImagedata[x][y][color] <= knee1[color])
                    OutputImagedata[x][y][color] = DEFAULT_2ND_THRESHOLD;
                else
                    OutputImagedata[x][y][color] = DEFAULT_1ST_THRESHOLD;
            }
    
    
    
//    // find the 1st one-thrid, 2nd one-third and 3rd one-third pixel point
//    for (int color = 0; color < BytesPerPixel; color++){
//        int sum_hist = 0;
//        for (int pixel = 0; pixel < COLOR_RANGE; pixel++) {
//            sum_hist += hist[color][pixel];
//            if (sum_hist >= Size1*Size2*2/5){
//                threshold[color][0] = pixel;
//                break;
//            }
//        }
//        sum_hist = 0;
//        for (int pixel = 0; pixel < COLOR_RANGE; pixel++) {
//            sum_hist += hist[color][pixel];
//            if (sum_hist >= Size1*Size2*3/5){
//                threshold[color][1] = pixel+1;
//                break;
//            }
//        }
//    }
//    
//    // ternary mapping
//    for (int x = 0; x < Size1; x++)
//        for (int y = 0; y < Size2; y++)
//            for (int color = 0; color < BytesPerPixel; color++){
//                if (OutputImagedata[x][y][color] >= threshold[color][1])
//                    OutputImagedata[x][y][color] = DEFAULT_3RD_THRESHOLD;
//                else if (OutputImagedata[x][y][color] <= threshold[color][0])
//                    OutputImagedata[x][y][color] = DEFAULT_2ND_THRESHOLD;
//                else
//                    OutputImagedata[x][y][color] = DEFAULT_1ST_THRESHOLD;
//            }
    
	// Write image data (filename specified by second argument) from image data matrix
	if (!(file=fopen(argv[2],"wb"))) {
		cout << "Cannot open file: " << argv[2] << endl;
		exit(1);
	}
	fwrite(OutputImagedata, sizeof(unsigned char), Size1*Size2*BytesPerPixel, file);
	fclose(file);
    
	return 0;
}





