#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} fs_in;

//

uniform sampler2D depthMap;
uniform sampler2D textureSamplerA;

uniform vec3 ambient_colour;
uniform vec3 light_color;
uniform vec3 light_position;
uniform vec3 view_position;

uniform bool render_shadows;

//

const float shadingDiffuseStrength = 0.9;
const float shadingSpecularStrength = 0.5;
const float shadingSpecularPower = 10;


//

float diffuse(vec3 light_position)
{	
	vec3 normal = normalize(fs_in.Normal);
	vec3 lightDirection = normalize(light_position - fs_in.FragPos);
	
	return shadingDiffuseStrength * max(dot(normal, lightDirection), 0.0f);
}

float specular(vec3 light_position)
{
	vec3 normal = normalize(fs_in.Normal);
	vec3 lightDirection = normalize(light_position - fs_in.FragPos);
	vec3 viewDirection = normalize(view_position - fs_in.FragPos);
	
	vec3 reflectLightDirection = reflect(-lightDirection, normal);
	
	return shadingSpecularStrength * pow(max(dot(reflectLightDirection, viewDirection), 0.0f), shadingSpecularPower);
}


float shadowCalculation(vec4 fragPosLightSpace)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
	
    float closestDepth = texture(depthMap, projCoords.xy).r; 
    float currentDepth = projCoords.z;

    float bias = 0.01f;
	float shadow = (currentDepth - bias > closestDepth) ? 1.0f : 0.0f;

    return shadow;
}

float shadowCalculationFiltered(vec4 fragPosLightSpace)
{
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
	
    float closestDepth = texture(depthMap, projCoords.xy).r; 
    float currentDepth = projCoords.z;
	
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightDir = normalize(light_position - fs_in.FragPos);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);

    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(depthMap, 0);
    for(int x = -1; x <= 1; x++)
    {
        for(int y = -1; y <= 1; y++)
        {
            float pcfDepth = texture(depthMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}

void main()
{           
    float diffuseLighting = diffuse(light_position);
	float specularLighting = specular(light_position);
	float shadow = render_shadows ? (1.0f - shadowCalculationFiltered(fs_in.FragPosLightSpace)) : 1.0f;
	vec3 combinedLighting = ambient_colour + shadow * (diffuseLighting + specularLighting) * light_color;
   
	vec3 textureColor = texture(textureSamplerA, fs_in.TexCoords).rgb;
	
	FragColor = vec4(textureColor * combinedLighting, 1.0); 
}