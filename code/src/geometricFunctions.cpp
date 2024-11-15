#include "geometricFunctions.h"
#include "PerlinNoise.h"

#include <cmath>

using namespace std;
using namespace glm;


// Structs.

// Code based on https://stackoverflow.com/questions/23042947/how-to-create-a-stdset-with-custom-comparator-in-c
struct CompareVec2
{
    bool operator()(const glm::vec2& leftHandSide, const glm::vec2& rightHandSide) const
    {
        return (leftHandSide.x > rightHandSide.x) || (leftHandSide.x == rightHandSide.x && (leftHandSide.y > rightHandSide.y));
    }
};

struct TexturedColoredVertex
{
    vec3 position;
    vec3 color;
    vec2 uv;
   
    TexturedColoredVertex(vec3 _position, vec3 _color, vec2 _uv) : position(_position), color(_color), uv(_uv) {}

    TexturedColoredVertex() : position(vec3(0.0f, 0.0f, 0.0f)), color(vec3(1.0f, 1.0f, 1.0f)), uv(vec2(0.0f, 0.0f)) {}
};

struct TexturedColoredNormalVertex
{
    vec3 position;
    vec3 color;
    vec2 uv;
    vec3 normals;
    
    // Constructors.
    TexturedColoredNormalVertex() : position(vec3(0.0f, 0.0f, 0.0f)), color(vec3(1.0f, 1.0f, 1.0f)), uv(vec2(0.0f, 0.0f)), normals(vec3(0.0f, 0.0f, 0.0f)) {}

    TexturedColoredNormalVertex(vec3 _position, vec3 _color, vec2 _uv, vec3 _normals) : position(_position), color(_color), uv(_uv), normals(_normals) {}

    // Copy constructor.
    TexturedColoredNormalVertex(TexturedColoredNormalVertex source, vec3 newNormals) : position(source.position), color(source.color), uv(source.uv), normals(newNormals) {}
};


// Geometric Functions.

int createCubeVBO()
{
    TexturedColoredNormalVertex texturedCubeVertexArray[] = {
        // Front face, red.
        TexturedColoredNormalVertex(vec3(-0.5f, 0.0f, 0.5f), vec3(1.0f, 0.0f, 0.0f), vec2(0.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f)), // Bottom left vertex.
        TexturedColoredNormalVertex(vec3(0.5f, 0.0f, 0.5f), vec3(1.0f, 0.0f, 0.0f), vec2(1.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f)),  // Bottom right vertex.
        TexturedColoredNormalVertex(vec3(-0.5f, 1.0f, 0.5f), vec3(1.0f, 0.0f, 0.0f), vec2(0.0f, 1.0f), vec3(0.0f, 0.0f, 1.0f)), // Top left vertex.

        TexturedColoredNormalVertex(vec3(0.5f, 0.0f, 0.5f), vec3(1.0f, 0.0f, 0.0f), vec2(1.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f)),  // Bottom right vertex.
        TexturedColoredNormalVertex(vec3(0.5f, 1.0f, 0.5f), vec3(1.0f, 0.0f, 0.0f), vec2(1.0f, 1.0f), vec3(0.0f, 0.0f, 1.0f)), // Top right vertex.
        TexturedColoredNormalVertex(vec3(-0.5f, 1.0f, 0.5f), vec3(1.0f, 0.0f, 0.0f), vec2(0.0f, 1.0f), vec3(0.0f, 0.0f, 1.0f)), // Top left vertex.

        // Top face, yellow.
        TexturedColoredNormalVertex(vec3(-0.5f, 1.0f, 0.5f), vec3(1.0f, 1.0f, 0.0f), vec2(0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f)), // Front left vertex.
        TexturedColoredNormalVertex(vec3(0.5f, 1.0f, 0.5f), vec3(1.0f, 1.0f, 0.0f), vec2(1.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f)),  // Front right vertex.
        TexturedColoredNormalVertex(vec3(-0.5f, 1.0f, -0.5f), vec3(1.0f, 1.0f, 0.0f), vec2(0.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f)), // Deep left vertex.

        TexturedColoredNormalVertex(vec3(0.5f, 1.0f, 0.5f), vec3(1.0f, 1.0f, 0.0f), vec2(1.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f)), // Front right vertex.
        TexturedColoredNormalVertex(vec3(0.5f, 1.0f, -0.5f), vec3(1.0f, 1.0f, 0.0f), vec2(1.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f)), // Deep right vertex.
        TexturedColoredNormalVertex(vec3(-0.5f, 1.0f, -0.5f), vec3(1.0f, 1.0f, 0.0f), vec2(0.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f)), // Deep left vertex.

        // Back face, green.
       TexturedColoredNormalVertex(vec3(-0.5f, 1.0f, -0.5f), vec3(0.0f, 1.0f, 0.0f), vec2(0.0f, 1.0f), vec3(0.0f, 0.0f, -1.0f)), // Top left vertex.
       TexturedColoredNormalVertex(vec3(0.5f, 1.0f, -0.5f), vec3(0.0f, 1.0f, 0.0f), vec2(1.0f, 1.0f), vec3(0.0f, 0.0f, -1.0f)), // Top right vertex.
       TexturedColoredNormalVertex(vec3(-0.5f, 0.0f, -0.5f), vec3(0.0f, 1.0f, 0.0f), vec2(0.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f)), // Bottom left vertex.

        TexturedColoredNormalVertex(vec3(0.5f, 1.0f, -0.5f), vec3(0.0f, 1.0f, 0.0f), vec2(1.0f, 1.0f), vec3(0.0f, 0.0f, -1.0f)), // Top right vertex.
        TexturedColoredNormalVertex(vec3(0.5f, 0.0f, -0.5f), vec3(0.0f, 1.0f, 0.0f), vec2(1.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f)), // Bottom right vertex.
        TexturedColoredNormalVertex(vec3(-0.5f, 0.0f, -0.5f), vec3(0.0f, 1.0f, 0.0f), vec2(0.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f)), // Bottom left vertex.

        // Bottom face, cyan.
        TexturedColoredNormalVertex(vec3(-0.5f, 0.0f, -0.5f), vec3(0.0f, 1.0f, 1.0f), vec2(0.0f, 0.0f), vec3(0.0f, -1.0f, 0.0f)), // Front left vertex.
        TexturedColoredNormalVertex(vec3(0.5f, 0.0f, -0.5f), vec3(0.0f, 1.0f, 1.0f), vec2(1.0f, 0.0f), vec3(0.0f, -1.0f, 0.0f)), // Front right vertex.
        TexturedColoredNormalVertex(vec3(-0.5f, 0.0f, 0.5f), vec3(0.0f, 1.0f, 1.0f), vec2(0.0f, 1.0f), vec3(0.0f, -1.0f, 0.0f)), // Deep left vertex.

        TexturedColoredNormalVertex(vec3(0.5f, 0.0f, -0.5f), vec3(0.0f, 1.0f, 1.0f), vec2(1.0f, 0.0f), vec3(0.0f, -1.0f, 0.0f)), // Front right vertex.
        TexturedColoredNormalVertex(vec3(0.5f, 0.0f, 0.5f), vec3(0.0f, 1.0f, 1.0f), vec2(1.0f, 1.0f), vec3(0.0f, -1.0f, 0.0f)), // Deep right vertex.
        TexturedColoredNormalVertex(vec3(-0.5f, 0.0f, 0.5f), vec3(0.0f, 1.0f, 1.0f), vec2(0.f, 1.f), vec3(0.0f, -1.0f, 0.0f)), // Deep left vertex.

        // Right face, blue.
        TexturedColoredNormalVertex(vec3(0.5f, 0.0f, 0.5f), vec3(0.0f, 0.0f, 1.0f), vec2(0.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f)), // Bottom front vertex.
        TexturedColoredNormalVertex(vec3(0.5f, 0.0f, -0.5f), vec3(0.0f, 0.0f, 1.0f), vec2(1.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f)), // Bottom deep vertex.
        TexturedColoredNormalVertex(vec3(0.5f, 1.0f, 0.5f), vec3(0.0f, 0.0f, 1.0f), vec2(0.0f, 1.0f), vec3(1.0f, 0.0f, 0.0f)), // Top front vertex.

        TexturedColoredNormalVertex(vec3(0.5f, 0.0f, -0.5f), vec3(0.0f, 0.0f, 1.0f), vec2(1.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f)), // Bottom deep vertex.
        TexturedColoredNormalVertex(vec3(0.5f, 1.0f, -0.5f), vec3(0.0f, 0.0f, 1.0f), vec2(1.0f, 1.0f), vec3(1.0f, 0.0f, 0.0f)), // Top deep vertex.
        TexturedColoredNormalVertex(vec3(0.5f, 1.0f, 0.5f), vec3(0.0f, 0.0f, 1.0f), vec2(0.0f, 1.0f), vec3(1.0f, 0.0f, 0.0f)), // Top front vertex.

        // Left face, magenta.
        TexturedColoredNormalVertex(vec3(-0.5f, 0.0f, 0.5f), vec3(1.0f, 0.0f, 1.0f), vec2(0.0f, 0.0f), vec3(-1.0f, 0.0f, 0.0f)), // Bottom front vertex.
        TexturedColoredNormalVertex(vec3(-0.5f, 1.0f, 0.5f), vec3(1.0f, 0.0f, 1.0f), vec2(0.0f, 1.0f), vec3(-1.0f, 0.0f, 0.0f)), // Top front vertex.
        TexturedColoredNormalVertex(vec3(-0.5f, 0.0f, -0.5f), vec3(1.0f, 0.0f, 1.0f), vec2(1.0f, 0.0f), vec3(-1.0f, 0.0f, 0.0f)), // Bottom deep vertex.

        TexturedColoredNormalVertex(vec3(-0.5f, 1.0f, 0.5f), vec3(1.0f, 0.0f, 1.0f), vec2(0.0f, 1.0f), vec3(-1.0f, 0.0f, 0.0f)), // Top front vertex.
        TexturedColoredNormalVertex(vec3(-0.5f, 1.0f, -0.5f), vec3(1.0f, 0.0f, 1.0f), vec2(1.0f, 1.0f), vec3(-1.0f, 0.0f, 0.0f)), // Top deep vertex.
        TexturedColoredNormalVertex(vec3(-0.5f, 0.0f, -0.5f), vec3(1.0f, 0.0f, 1.0f), vec2(1.0f, 0.0f), vec3(-1.0f, 0.0f, 0.0f))  // Bottom deep vertex.
    };

    // Create a vertex array
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

int createLinesVBO()
{
    vec3 vertexArray[] = {
        vec3(0.0f, 0.0f, 0.0f), // Point at origin.
        vec3(1.0f, 0.0f, 0.0f), // Red.
        vec3(5.0f, 0.0f, 0.0f), // Point at (5, 0, 0).
        vec3(1.0f, 0.0f, 0.0f), // Red.

        vec3(0.0f, 0.0f, 0.0f), // Point at origin.
        vec3(0.0f, 1.0f, 0.0f), // Green.
        vec3(0.0f, 5.0f, 0.0f), // Point at (0, 5, 0).
        vec3(0.0f, 1.0f, 0.0f), // Green.

        vec3(0.0f, 0.0f, 0.0f), // Point at origin.
        vec3(0.0f, 0.0f, 1.0f), // Blue.
        vec3(0.0f, 0.0f, 5.0f), // Point at (0, 0, 5).
        vec3(0.0f, 0.0f, 1.0f) // Blue.
    };

    // Create vertex array.
    GLuint vertexArrayObject;
    glGenVertexArrays(1, &vertexArrayObject);
    glBindVertexArray(vertexArrayObject);


    // Upload Vertex Buffer to GPU, save reference.
    GLuint vertexBufferObject;
    glGenBuffers(1, &vertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexArray), vertexArray, GL_STATIC_DRAW);

    glVertexAttribPointer(0,                   // attribute 0 matches aPos in Vertex Shader
        3,                   // size
        GL_FLOAT,            // type
        GL_FALSE,            // normalized?
        2 * sizeof(vec3), // stride - each vertex contain 2 vec3 (position, color)
        (void*)0             // array buffer offset
    );
    glEnableVertexAttribArray(0);


    glVertexAttribPointer(1,                            // attribute 1 matches aColor in Vertex Shader
        3,
        GL_FLOAT,
        GL_FALSE,
        2 * sizeof(vec3),
        (void*)sizeof(vec3)      // color is offseted a vec3 (comes after position)
    );
    glEnableVertexAttribArray(1);

    return vertexBufferObject;
}

int createPlaneVBO()
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

int createSphereVBO(float radius, float heightOffset, int radialSubdivisions, int verticalSubdivisions)
{
    vector<TexturedColoredNormalVertex> texturedSphereVertexVector;

    // Generate the vertices.
    // I originally referenced the code from http://www.songho.ca/opengl/gl_sphere.html, then completely redid the triangle drawing bits.

    const float PI = acos(-1);

    float x, y, z; // Vertex position.
    float u, v; // UV coordinate.
    float nx, ny, nz, lengthInv = 1.0f / radius; // Normal, will also be used as vertex colour.

    float radialStep = 2 * PI / radialSubdivisions;
    float verticalStep = PI / verticalSubdivisions;
    float radialAngle = 0.0f;
    float verticalAngle = 0.0f;

    // Sphere top cap.
    for (int i = 0; i < radialSubdivisions; i++)
    {
        float angleFromTop = verticalStep;

        // Summit
        // Vertex positions.
        x = 0;
        y = radius + heightOffset;
        z = 0;

        // Normalized vertex normal.
        nx = x * lengthInv;
        ny = y * lengthInv;
        nz = z * lengthInv;

        // UV coordinates.
        u = ((float)i/radialSubdivisions) + (1.0f/(2*radialSubdivisions));
        v = 0.0f;

        TexturedColoredNormalVertex newVertex = TexturedColoredNormalVertex(vec3(x, y, z), vec3(nx, ny, nz), vec2(u, v), vec3(nx, ny, nz));
        texturedSphereVertexVector.push_back(newVertex);

        // Bottom right vertex.
        // Vertex positions.
        x = radius * cos(radialAngle + radialStep) * sin(angleFromTop);
        y = radius * cos(angleFromTop) + heightOffset;
        z = radius * sin(radialAngle + radialStep) * sin(angleFromTop);

        // Normalized vertex normal.
        nx = x * lengthInv;
        ny = y * lengthInv;
        nz = z * lengthInv;

        // UV coordinates.
        u = ((float)i + 1.0f) / radialSubdivisions;
        v = 1.0f / verticalSubdivisions;

        newVertex = TexturedColoredNormalVertex(vec3(x, y, z), vec3(nx, ny, nz), vec2(u, v), vec3(nx, ny, nz));
        texturedSphereVertexVector.push_back(newVertex);

        // Bottom left vertex.
        // Vertex positions.
        x = radius * cos(radialAngle) * sin(angleFromTop);
        y = radius * cos(angleFromTop) + heightOffset;
        z = radius * sin(radialAngle) * sin(angleFromTop);

        // Normalized vertex normal.
        nx = x * lengthInv;
        ny = y * lengthInv;
        nz = z * lengthInv;

        // UV coordinates.
        u = (float)i / radialSubdivisions;
        v = 1.0f / verticalSubdivisions;

        newVertex = TexturedColoredNormalVertex(vec3(x, y, z), vec3(nx, ny, nz), vec2(u, v), vec3(nx, ny, nz));
        texturedSphereVertexVector.push_back(newVertex);


        radialAngle += radialStep;
    }
    
    // Sphere bottom cap.
    radialAngle = 0.0f;
    for (int i = 0; i < radialSubdivisions; i++)
    {
        float angleFromBottom = PI - verticalStep;

        // Summit
        // Vertex positions.
        x = 0;
        y = -radius + heightOffset;
        z = 0;

        // Normalized vertex normal.
        nx = x * lengthInv;
        ny = y * lengthInv;
        nz = z * lengthInv;

        // UV coordinates.
        u = ((float)i / radialSubdivisions) + (1.0f / (2 * radialSubdivisions));
        v = 1.0f;

        TexturedColoredNormalVertex newVertex = TexturedColoredNormalVertex(vec3(x, y, z), vec3(nx, ny, nz), vec2(u, v), vec3(nx, ny, nz));
        texturedSphereVertexVector.push_back(newVertex);

        // Top left vertex.
        // Vertex positions.
        x = radius * cos(radialAngle) * sin(angleFromBottom);
        y = radius * cos(angleFromBottom) + heightOffset;
        z = radius * sin(radialAngle) * sin(angleFromBottom);

        // Normalized vertex normal.
        nx = x * lengthInv;
        ny = y * lengthInv;
        nz = z * lengthInv;

        // UV coordinates.
        u = (float)i / radialSubdivisions;
        v = 1.0f - (1.0f / verticalSubdivisions);

        newVertex = TexturedColoredNormalVertex(vec3(x, y, z), vec3(nx, ny, nz), vec2(u, v), vec3(nx, ny, nz));
        texturedSphereVertexVector.push_back(newVertex);

        // Top right vertex.
        // Vertex positions.
        x = radius * cos(radialAngle + radialStep) * sin(angleFromBottom);
        y = radius * cos(angleFromBottom) + heightOffset;
        z = radius * sin(radialAngle + radialStep) * sin(angleFromBottom);

        // Normalized vertex normal.
        nx = x * lengthInv;
        ny = y * lengthInv;
        nz = z * lengthInv;

        // UV coordinates.
        u = ((float)i + 1.0f) / radialSubdivisions;
        v = 1.0f - (1.0f / verticalSubdivisions);

        newVertex = TexturedColoredNormalVertex(vec3(x, y, z), vec3(nx, ny, nz), vec2(u, v), vec3(nx, ny, nz));
        texturedSphereVertexVector.push_back(newVertex);


        radialAngle += radialStep;
    }

    // Sphere 'body'.
    for (int i = 1; i < verticalSubdivisions - 1; i++) // Vertical subdivisions.
    {
        verticalAngle += verticalStep;
        radialAngle = radians(0.0f);

        for (int j = 0; j < radialSubdivisions; j++) // Radial subdivisions.
        {
            // Triangle 1.
            
            // Bottom left.
            // Vertex positions.
            x = radius * cos(radialAngle) * sin(verticalAngle);
            y = radius * cos(verticalAngle) + heightOffset;
            z = radius * sin(radialAngle) * sin(verticalAngle);

            // Normalized vertex normal.
            nx = x * lengthInv;
            ny = y * lengthInv;
            nz = z * lengthInv;

            // UV coordinates.
            u = (float)j / radialSubdivisions;
            v = (float)i / verticalSubdivisions;

            TexturedColoredNormalVertex newVertex = TexturedColoredNormalVertex(vec3(x, y, z), vec3(nx, ny, nz), vec2(u, v), vec3(nx, ny, nz));
            texturedSphereVertexVector.push_back(newVertex);

            // Top right.
            // Vertex positions.
            x = radius * cos(radialAngle + radialStep) * sin(verticalAngle + verticalStep);
            y = radius * cos(verticalAngle + verticalStep) + heightOffset;
            z = radius * sin(radialAngle + radialStep) * sin(verticalAngle + verticalStep);

            // Normalized vertex normal.
            nx = x * lengthInv;
            ny = y * lengthInv;
            nz = z * lengthInv;

            // UV coordinates.
            u = (float)(j + 1) / radialSubdivisions;
            v = (float)(i + 1) / verticalSubdivisions;

            newVertex = TexturedColoredNormalVertex(vec3(x, y, z), vec3(nx, ny, nz), vec2(u, v), vec3(nx, ny, nz));
            texturedSphereVertexVector.push_back(newVertex);

            // Top left.
            // Vertex positions.
            x = radius * cos(radialAngle) * sin(verticalAngle + verticalStep);
            y = radius * cos(verticalAngle + verticalStep) + heightOffset;
            z = radius * sin(radialAngle) * sin(verticalAngle + verticalStep);

            // Normalized vertex normal.
            nx = x * lengthInv;
            ny = y * lengthInv;
            nz = z * lengthInv;

            // UV coordinates.
            u = (float)j / radialSubdivisions;
            v = (float)(i + 1) / verticalSubdivisions;

            newVertex = TexturedColoredNormalVertex(vec3(x, y, z), vec3(nx, ny, nz), vec2(u, v), vec3(nx, ny, nz));
            texturedSphereVertexVector.push_back(newVertex);


            // Triangle 2.

            // Bottom left.
            // Vertex positions.
            x = radius * cos(radialAngle) * sin(verticalAngle);
            y = radius * cos(verticalAngle) + heightOffset;
            z = radius * sin(radialAngle) * sin(verticalAngle);

            // Normalized vertex normal.
            nx = x * lengthInv;
            ny = y * lengthInv;
            nz = z * lengthInv;

            // UV coordinates.
            u = (float)j / radialSubdivisions;
            v = (float)i / verticalSubdivisions;

            newVertex = TexturedColoredNormalVertex(vec3(x, y, z), vec3(nx, ny, nz), vec2(u, v), vec3(nx, ny, nz));
            texturedSphereVertexVector.push_back(newVertex);

            // Bottom right.
            // Vertex positions.
            x = radius * cos(radialAngle + radialStep) * sin(verticalAngle);
            y = radius * cos(verticalAngle) + heightOffset;
            z = radius * sin(radialAngle + radialStep) * sin(verticalAngle);

            // Normalized vertex normal.
            nx = x * lengthInv;
            ny = y * lengthInv;
            nz = z * lengthInv;

            // UV coordinates.
            u = (float)(j + 1) / radialSubdivisions;
            v = (float)i / verticalSubdivisions;

            newVertex = TexturedColoredNormalVertex(vec3(x, y, z), vec3(nx, ny, nz), vec2(u, v), vec3(nx, ny, nz));
            texturedSphereVertexVector.push_back(newVertex);

            // Top right.
            // Vertex positions.
            x = radius * cos(radialAngle + radialStep) * sin(verticalAngle + verticalStep);
            y = radius * cos(verticalAngle + verticalStep) + heightOffset;
            z = radius * sin(radialAngle + radialStep) * sin(verticalAngle + verticalStep);

            // Normalized vertex normal.
            nx = x * lengthInv;
            ny = y * lengthInv;
            nz = z * lengthInv;

            // UV coordinates.
            u = (float)(j + 1) / radialSubdivisions;
            v = (float)(i + 1) / verticalSubdivisions;

            newVertex = TexturedColoredNormalVertex(vec3(x, y, z), vec3(nx, ny, nz), vec2(u, v), vec3(nx, ny, nz));
            texturedSphereVertexVector.push_back(newVertex);


            radialAngle += radialStep;
        }
    }


    // Create a vertex array
    GLuint vertexArrayObject;
    glGenVertexArrays(1, &vertexArrayObject);
    glBindVertexArray(vertexArrayObject);

    // Upload Vertex Buffer to GPU, save reference.
    GLuint vertexBufferObject;
    glGenBuffers(1, &vertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);

    glBufferData(GL_ARRAY_BUFFER, texturedSphereVertexVector.size() * sizeof(TexturedColoredNormalVertex), &texturedSphereVertexVector[0], GL_STATIC_DRAW);

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


// Terrain generation.
std::map<vec2, TexturedColoredNormalVertex, CompareVec2> terrainVertexMap; // This wouldn't let me declare it in the .h file :/

// uvTiling = how many quads does the texture stretch across before being repeated?
std::map<vec2, TexturedColoredNormalVertex, CompareVec2> createGroundVertexMap(unsigned int sizeX, unsigned int sizeZ, float uvTiling)
{
    //std::map<vec2, TexturedColoredNormalVertex, CompareVec2> terrainVertexMap;

    // Vertex parameters.
    vec3 position;
    vec3 color = vec3(1.0f, 1.0f, 1.0f);
    vec3 normals = vec3(0.0f, 1.0f, 0.0f);
    vec2 uv;

    float yCoord = 0.0f;

    for (int z = 0; z <= sizeZ; z++) // Columns.
    {
        for (int x = 0; x <= sizeX; x++) // Rows.
        {
            yCoord = generateHeightCoord(x, z, 0.05f); // Generate basic height variation using a perlin noise.
            yCoord += (sin((float)x) / 2 + (rand() % 12 + 1)) / 20; // Generate aditional variations using random numbers and a sin wave.

            position = vec3((float)x, yCoord, (float)z);
            uv = generateUVCoords(x, z, uvTiling);

            vec2 currentKey = vec2(x, z);
            TexturedColoredNormalVertex currentVertex = TexturedColoredNormalVertex(position, color, uv, normals);

            terrainVertexMap.insert(std::make_pair(currentKey, currentVertex));
        }
    }

    // Generate normals that account for the variable terrain height. Exclude the vertices at the very edges of the grid.
    for (int z = 1; z < sizeZ - 1; z++) // Columns.
    {
        for (int x = 1; x < sizeX - 1; x++) // Rows.
        {
            vec3 topLeft = generateFaceNormals(terrainVertexMap[vec2(x, z)].position, terrainVertexMap[vec2(x, z + 1)].position, terrainVertexMap[vec2(x + 1, z)].position);
            vec3 topMid = generateFaceNormals(terrainVertexMap[vec2(x, z)].position, terrainVertexMap[vec2(x - 1, z + 1)].position, terrainVertexMap[vec2(x, z + 1)].position);
            vec3 topRight = generateFaceNormals(terrainVertexMap[vec2(x, z)].position, terrainVertexMap[vec2(x - 1, z)].position, terrainVertexMap[vec2(x - 1, z - 1)].position);

            vec3 bottomLeft = generateFaceNormals(terrainVertexMap[vec2(x, z)].position, terrainVertexMap[vec2(x + 1, z)].position, terrainVertexMap[vec2(x + 1, z - 1)].position);
            vec3 bottomMid = generateFaceNormals(terrainVertexMap[vec2(x, z)].position, terrainVertexMap[vec2(x + 1, z - 1)].position, terrainVertexMap[vec2(x, z - 1)].position);
            vec3 bottomRight = generateFaceNormals(terrainVertexMap[vec2(x, z)].position, terrainVertexMap[vec2(x, z - 1)].position, terrainVertexMap[vec2(x - 1, z)].position);

            terrainVertexMap[vec2(x, z)].normals = (topLeft + topMid + topRight + bottomLeft + bottomMid + bottomRight) / 6.0f; // Divise the value by six to normalize it.
            //terrainVertexMap[vec2(x, z)].normals.x *= 0.75f;
            //terrainVertexMap[vec2(x, z)].normals.z *= 0.75f;

            cout << "Vertex normals at " << terrainVertexMap[vec2(x, z)].position.x << ", " << terrainVertexMap[vec2(x, z)].position.z << " :\n";
            cout << terrainVertexMap[vec2(x, z)].normals.x << ", " << terrainVertexMap[vec2(x, z)].normals.y << ", " << terrainVertexMap[vec2(x, z)].normals.z << " :\n";
        }
    }

    return terrainVertexMap;
}

// We are using this noise library: https://github.com/Reputeless/PerlinNoise.
float generateHeightCoord(unsigned int xCoord, unsigned int zCoord, float noiseScaling)
{
    // Do the perlin noise ~~
    const siv::PerlinNoise::seed_type seed = 42069u;
    const siv::PerlinNoise perlin{seed};

    const float noise = perlin.normalizedOctave2D((float)xCoord * noiseScaling, (float)zCoord * noiseScaling, 5);

    return noise * 5;
}

vec2 generateUVCoords(unsigned int posX, unsigned int posZ, float uvTiling)
{
    float uvPosX = (uvTiling == 1) ? posX : ((float)posX / uvTiling);
    float uvPosY = (uvTiling == 1) ? posZ : ((float)posZ / uvTiling);

    return vec2(uvPosX, uvPosY);
}

// Based on the algorithm at https://www.khronos.org/opengl/wiki/Calculating_a_Surface_Normal
vec3 generateFaceNormals(vec3 pointAPos, vec3 pointBPos, vec3 pointCPos)
{
    vec3 vectorU = pointBPos - pointAPos;
    vec3 vectorV = pointCPos - pointAPos;

    vec3 faceNormals = vec3(0.0f);
    faceNormals.x = (vectorU.y * vectorV.z) - (vectorU.z * vectorV.y);
    faceNormals.y = (vectorU.z * vectorV.x) - (vectorU.x * vectorV.z);
    faceNormals.z = (vectorU.x * vectorV.y) - (vectorU.y * vectorV.x);

    return faceNormals;
}

vector<TexturedColoredNormalVertex> createGroundVertexVector(std::map<vec2, TexturedColoredNormalVertex, CompareVec2> terrainVertexMap, unsigned int sizeX, unsigned int sizeZ)
{
    vector<TexturedColoredNormalVertex> vertexVector;

    for (int z = 0; z < sizeZ; z++) // Columns.
    {
        for (int x = 0; x < sizeX; x++) // Rows.
        {
            // Bottom triangle.
            vertexVector.push_back(terrainVertexMap[vec2(x, z)]); // (0, 0).
            vertexVector.push_back(terrainVertexMap[vec2(x, z + 1)]); // (0, 1).
            vertexVector.push_back(terrainVertexMap[vec2(x + 1, z)]); // (1, 0).
           
            // Top triangle.
            vertexVector.push_back(terrainVertexMap[vec2(x + 1, z)]); // (1, 0).
            vertexVector.push_back(terrainVertexMap[vec2(x, z + 1)]); // (0, 1).
            vertexVector.push_back(terrainVertexMap[vec2(x + 1, z + 1)]); // (1, 1). 
        }
    }

    return vertexVector;
}

int createGroundVBO(unsigned int sizeX, unsigned int sizeZ, float uvTiling)
{
    // Generate vertices.
    std::map<vec2, TexturedColoredNormalVertex, CompareVec2> groundVertexMap = createGroundVertexMap(sizeX, sizeZ, uvTiling);
    vector<TexturedColoredNormalVertex> groundVertexVector = createGroundVertexVector(groundVertexMap, sizeX, sizeZ);

    // Create a vertex array
    GLuint vertexArrayObject;
    glGenVertexArrays(1, &vertexArrayObject);
    glBindVertexArray(vertexArrayObject);

    // Upload Vertex Buffer to GPU, save reference.
    GLuint vertexBufferObject;
    glGenBuffers(1, &vertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);

    glBufferData(GL_ARRAY_BUFFER, groundVertexVector.size() * sizeof(TexturedColoredNormalVertex), &groundVertexVector[0], GL_STATIC_DRAW);

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

// Utility.
float returnHeightAtPoint(vec2 pointCoords)
{
    float heightAtPoint;
    cout << "Calculating ground height at point: " << pointCoords.x << ", " << pointCoords.y << ".\n";
    
    float closestLowX = floor(pointCoords.x);
    float closestHighX = closestLowX + 1;
    float xCoordDelta = pointCoords.x - closestLowX;

    /*
    cout << "Closest low x: " << closestLowX << ".\n";
    cout << "Closest high x: " << closestHighX << ".\n";
    cout << "Distance from closest low x: " << xCoordDelta << ".\n";
    */

    float closestLowZ = floor(pointCoords.y);
    float closestHighZ = closestLowZ + 1;
    float zCoordDelta = pointCoords.y - closestLowZ;

    /*
    cout << "Closest low z: " << closestLowZ << ".\n";
    cout << "Closest high z: " << closestHighZ << ".\n";
    cout << "Distance from closest low z: " << zCoordDelta << ".\n";
    */

    float lowXlowZHeight = terrainVertexMap[vec2(closestLowX, closestLowZ)].position.y;
    //cout << "Height at low x, low z: " << lowXlowZHeight << ".\n";
    float highXlowZHeight = terrainVertexMap[vec2(closestHighX, closestLowZ)].position.y;
    //cout << "Height at high x, low z: " << highXlowZHeight << ".\n";
    float lowXhighZHeight = terrainVertexMap[vec2(closestLowX, closestHighZ)].position.y;
    //cout << "Height at low x, high z: " << lowXhighZHeight << ".\n";
    float highXhighZHeight = terrainVertexMap[vec2(closestHighX, closestHighZ)].position.y;
    //cout << "Height at high x, high z: " << highXhighZHeight << ".\n";
    

    // Verify whether the point exists as is in the ground vertex map.
    if (pointCoords.x == closestLowX && pointCoords.y == closestLowZ)
    {
        return lowXlowZHeight;
    }
    if (pointCoords.x == closestHighX && pointCoords.y == closestLowZ)
    {
        return highXlowZHeight;
    }
    if (pointCoords.x == closestLowX && pointCoords.y == closestHighZ)
    {
        return lowXhighZHeight;
    }
    if (pointCoords.x == closestHighX && pointCoords.y == closestHighZ)
    {
        return highXhighZHeight;
    }


    // Otherwise,
    if (pointCoords.y > (1 - pointCoords.x)) // Point situated inside the top triangle.
    {
        //cout << "We are in the top triangle.\n";

        float heightAtXEdge = lerp(lowXhighZHeight, highXhighZHeight, xCoordDelta);
        //cout << "The midpoint height on the X edge is: " << heightAtXEdge << ".\n";
        heightAtPoint = lerp(highXlowZHeight, heightAtXEdge, zCoordDelta);
        
    }
    else // Point situated inside the bottom triangle.
    {
        //cout << "We are in the bottom triangle.\n";

        float heightAtXEdge = lerp(lowXlowZHeight, highXlowZHeight, xCoordDelta);
        //cout << "The midpoint height on the X edge is: " << heightAtXEdge << ".\n";
        heightAtPoint = lerp(heightAtXEdge, lowXhighZHeight, zCoordDelta);
    }

    cout << "The height at this point is: " << heightAtPoint << ".\n";
    return heightAtPoint;
}