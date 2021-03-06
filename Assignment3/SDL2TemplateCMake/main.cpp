/*
    Majid Joseph
    104558520
    Assignment 3
    February 18, 2019
*/

#include <cstdio> 
#include <iostream>
#include <unistd.h>
#include <vector>
#include <SDL.h>
#include <SDL_ttf.h>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define BLACK 0x000000FF
#define WHITE 0xFFFFFFFF

using namespace std;

// Making my life easier
struct Resources{
	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_Texture *texture;
	SDL_Surface *canvas;
	TTF_Font *font;
};

// I chose to represent the polygon as a vector of Points
struct Point{
	int x, y;
	Point(int x, int y) {this->x = x; this->y = y;}
	Point(){}
};

// Function prototypes
void update(Resources);
void erase(uint32_t (*)[SCREEN_WIDTH], Resources);
int mainMenu();
void drawLine(uint32_t (*)[SCREEN_WIDTH], Point, Point);
void floodFill(uint32_t (*)[SCREEN_WIDTH], int, int);
void end(Resources res);
vector<Point> inputPoly();
void drawPoly(uint32_t (*)[SCREEN_WIDTH], Resources, vector<Point>);
int y_intersect(Point, Point, Point, Point); 
int x_intersect(Point, Point, Point, Point);
void suthHodgClip(vector<Point> &, vector<Point>);
void clip(vector<Point> &, Point, Point);
void liangBarskyHelper(vector<Point>, uint32_t (*)[SCREEN_WIDTH], Resources);
void liangBarsky(float, float, float, float, Point, Point, uint32_t (*)[SCREEN_WIDTH]);
float maxi(float[],int);
float mini(float[], int);
void scanLine(uint32_t (*)[SCREEN_WIDTH], int, int, Resources, vector<Point>);
void translate(vector<Point> &, Point);
int getMinY(vector<Point>const&);
int getMaxY(vector<Point>const&);
int clipChoose();

int main(){

	// VERY IMPORTANT: Ensure SDL2 is initialized
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_EVENTS) < 0){
    	SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "could not initialize sdl2: %s\n", SDL_GetError());
    	return 1;
  	}

    // VERY IMPORTANT: if using text in your program, ensure SDL2_ttf library is
    // initialized
    if (TTF_Init() < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "could not initialize SDL2_ttf: %s\n", TTF_GetError());
    }

    Resources res;

    res.window = SDL_CreateWindow("Floor - SDL2 version", SDL_WINDOWPOS_UNDEFINED,
                                SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH,
                                SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

    if (res.window == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Could not create window!\n");
        return 1;
    }

    res.renderer = SDL_CreateRenderer(res.window, -1,
                                    0); // don't force hardware or software
                                        // accel -- let SDL2 choose what's best

    if (res.renderer == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "could not create renderer: %s\n", SDL_GetError());
        return 1;
    }

    res.canvas = SDL_CreateRGBSurfaceWithFormat(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32,
                                               SDL_PIXELFORMAT_RGBA8888);
    if (res.canvas == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "could not create surface: %s\n",
                 SDL_GetError());
        return 1;
    }

    res.texture =
        SDL_CreateTexture(res.renderer, SDL_PIXELFORMAT_RGBA8888,
                        SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);

    if (res.texture == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "could not create texture: %s\n",
                 SDL_GetError());
        return 1;
    }

    TTF_Font* font = TTF_OpenFont("iosevka-regular.ttf", 64);
	if (!font) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "TTF_OpenFont: %s\n", TTF_GetError());
		return 1;
	}

	SDL_FillRect(res.canvas, NULL, SDL_MapRGB(res.canvas->format, BLACK, BLACK, BLACK));
	uint32_t (*pixels)[SCREEN_WIDTH] = (uint32_t(*)[SCREEN_WIDTH]) res.canvas->pixels;

    // This vector represents the canvas
    // Will be used later for clipping
    vector<Point> clipper; 
    clipper.push_back(Point(0, 0));
    clipper.push_back(Point(0, SCREEN_HEIGHT - 1));
    clipper.push_back(Point(SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1));
    clipper.push_back(Point(SCREEN_WIDTH - 1, 0));

    while(1){
        int choice = mainMenu();

        if(choice == 1){ // Option 1
            break;
        }
        else if(choice == 2){ // Option 2
            vector<Point> poly = inputPoly();
            for(int i = 0; i < 2; i++){
                vector<Point> polyCopy = poly;
                Point p;
                cout << "Enter starting point " << i + 1 << ": ";
                cin  >> p.x >> p.y;
                translate(polyCopy, p);
                int clipChoice = clipChoose();
                if(clipChoice == 1){
                    suthHodgClip(polyCopy, clipper);
                    drawPoly(pixels, res, polyCopy);
                }
                else{
                    // No need for drawPoly(...) because this
                    // function takes care of it
                    liangBarskyHelper(polyCopy, pixels, res);
                }
            }
            cout << "Press anything to continue\n";
            read(0, NULL, 1);
        }
        else{ // Option 3
            Point p;
            vector<Point> poly = inputPoly();
            suthHodgClip(poly, clipper);
            drawPoly(pixels, res, poly);
            cout << "Enter in point inside polygon: ";
            cin >> p.x >> p.y;
            cout << "Demonstrating Recursive flood fill!\n";
            floodFill(pixels, p.x, p.y);
            update(res);
            cout << "Press anything to see scanline algorithm\n";
            read(0, NULL, 1);
            erase(pixels, res);
            drawPoly(pixels, res, poly);
            scanLine(pixels, getMinY(poly), getMaxY(poly), res, poly);
            cout << "Press anything to continue\n";
            read(0, NULL, 1);
        }
        erase(pixels, res);
    }

	end(res);
	SDL_Quit();
	return 0;
}

// Given a point inside the polygon, the function will recursively fill it
// Flood Fill algorithm
void floodFill(uint32_t (*pixels)[SCREEN_WIDTH], int x, int y){

	// Fail safe
	if(x <= 0 || x >= SCREEN_WIDTH || y <= 0 || y >= SCREEN_HEIGHT)
		return;

	if(pixels[y][x] != BLACK){
		pixels[y][x] = BLACK;
		
		// 4 Nearest neighbours is sufficient
		floodFill(pixels, x + 1, y);
		floodFill(pixels, x, y + 1);
		floodFill(pixels, x - 1, y);
		floodFill(pixels, x, y - 1);

        // Problems with 8 nearest neighbours due to not perfect lines
        // being drawn. Algorithm would break out of polygon when one of the
        // corner pixels was not filled in due to the slope of the line
        // Once it is outside of the polygon, it would fill the entire canvas
        // causing it to segment.
	}
}

// Implementation of Scanline algorithm
// Returns minimum Y value of polygon
int getMinY(vector<Point>const& poly){
    int min = poly[0].y;

    for(Point p : poly){
        if(p.y < min){
            min = p.y;
        }
    }

    return min;
}

// Implementation of Scanline algorithm
// Returns maximum Y value of polygon
int getMaxY(vector<Point>const& poly){
    int max = poly[0].y;

    for(Point p: poly){
        if(p.y > max){
            max = p.y;
        }
    }

    return max;
}

// Implementation of Scanline algorithm
void scanLine(uint32_t (*pixels)[SCREEN_WIDTH], int ymin, int ymax, Resources res, vector<Point> poly){


    for(int y = ymin + 1; y < ymax; y++){
        
        // First we need to find all the lines that will intersect the current scanline
        vector<pair<Point, Point>>edges;
        for(int i = 0; i < poly.size(); i++){
            int k = (i + 1) % poly.size();
            Point p1 = poly[i];
            Point p2 = poly[k];
            if(((y >= p1.y && y <= p2.y) || (y <= p1.y && y >= p2.y)) && (p1.y != p2.y)){
                edges.push_back(make_pair(p1,p2));
            }
        }

        // Find the intersection X point of each line
        vector<int>v;
        for(int i = 0; i < edges.size(); i++){
            Point pi = edges[i].first;
            Point pf = edges[i].second;
            double m = (double)(pf.y - pi.y)/(double)(pf.x - pi.x);
            double c = pf.y - (double)m*(double)pf.x;
            int x;
            if(pi.x == pf.x){ // vertical line
                x = pi.x;
            }
            else{
                x = round((y - c)/m); 
            }
            v.push_back(x);
        }

        // Sort the intersections by X
        sort(v.begin(), v.end());
        
        // Removing all duplicates for the case of intersection at a point
        vector<int>dups;
        for(auto i = v.begin(); i != v.end() - 1; ++i){
            if(*i == *(i+1)){
                dups.push_back(*i);
            }
        }
        for(auto i = dups.begin(); i != dups.end(); ++i){
            v.erase(remove(v.begin(), v.end(), *i), v.end());
        }

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
        update(res);
    }

}

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

// Implementation of Sutherland-Hodgman
// Helper function
void suthHodgClip(vector<Point> &poly, vector<Point> clipper){

    //i and k are two consecutive indexes 
    for (int i = 0; i < clipper.size(); i++){ 
        int k = (i + 1) % clipper.size(); 
  
        clip(poly, clipper[i], clipper[k]); 
    } 
} 

// Implementation of Sutherland-Hodgman
// Will determine new points for the polygon
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

void liangBarskyHelper(vector<Point> poly, uint32_t (*pixels)[SCREEN_WIDTH], Resources res){

    for(int i = 0; i < poly.size(); i++){
        int k = (i + 1) % poly.size();
        
        liangBarsky(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, poly[i], poly[k], pixels);
    }

    update(res);

} 

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

// Used to update the canvas
void update(Resources res){
    SDL_UpdateTexture(res.texture, NULL, res.canvas->pixels, res.canvas->pitch);
    SDL_RenderCopy(res.renderer, res.texture, NULL, NULL);
    SDL_RenderPresent(res.renderer);
}

// Used to erase the canvas
void erase(uint32_t (*pixels)[SCREEN_WIDTH], Resources res){
    
    // Paint every pixel on the screen white
    for(int i = 0; i < SCREEN_HEIGHT; i++){
        for(int j = 0; j < SCREEN_WIDTH; j++){
            pixels[i][j] = WHITE;
        }
    }

    update(res);
}   


// Decided to seperate the mainMenu options to a function
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

// Clean up at the end
void end(Resources res){
    SDL_DestroyTexture(res.texture);
    SDL_FreeSurface(res.canvas);
    SDL_DestroyRenderer(res.renderer);
    SDL_DestroyWindow(res.window); 
}

// Since I need to input multiple polygons
vector<Point> inputPoly(){
    
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
    cout << "Input points in clockwise order!\n";
    for(int i = 0; i < length; i++){
        cout << "Point " << i + 1 << ": ";
        cin >> x >> y;
        points.push_back(Point(x, y));
    }

    return points;
}

// Clipping menu
int clipChoose(){

    int choice;

    while(1){
        cout << "1: To clip by Sutherland-Hodgman\n"
             << "2: To clip by Liang-Barsky\n";
        cin >> choice;
        if(choice == 1 || choice == 2) break;
        else cout << "Invalid Entry!\n";
    }

    return choice;
}

// Draws the polygon given the representation of it
void drawPoly(uint32_t (*pixels)[SCREEN_WIDTH], Resources res, vector<Point> poly){

    for(int i = 0; i < poly.size() - 1; i++){
        Point p1 = poly[i];
        Point p2 = poly[i + 1];
        drawLine(pixels, p1, p2);
    }
    drawLine(pixels, poly[0], poly[poly.size() - 1]);

    update(res);
}

// Given two points, it draws a connecting line
// Uses Breesenham's line drawing algorithm
void drawLine(uint32_t (*pixels)[SCREEN_WIDTH], Point p1, Point p2){
    
    int x1, y1, x2, y2;
    x1 = p1.x; y1 = p1.y; x2 = p2.x; y2 = p2.y;

    bool steep = (fabs(y2 - y1) > fabs(x2 - x1));
    if(steep){
        swap(x1, y1);
        swap(x2, y2);
    }

    if(x1 > x2){
        swap(x1, x2);
        swap(y1, y2);
    }

    float dx = x2 - x1;
    float dy = fabs(y2 - y1);

    float error = dx / 2.0f;
    int ystep = (y1 < y2) ? 1 : -1;
    int y = (int)y1;

    int maxX = (int)x2;

    for(int x=(int)x1; x<maxX; x++){
        
        if(steep){
            pixels[x][y] = BLACK;
        }
        else{
            pixels[y][x] = BLACK;
        }

        error -= dy;
        if(error < 0){
            y += ystep;
            error += dx;
        }
    }
}

// Translates a polygon about a point
void translate(vector<Point> &poly, Point p){

    for(Point &p_ : poly){
        p_.x += p.x;
        p_.y += p.y;
    }
}