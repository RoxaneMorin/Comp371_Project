#include <iostream>

#define GLEW_STATIC 1   // This allows linking with Static Library on Windows, without DLL
#include <GL/glew.h>    // Include GLEW - OpenGL Extension Wrangler

#include <GLFW/glfw3.h> // GLFW provides a cross-platform interface for creating a graphical context,
// initializing OpenGL and binding inputs

#include <glm/glm.hpp>  // GLM is an optimized math library with syntax to similar to OpenGL Shading Language
#include <glm/gtc/matrix_transform.hpp> // include this to create transformation matrices
#include <glm/common.hpp>

using namespace std;
using namespace glm;


// Global variables.
const float light_far_plane = 500.0f;
const float light_near_plane = 0.1f;


// Inspired by https://learnopengl.com/Lighting/Light-casters.
class Light // Directional Light.
{
   // Parameters.
    public : vec3 color;
    public: vec3 position;
    public: vec3 lookAt;

    public: vec3 direction;

    public: mat4 lightProjection;
    public: mat4 lightView;
    public: mat4 lightSpaceMatrix;


    // Constructors.
    Light() : color(vec3(1.0f)), position(vec3(0.0f, 1.0f, 1.0f)), lookAt(vec3(0.0f))
    {
        setDirection();
        generateProjectionMatrix();
        generateViewMatrix();
        generateLightSpaceMatrix();
    }

    Light(vec3 _color, vec3 _position, vec3 _lookAt) : color(_color), position(_position), lookAt(_lookAt)
    {
        setDirection();
        generateProjectionMatrix();
        generateViewMatrix();
        generateLightSpaceMatrix();
    }


    // Methods.
    public: void setDirection()
    {
        direction = position - lookAt;
    }

    public: void generateProjectionMatrix()
    {
        lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, light_near_plane, light_far_plane);
    }

    public: void generateViewMatrix()
    {
        lightView = glm::lookAt(position, lookAt, vec3(0.0f, 1.0f, 0.0f));
    }

    public: void generateLightSpaceMatrix()
    {
        lightSpaceMatrix = lightProjection * lightView;
    }
};

