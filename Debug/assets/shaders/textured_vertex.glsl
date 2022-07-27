#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 3) in vec3 aNormals;
layout (location = 2) in vec2 aUV;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} vs_out;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 worldMatrix;
uniform mat4 lightSpaceMatrix;

void main()
{    
    vs_out.FragPos = vec3(worldMatrix * vec4(aPos, 1.0));
    vs_out.Normal = transpose(inverse(mat3(worldMatrix))) * aNormals;
    vs_out.TexCoords = aUV;
    vs_out.FragPosLightSpace = lightSpaceMatrix * vec4(vs_out.FragPos, 1.0);
    gl_Position = projectionMatrix * viewMatrix * vec4(vs_out.FragPos, 1.0);
}