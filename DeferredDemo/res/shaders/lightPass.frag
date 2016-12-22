#version 450 core

in vec2 vTexcoord;

uniform sampler2D colorSampler;
uniform sampler2D normalSampler;
uniform sampler2D depthSampler;
uniform uint numPointLights;

uniform CameraBlock
{
	mat4 viewProjectionMatrix;
	mat4 invViewProjectionMatrix;
	vec3 position;
} camera;

struct PointLight
{
	vec3 position;
	float range;
	vec3 color;
	float intensity;
	float exponent;
	float linear;
	float constant;
	float padding;
};

uniform PointLightBlock
{
	PointLight pointLights[1024];
} pl;

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

vec4 CalcPointLight(vec3 position, vec3 color, float intensity, float range, float exponent, float linear, float constant, vec3 worldPos, vec3 normal)
{
	vec3 lightDirection = worldPos - position;
	float distanceToPoint = length(lightDirection);
	
	if(distanceToPoint > range)
	{
		return vec4(0);
	}
	
	lightDirection /= distanceToPoint;
	
	vec4 result = CalcLight(color, intensity, lightDirection, worldPos, normal);
	
	float attenuation = constant + linear * distanceToPoint + exponent * distanceToPoint * distanceToPoint + 0.0001;
	
	return result / attenuation;
}

void main()
{
	vec4 color = texture(colorSampler, vTexcoord);
	vec3 normal = normalize(texture(normalSampler, vTexcoord).xyz * 2.0 - 1.0);
	float depth = texture(depthSampler, vTexcoord).r;
	
	vec4 worldPos = camera.invViewProjectionMatrix * vec4(vTexcoord.x * 2.0 - 1.0, vTexcoord.y * 2.0 - 1.0, depth * 2.0 - 1.0, 1);
	worldPos.xyz /= worldPos.w;
	
	vec4 lightColor = vec4(0);
	for(int i = 0; i < numPointLights; i++)
	{
		lightColor += CalcPointLight(pl.pointLights[i].position, pl.pointLights[i].color, pl.pointLights[i].intensity, pl.pointLights[i].range, 
									pl.pointLights[i].exponent, pl.pointLights[i].linear, pl.pointLights[i].constant, worldPos.xyz, normal);
	}
	
	outColor = color * lightColor;
}