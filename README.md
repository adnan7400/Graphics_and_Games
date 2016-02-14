### Graphics and Games
---

This repository contains three C++ projects completed for SFU's CMPT 361 - Intro to Computer Graphics

The projects are:  
* 2D Tetris clone w/ additional gameplay added from a fruit matching game
* 3D Tetris - same as the above, but extended to 3D with additional functionality
* Basic Raytracer

##### 2D Fruit-Tetris:  
This project involved developing the game logic, and OpenGL controll structures for input, as well as matrix manipulation 
and basic OpenGL drawing functions (ie: manipulating triangles for the vertex shader).

The controls are simple:
* UP-key: rotates pieces in-place counter-clockwise
* Left-key/Right-key/Down-key: Directional control for the pieces
* 'r': Restart the game
* 'q'/ESC: Exit the game
* Spacebar: Cycle piece colors

The game is a standard 2D tetris clone, however it also incorporates features of a 'Fruit Matching' game - each piece is 
defined by four blocks, and each block may be one of 5 random colors. When a piece is set (ie: when it's downward motion 
is stopped either by the bottom of the grid or any of its blocks is stopped by any block of another set piece), the game 
will check whether any rows/columns contain 3 or more of the same color of block - if so those 3 or more blocks will 
disapear, and blocks above them will drop until they are set.

![Figure 1: 2D Fruit Tetris game in progress](https://raw.githubusercontent.com/jonnykantor/Images-and-Screengrabs/master/Graphics/2d_tetris.png)
#####<sub>Figure 1: 2D Fruit Tetris game in progress</sub>

##### 3D Fruit-Tetris
This project took the previous project and extended it to 3D. This largely involved working with the vertex and fragment 
shaders, implementing a matrix-stack for manipulating individual models, reusing groups of vertices at different draw points, and utilyzing the model, view, and projection matrices.
The gameplay remains largely unchanged, however rather than new pieces dropping from the same point on the grid, new pieces now
appear at the tip of a manipulable robot arm, requiring the player to 'drop' the piece onto the 3D grid.

The controls are simple:
* W/A/S/D: Directional controls for the robot arm
* UP-key: rotates pieces in-place counter-clockwise once dropped
* Left-key/Right-key/Down-key: Directional control for the pieces once dropped
* 'r': Restart the game
* 'q'/ESC: Exit the game
* Spacebar: Drop piece/Cycle piece colors once dropped
* CTRL-Left/CTRL-Right: Rotate the scene around the vertical axis (effectively, rotate the camera around the scene)

![Figure 2: 3D Tetris game in progress, scene rotated slightly, piece falling](https://raw.githubusercontent.com/jonnykantor/Images-and-Screengrabs/master/Graphics/3d_rotated_falling.png)
#####<sub>Figure 2: 3D Tetris game in progress, scene rotated slightly, piece falling</sub>

![Figure 3: 3D Tetris game, robot arm holding next piece to be dropped, scene rotated for view behind grid](https://raw.githubusercontent.com/jonnykantor/Images-and-Screengrabs/master/Graphics/3d_Arm_Holding.png)
#####<sub>Figure 3: 3D Tetris game, robot arm holding next piece to be dropped, scene rotated for view behind grid</sub>

![Figure 4: 3D Tetris game, tip of arm is 'out of bounds', piece is grayed out for illegal drop](https://raw.githubusercontent.com/jonnykantor/Images-and-Screengrabs/master/Graphics/3d_Out_of_bounds.png)
#####<sub>Figure 4: 3D Tetris game, tip of arm is 'out of bounds', piece is grayed out for illegal drop</sub>

#####Basic Raytracer
This project involved constructing a basic raytracer using OpenGL for drawing a simple scene involving 3 spheres and a chessboard. This included sphere and plane intersection detection, implementing the Phong Local illumination model for ambient lighting and shadow along with reflectance and refractance modeling using multiple levels of recursive raytracing, stochastic ray generation, and supersampling.

The code here comes with a default scene already provided; to use the code, after compiling run `./raycast` with either `-d` for the default scene or `-u` for a user defined scene (the geometry for this will need to be provided by the user in `scene.cpp` prior to compiling), and a number from 0 to 5 for the levels of recursion desired. The user may also provide any of the following flags: `+s` for shadows, `+l` for reflections, `+c` for the chessboard plane, `+r` for refraction, `+f` for stochastic rays, `+p` for super-sampling (this will generate 5 rays/pixel rather than one).

![Figure 5: Default raytracer scene, using all available flags, with 5 levels of recursion](https://raw.githubusercontent.com/jonnykantor/Images-and-Screengrabs/master/Graphics/scene.bmp)
#####<sub>Figure 5: Default raytracer scene, using all available flags, with 5 levels of recursion</sub>
