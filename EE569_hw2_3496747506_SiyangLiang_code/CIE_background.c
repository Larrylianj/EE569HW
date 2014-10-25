#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include "colorspace.h"
#define COLOR_RANGE 256
#define Lab_ab_MAX 110
#define Lab_ab_MIN -110
#define Lab_L_MAX 100
#define Lab_L_MIN 0
#define L_RANGE 100
#define WINDOW_SIZE 7
#define SIGMA_C 3
#define SIGMA_S 10
#define LOOP_TIME 7


double gaussian_dis2(int sigma1, int sigma2, int x1, int x2);
double gaussian_dis3(double sigma1, double sigma2, double sigma3, double x1, double x2, double x3);
double max2(double a, double b);
double min2(double a, double b);

int main(int argc, char *argv[])

{
    
	// Define file pointer and variables
	FILE *file;
	int BytesPerPixel;
	int Size1 = 300;
    int Size2 = 400;
	
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
    double CIELabData[Size1][Size2][BytesPerPixel];
    double OutputImagedata[Size1][Size2][BytesPerPixel];
    double sum_k,sum_h;
    
	// Read image (filename specified by first argument) into image data matrix
	if (!(file=fopen(argv[1],"rb"))) {
		printf("Cannot open file: ");
		exit(1);
	}
	fread(Imagedata, sizeof(unsigned char), Size1*Size2*BytesPerPixel, file);
	fclose(file);
    
	///////////////////////// INSERT YOUR PROCESSING CODE HERE /////////////////////////
    
    // Convert from RGB to CIELAB
    for (int x = 0; x < Size1; x++)
        for (int y = 0; y < Size2; y++){
//            printf("[0] = %d [1] = %d [2] = %d\n",Imagedata[x][y][0],Imagedata[x][y][1],Imagedata[x][y][2]);
            num S[3] = {Imagedata[x][y][0]/(double)(COLOR_RANGE-1), Imagedata[x][y][1]/(double)(COLOR_RANGE-1), Imagedata[x][y][2]/(double)(COLOR_RANGE-1)};
            num D[3];
            colortransform Trans;
            
            if(!(GetColorTransform(&Trans, "RGB -> Lab")))
            {
                printf("Invalid syntax or unknown color space\n");
                exit(0);
            }
            
            ApplyColorTransform(Trans, &D[0], &D[1], &D[2], S[0], S[1], S[2]);
//            CIELabData[x][y][0] = D[0];
//            CIELabData[x][y][1] = D[1];
//            CIELabData[x][y][2] = D[2];

            CIELabData[x][y][0] = max2(min2(D[0], Lab_L_MAX),Lab_L_MIN);
            CIELabData[x][y][1] = max2(min2(D[1], Lab_ab_MAX),Lab_ab_MIN);
            CIELabData[x][y][2] = max2(min2(D[2], Lab_ab_MAX),Lab_ab_MIN);
//            printf("[0] = %f [1] = %f [2] = %f\n",CIELabData[x][y][0],CIELabData[x][y][1],CIELabData[x][y][2]);
//            printf("\n");
        }
    
    // apply Bilateral Filter several times
    for (int loop = 0; loop < LOOP_TIME; loop++) {
        for (int x = 0; x < Size1; x++){
            for (int y = 0; y < Size2; y++){
                for (int color = 0; color < BytesPerPixel; color++) {
                    sum_k = 0.0;
                    sum_h = 0.0;
                    for (int i = x - WINDOW_SIZE/2; i < x + WINDOW_SIZE/2; i++) {
                        for (int j = y - WINDOW_SIZE/2; j < y + WINDOW_SIZE/2; j++) {
                            if (i >= 0 && i < Size1 && j >= 0 && j < Size2) {
                                sum_k += gaussian_dis2(i, j, x, y)*gaussian_dis3(CIELabData[i][j][0], CIELabData[i][j][1], CIELabData[i][j][2], CIELabData[x][y][0],CIELabData[x][y][1], CIELabData[x][y][2]);
                                sum_h += CIELabData[i][j][color]*gaussian_dis2(i, j, x, y)*gaussian_dis3(CIELabData[i][j][0], CIELabData[i][j][1], CIELabData[i][j][2], CIELabData[x][y][0],CIELabData[x][y][1], CIELabData[x][y][2]);
                            }
                        }
                    }
                    OutputImagedata[x][y][color] = sum_h/sum_k;
                }
            }
        }
        for (int x = 0; x < Size1; x++)
            for (int y = 0; y < Size2; y++)
                for (int color = 0; color < BytesPerPixel; color++)
                    CIELabData[x][y][color] = OutputImagedata[x][y][color];
        
    }
    
    // Convert from CIELAB to RGB
    for (int x = 0; x < Size1; x++)
        for (int y = 0; y < Size2; y++){
            num S[3] = {CIELabData[x][y][0], CIELabData[x][y][1], CIELabData[x][y][2]};
            num D[3];
            colortransform Trans;
            
            if(!(GetColorTransform(&Trans, "Lab -> RGB")))
            {
                printf("Invalid syntax or unknown color space\n");
                exit(0);
            }
            
            ApplyColorTransform(Trans, &D[0], &D[1], &D[2], S[0], S[1], S[2]);
            Imagedata[x][y][0] = (unsigned char)round(D[0]*(COLOR_RANGE-1));
            Imagedata[x][y][1] = (unsigned char)round(D[1]*(COLOR_RANGE-1));
            Imagedata[x][y][2] = (unsigned char)round(D[2]*(COLOR_RANGE-1));
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

double gaussian_dis2(int sigma1, int sigma2, int x1, int x2)
{
    //printf("dis2 = %f\n",exp(-(pow((double)(sigma1-x1),2.0)+pow((double)(sigma2-x2), 2.0))/(2*pow((double)SIGMA_C, 2))));
    return exp(-(pow((double)(sigma1-x1),2.0)+pow((double)(sigma2-x2), 2.0))/(2*pow((double)SIGMA_C, 2)));
}

double gaussian_dis3(double sigma1, double sigma2, double sigma3, double x1, double x2, double x3)
{
    //printf("dis1 = %f\n",exp(-(pow((double)(sigma-x),2.0))/(2*pow((double)SIGMA_S, 2))));
    return exp(-(pow((sigma1-x1),2.0)+pow((sigma2-x2), 2.0)+pow((sigma3-x3), 2.0))/(2*pow(SIGMA_S, 2)));
}

double max2(double a, double b)
{
    return a>b?a:b;
}

double min2(double a, double b)
{
    return a>b?b:a;
}



