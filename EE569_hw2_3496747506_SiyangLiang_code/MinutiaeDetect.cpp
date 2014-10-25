#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <math.h>
#define COLOR_RANGE 256
#define BINARY_1 0 // black
#define BINARY_0 255 // white

using namespace std;

bool AcontainsB(unsigned char A[], int A_length, unsigned char B){
    for (int i = 0; i < A_length; i++) {
        if (A[i] == B) {
            return true;
        }
    }
    return false;
}

unsigned char CrossCase3[] = {0x2a,0xa2,0x8a,0xa8,0xca,0xac,0xfa,0xaf,0xeb,0xbe,0x2b,0xb2,0xa9,0x9a,0xba,0xab,0xda,0xad};
unsigned char CrossCase2[] = {0x01,0x10,0x04,0x40,0x02,0x20,0x80,0x08,0x81,0x03,0x06,0x0c,0x18,0x30,0x60,0xc0};

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
    int CrossNumber[Size1][Size2][BytesPerPixel];
    
	// Read image (filename specified by first argument) into image data matrix
	if (!(file=fopen(argv[1],"rb"))) {
		cout << "Cannot open file: " << argv[1] <<endl;
		exit(1);
	}
	fread(Imagedata, sizeof(unsigned char), Size1*Size2*BytesPerPixel, file);
	fclose(file);
    
	///////////////////////// INSERT YOUR PROCESSING CODE HERE /////////////////////////
    // fill in the visited
    for (int x = 0; x < Size1; x++) {
        for (int y = 0; y < Size2; y++) {
            for (int color = 0; color < BytesPerPixel; color++) {
                //printf("%d ",Imagedata[x][y][color]);
                Imagedata[x][y][color] = Imagedata[x][y][color] >= 127?BINARY_0:BINARY_1;
                //printf("%d ",Imagedata[x][y][color] == BINARY_1);
            }
        }
    }
    
    for (int x = 0; x < Size1; x++) {
        for (int y = 0; y < Size2; y++) {
            for (int color = 0; color < BytesPerPixel; color++) {
                
                if (Imagedata[x][y][color] == BINARY_1) {
                        if (x-1 >= 0 && x+1 < Size1 && y-1 >= 0 && y+1 < Size2) {
                            unsigned char string = 0x00;
                            // set the 8-bit string
                            string |= Imagedata[x][y+1][color] == BINARY_1? 0x01:0x00;
                            string <<= 1;
                            string |= Imagedata[x+1][y+1][color] == BINARY_1? 0x01:0x00;
                            string <<= 1;
                            string |= Imagedata[x+1][y][color] == BINARY_1? 0x01:0x00;
                            string <<= 1;
                            string |= Imagedata[x+1][y-1][color] == BINARY_1? 0x01:0x00;
                            string <<= 1;
                            string |= Imagedata[x][y-1][color] == BINARY_1? 0x01:0x00;
                            string <<= 1;
                            string |= Imagedata[x-1][y-1][color] == BINARY_1? 0x01:0x00;
                            string <<= 1;
                            string |= Imagedata[x-1][y][color] == BINARY_1? 0x01:0x00;
                            string <<= 1;
                            string |= Imagedata[x-1][y+1][color] == BINARY_1? 0x01:0x00;
                            //printf("%d %d %d %d %d %d %d %d\n",Imagedata[i-1][j+1][color],Imagedata[i-1][j][color],Imagedata[i-1][j-1][color],Imagedata[i][j-1][color],Imagedata[i+1][j-1][color],Imagedata[i+1][j][color],Imagedata[i+1][j+1][color],Imagedata[i][j+1][color]);
                            //printf("string = %x\n",string);
                            if (AcontainsB(CrossCase3, 18, string)) {
                                CrossNumber[x][y][color] = 3;
                            }else if (AcontainsB(CrossCase2, 24, string)) {
                                CrossNumber[x][y][color] = 2;
                            }else if ((string == 0x92) || (string == 0x4a) || (string == 0x29) || (string == 0xa4) || (string == 0x20) || (string == 0x02) || (string == 0x80) || (string == 0x08)) {
                                CrossNumber[x][y][color] = 1;
                            }else{
                                CrossNumber[x][y][color] = 0;
                            }
//                            if (CrossNumber[x][y][color] != 0) {
//                                printf("c = %d\n",CrossNumber[x][y][color]);
//                                printf("string = %x\n",string);
//                            }
                        }
                }
                
            }
        }
    }
    
    // generate marks
    BytesPerPixel = 3;
    unsigned char OutputImagedata[Size1][Size2][BytesPerPixel];
    for (int x = 0; x < Size1; x++) {
        for (int y = 0; y < Size2; y++) {
            if (x-1 >= 0 && x+1 < Size1 && y-1 >= 0 && y+1 < Size2) {
                if (CrossNumber[x][y][0] == 1) { // ridge ending minutia
                    for (int i = x-1; i <= x+1; i++) {
                        for (int j = y-1; j < y+1; j++) {
                            OutputImagedata[i][j][0] = 0;
                            OutputImagedata[i][j][1] = 255;
                            OutputImagedata[i][j][2] = 0;
                        }
                    }

                }else if (CrossNumber[x][y][0] == 2){ // intermedia minutia
                    for (int i = x-1; i <= x+1; i++) {
                        for (int j = y-1; j < y+1; j++) {
                            OutputImagedata[i][j][0] = 255;
                            OutputImagedata[i][j][1] = 0;
                            OutputImagedata[i][j][2] = 0;
                        }
                    }
                }else if (CrossNumber[x][y][0] == 3){ // bifurcation minutia
                    for (int i = x-1; i <= x+1; i++) {
                        for (int j = y-1; j < y+1; j++) {
                            OutputImagedata[i][j][0] = 0;
                            OutputImagedata[i][j][1] = 0;
                            OutputImagedata[i][j][2] = 255;
                        }
                    }
                }else{
                    OutputImagedata[x][y][0] = Imagedata[x][y][0];
                    OutputImagedata[x][y][1] = Imagedata[x][y][0];
                    OutputImagedata[x][y][2] = Imagedata[x][y][0];
                }
            }
        }
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


