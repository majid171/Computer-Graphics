/*
	Majid Joseph
	Assignment1
	Jan 18, 2019
*/

#include <stdio.h>

struct Pixel{

	// Unsigned char is 8 bits.
	// So, all togethor is 3 bytes
	unsigned char red, green, blue;

};
typedef struct Pixel Pixel;

int main(){

	int M, N;
	Pixel DISPLAY[M][N];

	/*
		For an arbitrary pixel at (X,Y),
		the number of pixels from (0,0) to 
		(X, Y) is X*M + y. Since each pixel is 3 bytes,
		the offset would be 3(X*M + Y) 
	*/

	return 0;

}