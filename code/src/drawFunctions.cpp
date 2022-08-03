#include "drawFunctions.h"

// Draw Functions.

void DrawGrid(int shaderProgram, GLfloat centerPosX, GLfloat centerPosZ, GLuint width, GLuint depth)
{
    // Set initial position.
    GLfloat initialPosX = centerPosX - width / 2;
    GLfloat currentPosX = initialPosX;
    GLfloat currentPosZ = centerPosZ - depth / 2;

    mat4 worldMatrix;

    for (int i = 0; i < depth; i++)
    {
        for (int j = 0; j < width; j++)
        {
            worldMatrix = translate(mat4(1.0f), vec3(currentPosX, 0.0f, currentPosZ));
            GLuint worldMatrixLocation = glGetUniformLocation(shaderProgram, "worldMatrix");
            glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);

            glDrawArrays(GL_TRIANGLES, 0, 6);

            currentPosX++;
        }
        currentPosX = initialPosX;
        currentPosZ++;
    }
}

void DawAxisStar(int shaderProgram)
{
    glLineWidth(3.0f); // Doesn't seem to work. Rendering lines seems like a crapshoot.

    mat4 linesWorldMatrix = translate(mat4(1.0f), vec3(0.0f, 0.01f, 0.0f));
    GLuint worldMatrixLocation = glGetUniformLocation(shaderProgram, "worldMatrix");
    glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &linesWorldMatrix[0][0]);

    glDrawArrays(GL_LINE_STRIP, 0, 6);
}

GLenum meshRenderMode = GL_TRIANGLES; // Declaring the variable here as it is used in multiple methods.

mat4 DrawParentedMesh(int shaderProgram, mat4 parentMatrix, vec3 translation, vec3 scale, unsigned int numberOfVerts)
{
    mat4 childMatrix = glm::translate(mat4(1.0f), translation) * glm::scale(glm::mat4(1.0f), scale);
    mat4 worldMatrix = parentMatrix * childMatrix;

    GLuint worldMatrixLocation = glGetUniformLocation(shaderProgram, "worldMatrix");
    glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);

    glDrawArrays(meshRenderMode, 0, numberOfVerts);

    return worldMatrix; // Return the resulting matrix so it can be used for further parenting.
}

mat4 DrawParentedMesh(int shaderProgram, mat4 parentMatrix, vec3 translation, vec3 scale, float angle, vec3 rotationAxis, unsigned int numberOfVerts) // With rotation.
{
    mat4 childMatrix = glm::translate(mat4(1.0f), translation) * glm::rotate(mat4(1.0f), radians(angle), rotationAxis) * glm::scale(glm::mat4(1.0f), scale);
    mat4 worldMatrix = parentMatrix * childMatrix;

    GLuint worldMatrixLocation = glGetUniformLocation(shaderProgram, "worldMatrix");
    glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);

    glDrawArrays(meshRenderMode, 0, numberOfVerts);

    return worldMatrix; // Return the resulting matrix so it can be used for further parenting.
} // To do: fix wonky nested rotations.