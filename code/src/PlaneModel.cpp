#include "PlaneModel.h"

#define GLEW_STATIC 1   // This allows linking with Static Library on Windows, without DLL
#include <GL/glew.h>    // Include GLEW - OpenGL Extension Wrangler

#include <GLFW/glfw3.h>

#include <glm/gtx/transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/common.hpp>
#include <limits>

using namespace glm;

unsigned int PlaneModel::PlaneModelVAO()
{
    TexturedColoredNormalVertex texturedCubeVertexArray[] = {
       TexturedColoredNormalVertex(vec3(0.0f, 0.0f, 0.0f), vec3(0.5f, 0.5f, 0.5f), vec2(0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f)), // Point at origin, light grey.
       TexturedColoredNormalVertex(vec3(0.0f, 0.0f, 1.0f), vec3(0.0f, 0.0f, 1.0f), vec2(0.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f)), // Point at (0, 1, 0), blue.
       TexturedColoredNormalVertex(vec3(1.0f, 0.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f), vec2(1.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f)), // Point at (1, 0, 0), red.

       TexturedColoredNormalVertex(vec3(1.0f, 0.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f), vec2(1.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f)), // Point at (1, 0, 0), red.
       TexturedColoredNormalVertex(vec3(0.0f, 0.0f, 1.0f), vec3(0.0f, 0.0f, 1.0f), vec2(0.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f)), // Point at (0, 1, 0), blue.
       TexturedColoredNormalVertex(vec3(1.0f, 0.0f, 1.0f), vec3(0.1f, 0.1f, 0.1f), vec2(1.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f)) // Point at (1, 0, 1), dark grey.
    };

    // Create vertex array.
    GLuint vertexArrayObject;
    glGenVertexArrays(1, &vertexArrayObject);
    glBindVertexArray(vertexArrayObject);


    // Upload Vertex Buffer to GPU, save reference.
    GLuint vertexBufferObject;
    glGenBuffers(1, &vertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(texturedCubeVertexArray), texturedCubeVertexArray, GL_STATIC_DRAW);

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

PlaneModel::PlaneModel() : Model() {}

PlaneModel::PlaneModel(vec3 position, vec3 rotation, vec3 scale) : Model(position, rotation, scale) {}

PlaneModel::PlaneModel(vec3 position, vec3 rotation, vec3 scale, mat4 parent) : Model(position, rotation, scale, parent) {}

PlaneModel::PlaneModel(bool useVAO) : Model()
{
    TexturedColoredNormalVertex texturedCubeVertexArray[] = {
        TexturedColoredNormalVertex(vec3(0.0f, 0.0f, 0.0f), vec3(0.5f, 0.5f, 0.5f), vec2(0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f)), // Point at origin, light grey.
        TexturedColoredNormalVertex(vec3(0.0f, 0.0f, 1.0f), vec3(0.0f, 0.0f, 1.0f), vec2(0.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f)), // Point at (0, 1, 0), blue.
        TexturedColoredNormalVertex(vec3(1.0f, 0.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f), vec2(1.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f)), // Point at (1, 0, 0), red.

        TexturedColoredNormalVertex(vec3(1.0f, 0.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f), vec2(1.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f)), // Point at (1, 0, 0), red.
        TexturedColoredNormalVertex(vec3(0.0f, 0.0f, 1.0f), vec3(0.0f, 0.0f, 1.0f), vec2(0.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f)), // Point at (0, 1, 0), blue.
        TexturedColoredNormalVertex(vec3(1.0f, 0.0f, 1.0f), vec3(0.1f, 0.1f, 0.1f), vec2(1.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f)) // Point at (1, 0, 1), dark grey.
    };

    // Create vertex array.
    glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);

    // Upload Vertex Buffer to GPU, save reference.
    glGenBuffers(1, &mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(texturedCubeVertexArray), texturedCubeVertexArray, GL_STATIC_DRAW);

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

PlaneModel::~PlaneModel()
{
    // Free the GPU from the Vertex Buffer
    glDeleteBuffers(1, &mVBO);
    glDeleteVertexArrays(1, &mVAO);
}

void PlaneModel::Update(float dt)
{

}

void PlaneModel::Draw(int shaderProgram, GLenum renderingMode)
{
    //glBindVertexArray(mVAO);
    //glBindBuffer(GL_ARRAY_BUFFER, mVBO);

    GLuint WorldMatrixLocation = glGetUniformLocation(shaderProgram, "worldMatrix");
    glUniformMatrix4fv(WorldMatrixLocation, 1, GL_FALSE, &GetWorldMatrix()[0][0]);

    // Draw the triangles
    glDrawArrays(renderingMode, 0, 6); 
}


//UNIMPLEMENTED
bool PlaneModel::IntersectsPlane(glm::vec3 planePoint, glm::vec3 planeNormal)
{
    return false;
}