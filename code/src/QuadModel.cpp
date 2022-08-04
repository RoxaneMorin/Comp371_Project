#include "QuadModel.h"

#include <glm/gtx/transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/common.hpp>
#include <limits>
#include <vector>

using namespace glm;
using namespace std;

unsigned int QuadModel::QuadModelVAO()
{
	TexturedColoredNormalVertex texturedCubeVertexArray[] = {
			TexturedColoredNormalVertex(vec3(-0.5, 0.5, 0),		vec3(1.0f, 0.0f, 0.0f),	vec2(1.0f, 0.0f),	vec3(0.0f, 0.0f, -1.0f)),
			TexturedColoredNormalVertex(vec3(0.5, 0.5, 0),		vec3(1.0f, 0.0f, 0.0f),	vec2(0.0f, 0.0f),	vec3(0.0f, 0.0f, -1.0f)),
			TexturedColoredNormalVertex(vec3(-0.5, -0.5, 0),	vec3(1.0f, 0.0f, 0.0f),	vec2(1.0f, 1.0f),	vec3(0.0f, 0.0f, -1.0f)),

			TexturedColoredNormalVertex(vec3(-0.5, -0.5, 0),	vec3(1.0f, 0.0f, 0.0f),	vec2(1.0f, 1.0f),	vec3(0.0f, 0.0f, -1.0f)),
			TexturedColoredNormalVertex(vec3(0.5, 0.5, 0),		vec3(1.0f, 0.0f, 0.0f),	vec2(0.0f, 0.0f),	vec3(0.0f, 0.0f, -1.0f)),
			TexturedColoredNormalVertex(vec3(0.5, -0.5, 0),		vec3(1.0f, 0.0f, 0.0f),	vec2(0.0f, 1.0f),	vec3(0.0f, 0.0f, -1.0f))
	};

	// Create a vertex array
	GLuint vertexArrayObject;
	glGenVertexArrays(1, &vertexArrayObject);
	glBindVertexArray(vertexArrayObject);

	// Upload Vertex Buffer to GPU, save reference.
	GLuint vertexBufferObject;
	glGenBuffers(1, &vertexBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texturedCubeVertexArray), texturedCubeVertexArray, GL_STATIC_DRAW);

	glVertexAttribPointer(0,                   // attribute 0 matches aPos in Vertex Shader
		3,                   // size
		GL_FLOAT,            // type
		GL_FALSE,            // normalized?
		sizeof(TexturedColoredNormalVertex), // stride - each vertex contain 2 vec3 (position, color)
		(void*)0             // array buffer offset
	);
	glEnableVertexAttribArray(0);


	glVertexAttribPointer(1,                            // attribute 1 matches aColor in Vertex Shader
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(TexturedColoredNormalVertex),
		(void*)sizeof(vec3)      // color is offseted a vec3 (comes after position)
	);
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2,                            // attribute 2 matches aUV in Vertex Shader
		2,
		GL_FLOAT,
		GL_FALSE,
		sizeof(TexturedColoredNormalVertex),
		(void*)(2 * sizeof(vec3))      // uv is offseted by 2 vec3 (comes after position and color)
	);
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(3,                            // attribute 3 matches aNormals in Vertex Shader
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(TexturedColoredNormalVertex),
		(void*)(2 * sizeof(vec3) + sizeof(vec2))    // normals are offsetted by two vec3 and a vec2.
	);
	glEnableVertexAttribArray(3);

	return vertexArrayObject;
}

QuadModel::QuadModel() : Model() { }

QuadModel::QuadModel(vec3 position, vec3 rotation, vec3 scale) : Model(position, rotation, scale) { }

QuadModel::QuadModel(vec3 position, vec3 rotation, vec3 scale, mat4 parent) : Model(position, rotation, scale, parent) { }


QuadModel::~QuadModel()
{
	// Free the GPU from the Vertex Buffer
	glDeleteBuffers(1, &mVBO);
	glDeleteVertexArrays(1, &mVAO);
}

void QuadModel::Update(float dt)
{
}

void QuadModel::Draw(int shaderProgram, GLenum renderingMode)
{
	GLuint WorldMatrixLocation = glGetUniformLocation(shaderProgram, "worldMatrix");
	glUniformMatrix4fv(WorldMatrixLocation, 1, GL_FALSE, &GetWorldMatrix()[0][0]);

	// Draw the triangles 
	glDrawArrays(renderingMode, 0, 6);
}

//Using an oriented bounding box (OBB)
bool QuadModel::ContainsPoint(glm::vec3 position)
{
	vec4 worldSpacePoint(position, 1.0f);

	//We will first transform the position into Cube Model Space
	mat4 mTranslate = translate(GetPosition());
	mat4 mRotation = mat4_cast(quat(mRotation));
	mat4 mScale = scale(GetScaling());

	glm::vec3 cubeModelSpacePoint = inverse(mTranslate * mRotation * mScale) * worldSpacePoint;

	//Then we simply check if the transformed point lies inside a unit cube at the origin
	return
		cubeModelSpacePoint.x >= -0.5f && cubeModelSpacePoint.x <= 0.5f &&
		cubeModelSpacePoint.y >= -0.5f && cubeModelSpacePoint.y <= 0.5f &&
		cubeModelSpacePoint.z >= -0.5f && cubeModelSpacePoint.z <= 0.5f;
}

//UNIMPLEMENTED
bool QuadModel::IntersectsPlane(glm::vec3 planePoint, glm::vec3 planeNormal)
{
	return false;
}