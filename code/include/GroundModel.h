#pragma once

#include "Model.h"

#include <vector>
#include <map>

class GroundModel : public Model
{
public:
	GroundModel(unsigned int sizeX, unsigned int sizeZ, float uvTiling);
	virtual ~GroundModel();

	virtual void Update(float dt);
	virtual void Draw(int shaderProgram);

	virtual bool isSphere() { return false; } //This is not at all object-oriented, but somewhat necessary due to need for a simple double-dispatch mechanism

protected:
	struct CompareVec2
	{
		bool operator()(const vec2& leftHandSide, const vec2& rightHandSide) const
		{
			return (leftHandSide.x > rightHandSide.x) || (leftHandSide.x == rightHandSide.x && (leftHandSide.y > rightHandSide.y));
		}
	};

	float generateHeightCoord(unsigned int xCoord, unsigned int zCoord, float noiseScaling);
	vec2 generateUVCoords(unsigned int posX, unsigned int posZ, float uvTiling);
	vec3 generateFaceNormals(vec3 pointAPos, vec3 pointBPos, vec3 pointCPos);
	void createGroundVertexVector(std::map<vec2, TexturedColoredNormalVertex, CompareVec2> terrainVertexMap, unsigned int sizeX, unsigned int sizeZ);
	void createGroundVertexMap(unsigned int sizeX, unsigned int sizeZ, float uvTiling);
	float returnHeightAtPoint(vec2 pointCoords);

private:
	float sizeX;
	float sizeZ;

	unsigned int mVAO;
	unsigned int mVBO;

	std::map<vec2, TexturedColoredNormalVertex, CompareVec2> terrainVertexMap;
	std::vector<TexturedColoredNormalVertex> vertexVector;
};