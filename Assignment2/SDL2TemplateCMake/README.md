# Assignment 2 - January 25, 2019

To download the solution, please clone my repository and ensure that you have the correct libraries required. Find those libraries and instructions [here](https://github.com/InBetweenNames/SDL2TemplateCMake).


# Interacting with the program

Once you successfully build and run the assignment, you will be greeted with an interactive menu. You have choices to exit, draw points, draw a line, and draw a circle. Each of those options may have sub options such as choosing number of points to draw, translating, rotating, and scaling. After each iteration of the menu, the canvas will be erased and you will be able to choose a new option.

## Fucntions
I created two helper functions. One being to update the canvas, and the other to erase it.

Both can be found here.

```c++
void update(SDL_Texture* texture, SDL_Surface* canvas, SDL_Renderer* renderer){
	SDL_UpdateTexture(texture, NULL, canvas->pixels, canvas->pitch);
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);
}
```

```c++
void erase(uint32_t (*pixels)[SCREEN_WIDTH], SDL_Texture* texture, SDL_Surface* canvas, SDL_Renderer* renderer){	
	
	// Paint every pixel on the screen white
	for(int i = 0; i < SCREEN_HEIGHT; i++){
		for(int j = 0; j < SCREEN_WIDTH; j++){
			pixels[i][j] = 0xFFFFFFFF;
		}
	}

	update(texture, canvas, renderer);
}
```

## Explaining the Code
I will explain the code for each action you can perform with the program. **Please note when you are asked to enter a colour, you must enter it in hexadecimal format. Example: 0x000000FF**

**Draw Points**

The user will immediately be asked to pick a number of points between [1-5]. If the user enters an incorrect number of points, they will be forced to enter a new number until a valid input is entered. After the person enters a valid input, the program will loop and ask the user to enter the coordinates and colour for each point and then draw it.
```c++
for(int i = 0; i < numberOfPoints; i++){
	std::cout << "Point " << i + 1 << " (X Y Colour)" << std::endl;
	scanf("%d %d %x", &x, &y, &colour);
	pixels[y][x] = colour;
}
```
<br/>
<br/>

**Draw a Line**

The user will be asked to enter the two end points as well as the colour of the line. Based on the algorithm we were given to draw a line, some edge cases arise. The algorithm will break if the second end point's x value is smaller than the first's. To fix this, I check for this and swap the lines when necessary. The algorithm can be found below.

```c++
int m = (y2-y1)/(x2-x1);
int c = y2 - m*x2;
for(int linex = x1; linex <= x2; linex++){
	int liney = m*linex + c;
	pixels[liney][linex] = colour;
}
```
 After the line is drawn, the user will be asked whether they want to rotate the line about the origin, or to translate it. The translating part is easy, you simply add the  x offset and the y offset to each end point. The rotation is a little more difficult. Since our line is not necessarily at the origin, we must translate it there first, then rotate it, and then finally translate it back to its original position. The code can be found below.

```c++
int midX = (SCREEN_WIDTH / 2) - 1;
int midY = (SCREEN_HEIGHT / 2) - 1;
for(int i = x1; i <= x2; i++){
	int j = m*i + c;

	double xPrime;
	double yPrime;

	xPrime = (cos(angle)*((double) i-midX)) - (sin(angle)*((double) j-midY));
	yPrime = (sin(angle)*((double) i-midX)) + (cos(angle)*((double) j-midY));
 
	i = (int) std::round(xPrime + midX);
	j = (int) std::round(yPrime + midY);
	pixels[j][i] = 0xFF0000FF; 
}
```
<br/>
<br/>

**Draw a Circle**

The user will be asked for the centre coordinates, a radius, and a colour for the circle. A perfect circle will be drawn. After the user will be given two options, either to scale the circle or to translate. The translation aspect is easy, you simply add the x and y offset. The more difficult part is scaling the circle. It can either grow uniform in all directions, or it can grow more in one direction than the other. This will result in a non perfect circle or an ellipse. I modified my algorithm to draw ellipses instead of circles. Since circles are a subset of ellipses, I just specify that the x and y offset is zero. The code can be found below.

```c++
void drawCircle(uint32_t (*pixels)[SCREEN_WIDTH], int xc, int yc, int R, int xOff, int yOff, int colour){

	// Modification of the circle drawing algorithm
	int height = R + yOff; 
	int width = R + xOff;

	for(int y=-height; y<=height; y++){
	    	for(int x=-width; x<=width; x++){
		        if(x*x*height*height+y*y*width*width <= height*height*width*width){
		            pixels[yc+y][xc+y] = colour;
        	}
    	}
	}
}

```
<br/>

# Conclusion
If you have any further questions please feel free to contact me.
