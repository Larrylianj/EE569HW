#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <unordered_set>
#include <queue>
#define COLOR_RANGE 256
#define BINARY_1 0 // black
#define BINARY_0 255 // white

using namespace std;


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
    bool visited[Size1][Size2][BytesPerPixel];
    
	// Read image (filename specified by first argument) into image data matrix
	if (!(file=fopen(argv[1],"rb"))) {
		cout << "Cannot open file: " << argv[1] <<endl;
		exit(1);
	}
	fread(Imagedata, sizeof(unsigned char), Size1*Size2*BytesPerPixel, file);
	fclose(file);
    
	///////////////////////// INSERT YOUR PROCESSING CODE HERE /////////////////////////
    queue<pair<int, int>> q;
    queue<int> ridge_length;
    char skip = ',';
    
    // fill in the visited
    for (int x = 0; x < Size1; x++) {
        for (int y = 0; y < Size2; y++) {
            for (int color = 0; color < BytesPerPixel; color++) {
                //printf("%d ",Imagedata[x][y][color]);
                Imagedata[x][y][color] = Imagedata[x][y][color] >= 127?BINARY_0:BINARY_1;
                //printf("%d ",Imagedata[x][y][color] == BINARY_1);
                visited[x][y][color] = false;
            }
        }
    }
    
    for (int x = 0; x < Size1; x++) {
        for (int y = 0; y < Size2; y++) {
            for (int color = 0; color < BytesPerPixel; color++) {
                if (Imagedata[x][y][color] == BINARY_1 && !visited[x][y][color]) {
                    pair<int, int> newPoint(x,y);
                    int length = 0;
                    visited[x][y][color] = true;
                    q.push(newPoint);
                    while (!q.empty()) {
                        length ++;
                        pair<int, int> curPoint = q.front();
                        q.pop();
                        // check the eight direction
                        if (curPoint.first+1 < Size1) {
                            if (Imagedata[curPoint.first+1][curPoint.second][color] == BINARY_1 && !visited[curPoint.first+1][curPoint.second][color]) {
                                visited[curPoint.first+1][curPoint.second][color] = true;
                                pair<int, int> p(curPoint.first+1,curPoint.second);
                                q.push(p);
                            }
                        }
                        if (curPoint.second+1 < Size2) {
                            if (Imagedata[curPoint.first][curPoint.second+1][color] == BINARY_1 && !visited[curPoint.first][curPoint.second+1][color]) {
                                visited[curPoint.first][curPoint.second+1][color] = true;
                                pair<int, int> p(curPoint.first,curPoint.second+1);
                                q.push(p);
                            }
                        }
                        if (curPoint.first-1 >= 0) {
                            if (Imagedata[curPoint.first-1][curPoint.second][color] == BINARY_1 && !visited[curPoint.first-1][curPoint.second][color]) {
                                visited[curPoint.first-1][curPoint.second][color] = true;
                                pair<int, int> p(curPoint.first-1,curPoint.second);
                                q.push(p);
                            }
                        }
                        if (curPoint.second-1 >= 0) {
                            if (Imagedata[curPoint.first][curPoint.second-1][color] == BINARY_1 && !visited[curPoint.first][curPoint.second-1][color]) {
                                visited[curPoint.first][curPoint.second-1][color] = true;
                                pair<int, int> p(curPoint.first,curPoint.second-1);
                                q.push(p);
                            }
                        }
                        if (curPoint.first+1 < Size1 && curPoint.second+1 < Size2) {
                            if (Imagedata[curPoint.first+1][curPoint.second+1][color] == BINARY_1 && !visited[curPoint.first+1][curPoint.second+1][color]) {
                                visited[curPoint.first+1][curPoint.second+1][color] = true;
                                pair<int, int> p(curPoint.first+1,curPoint.second+1);
                                q.push(p);
                            }
                        }
                        if (curPoint.first+1 < Size1 && curPoint.second-1 >= 0) {
                            if (Imagedata[curPoint.first+1][curPoint.second-1][color] == BINARY_1 && !visited[curPoint.first+1][curPoint.second-1][color]) {
                                visited[curPoint.first+1][curPoint.second-1][color] = true;
                                pair<int, int> p(curPoint.first+1,curPoint.second-1);
                                q.push(p);
                            }
                        }
                        if (curPoint.first-1 >= 0 && curPoint.second-1 >= 0) {
                            if (Imagedata[curPoint.first-1][curPoint.second-1][color] == BINARY_1 && !visited[curPoint.first-1][curPoint.second-1][color]) {
                                visited[curPoint.first-1][curPoint.second-1][color] = true;
                                pair<int, int> p(curPoint.first-1,curPoint.second-1);
                                q.push(p);
                            }
                        }
                        if (curPoint.first-1 >= 0 && curPoint.second+1 < Size2) {
                            if (Imagedata[curPoint.first-1][curPoint.second+1][color] == BINARY_1 && !visited[curPoint.first-1][curPoint.second+1][color]) {
                                visited[curPoint.first-1][curPoint.second+1][color] = true;
                                pair<int, int> p(curPoint.first-1,curPoint.second+1);
                                q.push(p);
                            }
                        }
                    }
                    //printf("%d\n",length);
                    ridge_length.push(length);
                }
            }
            
        }
    }
    
    // Write length data (filename specified by second argument)
	if (!(file=fopen(argv[2],"wb"))) {
		cout << "Cannot open file: " << argv[2] << endl;
		exit(1);
	}
    while (!ridge_length.empty()) {
        int len = ridge_length.front();
        string s = to_string(len);
        ridge_length.pop();
        fwrite(&s, sizeof(char), sizeof(s)/sizeof(char)-1, file);
        fwrite(&skip, sizeof(char), 1, file);
    }
	
	fclose(file);
    
	return 0;
}

