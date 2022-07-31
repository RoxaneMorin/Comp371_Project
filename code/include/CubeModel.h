#pragma once

#include "Model.h"

#include <vector>

class CubeModel : public Model
{
public:
	CubeModel(vec3 size = vec3(1.0f, 1.0f, 1.0f));
	virtual ~CubeModel();

	virtual void Update(float dt);
	virtual void Draw(int shaderProgram);

	virtual bool ContainsPoint(vec3 position);
	virtual bool IntersectsPlane(vec3 planePoint, vec3 planeNormal);

	virtual bool isSphere() { return false; } //This is not at all object-oriented, but somewhat necessary due to need for a simple double-dispatch mechanism

private:
	unsigned int mVAO;
	unsigned int mVBO;

	std::vector<vec3> vertexPositions;
};