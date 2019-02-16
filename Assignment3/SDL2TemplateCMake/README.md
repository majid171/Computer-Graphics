# Assignment 3 - February 18, 2019

To download the solution, please clone my repository and ensure that you have the correct libraries required. Find those libraries and instructions [here](https://github.com/InBetweenNames/SDL2TemplateCMake).
# Disclaimer
This program works best for polygons with *friendly* slopes. Will still work for other polygons, however you might run into trouble if the slopes are very *unfriendly* and you try to use the filling algorithms, more notably, the flood fill. An explanation is provided later.
# My Approach

- Made a struct called Resources to house all the SDL2 stuff
 - Made a struct called Point that houses an X and Y
 - Chose to represent a polygon as a vector of Points
 - Implemented Bresenham's algorithm to take care of drawing the lines for the polygon. 
	 - Crucial for the filling algorithms

# Interacting with the program

Once you successfully build and run the program...

 - You will be greeted with an interactive program
	 - 1: Exit
	 - 2: Clipping
	 - 3: Filling
- Clipping
	- Will be asked to input a polygon
	- Specify about which point to draw it at
	- Choose a clipping algorithm
	- Do the first three steps twice total
- Filling
	- Input a polygon, automatically clipped for you
	- Polygon will be drawn
	- Will be asked to input a point inside the polygon for the floodfill algorithm 
	- floodFill will successfully fill the polygon
	- Screen will be erased
	- Polygon redrawn
	- scanLine will refill the polygon

## Helper Functions
The functions that cleaned up the program, **not necessary** for any of the algorithms required for the assignment. When looking at the code, they will be near the bottom.
```c++
void update(Resources); // Updates canvas
void erase(uint32_t (*)[SCREEN_WIDTH], Resources); // Erases the canvas
int mainMenu(); // Displays interactive menu and returns user choice
void end(Resources res); // Cleans up at the end
int clipChoose(); // Menu for clipping options and returns choice
vector<Point> inputPoly(); // Inputs a polygon from user
void drawLine(uint32_t (*)[SCREEN_WIDTH], Point, Point); // Draws a line between two points
void drawPoly(uint32_t (*)[SCREEN_WIDTH], Resources, vector<Point>); // Draws the polygon
void translate(vector<Point> &, Point); // Translates a polygon about a point
```
Feel free to look at the functions more in depth in the main.cpp file.

# Flood Fill Algorithm

When implementing this algorithm, I had problems with eight nearest neighbours due to not perfect lines being drawn. Algorithm would break out of polygon when one of the corner pixels was not filled in due to the slope of the line once it is outside of the polygon, it would fill the entire canvas causing it to segment. Therefor four nearest neighbours is sufficient. There are still some cases where choppy lines are drawn due to the slope chosen, causing the algorithm to segment still.
```c++
//...
floodFill(pixels, x + 1, y); // Right
floodFill(pixels, x, y + 1); // Bottom
floodFill(pixels, x - 1, y); // Left
floodFill(pixels, x, y - 1); // Top
//...
```

# Scan Line Fill Algorithm
I implemented two helper functions so I can retrieve the minimum and maximum Y value of the polygon.

```c++
int getMinY(vector<Point>const&);
int getMaxY(vector<Point>const&);
```

 - Loop through each Y value (each Scan Line)
	 - Find all edges of the polygon such that the current Y value will eventually intersect it
	 - Find the intersection point (X value) of each edge from the previous step
	 - Sort intersections by X value
	 - Remove duplicates to handle the case of intersection at a point
	 - Fill between the intersection points

The filling part of my implementation...

```c++
// ...
// Fill while inside polygon
int k = 0;
int pos = v[k]; // Current intersection point we are looking for
int x = 0;
bool isFilling = false;
while(x < SCREEN_WIDTH){
	if(x == pos){
	    isFilling = !isFilling;
        pos = v[++k]; // Update the intersection
    }

    if(isFilling){
	    pixels[y][x] = BLACK;
    }
    else{
	    pixels[y][x] = WHITE;
    }
    x++;
}
// ...
```

# Sutherland-Hodgman Algorithm

I implemented a helper function to call the algorithm for each edge of the clipper window.

```c++
void suthHodgClip(vector<Point> &poly, vector<Point> clipper){

    //i and k are two consecutive indexes 
    for (int i = 0; i < clipper.size(); i++){ 
        int k = (i + 1) % clipper.size(); 
  
        clip(poly, clipper[i], clipper[k]); 
    } 
} 
```
**Clipper is defined in main as a vector of points. The points represent the canvas. You can change clipper to be a smaller box to see the effects.**

I also made two functions to determine the X and Y intersection of a point between two lines.
```c++
// Implementation of Sutherland-Hodgman
// Returns the x-value of point of interection of two lines
int x_intersect(Point p1, Point p2, Point p3, Point p4){ 
    int num = (p1.x*p2.y - p1.y*p2.x) * (p3.x-p4.x) - 
              (p1.x-p2.x) * (p3.x*p4.y - p3.y*p4.x); 
    int den = (p1.x-p2.x) * (p3.y-p4.y) - (p1.y-p2.y) * (p3.x-p4.x); 
    return num/den; 
} 

// Implementation of Sutherland-Hodgman
// Returns y-value of point of intersectipn of two lines 
int y_intersect(Point p1, Point p2, Point p3, Point p4){ 
    int num = (p1.x*p2.y - p1.y*p2.x) * (p3.y-p4.y) - 
              (p1.y-p2.y) * (p3.x*p4.y - p3.y*p4.x); 
    int den = (p1.x-p2.x) * (p3.y-p4.y) - (p1.y-p2.y) * (p3.x-p4.x); 
    return num/den; 
}
```

The actual clipping part of it.
```c++
void clip(vector<Point> &poly, Point p1, Point p2){ 
    
  	vector<Point> newPoly;
    
    int x1, x2, y1, y2;
    x1 = p1.x; y1 = p1.y; x2 = p2.x; y2 = p2.y;

    // (ix,iy),(kx,ky) are the values of the points 
    for (int i = 0; i < poly.size(); i++){ 
        int k = (i+1) % poly.size(); 
  		
        // Two consecutive points
        int ix = poly[i].x, iy = poly[i].y;
  		int kx = poly[k].x, ky = poly[k].y;

        // Calculating position of first point 
        // w.r.t. clipper line 
        int i_pos = (x2-x1) * (iy-y1) - (y2-y1) * (ix-x1); 
  
        // Calculating position of second point 
        // w.r.t. clipper line 
        int k_pos = (x2-x1) * (ky-y1) - (y2-y1) * (kx-x1); 
  
        // Case 1 : When both points are inside 
        if (i_pos < 0  && k_pos < 0){ 
            newPoly.push_back(Point(kx, ky));
        } 
  
        // Case 2: When only first point is outside 
        else if (i_pos >= 0  && k_pos < 0){ 
            Point temp;
            temp.x = x_intersect(Point(x1,y1), Point(x2,y2), Point(ix,iy), Point(kx,ky));                   
            temp.y = y_intersect(Point(x1,y1), Point(x2,y2), Point(ix,iy), Point(kx,ky));
  			newPoly.push_back(temp);
            newPoly.push_back(Point(kx, ky));
        } 
  
        // Case 3: When only second point is outside 
        else if (i_pos < 0  && k_pos >= 0){
            Point temp;
            temp.x = x_intersect(Point(x1,y1), Point(x2,y2), Point(ix,iy), Point(kx,ky));
            temp.y = y_intersect(Point(x1,y1), Point(x2,y2), Point(ix,iy), Point(kx,ky));
            newPoly.push_back(temp);
        } 
  
        // Case 4: When both points are outside, do nothing
    } 

    // The new polygon after being clipped
  	poly = newPoly;
}
```

# Liang-Barsky Algorithm
Created two helper functions to return the minimum and maximum elements of an array.

```c++
// Returns max of an array
float maxi(float arr[], int n) {
    float m = 0;
    for (int i = 0; i < n; ++i){
        if (m < arr[i]){
            m = arr[i];
        }
    }

    return m;
}

// Returns min of an array
float mini(float arr[], int n){
    float m = 1;
    for(int i = 0; i < n; ++i){
        if (m > arr[i]){
            m = arr[i];
        }
    }

    return m;
}
```

Helper function to call the algorithm for each edge of the polygon.
```c++
void liangBarskyHelper(vector<Point> poly, uint32_t (*pixels)[SCREEN_WIDTH], Resources res){

    for(int i = 0; i < poly.size(); i++){
        int k = (i + 1) % poly.size();
        
        liangBarsky(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, poly[i], poly[k], pixels);
    }

    update(res);
} 
```
To change the clipping window, change the first 4 arguements to a smaller box. 
Note:
 ```liangBarsky(minX, minY, maxX, maxY, poly[i], poly[k], pixels);```

Then finally the algorithm...
```c++
void liangBarsky(float xmin, float ymin, float xmax, float ymax, Point pi, Point pf, uint32_t (*pixels)[SCREEN_WIDTH]){
  
    float x1, y1, x2, y2;
    x1 = pi.x; y1 = pi.y; x2 = pf.x; y2 = pf.y;

    //defining variables
    float p1 = -(x2 - x1);
    float p2 = -p1;
    float p3 = -(y2 - y1);
    float p4 = -p3;

    float q1 = x1 - xmin;
    float q2 = xmax - x1;
    float q3 = y1 - ymin;
    float q4 = ymax - y1;

    float posarr[5], negarr[5];
    int posind = 1, negind = 1;
    posarr[0] = 1;
    negarr[0] = 0;

    if ((p1 == 0 && q1 < 0) || (p3 == 0 && q3 < 0)) {
        //Line is parallel to clipping window!
        return;
    }
    if (p1 != 0) {
        float r1 = q1 / p1;
        float r2 = q2 / p2;
        
        if (p1 < 0) {
            negarr[negind++] = r1; // for negative p1, add it to negative array
            posarr[posind++] = r2; // and add p2 to positive array
        }else{
            negarr[negind++] = r2;
            posarr[posind++] = r1;
        }
    }

    if (p3 != 0) {
        float r3 = q3 / p3;
        float r4 = q4 / p4;
        
        if (p3 < 0){
            negarr[negind++] = r3;
            posarr[posind++] = r4;
        }else{
            negarr[negind++] = r4;
            posarr[posind++] = r3;
        }
    }

    float xn1, yn1, xn2, yn2;
    float rn1, rn2;
    rn1 = maxi(negarr, negind); // maximum of negative array
    rn2 = mini(posarr, posind); // minimum of positive array

    if(rn1 > rn2){ // reject
        return;
    }

    // Computing the new points
    xn1 = x1 + p2 * rn1;
    yn1 = y1 + p4 * rn1; 
    xn2 = x1 + p2 * rn2;
    yn2 = y1 + p4 * rn2;

    // Directly drawing the line made by the 2 (new) points
    drawLine(pixels, Point(xn1, yn1), Point(xn2, yn2));
}
```
As you can see, each edge that is passed in gets drawn inside the function. Therefor there is no need to call ```drawPoly(...)``` after its done.

# Sample Run
Sample run showing the clipping option.
```
1: Exit Program
2: Input Polygon to Clip
3: Input Polygon to Fill
2
Input number of vertices: 4
Input points in clockwise order!
Point 1: 100 100
Point 2: 200 100
Point 3: 200 200
Point 4: 100 200
Enter starting point 1: -100 -100
1: To clip by Sutherland-Hodgman
2: To clip by Liang-Barsky
1
Enter starting point 2: 0 0
1: To clip by Sutherland-Hodgman
2: To clip by Liang-Barsky
2
Press anything to continue
1: Exit Program
2: Input Polygon to Clip
3: Input Polygon to Fill
1
```

Sample run showing the filling option.
```
1: Exit Program
2: Input Polygon to Clip
3: Input Polygon to Fill
3
Input number of vertices: 3
Input points in clockwise order!
Point 1: 100 150
Point 2: 200 250
Point 3: 300 200
Enter in point inside polygon: 250 200
Demonstrating Recursive flood fill!
Press anything to see scanline algorithm
Press anything to continue
1: Exit Program
2: Input Polygon to Clip
3: Input Polygon to Fill
1
```

# Conclusion
If you have any further questions please feel free to contact me.
