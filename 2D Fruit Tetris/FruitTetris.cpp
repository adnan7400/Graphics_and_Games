/*
CMPT 361 Assignment 1 - FruitTetris implementation Sample Skeleton Code

- This is ONLY a skeleton code showing:
How to use multiple buffers to store different objects
An efficient scheme to represent the grids and blocks

- Compile and Run:
Type make in terminal, then type ./FruitTetris

This code is extracted from Connor MacLeod's (crmacleo@sfu.ca) assignment submission
by Rui Ma (ruim@sfu.ca) on 2014-03-04. 

Modified in Sep 2014 by Honghua Li (honghual@sfu.ca).
*/

#include "include/Angel.h"
#include <cstdlib>
#include <iostream>
#include <random>
#include <cstring>

#include <unistd.h>

using namespace std;


// xsize and ysize represent the window size - updated if window is reshaped to prevent stretching of the game
int xsize = 400; 
int ysize = 720;

// current tile
vec2 tile[4]; // An array of 4 2d vectors representing displacement from a 'center' piece of the tile, on the grid
vec2 tilepos = vec2(5, 19); // The position of the current tile using grid coordinates ((0,0) is the bottom left corner)

// An array storing all possible orientations of all possible tiles
// The 'tile' array will always be some element [i][j] of this array (an array of vec2)
vec2 allRotationsLshape[4][4] = 
	{{vec2(0,0), vec2(-1,0), vec2(1, 0), vec2(-1,-1)},
	{vec2(0,0), vec2(0,-1), vec2(0,1), vec2(1, -1)},     
	{vec2(0,0), vec2(1,0), vec2(-1, 0), vec2(1,  1)},  
	{vec2(0,0), vec2(0,1), vec2(0, -1), vec2(-1, 1)}};

vec2 allRotationsTshape[4][4] =
	{{vec2(0,0), vec2(-1,0), vec2(1,0), vec2(0,-1)},
	{vec2(0,0), vec2(0,-1), vec2(0,1), vec2(1,0)},
	{vec2(0,0), vec2(1,0), vec2(-1,0), vec2(0,1)},
	{vec2(0,0), vec2(0,1), vec2(0,-1), vec2(-1,0)}};

vec2 allRotationsSshape[4][4] =
	{{vec2(0,-1), vec2(1,0), vec2(0,0), vec2(-1,-1)},
	{vec2(1,0), vec2(0,1), vec2(0,0), vec2(1,-1)},
	{vec2(0,0), vec2(-1,-1), vec2(0,-1), vec2(1,0)},
	{vec2(0,0), vec2(1,-1), vec2(1,0), vec2(0,1)}};

vec2 allRotationsIshape[4][4] =
	{{vec2(0,0), vec2(-1,0), vec2(1,0), vec2(-2,0)},
	{vec2(0,0), vec2(0,-1), vec2(0,1), vec2(0,-2)},
	{vec2(-1,0), vec2(0,0), vec2(-2,0), vec2(1,0)},
	{vec2(0,-1), vec2(0,0), vec2(0,-2), vec2(0,1)}};

vec2 shapes[4][4][4] =
	{allRotationsLshape,
	allRotationsTshape,
	allRotationsSshape,
	allRotationsIshape};

enum orientation
{
	DOWN,
	RIGHT,
	UP,
	LEFT
};

enum tiletypes
{
	L,
	T,
	S,
	I
};

enum colors
{
	GREEN,
	BLUE,
	RED,
	ORANGE,
	YELLOW
};


int current_tile_type;
int current_tile_orientation;

// colors 
vec4 green = vec4(0.0, 1.0, 0.0, 1.0); 
vec4 blue = vec4(0.0, 0.0, 1.0, 1.0); 
vec4 red = vec4(1.0, 0.0, 0.0, 1.0); 
vec4 orange = vec4(1.0, 0.5, 0.0, 1.0);
vec4 yellow = vec4(1.0, 1.0, 0.0, 1.0); 
vec4 white  = vec4(1.0, 1.0, 1.0, 1.0);
vec4 black  = vec4(0.0, 0.0, 0.0, 1.0);

vec4 colors[5] = {green, blue, red, orange, yellow}; 
 
//board[x][y] represents whether the cell (x,y) is occupied
bool board[10][20];

int board_ColorCells[10][20]; //store data about what colors are being used 

//An array containing the colour of each of the 10*20*2*3 vertices that make up the board
//Initially, all will be set to black. As tiles are placed, sets of 6 vertices (2 triangles; 1 square)
//will be set to the appropriate colour in this array before updating the corresponding VBO
vec4 boardcolours[1200];
vec4 newcolours[24];

// location of vertex attributes in the shader program
GLuint vPosition;
GLuint vColor;

// locations of uniform variables in shader program
GLuint locxsize;
GLuint locysize;

// VAO and VBO
GLuint vaoIDs[3]; // One VAO for each object: the grid, the board, the current piece
GLuint vboIDs[6]; // Two Vertex Buffer Objects for each VAO (specifying vertex positions and colours, respectively)

bool movetile(vec2 direction);

//-------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------
int random(int bound){
	random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<> dist(0,bound);
	int result = dist(gen);
	return result;
}
// When the current tile is moved or rotated (or created), update the VBO containing its vertex position data
void updatetile()
{
	// Bind the VBO containing current tile vertex positions
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[4]); 

	// For each of the 4 'cells' of the tile,
	for (int i = 0; i < 4; i++) 
	{
		// Calculate the grid coordinates of the cell
		GLfloat x = tilepos.x + tile[i].x; 
		GLfloat y = tilepos.y + tile[i].y;

		// Create the 4 corners of the square - these vertices are using location in pixels
		// These vertices are later converted by the vertex shader
		vec4 p1 = vec4(33.0 + (x * 33.0), 33.0 + (y * 33.0), .4, 1); 
		vec4 p2 = vec4(33.0 + (x * 33.0), 66.0 + (y * 33.0), .4, 1);
		vec4 p3 = vec4(66.0 + (x * 33.0), 33.0 + (y * 33.0), .4, 1);
		vec4 p4 = vec4(66.0 + (x * 33.0), 66.0 + (y * 33.0), .4, 1);

		// Two points are used by two triangles each
		vec4 newpoints[6] = {p1, p2, p3, p2, p3, p4}; 

		// Put new data in the VBO
		glBufferSubData(GL_ARRAY_BUFFER, i*6*sizeof(vec4), 6*sizeof(vec4), newpoints); 
	}

	glBindVertexArray(0);
}

//-------------------------------------------------------------------------------------------------------------------

// Called at the start of play and every time a tile is placed
void newtile()
{
	tilepos = vec2(5 , 19); // Put the tile at the top of the board
	int current_color = 0;
	int tile_type = random(3);
	current_tile_orientation = DOWN;
	current_tile_type = tile_type;
	// Update the geometry VBO of current tile
	for (int i = 0; i < 4; i++)
		switch(tile_type) {
			case 0:
				tile[i] = allRotationsLshape[current_tile_orientation][i]; // Get the 4 pieces of the new tile
				break;
			case 1:
				tile[i] = allRotationsTshape[current_tile_orientation][i]; // Get the 4 pieces of the new tile
				break;
			case 2:
				tile[i] = allRotationsSshape[current_tile_orientation][i]; // Get the 4 pieces of the new tile
				break;
			case 3:
				tile[i] = allRotationsIshape[current_tile_orientation][i]; // Get the 4 pieces of the new tile
				break;
			default:
				break;
		}
	updatetile(); 

	// Update the color VBO of current tile
	//vec4 newcolours[24];
	for (int i = 0; i < 24; i++){
		if((i%6)==0){
			current_color = random(4);
		}
		newcolours[i] = colors[current_color]; // You should randomlize the color
	}
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[5]); // Bind the VBO containing current tile vertex colours
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(newcolours), newcolours); // Put the colour data in the VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	if(board[5][19] == true || board[4][19] == true || board[6][19] == true) {
		printf("Cannot spawn new tile! Game exiting in 1 second...\n");
		sleep(1);
		exit(EXIT_SUCCESS);
	}
}

//-------------------------------------------------------------------------------------------------------------------

void initGrid()
{
	// ***Generate geometry data
	vec4 gridpoints[64]; // Array containing the 64 points of the 32 total lines to be later put in the VBO
	vec4 gridcolours[64]; // One colour per vertex
	// Vertical lines 
	for (int i = 0; i < 11; i++){
		gridpoints[2*i] = vec4((33.0 + (33.0 * i)), 33.0, 0, 1);
		gridpoints[2*i + 1] = vec4((33.0 + (33.0 * i)), 693.0, 0, 1);
		
	}
	// Horizontal lines
	for (int i = 0; i < 21; i++){
		gridpoints[22 + 2*i] = vec4(33.0, (33.0 + (33.0 * i)), 0, 1);
		gridpoints[22 + 2*i + 1] = vec4(363.0, (33.0 + (33.0 * i)), 0, 1);
	}
	// Make all grid lines white
	for (int i = 0; i < 64; i++)
		gridcolours[i] = white;


	// *** set up buffer objects
	// Set up first VAO (representing grid lines)
	glBindVertexArray(vaoIDs[0]); // Bind the first VAO
	glGenBuffers(2, vboIDs); // Create two Vertex Buffer Objects for this VAO (positions, colours)

	// Grid vertex positions
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[0]); // Bind the first grid VBO (vertex positions)
	glBufferData(GL_ARRAY_BUFFER, 64*sizeof(vec4), gridpoints, GL_STATIC_DRAW); // Put the grid points in the VBO
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0); 
	glEnableVertexAttribArray(vPosition); // Enable the attribute
	
	// Grid vertex colours
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[1]); // Bind the second grid VBO (vertex colours)
	glBufferData(GL_ARRAY_BUFFER, 64*sizeof(vec4), gridcolours, GL_STATIC_DRAW); // Put the grid colours in the VBO
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vColor); // Enable the attribute
}


void initBoard()
{
	// *** Generate the geometric data
	vec4 boardpoints[1200];
	for (int i = 0; i < 1200; i++)
		boardcolours[i] = black; // Let the empty cells on the board be black
	// Each cell is a square (2 triangles with 6 vertices)

	for (int i = 0; i < 20; i++){
		for (int j = 0; j < 10; j++)
		{		
			vec4 p1 = vec4(33.0 + (j * 33.0), 33.0 + (i * 33.0), .5, 1);
			vec4 p2 = vec4(33.0 + (j * 33.0), 66.0 + (i * 33.0), .5, 1);
			vec4 p3 = vec4(66.0 + (j * 33.0), 33.0 + (i * 33.0), .5, 1);
			vec4 p4 = vec4(66.0 + (j * 33.0), 66.0 + (i * 33.0), .5, 1);
			
			// Two points are reused
			boardpoints[6*(10*i + j)    ] = p1;
			boardpoints[6*(10*i + j) + 1] = p2;
			boardpoints[6*(10*i + j) + 2] = p3;
			boardpoints[6*(10*i + j) + 3] = p2;
			boardpoints[6*(10*i + j) + 4] = p3;
			boardpoints[6*(10*i + j) + 5] = p4;
		}
	}

	// Initially no cell is occupied
	for (int i = 0; i < 10; i++)
		for (int j = 0; j < 20; j++){
			board[i][j] = false;
			board_ColorCells[i][j] = -1; 
		}


	// *** set up buffer objects
	glBindVertexArray(vaoIDs[1]);
	glGenBuffers(2, &vboIDs[2]);

	// Grid cell vertex positions
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[2]);
	glBufferData(GL_ARRAY_BUFFER, 1200*sizeof(vec4), boardpoints, GL_STATIC_DRAW);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vPosition);

	// Grid cell vertex colours
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[3]);
	glBufferData(GL_ARRAY_BUFFER, 1200*sizeof(vec4), boardcolours, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vColor);
}

// No geometry for current tile initially
void initCurrentTile()
{
	glBindVertexArray(vaoIDs[2]);
	glGenBuffers(2, &vboIDs[4]);

	// Current tile vertex positions
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[4]);
	glBufferData(GL_ARRAY_BUFFER, 24*sizeof(vec4), NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vPosition);

	// Current tile vertex colours
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[5]);
	glBufferData(GL_ARRAY_BUFFER, 24*sizeof(vec4), NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vColor);
}

void init()
{
	// Load shaders and use the shader program
	GLuint program = InitShader("vshader.glsl", "fshader.glsl");
	glUseProgram(program);

	// Get the location of the attributes (for glVertexAttribPointer() calls)
	vPosition = glGetAttribLocation(program, "vPosition");
	vColor = glGetAttribLocation(program, "vColor");

	// Create 3 Vertex Array Objects, each representing one 'object'. Store the names in array vaoIDs
	glGenVertexArrays(3, &vaoIDs[0]);

	// Initialize the grid, the board, and the current tile
	initGrid();
	initBoard();
	initCurrentTile();

	// The location of the uniform variables in the shader program
	locxsize = glGetUniformLocation(program, "xsize"); 
	locysize = glGetUniformLocation(program, "ysize");

	// Game initialization
	newtile(); // create new next tile

	// set to default
	glBindVertexArray(0);
	glClearColor(0, 0, 0, 0);
}

//-------------------------------------------------------------------------------------------------------------------
bool checkRotation(){
	int x_pos = tilepos.x;
	int y_pos = tilepos.y;

	vec2 all_cell_pos[4] = shapes[current_tile_type][current_tile_orientation]; //array of cell positions for this tile
	for(int j = 0; j<4; ++j){ //Load positions for all tile cells into an array 
		all_cell_pos[j].x += x_pos;
		all_cell_pos[j].y += y_pos;
	}

	for(int i = 0; i<4; ++i){
		if(board[(int)all_cell_pos[i].x][(int)all_cell_pos[i].y]==true) return false;
	}

	bool move_attempt;

	switch(current_tile_type) {
		case L:	
			if(x_pos == 0) {
				move_attempt = movetile(vec2(1,0));
				return move_attempt;
			}
			else if(x_pos == 9) {
				move_attempt = movetile(vec2(-1,0));
				return move_attempt;
			}
			else if(y_pos == 0) { //no floor kick rotations
				return false;
			}
			else if(y_pos==19) {
				move_attempt = movetile(vec2(0,-1));
				return move_attempt;
			}	
			break;
		case T:
			if(x_pos == 0) {
				move_attempt = movetile(vec2(1,0));
				return move_attempt;
			}
			else if(x_pos == 9) {
				move_attempt = movetile(vec2(-1,0));
				return move_attempt;
			}
			else if(y_pos == 0) { //no floor kick rotations
				return false;
			}
			else if(y_pos==19) {
				move_attempt = movetile(vec2(0,-1));
				return move_attempt;
			}
			break;
		case S:
			if(x_pos == 0) {
				move_attempt = movetile(vec2(1,0));
				return move_attempt;
			}
			else if(y_pos==19) {
				move_attempt = movetile(vec2(0,-1));
				return move_attempt;
			}
			break;
		case I:
			if(x_pos == 0) {
				move_attempt = movetile(vec2(2,0));
				return move_attempt;
			}
			else if(x_pos == 1){
				move_attempt = movetile(vec2(1,0));
				return move_attempt;
			}
			else if(x_pos == 9) {
				move_attempt = movetile(vec2(-1,0));
				return move_attempt;
			}
			else if(y_pos == 0 || y_pos == 1) { //no floor kick rotations
				return false;
			}
			else if(y_pos==19) {
				move_attempt = movetile(vec2(0,-1));
				return move_attempt;
			}
			break;
		default:
			break;
	}
	return true;
}
// Rotates the current tile, if there is room
void changeOrientation(){
	for (int i = 0; i < 4; i++)
		switch(current_tile_type) {
			case L:
				tile[i] = allRotationsLshape[current_tile_orientation][i]; // Get the 4 pieces of the new tile
				break;
			case T:
				tile[i] = allRotationsTshape[current_tile_orientation][i]; // Get the 4 pieces of the new tile
				break;
			case S:
				tile[i] = allRotationsSshape[current_tile_orientation][i]; // Get the 4 pieces of the new tile
				break;
			case I:
				tile[i] = allRotationsIshape[current_tile_orientation][i]; // Get the 4 pieces of the new tile
				break;
			default:
			break;
		}
}

void rotate()
{      
	switch(current_tile_orientation){
		case DOWN:
			current_tile_orientation = RIGHT;
			changeOrientation();
			if(!checkRotation()){
				current_tile_orientation = DOWN;
				changeOrientation();
			}
			break;
		case RIGHT:
			current_tile_orientation = UP;
			changeOrientation();
			if(!checkRotation()){
				current_tile_orientation = RIGHT;
				changeOrientation();
			}
			break;
		case UP:
			current_tile_orientation = LEFT;
			changeOrientation();
			if(!checkRotation()){
				current_tile_orientation = UP;
				changeOrientation();
			}
			break;
		case LEFT:
			current_tile_orientation = DOWN;
			changeOrientation();
			if(!checkRotation()){
				current_tile_orientation = LEFT;
				changeOrientation();
			}
			break;
		default:
			break;
	}
	updatetile();
}

//-------------------------------------------------------------------------------------------------------------------

void printTables(){
	for(int i = 0; i<10; ++i){
					printf("Row %d: [ ", i);
						for(int k = 0; k<20; ++k){
							if(k==19){
								printf(" %d ", board_ColorCells[i][k]);
							}
							else{
								printf(" %d, ", board_ColorCells[i][k]);	
							}
						}
					printf(" ]\n");
				}

				for(int i = 0; i<10; ++i){
					printf("Row %d: [ ", i);
						for(int k = 0; k<20; ++k){
							if(k==19){
								if(board[i][k]==true){
									printf(" true ");
								}
								else printf(" false ");
							}
							else{
								if(board[i][k]==true){
									printf(" true, ");
								}
								else printf(" false, ");	
							}
						}
					printf(" ]\n");
				}	
}

bool checkForColorMatches(){

	int col_lower, col_higher, row_lower, row_higher, candidate; //ranges for potential cell deletion

	col_lower = 0; col_higher = 0; //initialized to first column entry, remember to reinitialize for next column
	candidate = board_ColorCells[0][0]; //ditto

	int intsize = sizeof(board_ColorCells[0][0]);

	for(int i = 0; i<10; ++i){ //check for column matches
		col_lower = 0; col_higher = 0;
		candidate = board_ColorCells[i][0];

		for(int j = 0; j<20; ++j){
			if(board_ColorCells[i][j]==-1 && !(col_higher-col_lower>2)){
				candidate = -1; col_lower = col_higher; col_lower++; col_higher++;
			}
			else if(board_ColorCells[i][j]!=candidate && !(col_higher-col_lower>2)){
				candidate = board_ColorCells[i][j]; col_lower = col_higher; col_higher++;
			}
			else if(board_ColorCells[i][j]!=candidate && (col_higher-col_lower>2)){				
				
				memmove(board[i]+col_lower, board[i]+col_higher, 20-1-col_higher);
				memmove(board_ColorCells[i]+col_lower, board_ColorCells[i]+col_higher, 20*intsize-intsize-intsize*col_higher); //possibility of integer size difference between systems
				board[i][19]=false;
				board_ColorCells[i][19]=-1;

				for(int k = 0; k<(19-col_higher); ++k){					
					memmove(boardcolours+((k+col_lower)*60)+(i*6), boardcolours+((k+col_higher)*60)+(i*6), 96);
				}
				boardcolours[19*60+(i*6)] = black;

				//DEBUG
				//printTables();
				//END
				//recursively call checkForColorMatches again to check if there was a new color match created by the removal of part of a column
				checkForColorMatches();
				return true;
				
				candidate = board_ColorCells[i][j]; col_higher=col_lower; col_higher++;//col_lower=col_higher; col_higher++; //col_higher=col_lower; col_higher++;
			}
			else if(board_ColorCells[i][j]==candidate){
				col_higher++;
			}
		}
	}

	for(int j = 0; j<20; ++j){ //now check for row-matches
		row_lower = 0; row_higher = 0;
		candidate = board_ColorCells[0][j];

		for(int i = 0; i<10; ++i){
			if(board_ColorCells[i][j]==-1 && !(row_higher-row_lower>2)){
				candidate = -1; row_lower = row_higher; row_lower++; row_higher++;
			}
			else if(board_ColorCells[i][j]!=candidate && !(row_higher-row_lower>2)){
				candidate = board_ColorCells[i][j]; row_lower = row_higher; row_higher++;
			}
			else if(board_ColorCells[i][j]!=candidate && (row_higher-row_lower>2)){ //MEMMOVE HAPPENS HERE
				for(int h = 0; h<(row_higher-row_lower); ++h){
					memmove(board[row_lower+h]+j, board[row_lower+h]+j+1, 20-j-1);
					memmove(board_ColorCells[row_lower+h]+j, board_ColorCells[row_lower+h]+j+1, 20*intsize-intsize-intsize*j); //possibility of integer size difference between systems
					board[row_lower+h][19]=false;
					board_ColorCells[row_lower+h][19]=-1;
				}

				for(int k = 0; k<(19-j); ++k){					
					memmove(boardcolours+((k+j)*60)+(row_lower*6), boardcolours+((k+1+j)*60)+(row_lower*6), 96*(row_higher-row_lower));
				}
				for(int h = row_lower; h<row_higher; ++h){
					boardcolours[19*60+(h*6)] = black;
				}

				//DEBUG				
				//printTables();
				//END

				checkForColorMatches();

				return true;

			}
			else if(board_ColorCells[i][j]==candidate){
				row_higher++;
			}	
		}

	}

	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[3]);
	glBufferData(GL_ARRAY_BUFFER, 1200*sizeof(vec4), boardcolours, GL_DYNAMIC_DRAW);

	return true;	
}

// Checks if the specified row (0 is the bottom 19 the top) is full
// If every cell in the row is occupied, it will clear that cell and everything above it will shift down one row
void checkfullrow(int row) //candidate forimprovement via memcpy of a setup 'false' row and memcmp for a setup 'true' row
{
	for(int k = 0; k<10; ++k){
		if(board[k][row]==false) {
			return;
		}
	}

	int intsize = sizeof(board_ColorCells[0][0]);

	for(int i = 0; i<10; ++i){ //move the cell detection table and the color lookup table rows column by column
		memmove(board[i]+row, board[i]+row+1, 20-1-row);
		memmove(board_ColorCells[i]+row, board_ColorCells[i]+row+1, 20*intsize-intsize-intsize*row); //possibility of integer size difference between systems
		board[i][19]=false;
		board_ColorCells[i][19]=-1;
	}
	
	memmove(boardcolours+(row*60), boardcolours+((row+1)*60), (18240)-((row)*60*16)); //this is a magic formula, it doesn't appear to actually work with how the memmove documentation says it should, but I think this is due to the strange vec4 struct
	for(int j = 1140; j< 1200; ++j){
		boardcolours[j] = black;
	}

	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[3]);
	glBufferData(GL_ARRAY_BUFFER, 1200*sizeof(vec4), boardcolours, GL_DYNAMIC_DRAW);

	checkForColorMatches();
}

//-------------------------------------------------------------------------------------------------------------------

// Places the current tile - update the board vertex colour VBO and the array maintaining occupied cells
void settile()
{
	int low = 20; int high = -1;
	int x_pos = tilepos.x;
	int y_pos = tilepos.y;

	vec2 all_cell_pos[4] = shapes[current_tile_type][current_tile_orientation]; //array of cell positions for coloring

	for(int j = 0; j<4; ++j){
		all_cell_pos[j].x += x_pos;
		all_cell_pos[j].y += y_pos;
		if((int)all_cell_pos[j].y > high) high = (int)all_cell_pos[j].y;
		if((int)all_cell_pos[j].y < low) low = (int)all_cell_pos[j].y;
	}

	for(int i = 0; i < 4; ++i){ //for each cell in the current tile...
		for(int k = 0; k < 6; ++k){ //for each vertex in the current cell...
			boardcolours[(60*(int)all_cell_pos[i].y + 6*(int)all_cell_pos[i].x) + k] = {newcolours[k+(i*6)][0], newcolours[k+(i*6)][1], newcolours[k+(i*6)][2], newcolours[k+(i*6)][3]};
		}

		if((newcolours[i*6][0] == 0.0f) && (newcolours[i*6][1] == 1.0f) && (newcolours[i*6][2] == 0.0f) && (newcolours[i*6][3] == 1.0f)){ //GREEN
			board_ColorCells [(int)all_cell_pos[i].x][(int)all_cell_pos[i].y] = GREEN;
		}
		else if((newcolours[i*6][0] == 0.0f) && (newcolours[i*6][1] == 0.0f) && (newcolours[i*6][2] == 1.0f) && (newcolours[i*6][3] == 1.0f)){ //BLUE
			board_ColorCells [(int)all_cell_pos[i].x][(int)all_cell_pos[i].y] = BLUE;
		}
		else if((newcolours[i*6][0] == 1.0f) && (newcolours[i*6][1] == 0.0f) && (newcolours[i*6][2] == 0.0f) && (newcolours[i*6][3] == 1.0f)){ //RED
			board_ColorCells [(int)all_cell_pos[i].x][(int)all_cell_pos[i].y] = RED;
		}
		else if((newcolours[i*6][0] == 1.0f) && (newcolours[i*6][1] == 0.5f) && (newcolours[i*6][2] == 0.0f) && (newcolours[i*6][3] == 1.0f)){ //ORANGE
			board_ColorCells [(int)all_cell_pos[i].x][(int)all_cell_pos[i].y] = ORANGE;
		}
		else if((newcolours[i*6][0] == 1.0f) && (newcolours[i*6][1] == 1.0f) && (newcolours[i*6][2] == 0.0f) && (newcolours[i*6][3] == 1.0f)){ //YELLOW
			board_ColorCells [(int)all_cell_pos[i].x][(int)all_cell_pos[i].y] = YELLOW;
		}

		board[(int)all_cell_pos[i].x][(int)all_cell_pos[i].y] = true;		
	}


	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[3]);
	glBufferData(GL_ARRAY_BUFFER, 1200*sizeof(vec4), boardcolours, GL_DYNAMIC_DRAW);

	//At this point, call checkrow
	//printf("Low: %d, high: %d\n", low, high);
	for(int l = high; l >=low; --l){ //check from the top down so that memmove operation won't affect where rows are
		checkfullrow(l);
	}

	//now check for consecutive matching colors in columns/rows
	checkForColorMatches();

	newtile();
}

//-------------------------------------------------------------------------------------------------------------------

// Given (x,y), tries to move the tile x squares to the right and y squares down
// Returns true if the tile was successfully moved, or false if there was some issue
bool movetile(vec2 direction)
{
	int left_border, right_border, bottom_border;
	left_border = 0; right_border = 0; bottom_border = 0;

	switch(current_tile_type) {
		case L: //0
			switch(current_tile_orientation) {
				case DOWN:
					left_border = 1; right_border = 8; bottom_border = 1;
					break;
				case RIGHT:
					left_border = 0; right_border = 8; bottom_border = 1;
					break;
				case UP:
					left_border = 1; right_border = 8; bottom_border = 0;
					break;
				case LEFT:
					left_border = 1; right_border = 9; bottom_border = 1;
					break;
				default:
					break;
			}
			break;
		case T: //1
			switch(current_tile_orientation) {
				case DOWN:
					left_border = 1; right_border = 8; bottom_border = 1;
					break;
				case RIGHT:
					left_border = 0; right_border = 8; bottom_border = 1;
					break;
				case UP:
					left_border = 1; right_border = 8; bottom_border = 0;
					break;
				case LEFT:
					left_border = 1; right_border = 9; bottom_border = 1;
					break;
				default:
					break;
			}
			break;
		case S: //2
			switch(current_tile_orientation) {
				case DOWN:
					left_border = 1; right_border = 8; bottom_border = 1;
					break;
				case RIGHT:
					left_border = 0; right_border = 8; bottom_border = 1;
					break;
				case UP:
					left_border = 1; right_border = 8; bottom_border = 1;
					break;
				case LEFT:
					left_border = 0; right_border = 8; bottom_border = 1;
					break;
				default:
					break;
			}
			break;
		case I: //3
			switch(current_tile_orientation) {
				case DOWN:
					left_border = 2; right_border = 8; bottom_border = 0;
					break;
				case RIGHT:
					left_border = 0; right_border = 9; bottom_border = 2;
					break;
				case UP:
					left_border = 2; right_border = 8; bottom_border = 0;
					break;
				case LEFT:
					left_border = 0; right_border = 9; bottom_border = 2;
					break;
				default:
					break;
			}
			break;
		default:
			break;
	}

	int x_pos = tilepos.x;
	int y_pos = tilepos.y;

	vec2 all_cell_pos[4] = shapes[current_tile_type][current_tile_orientation]; //array of cell positions for this tile
	for(int j = 0; j<4; ++j){ //Load positions for all tile cells into an array 
		all_cell_pos[j].x += x_pos;
		all_cell_pos[j].y += y_pos;
	}

	for(int k = 0; k<4; ++k){
		if((board[(int)all_cell_pos[k].x][(int)all_cell_pos[k].y-1]==true) && (direction.y!=0)){
			settile();
			return false;
		} 
	}

	if(tilepos.y==bottom_border && direction.y!=0) {
		settile();
		return false;
	}

	if(tilepos.y+direction.y >= bottom_border) tilepos.y += direction.y;
	if((tilepos.x+direction.x >= left_border) && 
		(tilepos.x+direction.x <= right_border)){ 
		for(int k = 0; k<4; ++k){
			if(board[(int)all_cell_pos[k].x + (int)direction.x][(int)all_cell_pos[k].y]==true){
				return false;
			}
		}
		tilepos.x += direction.x;
	}
	return true;
}
//-------------------------------------------------------------------------------------------------------------------

// Starts the game over - empties the board, creates new tiles, resets line counters
void restart()
{
	init();
}
//-------------------------------------------------------------------------------------------------------------------

// Draws the game
void display()
{

	glClear(GL_COLOR_BUFFER_BIT);

	glUniform1i(locxsize, xsize); // x and y sizes are passed to the shader program to maintain shape of the vertices on screen
	glUniform1i(locysize, ysize);

	glBindVertexArray(vaoIDs[1]); // Bind the VAO representing the grid cells (to be drawn first)
	glDrawArrays(GL_TRIANGLES, 0, 1200); // Draw the board (10*20*2 = 400 triangles)

	glBindVertexArray(vaoIDs[2]); // Bind the VAO representing the current tile (to be drawn on top of the board)
	glDrawArrays(GL_TRIANGLES, 0, 24); // Draw the current tile (8 triangles)

	glBindVertexArray(vaoIDs[0]); // Bind the VAO representing the grid lines (to be drawn on top of everything else)
	glDrawArrays(GL_LINES, 0, 64); // Draw the grid lines (21+11 = 32 lines)


	glutSwapBuffers();
}

//-------------------------------------------------------------------------------------------------------------------

// Reshape callback will simply change xsize and ysize variables, which are passed to the vertex shader
// to keep the game the same from stretching if the window is stretched
void reshape(GLsizei w, GLsizei h)
{
	xsize = w;
	ysize = h;
	glViewport(0, 0, w, h);
}

void shuffleColors(){

	vec4 temp[6];

	memcpy(temp, newcolours, 96);
	memmove(newcolours, newcolours+6, 288);
	memcpy(newcolours+18, temp, 96);
	//printf("Size: %lu\n", sizeof(temp[0]));

	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[5]); // Bind the VBO containing current tile vertex colours
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(newcolours), newcolours); // Put the colour data in the VBO

}
//-------------------------------------------------------------------------------------------------------------------

// Handle arrow key keypresses
void special(int key, int x, int y)
{
	switch(key)
	{
		case GLUT_KEY_UP:
			rotate();
			updatetile();
			break;
		case GLUT_KEY_DOWN:
			movetile(vec2(0,-1));
			updatetile();
			break;
		case GLUT_KEY_LEFT:
			movetile(vec2(-1,0));	
			updatetile();
			break;
		case GLUT_KEY_RIGHT:
			movetile(vec2(1,0));
			updatetile();
			break;
	}
}

//-------------------------------------------------------------------------------------------------------------------

// Handles standard keypresses
void keyboard(unsigned char key, int x, int y)
{
	switch(key) 
	{
		case ' ': 
			shuffleColors();
			break;
		case 033: // Both escape key and 'q' cause the game to exit
		    exit(EXIT_SUCCESS);
		    break;
		case 'q':
			exit (EXIT_SUCCESS);
			break;
		case 'r': // 'r' key restarts the game
			restart();
			break;
	}	
	glutPostRedisplay();
}

//-------------------------------------------------------------------------------------------------------------------

void idle(void)
{
	glutPostRedisplay();
}

void timedMove(int t){ //potentially restrict the movetile() call if there is some other event going on
	movetile(vec2(0, -1));
	glutTimerFunc(1000, timedMove, 0);
	updatetile();
}
//-------------------------------------------------------------------------------------------------------------------

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(xsize, ysize);
	glutInitWindowPosition(680, 178); // Center the game window (well, on a 1920x1080 display)
	glutCreateWindow("Fruit Tetris");
	glewInit();
	init();

	// Callback functions
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutSpecialFunc(special);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);
	glutTimerFunc(1000, timedMove, 0);

	glutMainLoop(); // Start main loop
	return 0;
}
