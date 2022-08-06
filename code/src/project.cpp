//
// Final Project.
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
#include "light.h"
#include "Model.h"
#include "QuadModel.h"
#include "CubeModel.h"
#include "PlaneModel.h"
#include "GroundModel.h"
#include "SphereModel.h"

#define VECTOR_UP vec3(0.0f, 1.0f, 0.0f)

using namespace std;
using namespace glm;


GLuint loadTexture(const string filename)
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
	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrChannels, 0);
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


enum ECameraType
{
	FirstPerson,
	Rotating,
	Static
};

// Shaders.
GLuint colourShaderProgram;
GLuint texturedShaderProgram;
GLuint groundShaderProgram;
GLuint shadowShaderProgram;

std::vector<unsigned int> allShaderPrograms;


// Shader variable setters.

// Mat 4.
void SetUniformMat4(GLuint shader_id, const char* uniform_name, const mat4& uniform_value)
{
	glUseProgram(shader_id);
	glUniformMatrix4fv(glGetUniformLocation(shader_id, uniform_name), 1, GL_FALSE, &uniform_value[0][0]);
}
// Iterate the above on all shaders.
void SetUniformMat4(const char* uniform_name, const mat4& uniform_value)
{
	for (vector<unsigned int>::iterator currentShader = allShaderPrograms.begin(); currentShader < allShaderPrograms.end(); ++currentShader)
	{
		SetUniformMat4(*currentShader, uniform_name, uniform_value);
	}
}

// Vec 3.
void SetUniformVec3(GLuint shader_id, const char* uniform_name, vec3 uniform_value)
{
	glUseProgram(shader_id);
	glUniform3fv(glGetUniformLocation(shader_id, uniform_name), 1, value_ptr(uniform_value));
}
// Iterate the above on all shaders.
void SetUniformVec3(const char* uniform_name, vec3 uniform_value)
{
	for (vector<unsigned int>::iterator currentShader = allShaderPrograms.begin(); currentShader < allShaderPrograms.end(); ++currentShader)
	{
		SetUniformVec3(*currentShader, uniform_name, uniform_value);
	}
}

// Float.
template <class T>
void SetUniform1Value(GLuint shader_id, const char* uniform_name, T uniform_value)
{
	glUseProgram(shader_id);
	glUniform1f(glGetUniformLocation(shader_id, uniform_name), uniform_value);
	glUseProgram(0);
}
// Iterate the above on all shaders.
template <class T>
void SetUniform1Value(const char* uniform_name, T uniform_value)
{
	for (vector<unsigned int>::iterator currentShader = allShaderPrograms.begin(); currentShader < allShaderPrograms.end(); ++currentShader)
	{
		SetUniform1Value(*currentShader, uniform_name, uniform_value);
	}
}



// Matrix & Camera Functions.

void setProjectionMatrix(int shaderProgram, mat4 projectionMatrix)
{
	glUseProgram(shaderProgram);
	GLuint projectionMatrixLocation = glGetUniformLocation(shaderProgram, "projectionMatrix");
	glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);
}

// To do : fix the functions using the allShaders vector so they actually update stuff.
void setProjectionMatrix(mat4 projectionMatrix)
{
	for (vector<unsigned int>::iterator currentShader = allShaderPrograms.begin(); currentShader < allShaderPrograms.end(); ++currentShader)
	{
		setProjectionMatrix(*currentShader, projectionMatrix);
	}
}

void setViewMatrix(int shaderProgram, mat4 viewMatrix)
{
	glUseProgram(shaderProgram);
	GLuint viewMatrixLocation = glGetUniformLocation(shaderProgram, "viewMatrix");
	glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
}

void setViewMatrix(mat4 viewMatrix)
{
	for (vector<unsigned int>::iterator currentShader = allShaderPrograms.begin(); currentShader < allShaderPrograms.end(); ++currentShader)
	{
		setViewMatrix(*currentShader, viewMatrix);
	}
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
void setUpLightForShadows(Light light);
void renderScene(GLuint shaderProgram);
void handleInputs();
void Update(float delta);


// Textures.
GLuint groundHighTextureID;
GLuint groundLowTextureID;
GLuint stoneTextureID;
GLuint woodTextureID;
GLuint metalTextureID;
GLuint grassTextureID;
GLuint moonTextureID;

GLuint groundHighDepthTextureID;
GLuint groundLowDepthTextureID;

GLuint groundHighNormalTextureID;
GLuint groundLowNormalTextureID;
GLuint grassNormalTextureID;

// Meshes.
int cubeVAO;
int planeVAO;
int lineVAO;
int sphereVAO;
int quadVAO;

// Additional info for spheres.
int sphereRadialDivs = 16;
int sphereVerticalDivs = 16;
int sphereVertexCount;

ECameraType cameraType = FirstPerson;

// Light parameters.
vec3 ambientColour = vec3(0.5, 0.5, 0.55);
Light sunLight;

// Shadow data.
bool useShadows = true;
// X rotation on Quad, uses SQRT
bool quadXRotation = true;

const unsigned int SHADOW_TEXTURE_WIDTH = 1024, SHADOW_TEXTURE_HEIGHT = 1024;
GLuint shadowMapTexture;
GLuint shadowMapFBO;
float aspect;

vec3 gravityVector(0.0f, -0.5f, 0.0f);

// Camera parameters.
float cameraTheta;
float cameraPhi;
float cameraRadius;
float cameraRotSpeed;
float cameraSpeed = 1.5f;
float cameraFastSpeed = 2 * cameraSpeed;
float currentCameraSpeed = 1.5f;
const float cameraAngularSpeed = 30.0f;
float cameraHorizontalAngle = 90.0f;
float cameraVerticalAngle = 0.0f;

// Camera matrixes.
vec3 cameraPosition;
vec3 cameraLookAt(0.0f, 0.0f, 0.0f);
vec3 cameraUpVector(0.0f, 1.0f, 0.0f);
vec3 cameraDirection(0.0f);

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
int previousFPress;
int previousPPress;
int previousLPress;
int previousTPress;
int previous1Press;
int previous2Press;
int previous3Press;
int previous4Press;

float dt;
float spinning = 0.0f;

vector<Model*> objects;
vector<QuadModel*> quads;

QuadModel* quad;
CubeModel* cubeBase;
CubeModel* cube;
CubeModel* cube2;
CubeModel* cube3;
CubeModel* cube4;
SphereModel* sphere;
SphereModel* cameraBoundingSphere;
GroundModel* ground;

// Ground info
GLuint groundSizeX = 50;
GLuint groundSizeZ = 50;
float groundUVTiling = 10.0f;


// Handle window resizing.
void window_size_callback(GLFWwindow* window, int width, int height)
{
	glfwGetWindowSize(window, &windowWidth, &windowHeigth);

	// Set projection matrix for shaders.
	mat4 projectionMatrix = perspective(70.0f, // fov in degrees.
		(float)width / (float)height, // aspect ratio.
		0.001f, 1000.0f); // near and far planes.
	setProjectionMatrix(projectionMatrix);

	//cout << "Window resized.\n";
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
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK)
	{
		std::cerr << "Failed to create GLEW" << std::endl;
		glfwTerminate();
		return -1;
	}

	// Frame time and last key presses.
	float lastFrameTime = glfwGetTime();
	glfwGetCursorPos(window, &lastMousePosX, &lastMousePosY);

	int lastMouseLeftState = GLFW_RELEASE;
	previousZPress = GLFW_RELEASE;
	previousCPress = GLFW_RELEASE;
	previousFPress = GLFW_RELEASE;
	previousPPress = GLFW_RELEASE;
	previousLPress = GLFW_RELEASE;
	previousTPress = GLFW_RELEASE;
	previousFPress = GLFW_RELEASE;
	previous1Press = GLFW_RELEASE;
	previous2Press = GLFW_RELEASE;
	previous3Press = GLFW_RELEASE;
	previous4Press = GLFW_RELEASE;

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

		Update(dt);

		// Render frame.

		// Shadows.
		// Size viewport, clear buffers, and render the depth cubemap.
		glViewport(0, 0, SHADOW_TEXTURE_WIDTH, SHADOW_TEXTURE_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
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
		// To do : either move this to its own function, or make it a loop over all relevant shaders.
		glUseProgram(texturedShaderProgram);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, shadowMapTexture);
		GLuint textureLocation = glGetUniformLocation(texturedShaderProgram, "shadowMap");
		glUniform1i(textureLocation, 0);

		glUseProgram(groundShaderProgram);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, shadowMapTexture);
		textureLocation = glGetUniformLocation(groundShaderProgram, "shadowMap");
		glUniform1i(textureLocation, 0);

		// Draw the main scene.
		glCullFace(GL_BACK);
		renderScene(groundShaderProgram);

		// End frame
		glfwSwapBuffers(window);

		handleInputs();

		// Save previous key presses.
		previousZPress = glfwGetKey(window, GLFW_KEY_Z);
		previousCPress = glfwGetKey(window, GLFW_KEY_C);
		previousYPress = glfwGetKey(window, GLFW_KEY_Y);
		previousIPress = glfwGetKey(window, GLFW_KEY_I);
		previousBPress = glfwGetKey(window, GLFW_KEY_B);
		previousPPress = glfwGetKey(window, GLFW_KEY_P);
		previousLPress = glfwGetKey(window, GLFW_KEY_L);
		previousTPress = glfwGetKey(window, GLFW_KEY_T);
		previous1Press = glfwGetKey(window, GLFW_KEY_1);
		previous2Press = glfwGetKey(window, GLFW_KEY_2);
		previous3Press = glfwGetKey(window, GLFW_KEY_3);
		previous4Press = glfwGetKey(window, GLFW_KEY_4);
	}

	// Shutdown GLFW
	glfwTerminate();
	return 0;
}

void initScene()
{
	// Background colour.
	glClearColor(0.5f, 0.75f, 1.0f, 1.0f);

	cout << "LOADING TEXTURES\n";
	// Load textures.
	const string texturePathPrefix = "assets/textures/";
	const string shaderPathPrefix = "assets/shaders/";

	groundHighTextureID = loadTexture(texturePathPrefix + "snow.png");
	groundLowTextureID = loadTexture(texturePathPrefix + "carrot.png");
	stoneTextureID = loadTexture(texturePathPrefix + "stone.png");
	woodTextureID = loadTexture(texturePathPrefix + "wood.png");
	metalTextureID = loadTexture(texturePathPrefix + "metal.png");
	grassTextureID = loadTexture(texturePathPrefix + "grass.png");
	moonTextureID = loadTexture(texturePathPrefix + "moon.png");

	groundHighDepthTextureID = loadTexture("assets/textures/snowDepth.png");
	groundLowDepthTextureID = loadTexture("assets/textures/stoneDepth.png");

	groundHighNormalTextureID = loadTexture("assets/textures/groundHighNormal.png");
	groundLowNormalTextureID = loadTexture("assets/textures/groundLowNormal.png");

	// Compile and link shaders.
	colourShaderProgram = loadSHADER(shaderPathPrefix + "vertexcolour_vertex.glsl", shaderPathPrefix + "vertexcolour_fragment.glsl");
	texturedShaderProgram = loadSHADER(shaderPathPrefix + "textured_vertex.glsl", shaderPathPrefix + "textured_fragment.glsl");
	groundShaderProgram = loadSHADER(shaderPathPrefix + "textured_vertex.glsl", shaderPathPrefix + "ground_fragment.glsl");
	shadowShaderProgram = loadSHADER(shaderPathPrefix + "shadow_vertex.glsl", shaderPathPrefix + "shadow_fragment.glsl");

	// Collect shaders into a vector for ease of iteration.
	allShaderPrograms.push_back(colourShaderProgram);
	allShaderPrograms.push_back(texturedShaderProgram);
	allShaderPrograms.push_back(groundShaderProgram);
	allShaderPrograms.push_back(shadowShaderProgram);


	// Define and upload geometry to the GPU.
	cubeVAO = CubeModel::CubeModelVAO();
	planeVAO = PlaneModel::PlaneModelVAO();
	sphereVAO = SphereModel::SphereModelVAO(1.0f, 0.5f, sphereRadialDivs, sphereVerticalDivs, sphereVertexCount);
	quadVAO = QuadModel::QuadModelVAO();
	//cubeVAO = createCubeVBO();
	//planeVAO = createPlaneVBO();
	lineVAO = createLinesVBO();
	//sphereVAO = createSphereVBO(0.75f, 0.5f, sphereRadialDivs, sphereVerticalDivs);
	//sphereVertexCount = sphereRadialDivs * sphereVerticalDivs * 6;
	//groundVAO = createGroundVBO(groundSizeX, groundSizeZ, groundUVTiling);

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
	setViewMatrix(viewMatrix);
	setProjectionMatrix(projectionMatrix);


	// Initialize main light.
	sunLight = Light(vec3(0.95f, 0.95f, 1.0f), vec3(0.0f, 5.0f, 5.0f), vec3(0.0f, 1.0f, 0.0f));

	setUpLightForShadows(sunLight);

	// Set constant light related parameters in texture shader.
	SetUniformVec3(texturedShaderProgram, "light_color", sunLight.color);
	SetUniformVec3(groundShaderProgram, "light_color", sunLight.color);


	// Set other parameters in textured shader program.
	// To do: loop these over shaders.

	SetUniformVec3(texturedShaderProgram, "view_position", cameraPosition);
	SetUniformVec3(groundShaderProgram, "view_position", cameraPosition);

	SetUniformVec3(texturedShaderProgram, "ambient_colour", ambientColour);
	SetUniformVec3(groundShaderProgram, "ambient_colour", ambientColour);

	SetUniform1Value(texturedShaderProgram, "light_near_plane", light_near_plane);
	SetUniform1Value(texturedShaderProgram, "light_far_plane", light_far_plane);

	SetUniform1Value(groundShaderProgram, "light_near_plane", light_near_plane);
	SetUniform1Value(groundShaderProgram, "light_far_plane", light_far_plane);

	SetUniform1Value(groundShaderProgram, "heightblend_factor", 0.45f);

	quad = new QuadModel(vec3(2.0f, 0.7f, 2.0f), vec3(0.0f), vec3(0.3f));
	cubeBase = new CubeModel(vec3(0.0f, 2.0f, 0.0f), vec3(0.0f), vec3(1.0f));

	cube = new CubeModel(vec3(0.0f, 5.0f, 0.0f), vec3(0.0f, 45.0f, 45.0f), vec3(2.0f));
	cube2 = new CubeModel(vec3(3.0f, 0.0f, -3.0f), vec3(0.0f), vec3(1.5f));
	cube3 = new CubeModel(vec3(2.0f, 0.5f, 2.0f), vec3(0.0f), vec3(1.0f));
	cube4 = new CubeModel(vec3(-3.0f, 0.0f, 3.0f), vec3(0.0f), vec3(2.0f));
	
	sphere = new SphereModel(vec3(20.0f, 15.0f, 20.0f), vec3(0.0f), vec3(2.5f));
	cameraBoundingSphere = new SphereModel(cameraPosition, vec3(0.0f), vec3(1.0f));
	ground = new GroundModel(groundSizeX, groundSizeZ, groundUVTiling);

	quads.push_back(quad);

	objects.push_back(cubeBase);
	objects.push_back(cube);
	objects.push_back(cube2);
	objects.push_back(cube3);
	objects.push_back(cube4);
	objects.push_back(sphere);
	objects.push_back(ground);

	// Other OpenGL states to set once
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_PROGRAM_POINT_SIZE);
	glEnable(GL_BLEND);
}

void setUpLightForShadows(Light light)
{
	// Set the light projection matrix in the textured shader.
	SetUniformMat4(texturedShaderProgram, "lightSpaceMatrix", light.lightSpaceMatrix);
	SetUniformMat4(groundShaderProgram, "lightSpaceMatrix", light.lightSpaceMatrix);

	SetUniformVec3(texturedShaderProgram, "light_position", light.position);
	SetUniformVec3(groundShaderProgram, "light_position", light.position);

	SetUniformVec3(texturedShaderProgram, "light_direction", light.direction);
	SetUniformVec3(groundShaderProgram, "light_direction", light.direction);

	// Set light related parameters in shadow shader.
	glUseProgram(shadowShaderProgram);
	SetUniformMat4(shadowShaderProgram, "lightSpaceMatrix", light.lightSpaceMatrix);
}

void initShadows() // All shadowcasting code references https://learnopengl.com/Advanced-Lighting/Shadows/Point-Shadows.
{
	// Create and configure the depth map.
	glGenTextures(1, &shadowMapTexture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, shadowMapTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_TEXTURE_WIDTH, SHADOW_TEXTURE_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Attach depth cubemap to framebuffer.
	glGenFramebuffers(1, &shadowMapFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMapTexture, 0);
	glDrawBuffer(GL_NONE);


	// Set up light clip information in shadow shader.
	glUseProgram(shadowShaderProgram);
	SetUniform1Value(shadowShaderProgram, "light_near_plane", light_near_plane);
	SetUniform1Value(shadowShaderProgram, "light_far_plane", light_far_plane);
}


void renderScene(GLuint shaderProgram)
{
	// Update light.


	// Draw geometry

	// Handle textures.
	glUseProgram(shaderProgram);

	// Set ground shader textures.
	// -> Texture A.
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, groundLowTextureID);
	GLuint textureLocation = glGetUniformLocation(shaderProgram, "textureSamplerA");
	glUniform1i(textureLocation, 1);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, groundLowDepthTextureID);
	textureLocation = glGetUniformLocation(shaderProgram, "depthSamplerA");
	glUniform1i(textureLocation, 3);

	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, groundLowNormalTextureID);
	textureLocation = glGetUniformLocation(shaderProgram, "normalSamplerA");
	glUniform1i(textureLocation, 5);

	// -> Texture B.
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, groundHighTextureID);
	textureLocation = glGetUniformLocation(shaderProgram, "textureSamplerB");
	glUniform1i(textureLocation, 2);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, groundHighDepthTextureID);
	textureLocation = glGetUniformLocation(shaderProgram, "depthSamplerB");
	glUniform1i(textureLocation, 4);

	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, groundHighNormalTextureID);
	textureLocation = glGetUniformLocation(shaderProgram, "normalSamplerB");
	glUniform1i(textureLocation, 6);


	// Draw ground. Object has it's own VAO
	ground->Draw(shaderProgram, meshRenderMode);


	// Render objects.

	if (shaderProgram != shadowShaderProgram)
	{
		//cout << "Not in shadow pass.\n";
		shaderProgram = texturedShaderProgram;
		glUseProgram(shaderProgram);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, woodTextureID);
		GLuint textureLocation = glGetUniformLocation(shaderProgram, "textureSampler");
		glUniform1i(textureLocation, 0);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, groundLowNormalTextureID);
		textureLocation = glGetUniformLocation(shaderProgram, "normalSampler");
		glUniform1i(textureLocation, 2);
	}

	//
	glBindVertexArray(cubeVAO);

	//cube->UpdatePosition(vec3(sinf(glfwGetTime()) / 10.0f, 0.0f, 0.0f));
	cube->Draw(shaderProgram, meshRenderMode);
	cube2->Draw(shaderProgram, meshRenderMode);
	cube3->Draw(shaderProgram, meshRenderMode);
	cube4->Draw(shaderProgram, meshRenderMode);

	cubeBase->UpdateRotation(vec3(0.0f, 1.0f * dt, 0.0f));
	cubeBase->Draw(shaderProgram, meshRenderMode);

	if (shaderProgram != shadowShaderProgram)
	{
		//cout << "Not in shadow pass.\n";
		shaderProgram = texturedShaderProgram;
		glUseProgram(shaderProgram);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, moonTextureID);
		GLuint textureLocation = glGetUniformLocation(shaderProgram, "textureSampler");
		glUniform1i(textureLocation, 0);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, moonTextureID);
		textureLocation = glGetUniformLocation(shaderProgram, "normalSampler");
		glUniform1i(textureLocation, 2);
	}

	glBindVertexArray(sphereVAO);

	spinning += 10.0f * dt;
	mat4 center = translate(mat4(1.0f), vec3(0.0f)) * rotate(mat4(1.0f), radians(spinning), VECTOR_UP);

	sphere->UpdateRotation(vec3(0.0f, radians(0.5f), 0.0f));
	sphere->SetParent(center);

	//sphere->Draw(shaderProgram, sphereVertexCount);
	sphere->Draw(shaderProgram, sphereVertexCount, meshRenderMode);

	glBindVertexArray(quadVAO);

	if (shaderProgram != shadowShaderProgram)
	{
		//cout << "Not in shadow pass.\n";
		shaderProgram = texturedShaderProgram;
		glUseProgram(shaderProgram);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, grassTextureID);
		GLuint textureLocation = glGetUniformLocation(shaderProgram, "textureSampler");
		glUniform1i(textureLocation, 0);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, grassTextureID);
		textureLocation = glGetUniformLocation(shaderProgram, "normalSampler");
		glUniform1i(textureLocation, 2);
	}

	//quad->UpdateRotation(vec3(0.0f, 0.0f, 1.0f * dt));
	quad->Draw(shaderProgram, meshRenderMode);
	//// > Base.
	//float groundCenterX{ 0 - (float)groundSizeX / 2 };
	//float groundCenterZ{ 0 - (float)groundSizeZ / 2 };

	//mat4 groundBaseMatrix = translate(mat4(1.0f), vec3(groundCenterX, 0.0f, groundCenterZ));
	//GLuint worldMatrixLocation = glGetUniformLocation(shaderProgram, "worldMatrix");
	//glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &groundBaseMatrix[0][0]);
	//glDrawArrays(meshRenderMode, 0, 6 * groundSizeX * groundSizeZ);

	// Objects will likely have to inherit the ground's transform as to follow its coordinates.
	// Or we only draw the world in the positive X, positive Z quadrants.

	//mat4 cubeBaseMatrix = translate(mat4(1.0f), vec3(0.0f, 2.0f, 0.0f));
	//GLuint worldMatrixLocation = glGetUniformLocation(shaderProgram, "worldMatrix");
	//glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &cubeBaseMatrix[0][0]);
	//glDrawArrays(meshRenderMode, 0, 36);

	// Unbind vertex array.
	glBindVertexArray(0);
}


// To do: better camera controls.
void handleInputs()
{
	// Detect inputs
	glfwPollEvents();

	// Toggles first Person Camera
	if (previous1Press == GLFW_RELEASE && glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
	{
		cameraType = FirstPerson;

		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}

	if (previous2Press == GLFW_RELEASE && glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
	{
		cameraPosition = { 20.0f, 30.0f, 20.0f };
		cameraLookAt = { 0.0f, 0.0f, 0.0f };

		cameraType = Static;
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	}

	if (previous3Press == GLFW_RELEASE && glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
	{
		cameraPosition = { -30.0f, 30.0f, -20.0f };
		cameraLookAt = { 0.0f, 0.0f, 0.0f };

		cameraType = Static;
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}

	if (previous4Press == GLFW_RELEASE && glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
	{
		cameraLookAt = { 0.0f, 0.0f, 0.0f };

		cameraType = Rotating;
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}

	// First Person Camera
	if (cameraType == FirstPerson)
	{
		// On Key Press actions.
		bool fastCam = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;
		currentCameraSpeed = (fastCam) ? cameraFastSpeed : cameraSpeed;

		// Mouse position housekeeping.
		double mousePosX, mousePosY;
		glfwGetCursorPos(window, &mousePosX, &mousePosY);

		double dx = mousePosX - lastMousePosX;
		double dy = mousePosY - lastMousePosY;
		lastMousePosX = mousePosX;
		lastMousePosY = mousePosY;

		cameraHorizontalAngle -= dx * cameraAngularSpeed * dt;
		cameraVerticalAngle -= dy * cameraAngularSpeed * dt;

		// Clamp vertical angle to [-85, 85] degrees
		cameraVerticalAngle = std::max(-85.0f, std::min(85.0f, cameraVerticalAngle));
		if (cameraHorizontalAngle > 360)
			cameraHorizontalAngle -= 360;
		else if (cameraHorizontalAngle < -360)
			cameraHorizontalAngle += 360;

		float theta = radians(cameraHorizontalAngle);
		float phi = radians(cameraVerticalAngle);

		cameraLookAt = vec3(cosf(phi) * cosf(theta), sinf(phi), -cosf(phi) * sinf(theta));
		vec3 cameraSideVector = glm::cross(cameraLookAt, vec3(0.0f, 1.0f, 0.0f));
		glm::normalize(cameraSideVector);


		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
			cameraPosition += cameraLookAt * dt * currentCameraSpeed;
			cameraDirection.z = 1.0f;
		}

		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
			cameraPosition -= cameraLookAt * dt * currentCameraSpeed;
			cameraDirection.z = -1.0f;
		}

		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
			cameraPosition += cameraSideVector * dt * currentCameraSpeed;
			cameraDirection.x = 1.0f;
		}

		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
			cameraPosition -= cameraSideVector * dt * currentCameraSpeed;
			cameraDirection.x = -1.0f;
		}

		if (glfwGetKey(window, GLFW_KEY_W) != GLFW_PRESS && glfwGetKey(window, GLFW_KEY_S) != GLFW_PRESS){
			cameraDirection.z = 0.0f;
		}

		if (glfwGetKey(window, GLFW_KEY_D) != GLFW_PRESS && glfwGetKey(window, GLFW_KEY_A) != GLFW_PRESS) {
			cameraDirection.x = 0.0f;
		}

		// Pressing the home button or 'H' key resets all camera parameters.
		if ((glfwGetKey(window, GLFW_KEY_HOME) || glfwGetKey(window, GLFW_KEY_H)) == GLFW_PRESS)
		{
			cameraLookAt = { 0.0f, 0.0f, -1.0f };
			cameraUpVector = VECTOR_UP;

			// initial orientation
			cameraHorizontalAngle = 90.0f;
			cameraVerticalAngle = 0.0f;

			cameraPosition = { 0.0f, 5.0f, 20.0f };
		}
	}

	// Depending on the camera the view Matrix updates accordingly
	switch (cameraType)
	{
	case FirstPerson:
		viewMatrix = lookAt(cameraPosition, cameraPosition + cameraLookAt, VECTOR_UP);
		break;
	case Rotating:
		cameraTheta += radians(45.0f) * dt;
		cameraPhi = radians(30.0f);

		cameraPosition = updateCameraPosition(cameraTheta, cameraPhi, 40.0f);

		viewMatrix = lookAt(cameraPosition, cameraLookAt, VECTOR_UP);
		break;
	case Static:
		viewMatrix = lookAt(cameraPosition, cameraLookAt, VECTOR_UP);
		break;
	default:
		break;
	}

	setViewMatrix(viewMatrix);

	// -> Rendering modes.
	// Press 'P' to change the snowman's rendering mode to points.
	if (previousPPress == GLFW_RELEASE && glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
	{
		meshRenderMode = GL_POINTS;
	}
	// Press 'L' to change the snowman's rendering mode to lines.
	if (previousLPress == GLFW_RELEASE && glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
	{
		meshRenderMode = GL_LINE_LOOP;
	}
	// Press 'T' to change the snowman's rendering mode to triangles.
	if (previousTPress == GLFW_RELEASE && glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
	{
		meshRenderMode = GL_TRIANGLES;
	}

	// Close the window if Escape is pressed.
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void Update(float delta)
{
	// applying gravity to the camera
	if (cameraType == FirstPerson) cameraPosition += gravityVector;

	cameraBoundingSphere->SetPosition(cameraPosition);

	vec3 cameraSideVector = cross(cameraLookAt, VECTOR_UP);
	normalize(cameraSideVector);

	//Check collisions
	for (vector<Model*>::iterator it = objects.begin(); it < objects.end(); ++it)
	{
		//Camera Collisions with ground
		if (*it == ground)
		{
			GroundModel* ground = dynamic_cast<GroundModel*>(*it);

			float groundHeight = ground->returnHeightAtPoint(vec2(cameraPosition.x + (float)groundSizeX / 2, cameraPosition.z + (float)groundSizeZ / 2));
			if (cameraPosition.y < groundHeight + cameraBoundingSphere->GetScaling().x)
			{
				if (cameraType == FirstPerson)
				{
					cameraPosition.y = groundHeight + cameraBoundingSphere->GetScaling().x;
					// Tried Lerping but it was more involved that anticipated, therefore just comented it
					//cameraPosition.y = std::lerp(cameraPosition.y, groundHeight + cameraBoundingSphere->GetScaling().x, dt * 20.0f);
				}
			}
		}

		if (dynamic_cast<CubeModel*>(*it))
		{
			CubeModel* cube = dynamic_cast<CubeModel*>(*it);
			if (cube->ContainsPoint(cameraBoundingSphere->GetPosition(), cameraBoundingSphere->GetScaling().x))
			{
				// Inverts camera direction and add the cubes height to the y
				cameraPosition += dt * currentCameraSpeed * ((-cameraLookAt * cameraDirection.z) + (cameraDirection.x * -cameraSideVector) + (VECTOR_UP * (cube->GetPosition().y + cube->GetScaling().y)));
			}
		}
	}

	// Update quads to billboards
	for (QuadModel* quad : quads)
	{
		vec3 lookAtDiff{ quad->GetPosition() - cameraPosition };
		float yRotation{ atan2(lookAtDiff.x, lookAtDiff.z) };

		float xRotation{ 0.0f };
		if (quadXRotation)
		{
			xRotation = atan2(lookAtDiff.y, sqrt(lookAtDiff.x * lookAtDiff.x + lookAtDiff.z * lookAtDiff.z));
			xRotation = std::max(0.0f, std::min(20.0f, xRotation));
		}

		quad->SetRotation(vec3(xRotation, yRotation, 0.0f));
	}

	// Update models
	for (Model* model : objects)
	{
		model->Update(delta);
	}

	//for (vector<Model*>::iterator it = objects.begin(); it < objects.end(); ++it)
	//{
	//	(*it)->Update(delta);
	//}
}

