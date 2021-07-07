# issue
Have been tested in windows.However freeglut3 has bugs when full screen and resize windows in ubuntu.


# GLUT_demo 
This is a simple demo concerning OpenGL and GLUT to describe some attributes about them. 

![Demo](https://github.com/renhaofan/GLUT_demo/blob/main/xyzCameraV3/demo.png) 

# KeyBoard Control
## Camera 
<kbd>w</kbd> - camera move forward. <kbd>s</kbd> - camera move backward 

<kbd>a</kbd> - camera move left. <kbd>d</kbd> - camera move right 

<kbd>q</kbd> - camera move up. <kbd>e</kbd> - camera move down

<kbd>←</kbd> and <kbd>←</kbd>rotate around axis v

<kbd>↑</kbd>, <kbd>↓</kbd> plotCamera not set.
## Scene

<kbd>/</kbd> - show world axes.

<kbd>r</kbd> - teapot rotation. 

<kbd>m</kbd> - polygon mode. 

# Bugs Log
* Drift
There's no process lock. If we alter the _view_matrix in scene1 expecially after rotation, the _view_matrix will change a litte. Thus, we need another GLCamera to manage top_view GLCamera's view matrix. It's also why remove the `stack`.
