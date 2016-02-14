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

#####<sub>Figure 1: 2D Fruit Tetris game in progress</sub>

##### 3D Fruit-Tetris
This project took the previous project and extended it to 3D. This largely involved working with the vertex and fragment 
shaders, implementing a matrix-stack for manipulating individual models, and utilyzing the model, view, and projection matrices.
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

#####<sub></sub>

#####<sub></sub>

