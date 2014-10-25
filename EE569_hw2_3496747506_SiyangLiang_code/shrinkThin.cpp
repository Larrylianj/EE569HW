#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <math.h>
#define COLOR_RANGE 256
#define BINARY_1 0 // black
#define BINARY_0 255 // white
#define S_1 {0x01,0x04,0x10,0x40}
#define S_2 {0x80,0x02,0x08,0x20}
#define S_3 {0x81,0x03,0x06,0x0c,0x18,0x30,0x60,0xc0}
#define TK_4 {0x82,0x0a,0x28,0xa0}
#define STK_4 {0xc1,0x07,0x1c,0x70}
#define ST_5 {0x86,0xc2,0x0b,0xa1,0x83,0x0e,0x38,0xe0}
#define ST_6 {0xc6,0x1b}
#define STK_6 {0x87,0xc3,0x0f,0x1e,0x3c,0x78,0xf0,0xe1}
#define STK_7 {0xc7,0x1f,0x7c,0xf1}
#define STK_8 {0xe3,0x8f,0x3e,0xf8}
#define STK_9 {0xe7,0xf3,0x9f,0xcf,0x3f,0x7e,0xfc,0xf9}
#define STK_10 {0xf7,0xdf,0x7f,0xfd}
#define K_11 {0xef,0xbf,0xfe,0xfb}

#define ST_UN_MARK {0x01,0x04,0x20,0x80,0x81,0x03,0x06,0x0c,0x18,0x30,0x60,0xc0,0x0b,0x86,0xc2,0xa1}
//#define ST_UN_MARK {0x01,0x04,0x20,0x80,0x81,0x03,0x06,0x0c,0x18,0x30,0x60,0xc0,0x0b,0x86,0xc2,0xa1}

#define MAX_LOOP 15

using namespace std;

bool AcontainsB(unsigned char A[], int A_length, unsigned char B);
bool SpurCornerCluster(unsigned char string);
bool CornerCluster(unsigned char string);
bool TeeBranch(unsigned char string);
bool VeeBranch(unsigned char string);
bool DiagonalBranch(unsigned char string);

int main(int argc, char *argv[])

{
    
	// Define file pointer and variables
	FILE *file;
	int BytesPerPixel;
	int Size1 = 388;
    int Size2 = 374;
	
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
    unsigned char TempImagedata[Size1][Size2][BytesPerPixel];
    unsigned char OutputImagedata[Size1][Size2][BytesPerPixel];
    unsigned char MaskImagedata[Size1][Size2][BytesPerPixel];
    
	// Read image (filename specified by first argument) into image data matrix
	if (!(file=fopen(argv[1],"rb"))) {
		cout << "Cannot open file: " << argv[1] <<endl;
		exit(1);
	}
	fread(Imagedata, sizeof(unsigned char), Size1*Size2*BytesPerPixel, file);
	fclose(file);
    
	///////////////////////// INSERT YOUR PROCESSING CODE HERE /////////////////////////
    // initialize mark patterns
    unsigned char S1[4] = S_1;
    unsigned char S2[4] = S_2;
    unsigned char S3[8] = S_3;
    unsigned char TK4[4] = TK_4;
    unsigned char STK4[4] = STK_4;
    unsigned char ST5[8] = ST_5;
    unsigned char ST6[2] = ST_6;
    unsigned char STK6[8] = STK_6;
    unsigned char STK7[4] = STK_7;
    unsigned char STK8[4] = STK_8;
    unsigned char STK9[8] = STK_9;
    unsigned char STK10[4] = STK_10;
    unsigned char K11[4] = K_11;
    unsigned char ST_UNMARK[16] = ST_UN_MARK;
    
//    for (int i = 0; i < Size1; i ++)
//        for(int j = 0; j < Size2; j++)
//            for(int color = 0; color < BytesPerPixel; color++)
//                TempImagedata[i][j][color] = Imagedata[i][j][color];
    //Imagedata[i][j][color] = Imagedata[i][j][color] > 127?255:0;
    
    // Shrink Loop here
    bool converge = false;
    int counter = 0;
    while (!converge) {
        counter++;
        converge = true;
        
        // Stage I
        for (int i = 0; i < Size1; i ++) {
            for(int j = 0; j < Size2; j++) {
                for(int color = 0; color < BytesPerPixel; color++) {
                    int bond = 0;
                    unsigned char string = 0x00;
                    unsigned char hit = 0;
                    if (i > 0 && i < Size1-1 && j > 0 && j < Size2 -1) {
                        if (Imagedata[i][j][color] == BINARY_1) {
                            // set the 8-bit string
                            string |= Imagedata[i][j+1][color] == BINARY_1? 0x01:0x00;
                            string <<= 1;
                            string |= Imagedata[i+1][j+1][color] == BINARY_1? 0x01:0x00;
                            string <<= 1;
                            string |= Imagedata[i+1][j][color] == BINARY_1? 0x01:0x00;
                            string <<= 1;
                            string |= Imagedata[i+1][j-1][color] == BINARY_1? 0x01:0x00;
                            string <<= 1;
                            string |= Imagedata[i][j-1][color] == BINARY_1? 0x01:0x00;
                            string <<= 1;
                            string |= Imagedata[i-1][j-1][color] == BINARY_1? 0x01:0x00;
                            string <<= 1;
                            string |= Imagedata[i-1][j][color] == BINARY_1? 0x01:0x00;
                            string <<= 1;
                            string |= Imagedata[i-1][j+1][color] == BINARY_1? 0x01:0x00;
                            //                            if (string != 0xff)
                            //                                printf("string1 = %x\n",string);
                            // set bond
                            bond += (Imagedata[i+1][j+1][color] == BINARY_1?1:0);
                            bond += (Imagedata[i+1][j-1][color] == BINARY_1?1:0);
                            bond += (Imagedata[i-1][j+1][color] == BINARY_1?1:0);
                            bond += (Imagedata[i-1][j-1][color] == BINARY_1?1:0);
                            bond += 2 * (Imagedata[i+1][j][color] == BINARY_1?1:0);
                            bond += 2 * (Imagedata[i][j+1][color] == BINARY_1?1:0);
                            bond += 2 * (Imagedata[i-1][j][color] == BINARY_1?1:0);
                            bond += 2 * (Imagedata[i][j-1][color] == BINARY_1?1:0);
                            //                            if (bond != 12) {
                            //                                printf("bond = %d\n",bond);
                            //                            }
                            // determine hit mask
                            switch (bond) {
                                case 1:
                                    hit = AcontainsB(S1, 4, string)?1:0;
                                    break;
                                case 2:
                                    hit = AcontainsB(S2, 4, string)?1:0;
                                    break;
                                case 3:
                                    hit = AcontainsB(S3, 8, string)?1:0;
                                    break;
                                case 4:
                                    hit = AcontainsB(STK4, 4, string)?1:0;
                                    break;
                                case 5:
                                    hit = AcontainsB(ST5, 8, string)?1:0;
                                    break;
                                case 6:
                                    hit = (AcontainsB(ST6, 2, string) || AcontainsB(STK6, 8, string))?1:0;
                                    break;
                                case 7:
                                    hit = AcontainsB(STK7, 4, string)?1:0;
                                    break;
                                case 8:
                                    hit = AcontainsB(STK8, 4, string)?1:0;
                                    break;
                                case 9:
                                    hit = AcontainsB(STK9, 8, string)?1:0;
                                    break;
                                case 10:
                                    hit = AcontainsB(STK10, 4, string)?1:0;
                                    break;
//                                case 12:
//                                    hit = 1;
//                                    break;
                                default:
                                    break;
                            }
                        }
                    }
                    MaskImagedata[i][j][color] = hit;
                }
            }
        }
        
        //        for (int i = 0; i < Size1; i ++) {
        //            for(int j = 0; j < Size2; j++) {
        //                if (MaskImagedata[i][j][0] != 0) {
        //                    printf("[%d][%d] ",i,j);
        //                }
        //            }
        //            printf("\n");
        //        }
        
        // Stage II
        for (int i = 0; i < Size1; i ++) {
            for(int j = 0; j < Size2; j++) {
                for(int color = 0; color < BytesPerPixel; color++){
                    unsigned char string = 0x00;
                    unsigned char hit = 0;
                    if (i > 0 && i < Size1-1 && j > 0 && j < Size2 -1) {
                        if (MaskImagedata[i][j][color] == 1) {
                            // set the 8 bit string
                            string |= MaskImagedata[i][j+1][color] == 1? 0x01:0x00;
                            string <<= 1;
                            string |= MaskImagedata[i+1][j+1][color] == 1? 0x01:0x00;
                            string <<= 1;
                            string |= MaskImagedata[i+1][j][color] == 1? 0x01:0x00;
                            string <<= 1;
                            string |= MaskImagedata[i+1][j-1][color] == 1? 0x01:0x00;
                            string <<= 1;
                            string |= MaskImagedata[i][j-1][color] == 1? 0x01:0x00;
                            string <<= 1;
                            string |= MaskImagedata[i-1][j-1][color] == 1? 0x01:0x00;
                            string <<= 1;
                            string |= MaskImagedata[i-1][j][color] == 1? 0x01:0x00;
                            string <<= 1;
                            string |= MaskImagedata[i-1][j+1][color] == 1? 0x01:0x00;
                            //printf("[%d][%d]'s string = %x\n",i,j,string);
                            // deteremine hit
                            if (AcontainsB(ST_UNMARK, 16, string) || SpurCornerCluster(string) || CornerCluster(string) || TeeBranch(string) || VeeBranch(string) || DiagonalBranch(string)) {
                                hit = 1;
                            }
                        }
                    }
                    if (hit) {
                        converge = false;
                    }
                    OutputImagedata[i][j][color] = hit ==1?BINARY_0:Imagedata[i][j][color];
                }
            }
        }
        
        // Substract the boundaries
        for (int i = 0; i < Size1; i ++)
            for(int j = 0; j < Size2; j++)
                for(int color = 0; color < BytesPerPixel; color++)
                    Imagedata[i][j][color] = OutputImagedata[i][j][color];
        
    }
    printf("counter = %d\n",counter);
    
	// Write image data (filename specified by second argument) from image data matrix
	if (!(file=fopen(argv[2],"wb"))) {
		cout << "Cannot open file: " << argv[2] << endl;
		exit(1);
	}
	fwrite(Imagedata, sizeof(unsigned char), Size1*Size2*BytesPerPixel, file);
	fclose(file);
    
	return 0;
}

bool AcontainsB(unsigned char A[], int A_length, unsigned char B){
    for (int i = 0; i < A_length; i++) {
        if (A[i] == B) {
            return true;
        }
    }
    return false;
}

bool SpurCornerCluster(unsigned char string){
    bool exit = false;
    if ((string & 0x11) == 0x11 && (string & 0x82) != 0x00 && (string & 0x6c) == 0x00) {
        exit = true;
    }else if ((string & 0x44) == 0x44 && (string & 0x0a) != 0x00 && (string & 0xb1) == 0x00) {
        exit = true;
    }else if ((string & 0x11) == 0x11 && (string & 0x28) != 0x00 && (string & 0xc6) == 0x00) {
        exit = true;
    }else if ((string & 0x44) == 0x44 && (string & 0xa0) != 0x00 && (string & 0x1b) == 0x00) {
        exit = true;
    }
    //    if (exit) {
    //        printf("SpurCorner\n");
    //    }
    return exit;
}

bool CornerCluster(unsigned char string){
    return (string & 0x0e) == 0x0e? true:false;
}

bool TeeBranch(unsigned char string){
    bool exit = false;
    if ((string & 0x8a) == 0x8a && (string & 0x61) == 0x00) {
        exit = true;
    }else if((string & 0x8a) == 0x8a && (string & 0x34) == 0x00){
        exit = true;
    }else if((string & 0xa8) == 0xa8 && (string & 0x16) == 0x00){
        exit = true;
    }else if((string & 0xa8) == 0xa8 && (string & 0x43) == 0x00){
        exit = true;
        
    }else if((string & 0x2a) == 0x2a && (string & 0xd0) == 0x00){
        exit = true;
    }else if((string & 0x2a) == 0x2a && (string & 0x85) == 0x00){
        exit = true;
    }else if((string & 0xa2) == 0xa2 && (string & 0x0d) == 0x00){
        exit = true;
    }else if((string & 0x2a) == 0x2a && (string & 0x58) == 0x00){
        exit = true;
    }
    //    if (exit) {
    //        printf("TeeBranch\n");
    //    }
    return exit;
}

bool VeeBranch(unsigned char string){
    bool exit = false;
    if ((string & 0x05) == 0x05 && (string & 0x70) != 0x00) {
        exit = true;
    }else if((string & 0x14) == 0x14 && (string & 0xc1) != 0x00){
        exit = true;
    }else if((string & 0x50) == 0x50 && (string & 0x07) != 0x00){
        exit = true;
    }else if((string & 0x41) == 0x41 && (string & 0x1c) != 0x00){
        exit = true;
    }
    //    if (exit) {
    //        printf("VeeBranch\n");
    //    }
    return exit;
}

bool DiagonalBranch(unsigned char string){
    bool exit = false;
    if ((string & 0x92) == 0x92 && (string & 0x29) == 0x00) {
        exit = true;
    }else if((string & 0x4a) == 0x4a && (string & 0xa4) == 0x00){
        exit = true;
    }else if((string & 0x29) == 0x29 && (string & 0x92) == 0x00){
        exit = true;
    }else if((string & 0xa4) == 0xa4 && (string & 0x4a) == 0x00){
        exit = true;
    }
    //    if (exit) {
    //        printf("DiagonalBranch\n");
    //    }
    
    return exit;
}

