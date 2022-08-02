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

uniform float light_cutoff_angle;
uniform float light_interior_angle;

uniform vec3 light2_color;
uniform vec3 light2_position;
uniform vec3 light2_direction;

uniform float light2_cutoff_angle;
uniform float light2_interior_angle;


uniform bool render_lighting = true;
uniform bool render_main_light = true;
uniform bool render_flash_light = true;
uniform bool render_shadows;

uniform float shadingSpecularStrength = 0.5;
uniform float shadingSpecularPower = 10;

//

const float PI = 3.1415926535897932384626433832795;

const float shadingDiffuseStrength = 0.95;
const float light2_intensity = 3.0f;

//

float diffuse(vec3 lightDir)
{	
	vec3 normal = normalize(fs_in.Normal);
	vec3 normlalizedLightDirection = normalize(lightDir);
	
	return shadingDiffuseStrength * max(dot(normal, normlalizedLightDirection), 0.0f);
}

float specular(vec3 lightDir)
{
	vec3 normal = normalize(fs_in.Normal);
	vec3 normlalizedLightDirection = normalize(lightDir);
	
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

float spotLightScalar(vec3 lightPos, vec3 lightDir, float lightCutoffAngle, float lightInteriorAngle)
{
	vec3 positionalLightDir = normalize(lightPos - fs_in.FragPos);
	//vec3 normlalizedLightDirection = normalize(lightDir);
	
	// Correct weird behaviour.
	float inverseCutOffAngle = radians(360.0f) - lightCutoffAngle;
	float inverseInteriorAngle = radians(360.0f) - lightInteriorAngle;
	
	float theta = dot(positionalLightDir, lightDir);
	
	if (theta > inverseInteriorAngle)
	{
		return 1.0f;
	}
	else if (theta > inverseCutOffAngle)
	{
		return (1.0 - cos(PI * (theta - inverseCutOffAngle) / (inverseInteriorAngle - inverseCutOffAngle))) / 2.0;
	}
	else
	{
		return 0.0f;
	}
	
}

float flashLightScalar(vec3 lightPos, vec3 lightDir, float lightCutoffAngle, float lightInteriorAngle)
{
	vec3 positionalLightDir = normalize(lightPos - fs_in.FragPos);
	vec3 normlalizedLightDirection = normalize(lightDir);
	
	float theta = dot(positionalLightDir, normlalizedLightDirection);
	
	float flashLightCone;
	
	float inner = lightInteriorAngle; // radians(57.0f);
	float outer = lightCutoffAngle; //radians(55.0f);
	
	if (theta > inner)
	{
		return 1.0f;
	}
	else if (theta > outer)
	{
		return (1.0 - cos(PI * (theta - outer) / (inner - outer))) / 2.0;
	}
	else
	{
		return 0.0f;
	}
}

//

void main()
{           
    // Main light.
	float diffuseLighting = diffuse(light_direction);
	float specularLighting = specular(light_direction);
	float shadow = render_shadows ? (1.0f - shadowCalculationFiltered(fs_in.FragPosLightSpace)) : 1.0f;
	float mainLightCone = spotLightScalar(light_position, light_direction, light_cutoff_angle, light_interior_angle);
	vec3 lightingMain = light_color * mainLightCone * shadow * (diffuseLighting + specularLighting);
	
	// Secondary/Flash light.
	float diffuseLighting2 = diffuse(light2_direction);
	float specularLighting2 = specular(light2_direction);
	float flashLightCone = flashLightScalar(light2_position, light2_direction, light2_cutoff_angle, light2_interior_angle);
	vec3 lightingFlash = light2_color * light2_intensity * flashLightCone * (diffuseLighting2 + specularLighting2);
	
	vec3 allLighting = ambient_colour + (render_main_light ? lightingMain : vec3(0.0f)) + (render_flash_light ? lightingFlash : vec3(0.0f));
	
	vec3 textureColor = texture(textureSampler, fs_in.TexCoords).rgb;
	vec3 screenColor = render_lighting ? (colour * textureColor * allLighting) : (colour * textureColor); 

	FragColor = vec4(screenColor, alpha);
}