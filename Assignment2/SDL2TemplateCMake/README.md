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

	for(int i = 0; i < SCREEN_HEIGHT; i++){
		for(int j = 0; j < SCREEN_WIDTH; j++){
			pixels[i][j] = 0xFFFFFFFF;
		}
	}

	update(texture, canvas, renderer);
}
```

## Explaining the Code
I will explain the code for each action you can perform with the program.

**Draw Points**

The user will immediately be asked to pick a number of points between [1-5]. If the user enters an incorrect number of points, they will be forced to enter a new number until a valid input is entered. After the person enters a valid input, the program will loop and ask the user to enter the coordinates and colour for each point and then draw it.
```c++
for(int i = 0; i < numberOfPoints; i++){
	std::cout << "Point " << i + 1 << " (X Y Colour)" <<std::endl;
	std::cin >> x >> y >> colour;
	pixels[x][y] = 0x000000FF;
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
 
	int iFinal = (int) std::round(xPrime + midX);
	int jFinal = (int) std::round(yPrime + midY);
	pixels[jFinal][iFinal] = 0xFF0000FF; 
}
```
<br/>
<br/>

**Draw a Circle**

The user will be asked for the centre coordinates, a radius, and a colour for the circle.
