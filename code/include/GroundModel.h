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
	//void Draw(int shaderProgram, int sizeX, int sizeZ, GLenum renderingModel = GL_TRIANGLES);

	virtual bool ContainsPoint(vec3 position);//Whether or not the given point is withing the model. For collisions.
	virtual bool IntersectsPlane(vec3 planePoint, vec3 planeNormal);

	//unsigned int static GroundModelVAO(unsigned int sizeX, unsigned int sizeZ, float uvTiling);

	float returnHeightAtPoint(vec2 pointCoords, bool debug = false);

	float generateHeightCoord(unsigned int xCoord, unsigned int zCoord, float noiseScaling);
	vec2 generateUVCoords(unsigned int posX, unsigned int posZ, float uvTiling);
	vec3 generateFaceNormals(vec3 pointAPos, vec3 pointBPos, vec3 pointCPos);
	void createGroundVertexVector(std::map<vec2, TexturedColoredNormalVertex, CompareVec2> terrainVertexMap, unsigned int sizeX, unsigned int sizeZ);
	void createGroundVertexMap(unsigned int sizeX, unsigned int sizeZ, float uvTiling = 1);

private:
	float sizeX;
	float sizeZ;

	unsigned int mVAO;
	unsigned int mVBO;

	std::map<vec2, TexturedColoredNormalVertex, CompareVec2> terrainVertexMap;
	std::vector<TexturedColoredNormalVertex> vertexVector;
};