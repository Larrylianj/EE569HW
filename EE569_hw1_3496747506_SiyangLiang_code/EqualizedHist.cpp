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

void printArray(int *arr, int len);

int main(int argc, char *argv[])

{
    // for debug only
//    char in_file[] = "/Users/liangsiyang/Documents/USC-learning/EE-569/HW1/images/desk.raw";
//    char out_file[] = "/Users/liangsiyang/Documents/USC-learning/EE-569/HW1/images/hw2_a.output.raw";
//    argc = 4;
//    argv[1] = in_file;
//    argv[2] = out_file;
//    *argv[3] = '3';
    
	// Define file pointer and variables
	FILE *file;
	int BytesPerPixel;
	int Size1 = 400;
    int Size2 = 300;
	
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
    
	// Read image (filename specified by first argument) into image data matrix
	if (!(file=fopen(argv[1],"rb"))) {
		cout << "Cannot open file: " << argv[1] <<endl;
		exit(1);
	}
	fread(Imagedata, sizeof(unsigned char), Size1*Size2*BytesPerPixel, file);
	fclose(file);
    
	///////////////////////// INSERT YOUR PROCESSING CODE HERE /////////////////////////
    double cdf[BytesPerPixel][COLOR_RANGE];
    unsigned char min[BytesPerPixel],max[BytesPerPixel];
    for (int color = 0; color < BytesPerPixel; color++){
        min[color] = Imagedata[0][0][color];
        max[color] = Imagedata[0][0][color];
    }

    for (int i = 0; i < Size1; i ++)
        for(int j = 0; j < Size2; j++)
            for(int u = 0; u < BytesPerPixel; u++){
                if (Imagedata[i][j][u] > max[u])
                    max[u] = Imagedata[i][j][u];
                if (Imagedata[i][j][u] < min[u])
                    min[u] = Imagedata[i][j][u];
            }
    for(int color = 0;color < BytesPerPixel;color++)
        for(int i = 0;i < COLOR_RANGE;i ++)
            cdf[color][i] = (double)i * (double)COLOR_RANGE/(double)(Size1*Size2);
    for(int color = 0;color < BytesPerPixel;color++)
        for(int i = 0;i < COLOR_RANGE;i ++)
            cdf[color][(int)round(cdf[color][i]*(double)(max[color]-min[color])) + (int)min[color]] = cdf[color][i];
    
    //construct mapping
    for (int i = 0; i < Size1; i++)
        for (int j = 0; j < Size2; j++)
            for (int color = 0; color < BytesPerPixel; color++)
                Imagedata[i][j][color] = (unsigned char)round(cdf[color][Imagedata[i][j][color]]*(double)COLOR_RANGE);
    
    //    // output histogram to a data file
    //    FILE * fp;
    //    char skip = '\n';
    //    char data_name[] = "/Users/liangsiyang/Documents/USC-learning/EE-569/HW1/hist01.txt";
    //    if (!(fp = fopen(data_name, "wb"))) {
    //        printf("Fail to write data\n");
    //    }
    //    for (int i = 0; i < BytesPerPixel; i++){
    //        for(int j = 0;j < COLOR_RANGE; j++){
    //            fwrite(segment[j], sizeof(int), 1, fp);
    //            fwrite(&skip, sizeof(char), 1, fp);
    //        }
    //    }
    //    fclose(fp);

    
	// Write image data (filename specified by second argument) from image data matrix
	if (!(file=fopen(argv[2],"wb"))) {
		cout << "Cannot open file: " << argv[2] << endl;
		exit(1);
	}
	fwrite(Imagedata, sizeof(unsigned char), Size1*Size2*BytesPerPixel, file);
	fclose(file);
    
	return 0;
}

