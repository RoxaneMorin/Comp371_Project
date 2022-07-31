#pragma once

#include "Model.h"

#include <vector>

class CubeModel : public Model
{
public:
	CubeModel();
	CubeModel(vec3 position, vec3 rotation, vec3 scale);
	CubeModel(vec3 position, vec3 rotation, vec3 scale, mat4 parent);
	CubeModel(vec3 size); // Creates Cube with VAO
	virtual ~CubeModel();

	virtual void Update(float dt);
	virtual void Draw(int shaderProgram, GLenum renderingMode = GL_TRIANGLES);

	virtual bool ContainsPoint(vec3 position);
	virtual bool IntersectsPlane(vec3 planePoint, vec3 planeNormal);

	virtual bool isSphere() { return false; } //This is not at all object-oriented, but somewhat necessary due to need for a simple double-dispatch mechanism

	unsigned int static CubeModelVAO();
private:
	unsigned int mVAO;
	unsigned int mVBO;

	std::vector<vec3> vertexPositions;
};

