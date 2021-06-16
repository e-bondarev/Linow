# Linow

## Description

Linow is a header-only **C++** library that uses **modern OpenGL** to render lines (e.g for debugging purposes).

## Requirements
C++17 or later.

## Installation

Just download <a href="https://github.com/e-bondarev/Linow/blob/main/Linow.h">Linow.h</a> and include it in your project.

## Motivation

* DRY

  3D lines are really important in debugging of graphics software. Sometimes I want to see the collision box of a character in a project, or render a path that a mesh should follow. I always end up implementing the same concept in all of my projects. This library prevents me and perhaps you from doing that.
* Simplicity

  1. include the library:
  ```c++
  #define LINOW_USE_GLM
  #include "Linow.h"
  ```

  2. Initialize:
  ```c++  
  Linow::Init();
  ```

  3. In your game loop:
  ```c++
  Linow::Clear();
    Linow::AddLine(Linow::Vec3(value, 0, 0), Linow::Vec3(25, 25, 0));
  Linow::Render(glm::value_ptr(proj), glm::value_ptr(view));
  ```

## Dependencies
<a href="https://github.com/nigels-com/glew">GLEW</a> - make sure the header <GL/glew.h> is available for the header Linow.h

[Optional] <a href="https://github.com/g-truc/glm">GLM</a> - if you want to use GLM's implementation of vec2, vec3, vec4, which I highly recommend, you just need to ```#define LINOW_USE_GLM ```before including "Linow.h". Once again, make sure the header <glm/glm.hpp> is available

## Options

```c++
/*
  Use GLM's implementation of vectors. 
*/
#define LINOW_USE_GLM     

/* 
  Output state changes 
  (Initialization, Shutdown, Constructors, Destructors)
*/
#define LINOW_DEBUGGING   
```

## Author
Eugen Bondarev, eugenbondarev@gmail.com
