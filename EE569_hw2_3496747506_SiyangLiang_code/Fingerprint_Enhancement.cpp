#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <list>
#define COLOR_RANGE 256
#define GX {{-1,0,1},{-2,0,2},{-1,0,1}}
#define GY {{1,2,1},{0,0,0},{-1,-2,-1}}
#define GAUSSIAN {{1.0/256,4.0/256,6.0/256,4.0/256,1.0/256},{4.0/256,16.0/256,24.0/256,16.0/256,4.0/256},{6.0/256,24.0/256,36.0/256,24.0/256,6.0/256},{4.0/256,16.0/256,24.0/256,16.0/256,4.0/256},{1.0/256,4.0/256,6.0/256,4.0/256,1.0/256}}
#define BLOCK_SIZE 16
#define NEIGHBOR_SIZE 7
#define LOWPASS_FILTER_SIZE 5
#define FREQUENCY_LENGTH 32
#define INTERPOLATION_FILTER_SIZE 7
#define GABOR_FILTER_SIZE 11
#define VARIANCE 9
#define STD_X 4.0
#define STD_Y 4.0
#define BINARY_0 255
#define BINARY_1 0
#define LOOP 3
#define THRESHOLD_CONSIST 1.5

using namespace std;

unsigned char roundToColor(double n);
double absAngle(double r);
double period(double x[], int len);
double Ux(double x);
double Cx(double x);
double validF(double x);
double gabor(int x,int y, double s, double f);
unsigned char clique(double x);

unsigned char threshold = 127;
unsigned char desired_mean = 100;
double desired_var = 200.0;

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
    int X_gradient[Size1][Size2][BytesPerPixel];
    int Y_gradient[Size1][Size2][BytesPerPixel];

    double X_continuousVector[Size1][Size2][BytesPerPixel];
    double Y_continuousVector[Size1][Size2][BytesPerPixel];
    unsigned char OutputImagedata[Size1][Size2][BytesPerPixel];
    
	// Read image (filename specified by first argument) into image data matrix
	if (!(file=fopen(argv[1],"rb"))) {
		cout << "Cannot open file: " << argv[1] <<endl;
		exit(1);
	}
	fread(Imagedata, sizeof(unsigned char), Size1*Size2*BytesPerPixel, file);
	fclose(file);
    
	///////////////////////// INSERT YOUR PROCESSING CODE HERE /////////////////////////
    // 0.Pre Work
    // generate 2D gaussian kernel for fuure use
    double gaussianKernel[INTERPOLATION_FILTER_SIZE][INTERPOLATION_FILTER_SIZE];
    for (int i = -INTERPOLATION_FILTER_SIZE/2; i < INTERPOLATION_FILTER_SIZE/2+1; i++) {
        for (int j = -INTERPOLATION_FILTER_SIZE/2; j < INTERPOLATION_FILTER_SIZE/2+1; j++) {
            gaussianKernel[i+INTERPOLATION_FILTER_SIZE/2][j+INTERPOLATION_FILTER_SIZE/2] = 1/(2*M_PI*VARIANCE)*exp(-(pow(i, 2)+pow(j, 2))/(2*VARIANCE));
            //printf("%f ",gaussianKernel[i+INTERPOLATION_FILTER_SIZE/2][j+INTERPOLATION_FILTER_SIZE/2]);
        }
        //printf("\n");
    }
    
    // 1.Normalization
    long long sum_mean = 0;
    double sum_var = 0;
    unsigned char mean;
    double var;
    for (int x = 0; x < Size1; x++)
        for (int y = 0; y < Size2; y++)
            sum_mean += Imagedata[x][y][0];
    mean = roundToColor((double)sum_mean/(double)(Size1*Size2));
    for (int x = 0; x < Size1; x++)
        for(int y = 0; y < Size2; y++)
            sum_var += pow((double)((int)Imagedata[x][y][0] - (int)mean), 2.0);
    var = sum_var/(double)(Size1*Size2);
    for (int x = 0; x < Size1; x++)
        for(int y = 0; y < Size2; y++)
            Imagedata[x][y][0] = Imagedata[x][y][0] > mean? (desired_mean + roundToColor(sqrt(desired_var*pow((double)(Imagedata[x][y][0] - mean), 2.0)/var))) : (desired_mean - roundToColor(sqrt(desired_var*pow((double)(Imagedata[x][y][0] - mean), 2.0)/var)));

    // 2.Orientation Image
    // divide the image int blocks
    int gx[3][3] = GX;
    int gy[3][3] = GY;
    double lowPass[LOWPASS_FILTER_SIZE][LOWPASS_FILTER_SIZE] = GAUSSIAN;
    int rows = Size1/BLOCK_SIZE;
    int cols = Size2/BLOCK_SIZE;
    double X_vector[rows][cols][BytesPerPixel];
    double Y_vector[rows][cols][BytesPerPixel];
    double angle[rows][cols][BytesPerPixel];
    double orientation[rows][cols][BytesPerPixel];
    double frequency[rows][cols][BytesPerPixel];
    unsigned char outputImageBlock[rows][cols][BytesPerPixel];
    
    // get the gradient of each pixel
    for (int color = 0; color < BytesPerPixel; color++) {
        for (int x = 0; x < Size1; x++){
            for (int y = 0; y < Size2; y++){
                
                int sumX = 0,sumY = 0;
                for (int i = -1; i < 2; i++) {
                    for (int j = -1; j < 2; j++) {
                        if (x+i >= 0 && x+i < Size1 && y+j >= 0 && y+j < Size2) {
                            sumX += gx[i+1][j+1] * (int)Imagedata[x+i][y+j][color];
                            sumY += gy[i+1][j+1] * (int)Imagedata[x+i][y+j][color];
                        }
                    }
                }
                X_gradient[x][y][color] = sumX;
                Y_gradient[x][y][color] = sumY;
                //printf("x = %d y=%d\n",sumX,sumY);
            }
        }
    }

    // get the angle and estimated vector field for each block
    for (int x = BLOCK_SIZE/2; x < rows*BLOCK_SIZE; x+= BLOCK_SIZE) {
        for (int y = BLOCK_SIZE/2; y < cols*BLOCK_SIZE; y+= BLOCK_SIZE) {
            for (int color = 0; color < BytesPerPixel; color++) {
                
                double sumXV = 0.0, sumYV = 0.0;
                for (int i = x-BLOCK_SIZE/2; i < x+BLOCK_SIZE/2+1; i++) {
                    for (int j = y-BLOCK_SIZE/2; j < y+BLOCK_SIZE/2+1; j++) {
                        if (i >= 0 && i < Size1 && j >= 0 && j < Size2) {
                            sumXV += 2*(double)X_gradient[i][j][color]*(double)Y_gradient[i][j][color];
                            sumYV += pow((double)X_gradient[i][j][color], 2.0)-pow((double)Y_gradient[i][j][color], 2.0);
                        }
                    }
                }
                //printf("x = %f y=%f\n",sumXV,sumYV);
                angle[(x - BLOCK_SIZE)/BLOCK_SIZE][(y - BLOCK_SIZE)/BLOCK_SIZE][color] = absAngle(0.5*atan2(sumYV, sumXV));
                //printf("angle = %f\n",angle[(x - BLOCK_SIZE)/BLOCK_SIZE][(y - BLOCK_SIZE)/BLOCK_SIZE][color]);
                X_vector[(x - BLOCK_SIZE)/BLOCK_SIZE][(y - BLOCK_SIZE)/BLOCK_SIZE][color] = cos(2*angle[(x - BLOCK_SIZE)/BLOCK_SIZE][(y - BLOCK_SIZE)/BLOCK_SIZE][color]);
                Y_vector[(x - BLOCK_SIZE)/BLOCK_SIZE][(y - BLOCK_SIZE)/BLOCK_SIZE][color] = sin(2*angle[(x - BLOCK_SIZE)/BLOCK_SIZE][(y - BLOCK_SIZE)/BLOCK_SIZE][color]);
                //printf("X_vector = %f Y_vector = %f\n",X_vector[(x - BLOCK_SIZE)/BLOCK_SIZE][(y - BLOCK_SIZE)/BLOCK_SIZE][color],Y_vector[(x - BLOCK_SIZE)/BLOCK_SIZE][(y - BLOCK_SIZE)/BLOCK_SIZE][color]);
            }
        }
    }
    
    // apply low-pass filter blockwise
    for (int x = 0; x < rows; x++) {
        for (int y = 0; y < cols; y++) {
            for (int color = 0; color < BytesPerPixel; color++) {
                
                double sumX = 0.0,sumY = 0.0;
                for (int i = -LOWPASS_FILTER_SIZE/2; i < (LOWPASS_FILTER_SIZE+1)/2; i++) {
                    for (int j = -LOWPASS_FILTER_SIZE/2; j < (LOWPASS_FILTER_SIZE+1)/2; j++) {
                        if (x + i >= 0 && x + i < rows && y + j >= 0 && y + j < cols) {
                            sumX += lowPass[i+LOWPASS_FILTER_SIZE/2][j+LOWPASS_FILTER_SIZE/2]*X_vector[x + i][y + j][color];
                            sumY += lowPass[i+LOWPASS_FILTER_SIZE/2][j+LOWPASS_FILTER_SIZE/2]*Y_vector[x + i][y + j][color];
                        }
                    }
                }
                //printf("x_v = %f y_v = %f\n",sumX,sumY);
                orientation[x][y][color] = 0.5*atan2(sumY, sumX);
                
            }
        }
    }
    
//    for (int x = 0; x < rows; x++) {
//        for (int y = 0; y < cols; y++) {
//            printf("%f ",orientation[x][y][0]);
//        }
//        printf("\n");
//    }
    
    // 3.Ridge Frequency Image
    double frequencyTemp[rows][cols][BytesPerPixel];
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            for (int color = 0; color < BytesPerPixel; color++) {
                
                double X_signature[FREQUENCY_LENGTH];
                for (int k = 0; k < FREQUENCY_LENGTH; k++) {
                    int sum = 0;
                    for (int d = 0; d < BLOCK_SIZE; d++) {
                        double u = i*BLOCK_SIZE+BLOCK_SIZE/2 + (d-BLOCK_SIZE/2)*cos(orientation[i][j][color])+(k-0.5)*sin(orientation[i][j][color]);
                        double v = j*BLOCK_SIZE+BLOCK_SIZE/2 + (d-BLOCK_SIZE/2)*sin(orientation[i][j][color])+(0.5-k)*cos(orientation[i][j][color]);
//                        printf("u = %f, v = %f\n",u,v);
//                        printf("rouned, u = %f, v = %f\n",round(u),round(v));
                        sum += (int)Imagedata[min(max(abs((int)round(u)),0),Size1-1)][min(max(abs((int)round(v)),0),Size2-1)][color];
                    }
                    X_signature[k] = (double)sum/(double)BLOCK_SIZE;
                }
                frequencyTemp[i][j][color] = validF(1.0/period(X_signature, FREQUENCY_LENGTH));
                
                //printf("frequency = %f\n",frequencyTemp[i][j][color]);
            }
        }
    }
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            for (int color = 0; color < BytesPerPixel; color++) {
                if (frequencyTemp[i][j][color] == -1.0) {
                    // apply interpolation filter
                    double sumUp = 0.0;
                    double sumDown = 0.0;
                    for (int u = -INTERPOLATION_FILTER_SIZE/2; u < (INTERPOLATION_FILTER_SIZE+1)/2; u++) {
                        for (int v = -INTERPOLATION_FILTER_SIZE/2; v < (INTERPOLATION_FILTER_SIZE+1)/2; v++) {
                            sumUp += gaussianKernel[u+INTERPOLATION_FILTER_SIZE/2][v+INTERPOLATION_FILTER_SIZE/2]*Ux(frequencyTemp[i+u][j+v][color]);
                            sumDown += gaussianKernel[u+INTERPOLATION_FILTER_SIZE/2][v+INTERPOLATION_FILTER_SIZE/2]*Cx(frequencyTemp[i+u][j+u][color]+1);
                        }
                    }
                    frequency[i][j][color] = validF(sumUp/sumDown);
                }else{
                    frequency[i][j][color] = frequencyTemp[i][j][color];
                }
                //printf("frequency = %f\n",frequency[i][j][color]);
            }
        }
    }
    
    // 4.Region Mask
    
    // 5.Image Filtering
    for (int i = 0; i < Size1; i++) {
        for (int j = 0; j < Size2; j++) {
            for (int color = 0; color < BytesPerPixel; color++) {
                
                double sum = 0;
                for (int u = -GABOR_FILTER_SIZE/2; u < (GABOR_FILTER_SIZE+1)/2; u++) {
                    for (int v = -GABOR_FILTER_SIZE/2; v < (GABOR_FILTER_SIZE+1)/2; v++) {
                        if (i+u >= 0 && i+u < Size1 && j+v >= 0 && j+v < Size2) {
                            sum += gabor(u, v, orientation[i/BLOCK_SIZE][j/BLOCK_SIZE][color], frequency[i/BLOCK_SIZE][j/BLOCK_SIZE][color]) * (int)Imagedata[i+u][j+v][color];
                        }
                    }
                }
                OutputImagedata[i][j][color] = sum > threshold?BINARY_0:BINARY_1;
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

unsigned char roundToColor(double n){
    if (n >= 255.0) {
        return 255;
    }else if (n <= 0.0){
        return 0;
    }else{
        return (unsigned char)round(n);
    }
}

double absAngle(double r){
    r = r + 2*M_PI > 2*M_PI? r:r+M_PI;
    r = r < M_PI? r:r-M_PI;
    return r;
}

// get the average period
double period(double x[], int len){
    double pre = x[0];
    std::list<int> peaks;
    int sum = 0,counter = 0;
    int curPeak,prePeak;
    for (int i = 1; i < len-1 ; i++) {
        if (x[i] > pre && x[i] > x[i+1]) {
            peaks.push_back(i);
        }
        pre = x[i];
    }
    if (peaks.empty()) {
        return -1.0;
    }
    curPeak = peaks.front();
    prePeak = curPeak;
    peaks.pop_front();
    while (!peaks.empty()) {
        curPeak = peaks.front();
        peaks.pop_front();
        sum += curPeak-prePeak;
        counter++;
        prePeak = curPeak;
    }
    if (sum == 0)
        return -1;
    else
        return (double)sum/(double)counter;
}

double Ux(double x){
    return x <= 0?0:x;
}

double Cx(double x){
    return x <= 0?0:1;
}

double gabor(int x,int y, double s, double f){
    double xs = x * cos(s) + y * sin(s);
    double ys = -x * sin(s) + y * cos(s);
    return exp(-0.5*(pow(xs, 2)/pow(STD_X, 2)+pow(ys, 2)/pow(STD_Y, 2)))*cos(2*M_PI*f*xs);
}

unsigned char clique(double x){
    return x > BINARY_0?BINARY_0:(x < BINARY_1?BINARY_1:(unsigned char)round(x));
}

double validF(double x){
    return x < 0?0:(x > 2.0?2.0:x);
}

