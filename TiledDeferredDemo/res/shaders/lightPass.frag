#version 450 core

#define TILE_GROUP_DIM 16
#define MAX_LIGHTS_PER_TILE 128

in vec2 vTexcoord;

uniform sampler2D colorSampler;
uniform sampler2D normalSampler;
uniform sampler2D depthSampler;
uniform uint numPointLights;

uniform CameraBlock
{
	mat4 viewProjectionMatrix;
	mat4 invViewProjectionMatrix;
	mat4 viewMatrix;
	mat4 projectionMatrix;
	vec3 position;
	float paddin;
	vec2 screenDimensions;
} camera;

struct PointLight
{
	vec3 position;
	float range;
	vec3 color;
	float intensity;
};

uniform PointLightBlock
{
	PointLight pointLights[1024];
} pl;

buffer LightListIndices
{
	int lightListIndices[];
};

out vec4 outColor;

vec4 CalcLight(vec3 color, float intensity, vec3 direction, vec3 worldPos, vec3 normal)
{
	float diffuseFactor = dot(normal, -direction);
	
	vec4 diffuseColor = vec4(0);
	vec4 specularColor = vec4(0);
	
	if(diffuseFactor > 0)
	{
		diffuseColor = vec4(color, 1) * intensity * diffuseFactor;
		
		vec3 directionToEye = normalize(camera.position - worldPos);
		vec3 reflectDirection = normalize(reflect(direction, normal));
		
		float specularFactor = dot(directionToEye, reflectDirection);
		specularFactor = pow(specularFactor, 10.0);
		
		if(specularFactor > 0)
		{
			specularColor = vec4(color, 1) * 0.5 * specularFactor * diffuseFactor;
		}
	}
	
	return diffuseColor + specularColor;
}

vec4 CalcPointLight(vec3 position, vec3 color, float intensity, float range, vec3 worldPos, vec3 normal)
{
	vec3 lightDirection = worldPos - position;
	float distanceToPoint = length(lightDirection);
	
	if(distanceToPoint > range)
	{
		return vec4(0);
	}
	
	lightDirection /= distanceToPoint;
	
	vec4 result = CalcLight(color, intensity, lightDirection, worldPos, normal);
	
	float attenuation = pow(max(1.0 - pow(distanceToPoint, 2.0) / pow(range, 2.0), 0), 2.0);
	
	return result * attenuation;
}

void main()
{
	vec4 color = texture(colorSampler, vTexcoord);
	vec3 normal = normalize(texture(normalSampler, vTexcoord).xyz * 2.0 - 1.0);
	float depth = texture(depthSampler, vTexcoord).r;
	
	vec4 worldPos = camera.invViewProjectionMatrix * vec4(vTexcoord.x * 2.0 - 1.0, vTexcoord.y * 2.0 - 1.0, depth * 2.0 - 1.0, 1);
	worldPos.xyz /= worldPos.w;
	
	ivec2 location = ivec2(gl_FragCoord.xy);
	ivec2 tileID = location / ivec2(16, 16);
	uint index = tileID.y * (uint(camera.screenDimensions.x) / TILE_GROUP_DIM) + tileID.x;
	uint offset = index * MAX_LIGHTS_PER_TILE;
	
	vec4 lightColor = vec4(0);
	for(uint i = offset; i < offset + MAX_LIGHTS_PER_TILE; i++)
	{
		int lightIndex = lightListIndices[i];
		if(lightIndex == -1)
			break;
			
		lightColor += CalcPointLight(pl.pointLights[lightIndex].position, pl.pointLights[lightIndex].color, pl.pointLights[lightIndex].intensity, pl.pointLights[lightIndex].range, worldPos.xyz, normal);
	}
	
	/*for(uint i = offset; i < offset + MAX_LIGHTS_PER_TILE; i++)
	{
		int lightIndex = lightListIndices[i];
		if(lightIndex == -1)
			break;
			
		lightColor += (1.0 / float(MAX_LIGHTS_PER_TILE));
	}*/
	
	/*for(uint i = 0; i < MAX_LIGHTS_PER_TILE; i++)
	{
		lightColor += CalcPointLight(pl.pointLights[i].position, pl.pointLights[i].color, pl.pointLights[i].intensity, pl.pointLights[i].range, worldPos.xyz, normal);
	}*/
	
	outColor = color * lightColor;
}