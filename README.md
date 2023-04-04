Dependencies needed:

-GLFW  
-GLEW  
-GLM  
-OPENGL  
-ASSIMP  
-SOIL2  

Commands:

W: zoom in  
S: zoom out  
D/Right: move right  
A/Left: move left  
Up: move up  
Down: move down  
P: pause movement  
O: show/hide orbital lines  
1: positions and locks camera above the solar system (view from above)  
2: unlocks camera  
3: positions and locks camera above earth, always oriented towards the middle  
M: increase orbital speed  
N: decrease orbital speed  
=: increase camera speed  
-: decrease camera speed

Running Instructions

Windows

Option #1 Open .sln file with Visual Studio 2022 and run (might need to update lib and include directories in solution properties)  
Option #2 Download the whole project and run the .exe in 'bin' fodler

Linux

1. Download whole project
2. Download dependencies mentionned on your linux machine (look up different commands to install all of them) and possibly modify CMakeLists.txt to make sure your libraries are linked properly
3. Cd into 'build' folder and follow README instructions
