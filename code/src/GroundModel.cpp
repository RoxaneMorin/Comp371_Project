#include "GroundModel.h"

#include "PerlinNoise.h"

#include <iostream>
#include <list>
#include <vector>
#include <algorithm>
#include <map>
#include <cmath>

using namespace std;
using namespace glm;

//unsigned int GroundModel::GroundModelVAO(unsigned int sizeX, unsigned int sizeZ, float uvTiling)
//{
//	// Generate vertices.
//	map<vec2, TexturedColoredNormalVertex, CompareVec2> groundVertexMap = createGroundVertexMap(sizeX, sizeZ, uvTiling);
//	vector<TexturedColoredNormalVertex> groundVertexVector = createGroundVertexVector(groundVertexMap, sizeX, sizeZ);
//
//	// Create a vertex array
//	GLuint vertexArrayObject;
//	glGenVertexArrays(1, &vertexArrayObject);
//	glBindVertexArray(vertexArrayObject);
//
//	// Upload Vertex Buffer to GPU, save reference.
//	GLuint vertexBufferObject;
//	glGenBuffers(1, &vertexBufferObject);
//	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
//
//	glBufferData(GL_ARRAY_BUFFER, groundVertexVector.size() * sizeof(TexturedColoredNormalVertex), &groundVertexVector[0], GL_STATIC_DRAW);
//
//	glVertexAttribPointer(0,                   // attribute 0 matches aPos in Vertex Shader
//		3,                   // size
//		GL_FLOAT,            // type
//		GL_FALSE,            // normalized?
//		sizeof(TexturedColoredNormalVertex), // stride - each vertex contain 2 vec3 (position, color)
//		(void*)0             // array buffer offset
//	);
//	glEnableVertexAttribArray(0);
//
//
//	glVertexAttribPointer(1,                            // attribute 1 matches aColor in Vertex Shader
//		3,
//		GL_FLOAT,
//		GL_FALSE,
//		sizeof(TexturedColoredNormalVertex),
//		(void*)sizeof(vec3)      // color is offseted a vec3 (comes after position)
//	);
//	glEnableVertexAttribArray(1);
//
//	glVertexAttribPointer(2,                            // attribute 2 matches aUV in Vertex Shader
//		2,
//		GL_FLOAT,
//		GL_FALSE,
//		sizeof(TexturedColoredNormalVertex),
//		(void*)(2 * sizeof(vec3))      // uv is offseted by 2 vec3 (comes after position and color)
//	);
//	glEnableVertexAttribArray(2);
//
//	glVertexAttribPointer(3,                            // attribute 3 matches aNormals in Vertex Shader
//		3,
//		GL_FLOAT,
//		GL_FALSE,
//		sizeof(TexturedColoredNormalVertex),
//		(void*)(2 * sizeof(vec3) + sizeof(vec2))    // normals are offsetted by two vec3 and a vec2.
//	);
//	glEnableVertexAttribArray(3);
//
//	return vertexArrayObject;
//}

GroundModel::GroundModel() { } 

GroundModel::GroundModel(unsigned int sizeX, unsigned int sizeZ, float uvTiling) : Model()
{
	this->sizeX = sizeX;
	this->sizeZ = sizeZ;

	// Generate vertices.
	createGroundVertexMap(sizeX, sizeZ, uvTiling);
	createGroundVertexVector(terrainVertexMap, sizeX, sizeZ);

	glGenVertexArrays(1, &mVAO);
	glBindVertexArray(mVAO);

	glGenBuffers(1, &mVBO);
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);

	glBufferData(GL_ARRAY_BUFFER, vertexVector.size() * sizeof(TexturedColoredNormalVertex), &vertexVector[0], GL_STATIC_DRAW);

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

GroundModel::~GroundModel()
{
	// Free the GPU from the Vertex Buffer
	glDeleteBuffers(1, &mVBO);
	glDeleteVertexArrays(1, &mVAO);
}

void GroundModel::Update(float dt)
{

}

void GroundModel::Draw(int shaderProgram, GLenum renderingModel)
{
	glBindVertexArray(mVAO);
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);

	// > Base.
	float groundCenterX = 0 - (float)sizeX / 2;
	float groundCenterZ = 0 - (float)sizeZ / 2;

	SetPosition(vec3(groundCenterX, 0.0f, groundCenterZ));

	GLuint worldMatrixLocation = glGetUniformLocation(shaderProgram, "worldMatrix");
	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &GetWorldMatrix()[0][0]);

	glDrawArrays(renderingModel, 0, 6 * sizeX * sizeZ);
}

//void GroundModel::Draw(int shaderProgram, int sizeX, int sizeZ, GLenum renderingModel) 
//{
//	// > Base.
//	float groundCenterX = 0 - (float)sizeX / 2;
//	float groundCenterZ = 0 - (float)sizeZ / 2;
//
//	SetPosition(vec3(groundCenterX, 0.0f, groundCenterZ));
//
//	GLuint worldMatrixLocation = glGetUniformLocation(shaderProgram, "worldMatrix");
//	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &GetWorldMatrix()[0][0]);
//
//	glDrawArrays(renderingModel, 0, 6 * sizeX * sizeZ);
//}

// uvTiling = how many quads does the texture stretch across before being repeated?
void GroundModel::createGroundVertexMap(unsigned int sizeX, unsigned int sizeZ, float uvTiling)
{
	//std::map<vec2, Model::TexturedColoredNormalVertex, CompareVec2> terrainVertexMap;

	// Vertex parameters.
	vec3 position;
	vec3 color = vec3(1.0f, 1.0f, 1.0f);
	vec3 normals = vec3(0.0f, 1.0f, 0.0f);
	vec2 uv;

	float yCoord = 0.0f;

	for (int z = 0; z <= sizeZ; z++) // Columns.
	{
		for (int x = 0; x <= sizeX; x++) // Rows.
		{
			yCoord = generateHeightCoord(x, z, 0.05f); // Generate basic height variation using a perlin noise.
			yCoord += (sin((float)x) / 2 + (rand() % 12 + 1)) / 20; // Generate aditional variations using random numbers and a sin wave.

			position = vec3((float)x, yCoord, (float)z);
			uv = generateUVCoords(x, z, uvTiling);

			vec2 currentKey = vec2(x, z);
			TexturedColoredNormalVertex currentVertex = TexturedColoredNormalVertex(position, color, uv, normals);

			terrainVertexMap.insert(std::make_pair(currentKey, currentVertex));
		}
	}

	// Generate normals that account for the variable terrain height. Exclude the vertices at the very edges of the grid.
	for (int z = 1; z < sizeZ - 1; z++) // Columns.
	{
		for (int x = 1; x < sizeX - 1; x++) // Rows.
		{
			vec3 topLeft = generateFaceNormals(terrainVertexMap[vec2(x, z)].position, terrainVertexMap[vec2(x, z + 1)].position, terrainVertexMap[vec2(x + 1, z)].position);
			vec3 topMid = generateFaceNormals(terrainVertexMap[vec2(x, z)].position, terrainVertexMap[vec2(x - 1, z + 1)].position, terrainVertexMap[vec2(x, z + 1)].position);
			vec3 topRight = generateFaceNormals(terrainVertexMap[vec2(x, z)].position, terrainVertexMap[vec2(x - 1, z)].position, terrainVertexMap[vec2(x - 1, z - 1)].position);

			vec3 bottomLeft = generateFaceNormals(terrainVertexMap[vec2(x, z)].position, terrainVertexMap[vec2(x + 1, z)].position, terrainVertexMap[vec2(x + 1, z - 1)].position);
			vec3 bottomMid = generateFaceNormals(terrainVertexMap[vec2(x, z)].position, terrainVertexMap[vec2(x + 1, z - 1)].position, terrainVertexMap[vec2(x, z - 1)].position);
			vec3 bottomRight = generateFaceNormals(terrainVertexMap[vec2(x, z)].position, terrainVertexMap[vec2(x, z - 1)].position, terrainVertexMap[vec2(x - 1, z)].position);

			terrainVertexMap[vec2(x, z)].normals = (topLeft + topMid + topRight + bottomLeft + bottomMid + bottomRight) / 6.0f; // Divise the value by six to normalize it.
			//terrainVertexMap[vec2(x, z)].normals.x *= 0.75f;
			//terrainVertexMap[vec2(x, z)].normals.z *= 0.75f;

			cout << "Vertex normals at " << terrainVertexMap[vec2(x, z)].position.x << ", " << terrainVertexMap[vec2(x, z)].position.z << " :\n";
			cout << terrainVertexMap[vec2(x, z)].normals.x << ", " << terrainVertexMap[vec2(x, z)].normals.y << ", " << terrainVertexMap[vec2(x, z)].normals.z << " :\n";
		}
	}
}

// We are using this noise library: https://github.com/Reputeless/PerlinNoise.
float GroundModel::generateHeightCoord(unsigned int xCoord, unsigned int zCoord, float noiseScaling)
{
	// Do the perlin noise ~~
	const siv::PerlinNoise::seed_type seed = 42069u;
	const siv::PerlinNoise perlin{ seed };

	const float noise = perlin.normalizedOctave2D((float)xCoord * noiseScaling, (float)zCoord * noiseScaling, 5);

	return noise * 5;
}

vec2 GroundModel::generateUVCoords(unsigned int posX, unsigned int posZ, float uvTiling)
{
	float uvPosX = (uvTiling == 1) ? posX : ((float)posX / uvTiling);
	float uvPosY = (uvTiling == 1) ? posZ : ((float)posZ / uvTiling);

	return vec2(uvPosX, uvPosY);
}

// Based on the algorithm at https://www.khronos.org/opengl/wiki/Calculating_a_Surface_Normal
vec3 GroundModel::generateFaceNormals(vec3 pointAPos, vec3 pointBPos, vec3 pointCPos)
{
	vec3 vectorU = pointBPos - pointAPos;
	vec3 vectorV = pointCPos - pointAPos;

	vec3 faceNormals = vec3(0.0f);
	faceNormals.x = (vectorU.y * vectorV.z) - (vectorU.z * vectorV.y);
	faceNormals.y = (vectorU.z * vectorV.x) - (vectorU.x * vectorV.z);
	faceNormals.z = (vectorU.x * vectorV.y) - (vectorU.y * vectorV.x);

	return faceNormals;
}

void  GroundModel::createGroundVertexVector(map<vec2, TexturedColoredNormalVertex, CompareVec2> terrainVertexMap, unsigned int sizeX, unsigned int sizeZ)
{
	for (int z = 0; z < sizeZ; z++) // Columns.
	{
		for (int x = 0; x < sizeX; x++) // Rows.
		{
			// Bottom triangle.
			vertexVector.push_back(terrainVertexMap[vec2(x, z)]); // (0, 0).
			vertexVector.push_back(terrainVertexMap[vec2(x, z + 1)]); // (0, 1).
			vertexVector.push_back(terrainVertexMap[vec2(x + 1, z)]); // (1, 0).

			// Top triangle.
			vertexVector.push_back(terrainVertexMap[vec2(x + 1, z)]); // (1, 0).
			vertexVector.push_back(terrainVertexMap[vec2(x, z + 1)]); // (0, 1).
			vertexVector.push_back(terrainVertexMap[vec2(x + 1, z + 1)]); // (1, 1). 
		}
	}
}

// Utility.
float GroundModel::returnHeightAtPoint(vec2 pointCoords, bool debug)
{
	float heightAtPoint;

	if (debug) std::cout << "Calculating ground height at point: " << pointCoords.x << ", " << pointCoords.y << ".\n";

	float closestLowX = floor(pointCoords.x);
	float closestHighX = closestLowX + 1;
	float xCoordDelta = pointCoords.x - closestLowX;

	/*
	cout << "Closest low x: " << closestLowX << ".\n";
	cout << "Closest high x: " << closestHighX << ".\n";
	cout << "Distance from closest low x: " << xCoordDelta << ".\n";
	*/

	float closestLowZ = floor(pointCoords.y);
	float closestHighZ = closestLowZ + 1;
	float zCoordDelta = pointCoords.y - closestLowZ;

	/*
	cout << "Closest low z: " << closestLowZ << ".\n";
	cout << "Closest high z: " << closestHighZ << ".\n";
	cout << "Distance from closest low z: " << zCoordDelta << ".\n";
	*/

	float lowXlowZHeight = terrainVertexMap[vec2(closestLowX, closestLowZ)].position.y;
	//cout << "Height at low x, low z: " << lowXlowZHeight << ".\n";
	float highXlowZHeight = terrainVertexMap[vec2(closestHighX, closestLowZ)].position.y;
	//cout << "Height at high x, low z: " << highXlowZHeight << ".\n";
	float lowXhighZHeight = terrainVertexMap[vec2(closestLowX, closestHighZ)].position.y;
	//cout << "Height at low x, high z: " << lowXhighZHeight << ".\n";
	float highXhighZHeight = terrainVertexMap[vec2(closestHighX, closestHighZ)].position.y;
	//cout << "Height at high x, high z: " << highXhighZHeight << ".\n";


	// Verify whether the point exists as is in the ground vertex map.
	if (pointCoords.x == closestLowX && pointCoords.y == closestLowZ)
	{
		return lowXlowZHeight;
	}
	if (pointCoords.x == closestHighX && pointCoords.y == closestLowZ)
	{
		return highXlowZHeight;
	}
	if (pointCoords.x == closestLowX && pointCoords.y == closestHighZ)
	{
		return lowXhighZHeight;
	}
	if (pointCoords.x == closestHighX && pointCoords.y == closestHighZ)
	{
		return highXhighZHeight;
	}


	// Otherwise,
	if (pointCoords.y > (1 - pointCoords.x)) // Point situated inside the top triangle.
	{
		//cout << "We are in the top triangle.\n";

		float heightAtXEdge = lerp(lowXhighZHeight, highXhighZHeight, xCoordDelta);
		//cout << "The midpoint height on the X edge is: " << heightAtXEdge << ".\n";
		heightAtPoint = lerp(highXlowZHeight, heightAtXEdge, zCoordDelta);

	}
	else // Point situated inside the bottom triangle.
	{
		//cout << "We are in the bottom triangle.\n";

		float heightAtXEdge = lerp(lowXlowZHeight, highXlowZHeight, xCoordDelta);
		//cout << "The midpoint height on the X edge is: " << heightAtXEdge << ".\n";
		heightAtPoint = lerp(heightAtXEdge, lowXhighZHeight, zCoordDelta);
	}

	if (debug) cout << "The height at this point is: " << heightAtPoint << ".\n";
	return heightAtPoint;
}

bool GroundModel::ContainsPoint(vec3 position)
{
	return true;
}

bool GroundModel::IntersectsPlane(vec3 planePoint, vec3 planeNormal)
{
	return true;
}
