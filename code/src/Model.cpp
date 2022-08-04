#include "Model.h"

#define GLEW_STATIC 1   // This allows linking with Static Library on Windows, without DLL
#include <GL/glew.h>    // Include GLEW - OpenGL Extension Wrangler

#include <GLFW/glfw3.h> // GLFW provides a cross-platform interface for creating a graphical context,
// initializing OpenGL and binding inputs

#include <glm/gtc/matrix_transform.hpp>
#include <glm/common.hpp>
#include <glm/gtc/quaternion.hpp>

using namespace std;
using namespace glm;

Model::Model() : mPosition(0.0f, 0.0f, 0.0f), mScaling(1.0f, 1.0f, 1.0f), mRotation(0.0f, 0.0f, 0.0f), mParent(mat4(1.0f))
{

}

Model::Model(vec3 position, vec3 rotation, vec3 scale) : mPosition(position), mRotation(rotation), mScaling(scale), mParent(mat4(1.0f))
{

}

Model::Model(vec3 position, vec3 rotation, vec3 scale, mat4 parent) : mPosition(position), mRotation(rotation), mScaling(scale), mParent(parent)
{

}

Model::~Model()
{
}

//void Model::Update(float dt)
//{
//
//}
//
//void Model::Draw(int shaderProgram)
//{
//}

void Model::SetWorldMatrix(vec3 position, vec3 rotation, vec3 scale) 
{
    mPosition = position;
    mRotation = rotation;
    mScaling = scale;
}

void Model::SetParent(mat4 parentMatrix)
{
    mParent = parentMatrix;
}

void Model::ResetParent()
{
    mParent = mat4(1.0f);
}

mat4 Model::GetWorldMatrix() const
{
    mat4 worldMatrix(1.0f);

    quat mQuat = quat(mRotation);
    
    mat4 t = translate(mat4(1.0f), mPosition);
    mat4 r = mat4_cast(mQuat);
    mat4 s = scale(mat4(1.0f), mScaling);

    worldMatrix = t * r * s;

    return mParent * worldMatrix;
}

void Model::SetPosition(vec3 position)
{
    mPosition = position;
}

void Model::SetScaling(vec3 scaling)
{
    mScaling = scaling;
}

void Model::SetRotation(vec3 rotation)
{
    mRotation = rotation;
}

void Model::UpdatePosition(vec3 position)
{
    mPosition += position;
}

void Model::UpdateRotation(vec3 rotation) 
{
    mRotation += rotation;
}

void Model::UpdateScale(vec3 scale)
{
    mScaling += scale;
}