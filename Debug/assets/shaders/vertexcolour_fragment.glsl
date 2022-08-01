#version 330 core

in vec3 vertexColor;
uniform vec3 colour = vec3(1.0f, 1.0f, 1.0f);

out vec4 FragColor;

void main()
{
   FragColor = vec4(vertexColor, 1.0f);
}