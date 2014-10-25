#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <math.h>
#define COLOR_RANGE 256
#define GX {{-1,0,1},{-2,0,2},{-1,0,1}}
#define GY {{1,2,1},{0,0,0},{-1,-2,-1}}
#define WINDOW_SIZE 5
#define BINARY_0 255
#define BINARY_1 0
#define LOOP 3

using namespace std;

unsigned char roundToColor(double n);
unsigned char threshold = 127;

int main(int argc, char *argv[])

{
    
	// Define file pointer and variables
	FILE *file;
	int BytesPerPixel;
	int Size1 = 374;
    int Size2 = 388;
	
	// Check for proper syntax
	if (argc < 3){
		cout << "Syntax Error - Incorrect Parameter Usage:" << endl;
		cout << "program_name input_image.raw output_image.raw [BytesPerPixel = 3] [Size1 = 256] [Size2 = 256]" << endl;
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
        if(argc >= 6)
            Size2 = atoi(argv[5]);
	}
	
	// Allocate image data array
	unsigned char Imagedata[Size1][Size2][BytesPerPixel];
    double angle[Size1][Size2][BytesPerPixel];
    double mag[Size1][Size2][BytesPerPixel];
    unsigned char OutputImagedata[Size1][Size2][BytesPerPixel];
    
	// Read image (filename specified by first argument) into image data matrix
	if (!(file=fopen(argv[1],"rb"))) {
		cout << "Cannot open file: " << argv[1] <<endl;
		exit(1);
	}
	fread(Imagedata, sizeof(unsigned char), Size1*Size2*BytesPerPixel, file);
	fclose(file);
    
	///////////////////////// INSERT YOUR PROCESSING CODE HERE /////////////////////////
    
    /************************************ Pre-Processing ********************************************/
    int segment[BytesPerPixel][COLOR_RANGE];
    double sum;
    double cdf[BytesPerPixel][COLOR_RANGE];
    
    // Equalized Cumulative Histogram to enhance contrast
    for (int i = 0; i < BytesPerPixel; i++)
        for(int j = 0;j < COLOR_RANGE; j++)
            segment[i][j] = 0;
    for (int i = 0; i < Size1; i ++)
        for(int j = 0; j < Size2; j++)
            for(int u = 0; u < BytesPerPixel; u++)
                for (int color = 0; color < COLOR_RANGE; color++)
                    if (Imagedata[i][j][u] == (unsigned char)color)
                        segment[u][color]++;
    for(int color = 0;color < BytesPerPixel;color++){
        sum = 0.0;
        for(int i = 0;i < COLOR_RANGE;i ++){
            sum += (double)segment[color][i]/(double)(Size1*Size2);
            cdf[color][i] = sum;
        }
    }

    //construct mapping
    for (int i = 0; i < Size1; i++)
        for (int j = 0; j < Size2; j++)
            for (int color = 0; color < BytesPerPixel; color++)
                Imagedata[i][j][color] = roundToColor(cdf[color][Imagedata[i][j][color]]*(double)COLOR_RANGE);
    
    /************************************ Binarization ********************************************/
    for (int i = 0; i < Size1; i++)
        for (int j = 0; j < Size2; j++)
            for (int color = 0; color < BytesPerPixel; color++)
                Imagedata[i][j][color] = Imagedata[i][j][color] > threshold?COLOR_RANGE-1:0;
    
    /************************************ Post-Processing ********************************************/
//    // remove isolated dots
//    for (int i = 0; i < Size1; i ++)
//        for(int j = 0; j < Size2; j++)
//            for(int color = 0; color < BytesPerPixel; color++){
//                int total = 0, count = 0;
//                if (i-1 >= 0 && j -1 >= 0) {
//                    total++;
//                    if (Imagedata[i-1][j-1][color] == COLOR_RANGE-1)
//                        count++;
//                }
//                if (i-1 >= 0) {
//                    total++;
//                    if (Imagedata[i-1][j][color] == COLOR_RANGE-1)
//                        count++;
//                }
//                if (i+1 < Size1) {
//                    total++;
//                    if (Imagedata[i+1][j][color] == COLOR_RANGE-1)
//                        count++;
//                }
//                if (j -1 >= 0) {
//                    total++;
//                    if (Imagedata[i][j-1][color] == COLOR_RANGE-1)
//                        count++;
//                }
//                if (j +1 < Size2) {
//                    total++;
//                    if (Imagedata[i][j+1][color] == COLOR_RANGE-1)
//                        count++;
//                }
//                if (i-1 >= 0 && j +1 < Size2) {
//                    total++;
//                    if (Imagedata[i-1][j+1][color] == COLOR_RANGE-1)
//                        count++;
//                }
//                if (j+1 < Size2) {
//                    total++;
//                    if (Imagedata[i][j+1][color] == COLOR_RANGE-1)
//                        count++;
//                }
//                if (i+1 < Size1 && j+1 < Size2) {
//                    total++;
//                    if (Imagedata[i+1][j+1][color] == COLOR_RANGE-1)
//                        count++;
//                }
//            }
    
//    // connect broken ridges
//    for (int loop = 0; loop < LOOP; loop++) {
//        // get the direction at each pixel
//        int gx[3][3] = GX;
//        int gy[3][3] = GY;
//        for (int color = 0; color < BytesPerPixel; color++)
//            for (int x = 0; x < Size1; x++){
//                for (int y = 0; y < Size2; y++){
//                    int sumX = 0,sumY = 0;
//                    bool edge = false;
//                    for (int i = -1; i < 2; i++) {
//                        for (int j = -1; j < 2; j++) {
//                            if (x+i >= 0 && x+i < Size1 && y+j >= 0 && y+j < Size2) {
//                                sumX += gx[i+1][j+1] * Imagedata[x+i][y+j][color];
//                                sumY += gy[i+1][j+1] * Imagedata[x+i][y+j][color];
//                            }else{
//                                edge = true;
//                            }
//                        }
//                    }
//                    angle[x][y][color] = atan2(sumY, sumX);
//                    mag[x][y][color] = sqrt(pow(sumX, 2.0)+pow(sumY, 2.0));
//                }
//            }
//        
//        // Global processing via Hough Transform
//        
//        
//        for (int color = 0; color < BytesPerPixel; color++)
//            for (int x = 0; x < Size1; x++)
//                for (int y = 0; y < Size2; y++)
//                    Imagedata[x][y][color] = OutputImagedata[x][y][color];
//    }
    

    
	// Write image data (filename specified by second argument) from image data matrix
    
	if (!(file=fopen(argv[2],"wb"))) {
		cout << "Cannot open file: " << argv[2] << endl;
		exit(1);
	}
	fwrite(Imagedata, sizeof(unsigned char), Size1*Size2*BytesPerPixel, file);
	fclose(file);
    
	return 0;
}

unsigned char roundToColor(double n){
    if (n >= 255.0) {
        return 255;
    }else if (n <= 0.0){
        return 0;
    }else{
        return (unsigned char)round(n);
    }
}

