#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define COLOR_RANGE 256
#define THRESHOLD 127
#define BINARY_1 0
#define BINARY_0 255
#define T_MATRIX_2 {{1,2},{3,0}}

int main(int argc, char *argv[])

{
	// Define file pointer and variables
	FILE *file;
    int method = 2; // 1.fixed thresholding 2.random thresholding 3.dithering matrix base2 4.base4
	int BytesPerPixel;
	int Size1 = 512;
    int Size2 = 512;
	
	// Check for proper syntax
	if (argc < 3){
		printf("Syntax Error - Incorrect Parameter Usage:");
		printf("program_name input_image.raw output_image.raw [BytesPerPixel = 1] [Size1 = 256] [Size2 = 256] [Method = 1~4]");
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
    if (argc >= 7) {
        method = atoi(argv[6]);
    }
	
	// Allocate image data array
	unsigned char Imagedata[Size1][Size2][BytesPerPixel];
    
	// Read image (filename specified by first argument) into image data matrix
	if (!(file=fopen(argv[1],"rb"))) {
		printf("Cannot open file: ");
		exit(1);
	}
	fread(Imagedata, sizeof(unsigned char), Size1*Size2*BytesPerPixel, file);
	fclose(file);
    
	///////////////////////// INSERT YOUR PROCESSING CODE HERE /////////////////////////
    int T2[2][2] = T_MATRIX_2;
    int T4[4][4];
    int n;
    switch (method) {
        case 1:
            // fixed thresholding
            for (int x = 0; x < Size1; x++)
                for(int y = 0; y < Size2; y++)
                    for(int color = 0;color < BytesPerPixel;color ++)
                        Imagedata[x][y][color] = Imagedata[x][y][color] >= THRESHOLD? BINARY_0:BINARY_1;
            break;
        case 2:
            // random thresholding
            for (int x = 0; x < Size1; x++)
                for(int y = 0; y < Size2; y++)
                    for(int color = 0;color < BytesPerPixel;color ++)
                        Imagedata[x][y][color] = (double)Imagedata[x][y][color] >= rand()%(COLOR_RANGE-1)? BINARY_0:BINARY_1;
            break;
        case 3:
            // dithering matrix base_2
            n = sizeof(T2[0])/sizeof(int);
            for (int x = 0; x < Size1; x++)
                for(int y = 0; y < Size2; y++)
                    for(int color = 0;color < BytesPerPixel;color ++)
                        Imagedata[x][y][color] = (double)Imagedata[x][y][color]/(double)(COLOR_RANGE) > ((double)T2[x%n][y%n]+0.5)/pow(n, 2.0)? BINARY_0:BINARY_1;
            break;
        case 4:
            // dithering matrix base_4
            n = sizeof(T2[0])/sizeof(int);
            for (int i = 0; i < 2; i++)
                for(int j = 0; j < 2; j++)
                    for (int x = 0; x < n; x++)
                        for(int y = 0;y < n;y++)
                            T4[x+i*2][y+j*2] = 4*T2[x][y] + (2*i+j+1)%4;
            n = 2*n;
            for (int x = 0; x < Size1; x++)
                for(int y = 0; y < Size2; y++)
                    for(int color = 0;color < BytesPerPixel;color ++)
                        Imagedata[x][y][color] = (double)Imagedata[x][y][color]/(double)(COLOR_RANGE) > ((double)T4[x%n][y%n]+0.5)/pow(n, 2.0)? BINARY_0:BINARY_1;
            
            break;
        default:
            break;
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


