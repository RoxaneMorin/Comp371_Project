#pragma once

#include "Model.h"

class SphereModel : public Model
{
public:
    SphereModel(float radius = 1.0f, float heightOffset = 1.0f, int radialSubdivisions = 18.0f, int verticalSubdivisions = 18.0f);
    virtual ~SphereModel(void);

    virtual void Update(float dt);
    virtual void Draw(int shaderProgram);

    //Assumes the sphere is evenly scaled
    virtual bool ContainsPoint(vec3 position);
    virtual bool IntersectsPlane(vec3 planePoint, glm::vec3 planeNormal);

    virtual bool isSphere() { return true; } //This is not at all object-oriented, but somewhat necessary due to need for a simple double-dispatch mechanism

private:
    // The vertex format could be different for different types of models
    struct Vertex
    {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec3 color;
    };

    unsigned int mVAO;
    unsigned int mVBO;
    unsigned int numOfVertices;
};
