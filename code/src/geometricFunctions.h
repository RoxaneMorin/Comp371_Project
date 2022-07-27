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
#include <map>

#define GLEW_STATIC 1   // This allows linking with Static Library on Windows, without DLL
#include <GL/glew.h>    // Include GLEW - OpenGL Extension Wrangler

#include <GLFW/glfw3.h> // GLFW provides a cross-platform interface for creating a graphical context,
                        // initializing OpenGL and binding inputs

#include <glm/glm.hpp>  // GLM is an optimized math library with syntax to similar to OpenGL Shading Language
#include <glm/gtc/matrix_transform.hpp> // include this to create transformation matrices
#include <glm/common.hpp>

using namespace std;
using namespace glm;


// Structs.

struct CompareVec2;

struct TexturedColoredVertex;

struct TexturedColoredNormalVertex;



// Geometric Functions.

int createCubeVBO();

int createLinesVBO();

int createPlaneVBO();

int createSphereVBO(float radius, float heightOffset, int sectorCount, int stackCount);

std::map<vec2, TexturedColoredNormalVertex, CompareVec2> createGroundVertexMap(unsigned int sizeX, unsigned int sizeZ, float uvTiling = 1);

float generateHeightCoord(unsigned int xCoord, unsigned int zCoord, float noiseScaling);

vec2 generateUVCoords(unsigned int posX, unsigned int posY, float uvTiling = 1);

vector<TexturedColoredNormalVertex> createGroundVertexVector(std::map<vec2, TexturedColoredNormalVertex, CompareVec2> vertexMap, unsigned int sizeX, unsigned int sizeZ);

int createGroundVBO(unsigned int sizeX, unsigned int sizeZ, float uvTiling = 1);


