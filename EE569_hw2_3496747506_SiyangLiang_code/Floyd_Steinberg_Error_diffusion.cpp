#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define COLOR_RANGE 256
#define THRESHOLD 127
#define BINARY_1 0
#define BINARY_0 255
#define ERRO_DIFFUSION_MATRIX {{0,0,0},{0,0,7/16},{3/16,5/16,1/16}}

unsigned char min2(unsigned char a, unsigned char b);
unsigned char max2(unsigned char a, unsigned char b);
unsigned char clique(int n);

int main(int argc, char *argv[])

{
    
	// Define file pointer and variables
	FILE *file;
    int BytesPerPixel;
	int Size1 = 512;
    int Size2 = 512;
	
	// Check for proper syntax
	if (argc < 3){
		printf("Syntax Error - Incorrect Parameter Usage:");
		printf("program_name input_image.raw output_image.raw [BytesPerPixel = 1] [Size1 = 256] [Size2 = 256]");
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
    unsigned char ThresholdData[Size1][Size2][BytesPerPixel];
    
	// Read image (filename specified by first argument) into image data matrix
	if (!(file=fopen(argv[1],"rb"))) {
		printf("Cannot open file: ");
		exit(1);
	}
	fread(Imagedata, sizeof(unsigned char), Size1*Size2*BytesPerPixel, file);
	fclose(file);
    
	///////////////////////// INSERT YOUR PROCESSING CODE HERE /////////////////////////
    //int matrix[3][3] = ERRO_DIFFUSION_MATRIX;
    for (int x = 0; x < Size1; x++) {
        for(int y = 0; y < Size2; y++){
            int err[BytesPerPixel];
            for (int color = 0; color < BytesPerPixel; color++) {
                
                ThresholdData[x][y][color] = Imagedata[x][y][color] >= THRESHOLD? BINARY_0:BINARY_1;
                err[color] = (int)Imagedata[x][y][color] - (int)ThresholdData[x][y][color];
                Imagedata[x][y][color] = Imagedata[x][y][color] >= THRESHOLD? BINARY_0:BINARY_1;
                
                // serpentine searching
                if (x%2 == 0) {
                    if (y+1 < Size2)
                        Imagedata[x][y+1][color] = clique((int)round((double)err[color]*7.0/16.0) + (int)Imagedata[x][y+1][color]);
                    if (x+1 < Size1 && y-1 >= 0)
                        Imagedata[x+1][y-1][color] = clique((int)round((double)err[color]*3.0/16.0) + (int)Imagedata[x+1][y-1][color]);
                    if (x+1 < Size1)
                        Imagedata[x+1][y][color] = clique((int)round((double)err[color]*5.0/16.0) + (int)Imagedata[x+1][y][color]);
                    if (x+1 < Size1 && y+1 < Size2)
                        Imagedata[x+1][y+1][color] = clique((int)round((double)err[color]*1.0/16.0) + (int)Imagedata[x+1][y+1][color]);
                }else{
                    y = Size2 - 1 - y;
                    if (y+1 >= 0)
                        Imagedata[x][y+1][color] = clique((int)round((double)err[color]*7.0/16.0) + (int)Imagedata[x][y+1][color]);
                    if (x+1 < Size1 && y-1 < Size2)
                        Imagedata[x+1][y-1][color] = clique((int)round((double)err[color]*3.0/16.0) + (int)Imagedata[x+1][y-1][color]);
                    if (x+1 >= 0)
                        Imagedata[x+1][y][color] = clique((int)round((double)err[color]*5.0/16.0) + (int)Imagedata[x+1][y][color]);
                    if (x-1 >= 0 && y+1 >= 0)
                        Imagedata[x-1][y+1][color] = clique((int)round((double)err[color]*1.0/16.0) + (int)Imagedata[x-1][y+1][color]);
                }
                
                
            }
        }
    }
    
	// Write image data (filename specified by second argument) from image data matrix
    
	if (!(file=fopen(argv[2],"wb"))) {
		printf("Cannot open output file: ");
		exit(1);
	}
	fwrite(Imagedata, sizeof(unsigned char), Size1*Size2*BytesPerPixel, file);
	fclose(file);
    
	return 0;
}

unsigned char min2(unsigned char a, unsigned char b){
    return a > b? b:a;
}

unsigned char max2(unsigned char a, unsigned char b){
    return a > b? a:b;
}

unsigned char clique(int n)
{
    return n > 255? 255:(n < 0?0:(unsigned char)n);
}



