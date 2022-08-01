#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

uniform mat4 worldMatrix;
uniform mat4 viewMatrix = mat4(1.0);  // default value for view matrix (identity)
uniform mat4 projectionMatrix = mat4(1.0);

out vec3 vertexColor;

void main()
{
   gl_PointSize = 10.0;

   vertexColor = aColor + vec3(aPos.x/10, aPos.y/10,aPos.z/10); // Both the vertices' colours and position will influence the final result.
   mat4 modelViewProjection = projectionMatrix * viewMatrix * worldMatrix;
   gl_Position = modelViewProjection * vec4(aPos.x, aPos.y, aPos.z, 1.0);
}