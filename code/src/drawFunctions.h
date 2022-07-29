//
// Roxane Morin (40191881)
// Assignment 02
//
// COMP 371 Assignment Framework
//
// Created by Nicolas Bergeron on 20/06/2019.
//
// Inspired by the following tutorials:
// - https://learnopengl.com/Getting-started/Hello-Window
// - https://learnopengl.com/Getting-started/Hello-Triangle

#include <iostream>
#include <list>
#include <vector>
#include <algorithm>

#define GLEW_STATIC 1   // This allows linking with Static Library on Windows, without DLL
#include <GL/glew.h>    // Include GLEW - OpenGL Extension Wrangler

#include <GLFW/glfw3.h> // GLFW provides a cross-platform interface for creating a graphical context,
                        // initializing OpenGL and binding inputs

#include <glm/glm.hpp>  // GLM is an optimized math library with syntax to similar to OpenGL Shading Language
#include <glm/gtc/matrix_transform.hpp> // include this to create transformation matrices
#include <glm/common.hpp>

using namespace std;
using namespace glm;

// Draw Functions.

void DrawGrid(int shaderProgram, GLfloat centerPosX, GLfloat centerPosZ, GLuint width, GLuint depth);

void DawAxisStar(int shaderProgram);

extern int meshRenderMode; // Declaring the variable here as it is used in multiple methods.

mat4 DrawParentedMesh(int shaderProgram, mat4 parentMatrix, vec3 translation, vec3 scale, unsigned int numberOfVerts);

mat4 DrawParentedMesh(int shaderProgram, mat4 parentMatrix, vec3 translation, vec3 scale, float angle, vec3 rotationAxis, unsigned int numberOfVerts);