﻿//
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
#include <algorithm>
#include <vector>

#define GLEW_STATIC 1   // This allows linking with Static Library on Windows, without DLL
#include <GL/glew.h>    // Include GLEW - OpenGL Extension Wrangler

#include <GLFW/glfw3.h> // GLFW provides a cross-platform interface for creating a graphical context,
                        // initializing OpenGL and binding inputs

#include <glm/glm.hpp>  // GLM is an optimized math library with syntax to similar to OpenGL Shading Language
#include <glm/gtc/matrix_transform.hpp> // include this to create transformation matrices
#include <glm/gtc/type_ptr.hpp>
#include <glm/common.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <shaderloader.h>

#include "geometricFunctions.h"
#include "drawFunctions.h"

using namespace std;
using namespace glm;


GLuint loadTexture(const char* filename)
{
    // Create and bind textures¸.
    GLuint textureId = 0;
    glGenTextures(1, &textureId);
    assert(textureId != 0);

    glBindTexture(GL_TEXTURE_2D, textureId);

    // Set filter parameters.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Load texture with its dimensional data.
    int width, height, nrChannels;
    unsigned char* data = stbi_load(filename, &width, &height, &nrChannels, 0);
    if (!data)
    {
        std::cerr << "Error::Texture could not load texture file:" << filename << std::endl;
        return 0;
    }

    // Upload the texture to the PU.
    GLenum format = 0;
    if (nrChannels == 1)
        format = GL_RED;
    else if (nrChannels == 3)
        format = GL_RGB;
    else if (nrChannels == 4)
        format = GL_RGBA;
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height,
        0, format, GL_UNSIGNED_BYTE, data);

    // Clean up.
    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);
    return textureId;
}


// Shaders.
GLuint colourShaderProgram;
GLuint texturedShaderProgram;
GLuint groundShaderProgram;
GLuint shadowShaderProgram;

vector<GLuint> allShaders;


// shader variable setters
void SetUniformMat4(GLuint shader_id, const char* uniform_name, mat4 uniform_value)
{
    glUseProgram(shader_id);
    glUniformMatrix4fv(glGetUniformLocation(shader_id, uniform_name), 1, GL_FALSE, &uniform_value[0][0]);
}

void SetUniformVec3(GLuint shader_id, const char* uniform_name, vec3 uniform_value)
{
    glUseProgram(shader_id);
    glUniform3fv(glGetUniformLocation(shader_id, uniform_name), 1, value_ptr(uniform_value));
}

template <class T>
void SetUniform1Value(GLuint shader_id, const char* uniform_name, T uniform_value)
{
    glUseProgram(shader_id);
    glUniform1f(glGetUniformLocation(shader_id, uniform_name), uniform_value);
    glUseProgram(0);
}


// Matrix & Camera Functions.

void setProjectionMatrix(int shaderProgram, mat4 projectionMatrix)
{
    glUseProgram(shaderProgram);
    GLuint projectionMatrixLocation = glGetUniformLocation(shaderProgram, "projectionMatrix");
    glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);
}

void setProjectionMatrix(vector<GLuint> shaderPrograms, mat4 projectionMatrix)
{
    for (int shaderProgram : shaderPrograms)
        setProjectionMatrix(shaderProgram, projectionMatrix);
}

void setViewMatrix(int shaderProgram, mat4 viewMatrix)
{
    glUseProgram(shaderProgram);
    GLuint viewMatrixLocation = glGetUniformLocation(shaderProgram, "viewMatrix");
    glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
}

void setViewMatrix(vector<GLuint> shaderPrograms, mat4 viewMatrix)
{
    for (int shaderProgram : shaderPrograms)
        setProjectionMatrix(shaderProgram, viewMatrix);
}

void setWorldMatrix(int shaderProgram, mat4 worldMatrix)
{
    glUseProgram(shaderProgram);
    GLuint worldMatrixLocation = glGetUniformLocation(shaderProgram, "worldMatrix");
    glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
}

vec3 updateCameraPosition(float cameraTheta, float cameraPhi, float cameraRadius)
{
    // Recalculate the camera's position from its spherical coordinates.
    float cameraX = cameraRadius * cosf(cameraTheta) * cosf(cameraPhi);
    float cameraZ = cameraRadius * sinf(cameraTheta) * -cosf(cameraPhi);
    float cameraY = cameraRadius * sinf(cameraPhi);
    return vec3(cameraX, cameraY, cameraZ);
}


// MAIN
// Declare functions and variables for later use.

// Functions.
void initScene();
void initShadows();
void renderScene(GLuint shaderProgram);
void handleInputs();


// Textures.
GLuint snowTextureID;
GLuint carrotTextureID;
GLuint stoneTextureID;
GLuint woodTextureID;
GLuint metalTextureID;

GLuint snowDepthTextureID;
GLuint stoneDepthTextureID;




// Meshes.
int cubeVAO;
int planeVAO;
int lineVAO;
int sphereVAO;
int groundVAO;

// Additional info for spheres.
int sphereRadialDivs = 8;
int sphereVerticalDivs = 8;
int sphereVertexCount;


// Light parameters.
vec3 ambientColour = vec3(0.5, 0.5, 0.55);
vec3 lightColour = vec3(0.75, 0.75, 1.0);
vec3 lightPosition = vec3(0.0f, 10.0f, 5.0f);;

// Shadow data.
bool useShadows = true;

const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
GLuint depthMap;
GLuint depthMapFBO;

float aspect;
float lightNearPlane = 0.1f;
float lightFarPlane = 100.0f;

glm::mat4 lightProjection;
glm::mat4 lightView;
glm::mat4 lightSpaceMatrix;


// Camera parameters.
float cameraTheta;
float cameraPhi;
float cameraRadius;
float cameraRotSpeed;

// Camera matrixes.
vec3 cameraPosition;
vec3 cameraLookAt(0.0f, 0.0f, 0.0f);
vec3 cameraUpVector(0.0f, 1.0f, 0.0f);

vec3 cameraSideVector;
mat4 viewMatrix;


// Misc information.
GLFWwindow* window;
int windowWidth = 1024, windowHeigth = 768;

double lastMousePosX, lastMousePosY;
int previousZPress;
int previousCPress;
int previousYPress;
int previousIPress;
int previousBPress;
float dt;


// Ground info
GLuint groundSizeX = 50;
GLuint groundSizeZ = 50;
float groundUVTiling = 5.0f;


// Handle window resizing.
void window_size_callback(GLFWwindow* window, int width, int height)
{
    glfwGetWindowSize(window, &windowWidth, &windowHeigth);

    // Set projection matrix for shaders.
    mat4 projectionMatrix = perspective(70.0f, // fov in degrees.
        (float)width / (float)height, // aspect ratio.
        0.001f, 1000.0f); // near and far planes.


    setProjectionMatrix(allShaders, projectionMatrix);
    setProjectionMatrix(groundShaderProgram, projectionMatrix);

    cout << "Window resized.\n";
}

int main(int argc, char* argv[])
{
    // Initialize GLFW and OpenGL version
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);

    // Create Window and rendering context using GLFW, resolution is 1024x768
    window = glfwCreateWindow(windowWidth, windowHeigth, "Comp371 - Assignment 02", NULL, NULL);
    if (window == NULL)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);


    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to create GLEW" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Frame time and last key presses.
    float lastFrameTime = glfwGetTime();
    int lastMouseLeftState = GLFW_RELEASE;
    glfwGetCursorPos(window, &lastMousePosX, &lastMousePosY);

    previousZPress = GLFW_RELEASE;
    previousCPress = GLFW_RELEASE;

    initScene();
    initShadows();

    glUseProgram(texturedShaderProgram);
    glUniform1i(glGetUniformLocation(texturedShaderProgram, "render_shadows"), (int)true);
    glUseProgram(groundShaderProgram);
    glUniform1i(glGetUniformLocation(texturedShaderProgram, "render_shadows"), (int)true);

    glfwSetWindowSizeCallback(window, window_size_callback);

    // Entering Main Loop
    while (!glfwWindowShouldClose(window))
    {
        // Frame time calculation.
        dt = glfwGetTime() - lastFrameTime;
        lastFrameTime += dt;


        // Render frame.
        
        // Shadows.
        // Size viewport, clear buffers, and render the depth cubemap.
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);

        glCullFace(GL_FRONT);
        renderScene(shadowShaderProgram);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);


        // Scene.
        // Size viewport, clear buffers, and render the scene itself.
        glViewport(0, 0, windowWidth, windowHeigth);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Draw colourful lines.
        glUseProgram(colourShaderProgram);
        glBindVertexArray(lineVAO);
        DawAxisStar(colourShaderProgram);

        // Prepare textures.
        glUseProgram(texturedShaderProgram);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        GLuint textureLocation = glGetUniformLocation(texturedShaderProgram, "depthMap");
        glUniform1i(textureLocation, 0);

        glUseProgram(groundShaderProgram);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        textureLocation = glGetUniformLocation(groundShaderProgram, "depthMap");
        glUniform1i(textureLocation, 0);

        // Draw the main scene.
        glCullFace(GL_BACK);
        renderScene(groundShaderProgram);

        // End frame
        glfwSwapBuffers(window);


        // Save previous key presses.
        previousZPress = glfwGetKey(window, GLFW_KEY_Z);
        previousCPress = glfwGetKey(window, GLFW_KEY_C);
        previousYPress = glfwGetKey(window, GLFW_KEY_Y);
        previousIPress = glfwGetKey(window, GLFW_KEY_I);
        previousBPress = glfwGetKey(window, GLFW_KEY_B);

        handleInputs();
    }

    // Shutdown GLFW
    glfwTerminate();
    return 0;
}

void initScene()
{
    // Background colour.
    glClearColor(0.5f, 0.75f, 1.0f, 1.0f);


    // Load textures.
    snowTextureID = loadTexture("assets/textures/snow.png");
    carrotTextureID = loadTexture("assets/textures/carrot.png");
    stoneTextureID = loadTexture("assets/textures/stone.png");
    woodTextureID = loadTexture("assets/textures/wood.png");
    metalTextureID = loadTexture("assets/textures/metal.png");

    snowDepthTextureID = loadTexture("assets/textures/snowDepth.png");
    stoneDepthTextureID = loadTexture("assets/textures/stoneDepth.png");

    // Compile and link shaders.
    std::string shaderPathPrefix = "assets/shaders/";

    colourShaderProgram = loadSHADER(shaderPathPrefix + "vertexcolour_vertex.glsl", shaderPathPrefix + "vertexcolour_fragment.glsl");
    texturedShaderProgram = loadSHADER(shaderPathPrefix + "textured_vertex.glsl", shaderPathPrefix + "textured_fragment.glsl");
    groundShaderProgram = loadSHADER(shaderPathPrefix + "textured_vertex.glsl", shaderPathPrefix + "ground_fragment.glsl");

    shadowShaderProgram = loadSHADER(shaderPathPrefix + "shadow_vertex.glsl", shaderPathPrefix + "shadow_fragment.glsl"); //,  shaderPathPrefix + "shadow_geometry.glsl");

    allShaders.push_back(colourShaderProgram);
    allShaders.push_back(texturedShaderProgram);
    allShaders.push_back(groundShaderProgram);




    // Define and upload geometry to the GPU.
    cubeVAO = createCubeVBO();
    planeVAO = createPlaneVBO();
    lineVAO = createLinesVBO();
    sphereVAO = createSphereVBO(0.75f, 0.5f, sphereRadialDivs, sphereVerticalDivs);
    sphereVertexCount = sphereRadialDivs * sphereVerticalDivs * 6;
    groundVAO = createGroundVBO(groundSizeX, groundSizeZ, groundUVTiling);



    // Initial camera parameters.
    cameraTheta = radians(270.0f);
    cameraPhi = radians(10.0f);
    cameraRadius = 10.0f;
    cameraRotSpeed = radians(1.0f);


    // Set projection matrix for shaders.
    mat4 projectionMatrix = perspective(70.0f, // fov in degrees.
        (float)windowWidth / (float)windowHeigth, // aspect ratio.
        0.001f, 1000.0f); // near and far planes.


    // Set initial view matrix
    cameraPosition = updateCameraPosition(cameraTheta, cameraPhi, cameraRadius);
    cameraSideVector = cross(cameraLookAt, cameraUpVector);
    normalize(cameraSideVector);
    viewMatrix = lookAt(cameraPosition, cameraLookAt, cameraUpVector); // eye, center, up.


    // Set View and Projection matrices.
    setViewMatrix(allShaders, viewMatrix);
    setViewMatrix(groundShaderProgram, viewMatrix);
    setViewMatrix(shadowShaderProgram, viewMatrix);

    setProjectionMatrix(allShaders, projectionMatrix);
    setProjectionMatrix(groundShaderProgram, projectionMatrix);
    setProjectionMatrix(shadowShaderProgram, projectionMatrix);


    // Set parameters in textured shader program.

    SetUniformVec3(texturedShaderProgram, "view_position", cameraPosition);
    SetUniformVec3(groundShaderProgram, "view_position", cameraPosition);

    SetUniformVec3(texturedShaderProgram, "ambient_colour", ambientColour);
    SetUniformVec3(groundShaderProgram, "ambient_colour", ambientColour);

    SetUniformVec3(texturedShaderProgram, "light_color", lightColour);
    SetUniformVec3(groundShaderProgram, "light_color", lightColour);

    SetUniformVec3(texturedShaderProgram, "light_position", lightPosition);
    SetUniformVec3(groundShaderProgram, "light_position", lightPosition);
    SetUniformVec3(shadowShaderProgram, "light_position", lightPosition);

    SetUniform1Value(texturedShaderProgram, "light_near_plane", lightNearPlane);
    SetUniform1Value(texturedShaderProgram, "light_far_plane", lightFarPlane);

    SetUniform1Value(groundShaderProgram, "light_near_plane", lightNearPlane);
    SetUniform1Value(groundShaderProgram, "light_far_plane", lightFarPlane);

    SetUniform1Value(groundShaderProgram, "heightblend_factor", 0.5f);

    
    // Other OpenGL states to set once
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_PROGRAM_POINT_SIZE);
}

void initShadows() // All shadowcasting code references https://learnopengl.com/Advanced-Lighting/Shadows/Point-Shadows.
{
    glGenFramebuffers(1, &depthMapFBO);    

     // Create and configure the depth map.
    glGenTextures(1, &depthMap);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, depthMap);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    // Attach depth cubemap to framebuffer.
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Prepare light space transforms.
    lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, lightNearPlane, lightFarPlane);
    lightView = glm::lookAt(lightPosition, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
    lightSpaceMatrix = lightProjection * lightView;

    SetUniformMat4(shadowShaderProgram, "lightSpaceMatrix", lightSpaceMatrix);
    SetUniformMat4(texturedShaderProgram, "lightSpaceMatrix", lightSpaceMatrix);
    SetUniformMat4(groundShaderProgram, "lightSpaceMatrix", lightSpaceMatrix);

    // Set parameters in shadow shader program.
    glUseProgram(texturedShaderProgram);
    glUniform1i(glGetUniformLocation(texturedShaderProgram, "render_shadows"), useShadows);
    glUseProgram(groundShaderProgram);
    glUniform1i(glGetUniformLocation(groundShaderProgram, "render_shadows"), useShadows);

    SetUniform1Value(shadowShaderProgram, "light_near_plane", lightNearPlane);
    SetUniform1Value(shadowShaderProgram, "light_far_plane", lightFarPlane);
}

void renderScene(GLuint shaderProgram)
{
    // Draw geometry

    // Handle textures.
    glUseProgram(shaderProgram);

    // -> Texture A.
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, snowTextureID);
    GLuint textureLocation = glGetUniformLocation(shaderProgram, "textureSamplerA");
    glUniform1i(textureLocation, 1);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, snowDepthTextureID);
    textureLocation = glGetUniformLocation(shaderProgram, "depthSamplerA");
    glUniform1i(textureLocation, 3);

    // -> Texture B.
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, stoneTextureID);
    textureLocation = glGetUniformLocation(shaderProgram, "textureSamplerB");
    glUniform1i(textureLocation, 2);

    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, stoneDepthTextureID);
    textureLocation = glGetUniformLocation(shaderProgram, "depthSamplerB");
    glUniform1i(textureLocation, 4);



    // Draw floor grid.
    glBindVertexArray(groundVAO);
    
    // > Base.
    float groundCenterX = 0 - (float)groundSizeX / 2;
    float groundCenterZ = 0 - (float)groundSizeZ / 2;

    mat4 groundBaseMatrix = translate(mat4(1.0f), vec3(groundCenterX, 0.0f, groundCenterZ));
    GLuint worldMatrixLocation = glGetUniformLocation(shaderProgram, "worldMatrix");
    glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &groundBaseMatrix[0][0]);
    glDrawArrays(cubeRenderMode, 0, 6*groundSizeX*groundSizeZ);

    

    // Unbind vertex array.
    glBindVertexArray(0);
}


void handleInputs()
{
    // Detect inputs
    glfwPollEvents();

    // On Key Press actions.

    // -> Camera movements.
    // Pressing the right arrow key rotates the camera counterclockwise.
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
        if (cameraTheta < radians(360.0f))
            cameraTheta += cameraRotSpeed;
        else cameraTheta = radians(0.0f);

        cameraPosition = updateCameraPosition(cameraTheta, cameraPhi, cameraRadius);
        viewMatrix = lookAt(cameraPosition, cameraLookAt, cameraUpVector); // eye, center, up.

        setViewMatrix(groundShaderProgram, viewMatrix);
        //setViewMatrix(allShaders, viewMatrix);
    }
    // Pressing the left arrow key rotates the camera clockwise.
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    {
        if (cameraTheta > radians(0.0f))
            cameraTheta -= cameraRotSpeed;
        else cameraTheta = radians(360.0f);

        cameraPosition = updateCameraPosition(cameraTheta, cameraPhi, cameraRadius);
        viewMatrix = lookAt(cameraPosition, cameraLookAt, cameraUpVector); // eye, center, up.

        setViewMatrix(groundShaderProgram, viewMatrix);
        //setViewMatrix(allShaders, viewMatrix);
    }

    /// Pressing the up arrow key rotates the camera upward.
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        if (cameraPhi < radians(89.0f))
            cameraPhi += cameraRotSpeed;
        else cameraPhi = radians(89.0f);

        cameraPhi += cameraRotSpeed;

        cameraPosition = updateCameraPosition(cameraTheta, cameraPhi, cameraRadius);
        viewMatrix = lookAt(cameraPosition, cameraLookAt, cameraUpVector); // eye, center, up.

        setViewMatrix(groundShaderProgram, viewMatrix);
        //setViewMatrix(allShaders, viewMatrix);
    }
    /// Pressing the down arrow key rotates the camera downward.
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        if (cameraPhi > radians(1.0f))
            cameraPhi -= cameraRotSpeed;
        else cameraPhi = radians(1.0f);

        cameraPosition = updateCameraPosition(cameraTheta, cameraPhi, cameraRadius);
        viewMatrix = lookAt(cameraPosition, cameraLookAt, cameraUpVector); // eye, center, up.

        setViewMatrix(groundShaderProgram, viewMatrix);
        //setViewMatrix(allShaders, viewMatrix);
    }

    // Mouse position housekeeping.
    double mousePosX, mousePosY;
    glfwGetCursorPos(window, &mousePosX, &mousePosY);

    double dx = mousePosX - lastMousePosX;
    double dy = mousePosY - lastMousePosY;
    lastMousePosX = mousePosX;
    lastMousePosY = mousePosY;

    // Dragging the mouse while its left button is pressed controls the camera spherical coordinates' radius (zoom).
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        cameraRadius += dy * dt;

        cameraPosition = updateCameraPosition(cameraTheta, cameraPhi, cameraRadius);
        viewMatrix = lookAt(cameraPosition, cameraLookAt, cameraUpVector); // eye, center, up.

        setViewMatrix(groundShaderProgram, viewMatrix);
        //setViewMatrix(allShaders, viewMatrix);
    }

    // Dragging the mouse while its right button is pressed controls the camera lookat point's left/right position (pan).
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
    {
        cameraSideVector = cross(cameraPosition, cameraUpVector);
        normalize(cameraSideVector);

        float delta = dx * dt * 0.1f;
        cameraLookAt += cameraSideVector * delta;
        viewMatrix = lookAt(cameraPosition, cameraLookAt, cameraUpVector); // eye, center, up.

        setViewMatrix(groundShaderProgram, viewMatrix);
        //setViewMatrix(allShaders, viewMatrix);
    }

    // Dragging the mouse while its middle button is pressed controls the camera lookat point height (tilt).
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS)
    {
        if (cameraLookAt.y + (dy * dt) > 0)
            cameraLookAt.y += dy * dt;
        else cameraLookAt.y = 0;

        viewMatrix = lookAt(cameraPosition, cameraLookAt, cameraUpVector); // eye, center, up.

        setViewMatrix(groundShaderProgram, viewMatrix);
        //setViewMatrix(allShaders, viewMatrix);
    }

    /// Pressing the home button or 'H' key resets all camera parameters.
    if ((glfwGetKey(window, GLFW_KEY_HOME) || glfwGetKey(window, GLFW_KEY_H)) == GLFW_PRESS)
    {
        cameraTheta = radians(270.0f);
        cameraPhi = radians(10.0f);
        cameraRadius = 10.0f;

        cameraLookAt = vec3(0.0f);

        cameraPosition = updateCameraPosition(cameraTheta, cameraPhi, cameraRadius);
        viewMatrix = lookAt(cameraPosition, cameraLookAt, cameraUpVector); // eye, center, up.

        setViewMatrix(groundShaderProgram, viewMatrix);
        //setViewMatrix(allShaders, viewMatrix);
    }


    // -> Rendering modes.
    // Press 'P' to change the snowman's rendering mode to points.
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
    {
        cubeRenderMode = GL_POINTS;
    }
    // Press 'L' to change the snowman's rendering mode to lines.
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
    {
        cubeRenderMode = GL_LINE_LOOP;
    }
    // Press 'T' to change the snowman's rendering mode to triangles.
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
    {
        cubeRenderMode = GL_TRIANGLES;
    }

    // Press 'B' to switch whether shadows are rendered.
    if ((glfwGetKey(window, GLFW_KEY_B) == GLFW_RELEASE) && previousBPress == GLFW_PRESS)
    {
        useShadows = !useShadows;

        glUseProgram(texturedShaderProgram);
        glUniform1i(glGetUniformLocation(texturedShaderProgram, "render_shadows"), useShadows);

        glUseProgram(groundShaderProgram);
        glUniform1i(glGetUniformLocation(texturedShaderProgram, "render_shadows"), useShadows);
    }

    // Close the window if Escape is pressed.
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}


// TO DO:
// -> Shadows using two pass algorithm.

