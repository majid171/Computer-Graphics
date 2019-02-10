/*
	Majid Joseph
	104558520
	Assignment 3
	February 15, 2019
*/

//Standard library includes
#include <cstdio> //when you want to include a C standard library header, like stdio.h, use cstdio instead
#include <iostream>
#include <unistd.h>
#include <vector>

#include <SDL.h>
#include <SDL_ttf.h>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

using namespace std;

struct Point{

	int x, y;

	Point(int x, int y){
		this->x = x;
		this->y = y;
	}

	Point(){}
};

struct Polygon{

	Point center;
	vector <Point> vertices;
	int length;

	Polygon(vector<Point> vertices){
		int minX = 0xFFFF, minY = 0xFFFF, maxX = 0, maxY = 0;
	    this->length = vertices.size();
	    this->vertices = vertices;
	    for (int i = 0; i < this->length; i++) {
	        this->vertices[i] = Point(vertices[i].x, vertices[i].y);
	        if (this->vertices[i].x > maxX) maxX = this->vertices[i].x;
	        if (this->vertices[i].x < minX) minX = this->vertices[i].x;
	        if (this->vertices[i].y > maxY) maxY = this->vertices[i].y;
	        if (this->vertices[i].y < minY) minY = this->vertices[i].y;
	    }
	    this->center.x = minX + ((maxX - minX) / 2);
	    this->center.y = minY + ((maxY - minY) / 2);
	}
};

ostream& operator <<(ostream& os, vector<Point> const& p)
{
  
  for(Point i : p){
  	cout << i.x << "--" << i.y << "\n";
  }

  return os;
}

ostream& operator <<(ostream& os, Point const& p)
{
  
  cout << p.x << "--" << p.y << "\n";

  return os;
}

// Functions I created to assist me
int mainMenu();
void update(SDL_Texture*, SDL_Surface*, SDL_Renderer*);
void erase(uint32_t (*pixels)[SCREEN_WIDTH], SDL_Texture*, SDL_Surface*, SDL_Renderer*);
void floodFill(uint32_t (*pixels)[SCREEN_WIDTH], int, int, int);
void drawcircle(uint32_t (*pixels)[SCREEN_WIDTH], int x0, int y0, int radius);
void drawLine(uint32_t (*pixels)[SCREEN_WIDTH], SDL_Texture* texture, SDL_Surface* canvas, SDL_Renderer* renderer, Point p1, Point p2);
Polygon inputPoly();
void drawPoly(uint32_t (*pixels)[SCREEN_WIDTH], SDL_Texture* texture, SDL_Surface* canvas, SDL_Renderer* renderer, Polygon poly);
void Line(uint32_t (*pixels)[SCREEN_WIDTH], float x1, float y1, float x2, float y2);

int main(int argc, char* args[]) {

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "could not initialize sdl2: %s\n", SDL_GetError());
		return 1;
	}
	
	if (TTF_Init() < 0)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "could not initialize SDL2_ttf: %s\n", TTF_GetError());
	}

	SDL_Window* window = SDL_CreateWindow(
		"hello_sdl2",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		SCREEN_WIDTH, SCREEN_HEIGHT,
		SDL_WINDOW_SHOWN
	);

	if (window == NULL) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "could not create window: %s\n", SDL_GetError());
		return 1;
	}

	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0); //the "0" lets the SDL2 choose what's best.

	if (renderer == NULL) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "could not create renderer: %s\n", SDL_GetError());
		return 1;
	}

	SDL_Surface* canvas = SDL_CreateRGBSurfaceWithFormat(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, SDL_PIXELFORMAT_RGBA8888);
	if (canvas == NULL) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "could not create surface: %s\n", SDL_GetError());
		return 1;
	}

	SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING,
		SCREEN_WIDTH, SCREEN_HEIGHT);

	if (texture == NULL)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "could not create texture: %s\n", SDL_GetError());
		return 1;
	}

	TTF_Font* font = TTF_OpenFont("iosevka-regular.ttf", 64);
	if (!font) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "TTF_OpenFont: %s\n", TTF_GetError());
		return 1;
	}

	SDL_FillRect(canvas, NULL, SDL_MapRGB(canvas->format, 0xFF, 0xFF, 0xFF));
	uint32_t (*pixels)[SCREEN_WIDTH] = (uint32_t(*)[SCREEN_WIDTH]) canvas->pixels;

	// while(1){
	// 	int choice = mainMenu();

	// 	if(choice == 1){
	// 		break;
	// 	}
	// 	else if(choice == 2){ // Clipping algorithms
			
	// 	}
	// 	else{ // Fill algorithms
		
	// 	}

	// 	erase(pixels, texture, canvas, renderer);
	// }

	// Polygon poly = inputPoly();
	// drawPoly(pixels, texture, canvas, renderer, poly);

	// read(STDIN_FILENO, NULL, 1);
	// floodFill(pixels, poly.center.x, poly.center.y, 0x000000FF);
	// update(texture, canvas, renderer);
	
	drawLine(pixels, texture, canvas, renderer, Point(100, 100), Point(120, 400));
	read(STDIN_FILENO, NULL, 1);
	erase(pixels, texture, canvas, renderer);
	Line(pixels, 100,100,120,400);
	update(texture, canvas, renderer);
	read(STDIN_FILENO, NULL, 1);


	
	// Ending the program properly
	SDL_DestroyTexture(texture);
	SDL_FreeSurface(canvas);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window); 
	SDL_Quit();
	return 0; 
}

// Function to display the main menu and accept user input
int mainMenu(){
	int choice = 0;

	while(1){
		cout << "1: Exit Program\n"
		 << "2: Input Polygon to Clip\n"
		 << "3: Input Polygon to Fill\n";

		cin >> choice;
		if(choice < 1 || choice > 3){
			cout << "Invalid Entry!\n";
		}
		else break;
	}
	
	return choice;
}

Polygon inputPoly(){
	
	int length = 0;
	int x, y;
	vector<Point> points;

	while(1){
		cout << "Input number of vertices: ";
		cin >> length;

		if(length > 2){
			break;
		}
		else{
			cout << "Invalid Number of Points!\n";
		}
	}
	
	for(int i = 0; i < length; i++){
		cout << "Point " << i + 1 << ": ";
		cin >> x >> y;
		points.push_back(Point(x, y));
	}

	return Polygon(points);
}

void drawPoly(uint32_t (*pixels)[SCREEN_WIDTH], SDL_Texture* texture, SDL_Surface* canvas, SDL_Renderer* renderer, Polygon poly){

	for(int i = 0; i < poly.length - 1; i++){
		Point p1 = poly.vertices[i];
		Point p2 = poly.vertices[i + 1];
		drawLine(pixels, texture, canvas, renderer, p1, p2);
	}
	drawLine(pixels, texture, canvas, renderer, poly.vertices[0], poly.vertices[poly.length-1]);
}

// Fucntion to update the screen
void update(SDL_Texture* texture, SDL_Surface* canvas, SDL_Renderer* renderer){
	SDL_UpdateTexture(texture, NULL, canvas->pixels, canvas->pitch);
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);
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

void floodFill(uint32_t (*pixels)[SCREEN_WIDTH], int x, int y, int newColour){

	if(pixels[x][y] != 0x000000FF){
		pixels[x][y] = newColour;
		
		// 4 Nearest neighbours is sufficient
		floodFill(pixels, x + 1, y, newColour);
		floodFill(pixels, x, y + 1, newColour);
		floodFill(pixels, x - 1, y, newColour);
		floodFill(pixels, x, y - 1, newColour);
		// floodFill(pixels, x + 1, y + 1, newColour);
		// floodFill(pixels, x + 1, y - 1, newColour);
		// floodFill(pixels, x - 1, y - 1, newColour);
		// floodFill(pixels, x - 1, y + 1, newColour);

	}
}

void drawLine(uint32_t (*pixels)[SCREEN_WIDTH], SDL_Texture* texture, SDL_Surface* canvas, SDL_Renderer* renderer, Point p1, Point p2){
	
	int x1, y1, x2, y2;
	x1 = p1.x; y1 = p1.y; x2 = p2.x; y2 = p2.y;
	int colour = 0x000000FF;
	if(x2 < x1){
		swap(x1, x2);
		swap(y1, y2);
	}
	double m, c;

	if(x1 == x2){
		m = 0;
		c = y2;
		if(y2 < y1){
			swap(x1, x2);
			swap(y1, y2);
		}
		for(int i = y1; i <= y2; i++){
			pixels[i][x1] = colour;
		}	
	}
	else{
		m = ((double)y2-(double)y1)/((double)x2-(double)x1);
		c = y2 - m*(double)x2;
		for(int linex = x1; linex <= x2; linex++){
			int liney = m*linex + c;
			pixels[liney][linex] = colour;
		}
	}

	update(texture, canvas, renderer);
}

void Line(uint32_t (*pixels)[SCREEN_WIDTH], float x1, float y1, float x2, float y2){
    
    // Bresenham's line algorithm
    const bool steep = (fabs(y2 - y1) > fabs(x2 - x1));
    if(steep)
    {
        swap(x1, y1);
        swap(x2, y2);
    }

    if(x1 > x2)
    {
        swap(x1, x2);
        swap(y1, y2);
    }

    const float dx = x2 - x1;
    const float dy = fabs(y2 - y1);

    float error = dx / 2.0f;
    const int ystep = (y1 < y2) ? 1 : -1;
    int y = (int)y1;

    const int maxX = (int)x2;

    for(int x=(int)x1; x<maxX; x++)
    {
        if(steep)
        {
            pixels[x][y] = 0x000000FF;
        }
        else
        {
            pixels[y][x] = 0x000000FF ;
        }

        error -= dy;
        if(error < 0)
        {
            y += ystep;
            error += dx;
        }
    }
}
