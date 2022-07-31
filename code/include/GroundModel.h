#pragma once

#include "Model.h"

#include <vector>
#include <map>

// Code based on https://stackoverflow.com/questions/23042947/how-to-create-a-stdset-with-custom-comparator-in-c
struct CompareVec2
{
	bool operator()(const vec2& leftHandSide, const vec2& rightHandSide) const
	{
		return (leftHandSide.x > rightHandSide.x) || (leftHandSide.x == rightHandSide.x && (leftHandSide.y > rightHandSide.y));
	}
};

class GroundModel : public Model
{
public:
	GroundModel();
	GroundModel(unsigned int sizeX, unsigned int sizeZ, float uvTiling); // Return a GroundModel with its own VAO
	virtual ~GroundModel();

	virtual void Update(float dt);
	virtual void Draw(int shaderProgram, GLenum renderingModel = GL_TRIANGLES);
	void Draw(int shaderProgram, int sizeX, int sizeZ, GLenum renderingModel = GL_TRIANGLES);

	virtual bool isSphere() { return false; } //This is not at all object-oriented, but somewhat necessary due to need for a simple double-dispatch mechanism

	unsigned int static GroundModelVAO(unsigned int sizeX, unsigned int sizeZ, float uvTiling);

	float returnHeightAtPoint(vec2 pointCoords);

	float static generateHeightCoord(unsigned int xCoord, unsigned int zCoord, float noiseScaling);
	vec2 static generateUVCoords(unsigned int posX, unsigned int posZ, float uvTiling);
	vec3 static generateFaceNormals(vec3 pointAPos, vec3 pointBPos, vec3 pointCPos);
	std::vector<TexturedColoredNormalVertex> static createGroundVertexVector(std::map<vec2, TexturedColoredNormalVertex, CompareVec2> terrainVertexMap, unsigned int sizeX, unsigned int sizeZ);
	std::map<vec2, TexturedColoredNormalVertex, CompareVec2> static createGroundVertexMap(unsigned int sizeX, unsigned int sizeZ, float uvTiling = 1);

private:
	float sizeX;
	float sizeZ;

	unsigned int mVAO;
	unsigned int mVBO;

	std::map<vec2, TexturedColoredNormalVertex, CompareVec2> terrainVertexMap;
	std::vector<TexturedColoredNormalVertex> vertexVector;
};