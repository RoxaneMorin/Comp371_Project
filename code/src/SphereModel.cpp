#include "SphereModel.h"

#include <vector>

#include <glm/gtx/transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/common.hpp>
#include <limits>

#define GLEW_STATIC 1   // This allows linking with Static Library on Windows, without DLL
#include <GL/glew.h>    // Include GLEW - OpenGL Extension Wrangler

unsigned int SphereModel::SphereModelVAO(float radius, float heightOffset, int radialSubdivisions, int verticalSubdivisions, int& numOfVertices)
{
	std::vector<TexturedColoredNormalVertex> texturedSphereVertexVector;

	// Generate the vertices.
	// I originally referenced the code from http://www.songho.ca/opengl/gl_sphere.html, then completely redid the triangle drawing bits.

	const float PI = acos(-1);

	float x, y, z; // Vertex position.
	float u, v; // UV coordinate.
	float nx, ny, nz, lengthInv = 1.0f / radius; // Normal, will also be used as vertex colour.

	float radialStep = 2 * PI / radialSubdivisions;
	float verticalStep = PI / verticalSubdivisions;
	float radialAngle = 0.0f;
	float verticalAngle = 0.0f;

	// Sphere top cap.
	for (int i = 0; i < radialSubdivisions; i++)
	{
		float angleFromTop = verticalStep;

		// Summit
		// Vertex positions.
		x = 0;
		y = radius + heightOffset;
		z = 0;

		// Normalized vertex normal.
		nx = x * lengthInv;
		ny = y * lengthInv;
		nz = z * lengthInv;

		// UV coordinates.
		u = ((float)i / radialSubdivisions) + (1.0f / (2 * radialSubdivisions));
		v = 0.0f;

		TexturedColoredNormalVertex newVertex = TexturedColoredNormalVertex(vec3(x, y, z), vec3(nx, ny, nz), vec2(u, v), vec3(nx, ny, nz));
		texturedSphereVertexVector.push_back(newVertex);

		// Bottom right vertex.
		// Vertex positions.
		x = radius * cos(radialAngle + radialStep) * sin(angleFromTop);
		y = radius * cos(angleFromTop) + heightOffset;
		z = radius * sin(radialAngle + radialStep) * sin(angleFromTop);

		// Normalized vertex normal.
		nx = x * lengthInv;
		ny = y * lengthInv;
		nz = z * lengthInv;

		// UV coordinates.
		u = ((float)i + 1.0f) / radialSubdivisions;
		v = 1.0f / verticalSubdivisions;

		newVertex = TexturedColoredNormalVertex(vec3(x, y, z), vec3(nx, ny, nz), vec2(u, v), vec3(nx, ny, nz));
		texturedSphereVertexVector.push_back(newVertex);

		// Bottom left vertex.
		// Vertex positions.
		x = radius * cos(radialAngle) * sin(angleFromTop);
		y = radius * cos(angleFromTop) + heightOffset;
		z = radius * sin(radialAngle) * sin(angleFromTop);

		// Normalized vertex normal.
		nx = x * lengthInv;
		ny = y * lengthInv;
		nz = z * lengthInv;

		// UV coordinates.
		u = (float)i / radialSubdivisions;
		v = 1.0f / verticalSubdivisions;

		newVertex = TexturedColoredNormalVertex(vec3(x, y, z), vec3(nx, ny, nz), vec2(u, v), vec3(nx, ny, nz));
		texturedSphereVertexVector.push_back(newVertex);


		radialAngle += radialStep;
	}

	// Sphere bottom cap.
	radialAngle = 0.0f;
	for (int i = 0; i < radialSubdivisions; i++)
	{
		float angleFromBottom = PI - verticalStep;

		// Summit
		// Vertex positions.
		x = 0;
		y = -radius + heightOffset;
		z = 0;

		// Normalized vertex normal.
		nx = x * lengthInv;
		ny = y * lengthInv;
		nz = z * lengthInv;

		// UV coordinates.
		u = ((float)i / radialSubdivisions) + (1.0f / (2 * radialSubdivisions));
		v = 1.0f;

		TexturedColoredNormalVertex newVertex = TexturedColoredNormalVertex(vec3(x, y, z), vec3(nx, ny, nz), vec2(u, v), vec3(nx, ny, nz));
		texturedSphereVertexVector.push_back(newVertex);

		// Top left vertex.
		// Vertex positions.
		x = radius * cos(radialAngle) * sin(angleFromBottom);
		y = radius * cos(angleFromBottom) + heightOffset;
		z = radius * sin(radialAngle) * sin(angleFromBottom);

		// Normalized vertex normal.
		nx = x * lengthInv;
		ny = y * lengthInv;
		nz = z * lengthInv;

		// UV coordinates.
		u = (float)i / radialSubdivisions;
		v = 1.0f - (1.0f / verticalSubdivisions);

		newVertex = TexturedColoredNormalVertex(vec3(x, y, z), vec3(nx, ny, nz), vec2(u, v), vec3(nx, ny, nz));
		texturedSphereVertexVector.push_back(newVertex);

		// Top right vertex.
		// Vertex positions.
		x = radius * cos(radialAngle + radialStep) * sin(angleFromBottom);
		y = radius * cos(angleFromBottom) + heightOffset;
		z = radius * sin(radialAngle + radialStep) * sin(angleFromBottom);

		// Normalized vertex normal.
		nx = x * lengthInv;
		ny = y * lengthInv;
		nz = z * lengthInv;

		// UV coordinates.
		u = ((float)i + 1.0f) / radialSubdivisions;
		v = 1.0f - (1.0f / verticalSubdivisions);

		newVertex = TexturedColoredNormalVertex(vec3(x, y, z), vec3(nx, ny, nz), vec2(u, v), vec3(nx, ny, nz));
		texturedSphereVertexVector.push_back(newVertex);


		radialAngle += radialStep;
	}

	// Sphere 'body'.
	for (int i = 1; i < verticalSubdivisions - 1; i++) // Vertical subdivisions.
	{
		verticalAngle += verticalStep;
		radialAngle = radians(0.0f);

		for (int j = 0; j < radialSubdivisions; j++) // Radial subdivisions.
		{
			// Triangle 1.

			// Bottom left.
			// Vertex positions.
			x = radius * cos(radialAngle) * sin(verticalAngle);
			y = radius * cos(verticalAngle) + heightOffset;
			z = radius * sin(radialAngle) * sin(verticalAngle);

			// Normalized vertex normal.
			nx = x * lengthInv;
			ny = y * lengthInv;
			nz = z * lengthInv;

			// UV coordinates.
			u = (float)j / radialSubdivisions;
			v = (float)i / verticalSubdivisions;

			TexturedColoredNormalVertex newVertex = TexturedColoredNormalVertex(vec3(x, y, z), vec3(nx, ny, nz), vec2(u, v), vec3(nx, ny, nz));
			texturedSphereVertexVector.push_back(newVertex);

			// Top right.
			// Vertex positions.
			x = radius * cos(radialAngle + radialStep) * sin(verticalAngle + verticalStep);
			y = radius * cos(verticalAngle + verticalStep) + heightOffset;
			z = radius * sin(radialAngle + radialStep) * sin(verticalAngle + verticalStep);

			// Normalized vertex normal.
			nx = x * lengthInv;
			ny = y * lengthInv;
			nz = z * lengthInv;

			// UV coordinates.
			u = (float)(j + 1) / radialSubdivisions;
			v = (float)(i + 1) / verticalSubdivisions;

			newVertex = TexturedColoredNormalVertex(vec3(x, y, z), vec3(nx, ny, nz), vec2(u, v), vec3(nx, ny, nz));
			texturedSphereVertexVector.push_back(newVertex);

			// Top left.
			// Vertex positions.
			x = radius * cos(radialAngle) * sin(verticalAngle + verticalStep);
			y = radius * cos(verticalAngle + verticalStep) + heightOffset;
			z = radius * sin(radialAngle) * sin(verticalAngle + verticalStep);

			// Normalized vertex normal.
			nx = x * lengthInv;
			ny = y * lengthInv;
			nz = z * lengthInv;

			// UV coordinates.
			u = (float)j / radialSubdivisions;
			v = (float)(i + 1) / verticalSubdivisions;

			newVertex = TexturedColoredNormalVertex(vec3(x, y, z), vec3(nx, ny, nz), vec2(u, v), vec3(nx, ny, nz));
			texturedSphereVertexVector.push_back(newVertex);


			// Triangle 2.

			// Bottom left.
			// Vertex positions.
			x = radius * cos(radialAngle) * sin(verticalAngle);
			y = radius * cos(verticalAngle) + heightOffset;
			z = radius * sin(radialAngle) * sin(verticalAngle);

			// Normalized vertex normal.
			nx = x * lengthInv;
			ny = y * lengthInv;
			nz = z * lengthInv;

			// UV coordinates.
			u = (float)j / radialSubdivisions;
			v = (float)i / verticalSubdivisions;

			newVertex = TexturedColoredNormalVertex(vec3(x, y, z), vec3(nx, ny, nz), vec2(u, v), vec3(nx, ny, nz));
			texturedSphereVertexVector.push_back(newVertex);

			// Bottom right.
			// Vertex positions.
			x = radius * cos(radialAngle + radialStep) * sin(verticalAngle);
			y = radius * cos(verticalAngle) + heightOffset;
			z = radius * sin(radialAngle + radialStep) * sin(verticalAngle);

			// Normalized vertex normal.
			nx = x * lengthInv;
			ny = y * lengthInv;
			nz = z * lengthInv;

			// UV coordinates.
			u = (float)(j + 1) / radialSubdivisions;
			v = (float)i / verticalSubdivisions;

			newVertex = TexturedColoredNormalVertex(vec3(x, y, z), vec3(nx, ny, nz), vec2(u, v), vec3(nx, ny, nz));
			texturedSphereVertexVector.push_back(newVertex);

			// Top right.
			// Vertex positions.
			x = radius * cos(radialAngle + radialStep) * sin(verticalAngle + verticalStep);
			y = radius * cos(verticalAngle + verticalStep) + heightOffset;
			z = radius * sin(radialAngle + radialStep) * sin(verticalAngle + verticalStep);

			// Normalized vertex normal.
			nx = x * lengthInv;
			ny = y * lengthInv;
			nz = z * lengthInv;

			// UV coordinates.
			u = (float)(j + 1) / radialSubdivisions;
			v = (float)(i + 1) / verticalSubdivisions;

			newVertex = TexturedColoredNormalVertex(vec3(x, y, z), vec3(nx, ny, nz), vec2(u, v), vec3(nx, ny, nz));
			texturedSphereVertexVector.push_back(newVertex);


			radialAngle += radialStep;
		}
	}

	numOfVertices = texturedSphereVertexVector.size();

	// Create a vertex array
	GLuint vertexArrayObject;
	glGenVertexArrays(1, &vertexArrayObject);
	glBindVertexArray(vertexArrayObject);

	// Upload Vertex Buffer to GPU, save reference.
	GLuint vertexBufferObject;
	glGenBuffers(1, &vertexBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);

	glBufferData(GL_ARRAY_BUFFER, texturedSphereVertexVector.size() * sizeof(TexturedColoredNormalVertex), &texturedSphereVertexVector[0], GL_STATIC_DRAW);

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

SphereModel::SphereModel() : Model() { }

SphereModel::SphereModel(vec3 position, vec3 rotation, vec3 scale) : Model(position, rotation, scale) { }

SphereModel::SphereModel(vec3 position, vec3 rotation, vec3 scale, mat4 parent) : Model(position, rotation, scale, parent) { }

SphereModel::SphereModel(float radius, float heightOffset, int radialSubdivisions, int verticalSubdivisions) : Model()
{
	std::vector<TexturedColoredNormalVertex> texturedSphereVertexVector;

	// Generate the vertices.
	// I originally referenced the code from http://www.songho.ca/opengl/gl_sphere.html, then completely redid the triangle drawing bits.

	const float PI = acos(-1);

	float x, y, z; // Vertex position.
	float u, v; // UV coordinate.
	float nx, ny, nz, lengthInv = 1.0f / radius; // Normal, will also be used as vertex colour.

	float radialStep = 2 * PI / radialSubdivisions;
	float verticalStep = PI / verticalSubdivisions;
	float radialAngle = 0.0f;
	float verticalAngle = 0.0f;

	// Sphere top cap.
	for (int i = 0; i < radialSubdivisions; i++)
	{
		float angleFromTop = verticalStep;

		// Summit
		// Vertex positions.
		x = 0;
		y = radius + heightOffset;
		z = 0;

		// Normalized vertex normal.
		nx = x * lengthInv;
		ny = y * lengthInv;
		nz = z * lengthInv;

		// UV coordinates.
		u = ((float)i / radialSubdivisions) + (1.0f / (2 * radialSubdivisions));
		v = 0.0f;

		TexturedColoredNormalVertex newVertex = TexturedColoredNormalVertex(vec3(x, y, z), vec3(nx, ny, nz), vec2(u, v), vec3(nx, ny, nz));
		texturedSphereVertexVector.push_back(newVertex);

		// Bottom right vertex.
		// Vertex positions.
		x = radius * cos(radialAngle + radialStep) * sin(angleFromTop);
		y = radius * cos(angleFromTop) + heightOffset;
		z = radius * sin(radialAngle + radialStep) * sin(angleFromTop);

		// Normalized vertex normal.
		nx = x * lengthInv;
		ny = y * lengthInv;
		nz = z * lengthInv;

		// UV coordinates.
		u = ((float)i + 1.0f) / radialSubdivisions;
		v = 1.0f / verticalSubdivisions;

		newVertex = TexturedColoredNormalVertex(vec3(x, y, z), vec3(nx, ny, nz), vec2(u, v), vec3(nx, ny, nz));
		texturedSphereVertexVector.push_back(newVertex);

		// Bottom left vertex.
		// Vertex positions.
		x = radius * cos(radialAngle) * sin(angleFromTop);
		y = radius * cos(angleFromTop) + heightOffset;
		z = radius * sin(radialAngle) * sin(angleFromTop);

		// Normalized vertex normal.
		nx = x * lengthInv;
		ny = y * lengthInv;
		nz = z * lengthInv;

		// UV coordinates.
		u = (float)i / radialSubdivisions;
		v = 1.0f / verticalSubdivisions;

		newVertex = TexturedColoredNormalVertex(vec3(x, y, z), vec3(nx, ny, nz), vec2(u, v), vec3(nx, ny, nz));
		texturedSphereVertexVector.push_back(newVertex);


		radialAngle += radialStep;
	}

	// Sphere bottom cap.
	radialAngle = 0.0f;
	for (int i = 0; i < radialSubdivisions; i++)
	{
		float angleFromBottom = PI - verticalStep;

		// Summit
		// Vertex positions.
		x = 0;
		y = -radius + heightOffset;
		z = 0;

		// Normalized vertex normal.
		nx = x * lengthInv;
		ny = y * lengthInv;
		nz = z * lengthInv;

		// UV coordinates.
		u = ((float)i / radialSubdivisions) + (1.0f / (2 * radialSubdivisions));
		v = 1.0f;

		TexturedColoredNormalVertex newVertex = TexturedColoredNormalVertex(vec3(x, y, z), vec3(nx, ny, nz), vec2(u, v), vec3(nx, ny, nz));
		texturedSphereVertexVector.push_back(newVertex);

		// Top left vertex.
		// Vertex positions.
		x = radius * cos(radialAngle) * sin(angleFromBottom);
		y = radius * cos(angleFromBottom) + heightOffset;
		z = radius * sin(radialAngle) * sin(angleFromBottom);

		// Normalized vertex normal.
		nx = x * lengthInv;
		ny = y * lengthInv;
		nz = z * lengthInv;

		// UV coordinates.
		u = (float)i / radialSubdivisions;
		v = 1.0f - (1.0f / verticalSubdivisions);

		newVertex = TexturedColoredNormalVertex(vec3(x, y, z), vec3(nx, ny, nz), vec2(u, v), vec3(nx, ny, nz));
		texturedSphereVertexVector.push_back(newVertex);

		// Top right vertex.
		// Vertex positions.
		x = radius * cos(radialAngle + radialStep) * sin(angleFromBottom);
		y = radius * cos(angleFromBottom) + heightOffset;
		z = radius * sin(radialAngle + radialStep) * sin(angleFromBottom);

		// Normalized vertex normal.
		nx = x * lengthInv;
		ny = y * lengthInv;
		nz = z * lengthInv;

		// UV coordinates.
		u = ((float)i + 1.0f) / radialSubdivisions;
		v = 1.0f - (1.0f / verticalSubdivisions);

		newVertex = TexturedColoredNormalVertex(vec3(x, y, z), vec3(nx, ny, nz), vec2(u, v), vec3(nx, ny, nz));
		texturedSphereVertexVector.push_back(newVertex);


		radialAngle += radialStep;
	}

	// Sphere 'body'.
	for (int i = 1; i < verticalSubdivisions - 1; i++) // Vertical subdivisions.
	{
		verticalAngle += verticalStep;
		radialAngle = radians(0.0f);

		for (int j = 0; j < radialSubdivisions; j++) // Radial subdivisions.
		{
			// Triangle 1.

			// Bottom left.
			// Vertex positions.
			x = radius * cos(radialAngle) * sin(verticalAngle);
			y = radius * cos(verticalAngle) + heightOffset;
			z = radius * sin(radialAngle) * sin(verticalAngle);

			// Normalized vertex normal.
			nx = x * lengthInv;
			ny = y * lengthInv;
			nz = z * lengthInv;

			// UV coordinates.
			u = (float)j / radialSubdivisions;
			v = (float)i / verticalSubdivisions;

			TexturedColoredNormalVertex newVertex = TexturedColoredNormalVertex(vec3(x, y, z), vec3(nx, ny, nz), vec2(u, v), vec3(nx, ny, nz));
			texturedSphereVertexVector.push_back(newVertex);

			// Top right.
			// Vertex positions.
			x = radius * cos(radialAngle + radialStep) * sin(verticalAngle + verticalStep);
			y = radius * cos(verticalAngle + verticalStep) + heightOffset;
			z = radius * sin(radialAngle + radialStep) * sin(verticalAngle + verticalStep);

			// Normalized vertex normal.
			nx = x * lengthInv;
			ny = y * lengthInv;
			nz = z * lengthInv;

			// UV coordinates.
			u = (float)(j + 1) / radialSubdivisions;
			v = (float)(i + 1) / verticalSubdivisions;

			newVertex = TexturedColoredNormalVertex(vec3(x, y, z), vec3(nx, ny, nz), vec2(u, v), vec3(nx, ny, nz));
			texturedSphereVertexVector.push_back(newVertex);

			// Top left.
			// Vertex positions.
			x = radius * cos(radialAngle) * sin(verticalAngle + verticalStep);
			y = radius * cos(verticalAngle + verticalStep) + heightOffset;
			z = radius * sin(radialAngle) * sin(verticalAngle + verticalStep);

			// Normalized vertex normal.
			nx = x * lengthInv;
			ny = y * lengthInv;
			nz = z * lengthInv;

			// UV coordinates.
			u = (float)j / radialSubdivisions;
			v = (float)(i + 1) / verticalSubdivisions;

			newVertex = TexturedColoredNormalVertex(vec3(x, y, z), vec3(nx, ny, nz), vec2(u, v), vec3(nx, ny, nz));
			texturedSphereVertexVector.push_back(newVertex);


			// Triangle 2.

			// Bottom left.
			// Vertex positions.
			x = radius * cos(radialAngle) * sin(verticalAngle);
			y = radius * cos(verticalAngle) + heightOffset;
			z = radius * sin(radialAngle) * sin(verticalAngle);

			// Normalized vertex normal.
			nx = x * lengthInv;
			ny = y * lengthInv;
			nz = z * lengthInv;

			// UV coordinates.
			u = (float)j / radialSubdivisions;
			v = (float)i / verticalSubdivisions;

			newVertex = TexturedColoredNormalVertex(vec3(x, y, z), vec3(nx, ny, nz), vec2(u, v), vec3(nx, ny, nz));
			texturedSphereVertexVector.push_back(newVertex);

			// Bottom right.
			// Vertex positions.
			x = radius * cos(radialAngle + radialStep) * sin(verticalAngle);
			y = radius * cos(verticalAngle) + heightOffset;
			z = radius * sin(radialAngle + radialStep) * sin(verticalAngle);

			// Normalized vertex normal.
			nx = x * lengthInv;
			ny = y * lengthInv;
			nz = z * lengthInv;

			// UV coordinates.
			u = (float)(j + 1) / radialSubdivisions;
			v = (float)i / verticalSubdivisions;

			newVertex = TexturedColoredNormalVertex(vec3(x, y, z), vec3(nx, ny, nz), vec2(u, v), vec3(nx, ny, nz));
			texturedSphereVertexVector.push_back(newVertex);

			// Top right.
			// Vertex positions.
			x = radius * cos(radialAngle + radialStep) * sin(verticalAngle + verticalStep);
			y = radius * cos(verticalAngle + verticalStep) + heightOffset;
			z = radius * sin(radialAngle + radialStep) * sin(verticalAngle + verticalStep);

			// Normalized vertex normal.
			nx = x * lengthInv;
			ny = y * lengthInv;
			nz = z * lengthInv;

			// UV coordinates.
			u = (float)(j + 1) / radialSubdivisions;
			v = (float)(i + 1) / verticalSubdivisions;

			newVertex = TexturedColoredNormalVertex(vec3(x, y, z), vec3(nx, ny, nz), vec2(u, v), vec3(nx, ny, nz));
			texturedSphereVertexVector.push_back(newVertex);


			radialAngle += radialStep;
		}
	}

	numOfVertices = texturedSphereVertexVector.size();

	// Create a vertex array
	glGenVertexArrays(1, &mVAO);
	glBindVertexArray(mVAO);

	// Upload Vertex Buffer to GPU, save reference.
	glGenBuffers(1, &mVBO);
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);

	glBufferData(GL_ARRAY_BUFFER, texturedSphereVertexVector.size() * sizeof(TexturedColoredNormalVertex), &texturedSphereVertexVector[0], GL_STATIC_DRAW);

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
}

SphereModel::~SphereModel()
{
	glDeleteBuffers(1, &mVBO);
	glDeleteVertexArrays(1, &mVAO);
}

void SphereModel::Update(float dt)
{
	mPosition += dt * mVelocity;
}

void SphereModel::Accelerate(glm::vec3 acceleration, float delta)
{
	mVelocity += acceleration * delta;
}

void SphereModel::Draw(int shaderProgram, GLenum renderingMode)
{
	// Draw the Vertex Buffer
	// The Model View Projection transforms are computed in the Vertex Shader
	//glBindVertexArray(mVAO);
	//glBindBuffer(GL_ARRAY_BUFFER, mVBO);

	GLuint WorldMatrixLocation = glGetUniformLocation(shaderProgram, "worldMatrix");
	glUniformMatrix4fv(WorldMatrixLocation, 1, GL_FALSE, &GetWorldMatrix()[0][0]);

	// Draw the triangles !
	glDrawArrays(renderingMode, 0, numOfVertices);
}

void SphereModel::Draw(int shaderProgram, int numOfVertices, GLenum renderingMode)
{
	// Draw the Vertex Buffer
	// The Model View Projection transforms are computed in the Vertex Shader

	GLuint WorldMatrixLocation = glGetUniformLocation(shaderProgram, "worldMatrix");
	glUniformMatrix4fv(WorldMatrixLocation, 1, GL_FALSE, &GetWorldMatrix()[0][0]);

	// Draw the triangles !
	glDrawArrays(renderingMode, 0, numOfVertices);
}


//Assumes the sphere is evenly scaled
bool SphereModel::ContainsPoint(glm::vec3 position)
{
	float radius = GetScaling().x; //This is where the assumption lies
	float distance = glm::distance(GetPosition(), position);

	return distance <= radius;
}

//Assumes the sphere is evenly scaled
bool SphereModel::IntersectsPlane(glm::vec3 planePoint, glm::vec3 planeNormal)
{
	//We simply compare the distance between the ground and sphere center, with its radius
	float radius = GetScaling().x;
	return glm::dot(planeNormal, GetPosition() - planePoint) < radius;
}
