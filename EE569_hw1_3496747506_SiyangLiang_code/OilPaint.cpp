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
#define FACTOR_R 3
#define FACTOR_G 3
#define FACTOR_B 2

using namespace std;

unsigned char findPluralOf(unsigned char * arr, int len);

int main(int argc, char *argv[])

{
    // for debug only
//    char in_file[] = "/Users/liangsiyang/Documents/USC-learning/EE-569/HW1/images/scene2_256.raw";
//    char out_file[] = "/Users/liangsiyang/Documents/USC-learning/EE-569/HW1/images/hw1_2_b.output.raw";
//    argc = 5;
//    argv[1] = in_file;
//    argv[2] = out_file;
//    *argv[3] = '3';
//    char BPP[] = "360";
//    argv[4] = BPP;

	// Define file pointer and variables
	FILE *file;
	int BytesPerPixel;
	int Size = 256;
    int N = 9;
	
	// Check for proper syntax
	if (argc < 3){
		cout << "Syntax Error - Incorrect Parameter Usage:" << endl;
		cout << "program_name input_image.raw output_image.raw [BytesPerPixel = 1] [Size = 256] [N = 3]" << endl;
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
        if (argc >= 6) {
            N = atoi(argv[5]);
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
    
    unsigned char OutputImagedata[Size][Size][BytesPerPixel];
    // use 3-3-2 quantization method
//    unsigned char color_set_R[(int)pow(2,FACTOR_R)+1];
//    unsigned char color_set_G[(int)pow(2,FACTOR_G)+1];
//    unsigned char color_set_B[(int)pow(2,FACTOR_B)+1];
//    
//    int slot_range = COLOR_RANGE/(sizeof(color_set_R)/sizeof(unsigned char));
//    for (int i = 0; i < (int)pow(2,FACTOR_R)+1; i++)
//        color_set_R[i] = (unsigned char)i * slot_range;
//    slot_range = COLOR_RANGE/(sizeof(color_set_G)/sizeof(unsigned char));
//    for (int i = 0; i < (int)pow(2,FACTOR_R)+1; i++)
//        color_set_G[i] = (unsigned char)i * slot_range;
//    slot_range = COLOR_RANGE/(sizeof(color_set_B)/sizeof(unsigned char));
//    for (int i = 0; i < (int)pow(2,FACTOR_B)+1; i++)
//        color_set_B[i] = (unsigned char)i * slot_range;
//    
//    // mapping
//    for (int i = 0; i < Size; i++){
//        for(int j = 0;j < Size; j++){
//            for (int slot = 0; slot < (int)pow(2,FACTOR_R); slot ++)
//                if (Imagedata[i][j][0] >= color_set_R[slot] && Imagedata[i][j][0] < color_set_R[slot+1])
//                    QuantizedImagedata[i][j][0] = (color_set_R[slot+1]-color_set_R[slot])/2 + color_set_R[slot];
//            for (int slot = 0; slot < (int)pow(2,FACTOR_G); slot ++)
//                if (Imagedata[i][j][1] >= color_set_G[slot] && Imagedata[i][j][1] < color_set_G[slot+1])
//                    QuantizedImagedata[i][j][1] = (color_set_G[slot+1]-color_set_G[slot])/2 + color_set_G[slot];
//            for (int slot = 0; slot < (int)pow(2,FACTOR_B); slot ++)
//                if (Imagedata[i][j][2] >= color_set_B[slot] && Imagedata[i][j][2] < color_set_B[slot+1])
//                    QuantizedImagedata[i][j][2] = (color_set_B[slot+1]-color_set_B[slot])/2 + color_set_B[slot];
//        }
//    }
    
    // add effect
    unsigned char window[N * N];
    int count;
    for (int i = 0; i < Size; i++){
        for(int j = 0;j < Size; j++){
            for(int u = 0;u < BytesPerPixel; u++){
                count = 0;
                for(int x = i - N/2; x < i + N/2; x++)
                    for(int y = j - N/2; y < j + N/2; y ++)
                        if(x >= 0 && x < Size && y >= 0 && y < Size)
                            window[count++] = Imagedata[x][y][u];
                OutputImagedata[i][j][u] = findPluralOf(window, count);
            }
        }
    }
    
    
	// Write image data (filename specified by second argument) from image data matrix
    
	if (!(file=fopen(argv[2],"wb"))) {
		cout << "Cannot open file: " << argv[2] << endl;
		exit(1);
	}
	fwrite(OutputImagedata, sizeof(unsigned char), Size*Size*BytesPerPixel, file);
	fclose(file);
    
	return 0;
}


unsigned char findPluralOf(unsigned char * arr, int len){
    int map[len];
    int count[len];
    int remain;
    for (int i = 0; i < len; i++)
        map[i] = COLOR_RANGE + 1;
    memset(count,0,len);
    for(int i = 0;i < len;i++){
        remain = (int)(arr[i])%len;
        while (map[remain] != COLOR_RANGE + 1) {
            if (map[remain] == (int)arr[i]) {
                count[remain]++;
            }
            remain ++;
            remain %= len;
        }
        map[remain] = (int)arr[i];
    }
    
    remain = count[0];
    for (int i = 0; i < len; i++)
        if(count[i] > remain)
            remain = i;
    
    return map[remain];
}
