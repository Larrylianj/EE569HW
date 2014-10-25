#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <algorithm>
#define COLOR_RANGE 256
#define THRESHOLD 127
#define BINARY_1 255
#define BINARY_0 0

unsigned char min2(unsigned char a, unsigned char b);
unsigned char max2(unsigned char a, unsigned char b);double SquareDistance3(double a1, double a2, double a3, double b1, double b2, double b3);
bool double_equals(double a, double b);
unsigned char clique(int n);

using namespace std;

int main(int argc, char *argv[])

{
    
	// Define file pointer and variables
	FILE *file;
    int BytesPerPixel;
	int Size1 = 350;
    int Size2 = 258;
	
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
    unsigned char Thresholddata[Size1][Size2][BytesPerPixel];
    int err[Size1][Size2][BytesPerPixel];
    
	// Read image (filename specified by first argument) into image data matrix
	if (!(file=fopen(argv[1],"rb"))) {
		printf("Cannot open file: ");
		exit(1);
	}
	fread(Imagedata, sizeof(unsigned char), Size1*Size2*BytesPerPixel, file);
	fclose(file);
    
	///////////////////////// INSERT YOUR PROCESSING CODE HERE /////////////////////////
    // convert to CMY space
    for (int x = 0; x < Size1; x++)
        for(int y = 0; y < Size2; y++)
            for (int color = 0; color < BytesPerPixel; color++)
                Imagedata[x][y][color] = COLOR_RANGE-1 - Imagedata[x][y][color];
    // err[x][y]or Diffusion
    for (int x = 0; x < Size1; x++) {
        for(int y = 0; y < Size2; y++){
            for (int color = 0; color < BytesPerPixel; color++) {
                Thresholddata[x][y][color] = Imagedata[x][y][color] > THRESHOLD? BINARY_1:BINARY_0;
                err[x][y][color] = (int)Imagedata[x][y][color] - (int)Thresholddata[x][y][color];
                Imagedata[x][y][color] = Thresholddata[x][y][color];
            }
            // determine the quadrant
            int quadrant = 0;
            unsigned char Vertex[3];
            double Cx = (double)Imagedata[x][y][0]/(double)BINARY_1;
            double Cy = (double)Imagedata[x][y][1]/(double)BINARY_1;
            double Cz = (double)Imagedata[x][y][2]/(double)BINARY_1;
            if (Cx + Cy + Cz <= 1.0) {
                quadrant = 1;
            }else if (Cx + Cy + Cz >= 2.0) {
                quadrant = 4;
            }else if (Cx + Cy <= 1.0) {
                if (Cx + Cz <= 1.0) {
                    quadrant = 2;
                }else{
                    quadrant = 3;
                }
            }else{
                if (Cx + Cz <= 1.0) {
                    quadrant = 6;
                }else{
                    quadrant = 5;
                }
            }
            // find the nearest vertex
            double disW,disM,disY,disC,disB,disK,disG,disR,minDis;
            switch (quadrant) {
                case 1: // CMYW
                    disW = SquareDistance3(Cx, Cy, Cz, 0, 0, 0);
                    disM = SquareDistance3(Cx, Cy, Cz, 1, 0, 0);
                    disY = SquareDistance3(Cx, Cy, Cz, 0, 0, 1);
                    disC = SquareDistance3(Cx, Cy, Cz, 0, 1, 0);
                    minDis = min(min(disM,disW),min(disC,disY));
                    if (double_equals(minDis,disW)) {
                        Vertex[0] = 0; Vertex[1] = 0; Vertex[2] = 0;
                    }else if (double_equals(minDis,disM)) {
                        Vertex[0] = 1; Vertex[1] = 0; Vertex[2] = 0;
                    }else if (double_equals(minDis,disY)) {
                        Vertex[0] = 0; Vertex[1] = 0; Vertex[2] = 1;
                    }else if (double_equals(minDis,disC)){
                        Vertex[0] = 0; Vertex[1] = 1; Vertex[2] = 0;
                    }else{
                        printf("Error\n");
                    }
                    break;
                case 2: // MYGC
                    disW = SquareDistance3(Cx, Cy, Cz, 0, 0, 0);
                    disM = SquareDistance3(Cx, Cy, Cz, 1, 0, 0);
                    disG = SquareDistance3(Cx, Cy, Cz, 0, 1, 1);
                    disC = SquareDistance3(Cx, Cy, Cz, 0, 1, 0);
                    minDis = min(min(disM,disW),min(disC,disG));
                    if (double_equals(minDis,disW)) {
                        Vertex[0] = 0; Vertex[1] = 0; Vertex[2] = 0;
                    }else if (double_equals(minDis,disM)) {
                        Vertex[0] = 1; Vertex[1] = 0; Vertex[2] = 0;
                    }else if (double_equals(minDis,disG)) {
                        Vertex[0] = 0; Vertex[1] = 1; Vertex[2] = 1;
                    }else if (double_equals(minDis,disC)){
                        Vertex[0] = 0; Vertex[1] = 1; Vertex[2] = 0;
                    }else{
                        printf("Error\n");
                    }
                    break;
                case 3: // RGMY
                    disR = SquareDistance3(Cx, Cy, Cz, 1, 0, 1);
                    disM = SquareDistance3(Cx, Cy, Cz, 1, 0, 0);
                    disY = SquareDistance3(Cx, Cy, Cz, 0, 0, 1);
                    disG = SquareDistance3(Cx, Cy, Cz, 0, 1, 1);
                    minDis = min(min(disM,disR),min(disG,disY));
                    if (double_equals(minDis,disR)) {
                        Vertex[0] = 1; Vertex[1] = 0; Vertex[2] = 1;
                    }else if (double_equals(minDis,disM)) {
                        Vertex[0] = 1; Vertex[1] = 0; Vertex[2] = 0;
                    }else if (double_equals(minDis,disY)) {
                        Vertex[0] = 0; Vertex[1] = 0; Vertex[2] = 1;
                    }else if (double_equals(minDis,disG)) {
                        Vertex[0] = 0; Vertex[1] = 1; Vertex[2] = 1;
                    }else{
                        printf("Error\n");
                    }
                    break;
                case 4: // KRGB
                    disK = SquareDistance3(Cx, Cy, Cz, 1, 1, 1);
                    disR = SquareDistance3(Cx, Cy, Cz, 1, 0, 1);
                    disG = SquareDistance3(Cx, Cy, Cz, 0, 1, 1);
                    disB = SquareDistance3(Cx, Cy, Cz, 1, 1, 0);
                    minDis = min(min(disK,disR),min(disG,disB));
                    if (double_equals(minDis,disK)) {
                        Vertex[0] = 1; Vertex[1] = 1; Vertex[2] = 1;
                    }else if (double_equals(minDis,disR)) {
                        Vertex[0] = 1; Vertex[1] = 0; Vertex[2] = 1;
                    }else if (double_equals(minDis,disG)) {
                        Vertex[0] = 0; Vertex[1] = 1; Vertex[2] = 1;
                    }else if (double_equals(minDis,disB)) {
                        Vertex[0] = 1; Vertex[1] = 1; Vertex[2] = 0;
                    }else{
                       printf("Error\n");
                    }
                    break;
                case 5: // RGBM
                    disR = SquareDistance3(Cx, Cy, Cz, 1, 0, 1);
                    disG = SquareDistance3(Cx, Cy, Cz, 0, 1, 1);
                    disB = SquareDistance3(Cx, Cy, Cz, 1, 1, 0);
                    disM = SquareDistance3(Cx, Cy, Cz, 1, 0, 0);
                    minDis = min(min(disR,disG),min(disB,disM));
                    if (double_equals(minDis,disR)) {
                        Vertex[0] = 1; Vertex[1] = 0; Vertex[2] = 1;
                    }else if (double_equals(minDis,disG)) {
                        Vertex[0] = 0; Vertex[1] = 1; Vertex[2] = 1;
                    }else if (double_equals(minDis,disB)) {
                        Vertex[0] = 1; Vertex[1] = 1; Vertex[2] = 0;
                    }else if (double_equals(minDis,disM)) {
                        Vertex[0] = 1; Vertex[1] = 0; Vertex[2] = 0;
                    }else{
                        printf("Error\n");
                    }
                    break;
                case 6: // CMGB
                    disC = SquareDistance3(Cx, Cy, Cz, 0, 1, 0);
                    disM = SquareDistance3(Cx, Cy, Cz, 1, 0, 0);
                    disG = SquareDistance3(Cx, Cy, Cz, 0, 1, 1);
                    disB = SquareDistance3(Cx, Cy, Cz, 1, 1, 0);
                    minDis = min(min(disM,disC),min(disG,disB));
                    if (double_equals(minDis,disC)) {
                        Vertex[0] = 0; Vertex[1] = 1; Vertex[2] = 0;
                    }else if (double_equals(minDis,disM)) {
                        Vertex[0] = 1; Vertex[1] = 0; Vertex[2] = 0;
                    }else if (double_equals(minDis,disG)) {
                        Vertex[0] = 0; Vertex[1] = 1; Vertex[2] = 1;
                    }else if (double_equals(minDis,disB)) {
                        Vertex[0] = 1; Vertex[1] = 1; Vertex[2] = 0;
                    }else{
                        printf("Error\n");
                    }
                    break;
                default:
                    break;
            }
            // new quantized error
            for (int color = 0; color < BytesPerPixel; color++) {
                err[x][y][color] = (int)Thresholddata[x][y][color] + err[x][y][color]- BINARY_1 * Vertex[color];
                // serpentine searching
                if (x%2 == 0) {
                    if (y+1 < Size2)
                        Imagedata[x][y+1][color] = clique((int)round((double)err[x][y][color]*7.0/16.0) + (int)Imagedata[x][y+1][color]);
                    if (x+1 < Size1 && y-1 >= 0)
                        Imagedata[x+1][y-1][color] = clique((int)round((double)err[x][y][color]*3.0/16.0) + (int)Imagedata[x+1][y-1][color]);
                    if (x+1 < Size1)
                        Imagedata[x+1][y][color] = clique((int)round((double)err[x][y][color]*5.0/16.0) + (int)Imagedata[x+1][y][color]);
                    if (x+1 < Size1 && y+1 < Size2)
                        Imagedata[x+1][y+1][color] = clique((int)round((double)err[x][y][color]*1.0/16.0) + (int)Imagedata[x+1][y+1][color]);
                }else{
                    y = Size2 - 1 - y;
                    if (y+1 >= 0)
                        Imagedata[x][y+1][color] = clique((int)round((double)err[x][y][color]*7.0/16.0) + (int)Imagedata[x][y+1][color]);
                    if (x+1 < Size1 && y-1 < Size2)
                        Imagedata[x+1][y-1][color] = clique((int)round((double)err[x][y][color]*3.0/16.0) + (int)Imagedata[x+1][y-1][color]);
                    if (x+1 >= 0)
                        Imagedata[x+1][y][color] = clique((int)round((double)err[x][y][color]*5.0/16.0) + (int)Imagedata[x+1][y][color]);
                    if (x-1 >= 0 && y+1 >= 0)
                        Imagedata[x-1][y+1][color] = clique((int)round((double)err[x][y][color]*1.0/16.0) + (int)Imagedata[x-1][y+1][color]);
                }
                Imagedata[x][y][color] = Thresholddata[x][y][color];
            }
        }
    }
    
    // convert back to RGB space
    for (int x = 0; x < Size1; x++)
        for(int y = 0; y < Size2; y++)
            for (int color = 0; color < BytesPerPixel; color++)
                Imagedata[x][y][color] = COLOR_RANGE-1 - Imagedata[x][y][color];
    
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

double SquareDistance3(double a1, double a2, double a3, double b1, double b2, double b3){
    return pow(a1-b1, 2.0)+pow(a2-b2, 2.0)+pow(a3-b3, 2.0);
}

bool double_equals(double a, double b)
{
    double epsilon = 0.00000001;
    return abs(a - b) < epsilon;
}

unsigned char clique(int n)
{
    return n > BINARY_1? BINARY_1:(n < BINARY_0?BINARY_0:(unsigned char)n);
}



