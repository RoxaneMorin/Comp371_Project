#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
    vec4 FragPosLight2Space;
} fs_in;

//

uniform sampler2D textureSampler;
uniform sampler2D shadowMap;
uniform sampler2D normalSampler;

uniform vec3 colour = vec3(1.0f, 1.0f, 1.0f);
uniform float alpha = 1.0f;

uniform vec3 view_position;

uniform vec3 ambient_colour;


uniform vec3 light_color;
uniform vec3 light_position;
uniform vec3 light_direction;


uniform float shadingSpecularStrength = 0.5;
uniform float shadingSpecularPower = 10;

//

const float PI = 3.1415926535897932384626433832795;

const float shadingDiffuseStrength = 0.95;
const float light2_intensity = 3.0f;


// Lighting.

float diffuse(vec3 lightDir, vec3 sourceNormal)
{	
	vec3 normal = normalize(sourceNormal);
	vec3 invertedLightDir =  vec3(-light_direction.x, light_direction.y, -light_direction.z);
	vec3 normlalizedLightDirection = normalize(invertedLightDir);
	
	return shadingDiffuseStrength * max(dot(normal, normlalizedLightDirection), 0.0f);
}

float specular(vec3 lightDir, vec3 sourceNormal)
{
	vec3 normal = normalize(sourceNormal);
	vec3 invertedLightDir =  vec3(-light_direction.x, light_direction.y, -light_direction.z);
	vec3 normlalizedLightDirection = normalize(invertedLightDir);
	
	vec3 viewDirection = normalize(view_position - fs_in.FragPos);
	
	vec3 reflectLightDirection = reflect(-normlalizedLightDirection, normal);
	
	return shadingSpecularStrength * pow(max(dot(reflectLightDirection, viewDirection), 0.0f), shadingSpecularPower) * 2;
}


float shadowCalculation(vec4 fragPosLightSpace)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
	
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    float currentDepth = projCoords.z;

    float bias = 0.001f;
	float shadow = (currentDepth - bias > closestDepth) ? 1.0f : 0.0f;

    return shadow;
}

float shadowCalculationFiltered(vec4 fragPosLightSpace)
{
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
	
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    float currentDepth = projCoords.z;
	
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightDir = normalize(light_position - fs_in.FragPos);
    float bias = 0.001f;

    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; x++)
    {
        for(int y = -1; y <= 1; y++)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}

// Normals.

vec3 handleNormalMap(sampler2D currentNormalSampler)
{
	vec3 normal = texture(currentNormalSampler, fs_in.TexCoords).rgb;
	normal = normalize(normal * 2.0f - 1.0f);
	
	// Compute tangent T and bitangent B.
	// The technique used was found here: https://community.khronos.org/t/computing-the-tangent-space-in-the-fragment-shader/52861.
	vec3 Q1 = dFdx(fs_in.FragPos);
	vec3 Q2 = dFdy(fs_in.FragPos);
	vec2 st1 = dFdx(fs_in.TexCoords);
	vec2 st2 = dFdy(fs_in.TexCoords);
	
	vec3 T = normalize(Q1 * st2.t - Q2 * st1.t);
	vec3 B = normalize(-Q1 * st2.s + Q2 * st1.s);
	//vec3 B = cross(T, texture(currentNormalSampler, fs_in.TexCoords).rgb); // Seems to mess up the shadow direction, try again with a real normal map.
	
	// Transpose texture-to-eye space matrix.
	mat3 TBN = mat3(T, B, fs_in.Normal);
	
	// Transform the normal into eye space.
	normal = normal * TBN;
	
	return vec3(normal);
}


// Main

void main()
{           
    vec3 normals = handleNormalMap(normalSampler);
	
	// Main light.
	float diffuseLighting = diffuse(light_direction, normals);
	float specularLighting = specular(light_direction, normals);
	float shadow = 1.0f - shadowCalculationFiltered(fs_in.FragPosLightSpace);
	vec3 lightingMain = light_color * shadow * (diffuseLighting + specularLighting);
	
	vec3 allLighting = ambient_colour + lightingMain;
	
	vec4 textureColor = texture(textureSampler, fs_in.TexCoords);
	if (textureColor.a < 0.01)
		discard;
	
	vec4 screenColor = textureColor * vec4(colour, alpha) * vec4(allLighting, alpha);

	FragColor = screenColor;
}