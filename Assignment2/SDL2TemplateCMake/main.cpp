/*
	Majid Joseph
	104558520
	Assignment 2
	January 25, 2019
*/

// main.cpp : This file contains the 'main' function. Program execution begins and ends there.

//Standard library includes
#include <cstdio> //when you want to include a C standard library header, like stdio.h, use cstdio instead
#include <iostream>
#include <unistd.h>
#include <cmath>

//SDL2 includes
#include <SDL.h>
#include <SDL_ttf.h>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

// Functions I created to assist me
void update(SDL_Texture*, SDL_Surface*, SDL_Renderer*);
void drawCircle(uint32_t (*pixels)[SCREEN_WIDTH], int, int, int, int, int, int);
void erase(uint32_t (*pixels)[SCREEN_WIDTH], SDL_Texture*, SDL_Surface*, SDL_Renderer*);

//The example code given is C code, but feel free to use C++ at any time in your work.

int main(int argc, char* args[]) {

	//VERY IMPORTANT: Ensure SDL2 is initialized
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "could not initialize sdl2: %s\n", SDL_GetError());
		return 1;
	}
	
	//VERY IMPORTANT: if using text in your program, ensure SDL2_ttf library is initialized
	if (TTF_Init() < 0)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "could not initialize SDL2_ttf: %s\n", TTF_GetError());
	}

	//This creates the actual window in which graphics are displayed
	SDL_Window* window = SDL_CreateWindow(
		"hello_sdl2",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		SCREEN_WIDTH, SCREEN_HEIGHT,
		SDL_WINDOW_SHOWN
	);

	//Check for errors: window will be NULL if there was a problem
	if (window == NULL) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "could not create window: %s\n", SDL_GetError());
		return 1;
	}

	//A window by itself can't do many useful things.  We need a renderer so that we can paint in this window.
	//Think of a renderer like a paint brush.
	//First, let's instantiate a renderer in the window using the system's preferred graphics API.
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0); //the "0" lets the SDL2 choose what's best.

	if (renderer == NULL) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "could not create renderer: %s\n", SDL_GetError());
		return 1;
	}

	//OK, now we have the window and the rendering context.
	//Let's create our virtual "canvas" that we'll use to paint on.
	//This will live entirely on the CPU, and is stored in an SDL_Surface.
	//SDL_Surfaces are always CPU-only objects.
	//Note that we are requesting an RGBA8 surface.
	SDL_Surface* canvas = SDL_CreateRGBSurfaceWithFormat(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, SDL_PIXELFORMAT_RGBA8888);
	if (canvas == NULL) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "could not create surface: %s\n", SDL_GetError());
		return 1;
	}

	//Unfortunately, we can't output CPU surfaces directly to the window with the renderer.  We have to copy
	//it over first.  Create a texture on the GPU that will receive our rendered images.  Consider it to be the GPU
	//side of our canvas.
	//The pixel format should be in agreement with the surface given.
	SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING,
		SCREEN_WIDTH, SCREEN_HEIGHT);
	//SDL_TEXTUREACCESS_STREAMING allows the texture to be streamed from the CPU.

	if (texture == NULL)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "could not create texture: %s\n", SDL_GetError());
		return 1;
	}

	// Just in case you need text:
	// load iosevka-regular.ttf at a large size into font
	TTF_Font* font = TTF_OpenFont("iosevka-regular.ttf", 64);
	if (!font) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "TTF_OpenFont: %s\n", TTF_GetError());
		return 1;
	}

	//OK, now we're all set to start rendering.  Let's paint a white background.
	//FillRect essentially fills a rectangle of pixels with a solid colour for us.
	SDL_FillRect(canvas, NULL, SDL_MapRGB(canvas->format, 0xFF, 0xFF, 0xFF));
	uint32_t (*pixels)[SCREEN_WIDTH] = (uint32_t(*)[SCREEN_WIDTH]) canvas->pixels;

	int choice;
	char waitForUser;
	while(1){
		std::cout << "1 : Exit program\n2 : Draw (1-5) Points\n3 : Draw a Line\n4 : Draw a Circle\n" << std::endl;
		scanf("%d", &choice);
		
		if(choice == 1){
			break;
		}
		else if(choice == 2){
			int numberOfPoints;
			while(1){
				puts("How many points would you like to draw (1-5): ");
				std::cin >> numberOfPoints;
				if(numberOfPoints <= 5 && numberOfPoints >= 1){
					break;
				}
			}
			int x, y, colour;
			for(int i = 0; i < numberOfPoints; i++){
				std::cout << "Point " << i + 1 << " (X Y Colour)" << std::endl;
				scanf("%d %d %x", &x, &y, &colour);
				pixels[y][x] = colour;
			}

			update(texture, canvas, renderer);
		}
		else if(choice == 3){
			int x1, y1, x2, y2, colour;
			int m, c;
			std::cout << "Enter End Points (X1 Y1 X2 Y2 Colour): ";
			scanf("%d %d %d %d %x", &x1, &y1, &x2, &y2, &colour);
			
			// Handles the case when x1 is greater than x2
			if(x2 < x1){ 
				std::swap(x2, x1);
				std::swap(y2, y1);
			}

			// Iterate through [x1, x2] and calculate the corresponding y
			m = (y2-y1)/(x2-x1);
			c = y2 - m*x2;
			for(int linex = x1; linex <= x2; linex++){
				int liney = m*linex + c;
				pixels[liney][linex] = colour;
			}
			update(texture, canvas, renderer);

			// Option to rotate or translate
			int choice;
			std::cout << "1 : Translate\n2 : Rotate\n";
			std::cin >> choice;
			
			if(choice == 1){
				// Translating the line
				int xoff, yoff;
				std::cout << "Translate it by (x y): ";
				std::cin >> xoff >> yoff;

				// Add the offsets to the end points
				// Draw the line the same way as before
				x1 += xoff; x2 += xoff;
				y1 += yoff; y2 += yoff;
				if(x2 < x1){ 
					std::swap(x2, x1);
					std::swap(y2, y1);
				}

				m = (y2-y1)/(x2-x1);
				c = y2 - m*x2;
				for(int linex = x1; linex <= x2; linex++){
					int liney = m*linex + c;
					pixels[liney][linex] = 0xFF0000FF; 
				}
				update(texture, canvas, renderer);
			}
			else if(choice == 2){
				printf("Enter an angle in degrees: ");
				int temp123;
				scanf("%d", &temp123);
				float angle = temp123 * (M_PI/180); // make sure angle is in radians
				
				// Expansion of the algorithm from the slides
				int midX = (SCREEN_WIDTH / 2) - 1;
				int midY = (SCREEN_HEIGHT / 2) - 1;
				for(int i = x1; i <= x2; i++){
					int j = m*i + c;

					double xPrime;
					double yPrime;

					xPrime = (cos(angle)*((double) i-midX)) - (sin(angle)*((double) j-midY));
					yPrime = (sin(angle)*((double) i-midX)) + (cos(angle)*((double) j-midY));
 
					int iFinal = (int) std::round(xPrime + midX);
					int jFinal = (int) std::round(yPrime + midY);
					pixels[jFinal][iFinal] = 0xFF0000FF; 
				}

				update(texture, canvas, renderer);
				
			}
			else{
				std::cout << "Invalid Entry\n";
			}
		}
		else if(choice == 4){
			int xc, yc, R, colour;
			std::cout << "Enter circle (X Y R Colour): ";
			scanf("%d %d %d %x", &xc, &yc, &R, &colour);

			// Passing 0,0 to achieve a perfect circle
			drawCircle(pixels, xc, yc, R, 0, 0, colour);
			update(texture, canvas, renderer);

			int choice;
			std::cout << "1 : Translate\n2 : Scale\n";
			std::cin >> choice;

			if(choice == 1){
				int xc_, yc_;
				std::cout << "Translate it by (x y): ";
				scanf("%d %d", &xc_, &yc_);
				drawCircle(pixels, xc+xc_, yc+yc_, R, 0, 0, 0xFF0000FF); // Add the coordinates
				update(texture, canvas, renderer);
			}
			else if(choice == 2){
				int xOff, yOff;
				std::cout << "Enter X and Y amounts you want to offset by: ";
				scanf("%d %d", &xOff, &yOff);
				drawCircle(pixels, xc, yc, R, xOff, yOff, 0xFF0000FF); // Instead of 0 0, provide the actual offsets
				update(texture, canvas, renderer);
			}
			else{
				std::cout << "Invalid Entry\n";
			}
		}
		else{
			std::cout << "Invalid Entry\n";
		}
				
		// When the user is ready, the program will erase the canvas and get ready for next iteration
		std::cout << "Press any character to erase canvas\n";
		fflush(stdin);
		read(0, NULL, 1); 
		erase(pixels, texture, canvas, renderer);
	}
	
	//VERY IMPORTANT: free your resources when you are done with them.
	SDL_DestroyTexture(texture);
	SDL_FreeSurface(canvas);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window); 

	//SDL_Quit() handles program exit, so no "return 0" statement is necessary.
	SDL_Quit();

	return 0; //Include a return statement anyway to make the compiler happy.
}

// Fucntion to update the screen
void update(SDL_Texture* texture, SDL_Surface* canvas, SDL_Renderer* renderer){
	SDL_UpdateTexture(texture, NULL, canvas->pixels, canvas->pitch);
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);
}

// Function to draw an ellippse, also draws a circle when the xOff and yOff are zero
void drawCircle(uint32_t (*pixels)[SCREEN_WIDTH], int xc, int yc, int R, int xOff, int yOff, int colour){

	// Modification of the circle drawing algorithm
	int height = R + yOff; 
	int width = R + xOff;

	for(int y=-height; y<=height; y++){
    	for(int x=-width; x<=width; x++){
        	if(x*x*height*height+y*y*width*width <= height*height*width*width){
        		// So it doesnt go off screen and draw pixels in the incorrect positions
            	if(yc + y <= SCREEN_HEIGHT && yc + y >= 0 && xc+x <= SCREEN_WIDTH && xc+x >= 0){
            		pixels[yc+y][xc+x] = colour;
            	}
        	}
    	}
	}
}

// Function to erase the canvas for the next iteration
void erase(uint32_t (*pixels)[SCREEN_WIDTH], SDL_Texture* texture, SDL_Surface* canvas, SDL_Renderer* renderer){	

	// Paint every pixel on the screen white
	for(int i = 0; i < SCREEN_HEIGHT; i++){
		for(int j = 0; j < SCREEN_WIDTH; j++){
			pixels[i][j] = 0xFFFFFFFF;
		}
	}

	update(texture, canvas, renderer);
}