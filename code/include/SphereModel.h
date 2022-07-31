#pragma once

#include "Model.h"

class SphereModel : public Model
{
public:
    SphereModel();
    SphereModel(vec3 position, vec3 rotation, vec3 scale);
    SphereModel(vec3 position, vec3 rotation, vec3 scale, mat4 parent);
    SphereModel(float radius, float heightOffset, int radialSubdivisions, int verticalSubdivisions); // returns an object with its own VAO
    virtual ~SphereModel(void);

    virtual void Update(float dt);
    virtual void Draw(int shaderProgram, GLenum renderingMode = GL_TRIANGLES);
    virtual void Draw(int shaderProgram, int numOfVertices, GLenum renderingMode = GL_TRIANGLES);

    //Assumes the sphere is evenly scaled
    virtual bool ContainsPoint(vec3 position);
    virtual bool IntersectsPlane(vec3 planePoint, vec3 planeNormal);

    virtual bool isSphere() { return true; } //This is not at all object-oriented, but somewhat necessary due to need for a simple double-dispatch mechanism

    unsigned int static SphereModelVAO(float radius, float heightOffset, int radialSubdivisions, int verticalSubdivisions, int& numOfVertices);
private:
    unsigned int mVAO;
    unsigned int mVBO;
    unsigned int numOfVertices;
};
