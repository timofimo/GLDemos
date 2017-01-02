#version 450 core

in vec4 vPosition;
flat in uint vDrawIndex;

uniform sampler2D colorSampler;
uniform sampler2D normalSampler;
uniform sampler2D depthSampler;

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
	else
		discard;
	
	return diffuseColor + specularColor;
}

vec4 CalcPointLight(vec3 position, vec3 color, float intensity, float range, vec3 worldPos, vec3 normal)
{
	vec3 lightDirection = worldPos - position;
	float distanceToPoint = length(lightDirection);
	
	if(distanceToPoint > range)
		discard;
	
	lightDirection /= distanceToPoint;
	
	vec4 result = CalcLight(color, intensity, lightDirection, worldPos, normal);
	
	float attenuation = pow(max(1.0 - pow(distanceToPoint, 2.0) / pow(range, 2.0), 0), 2.0);
	
	return result * attenuation;
}

void main()
{
	vec2 texcoord = (vPosition.xy / vPosition.w + 1) * 0.5;
	vec4 color = texture(colorSampler, texcoord);
	vec3 normal = normalize(texture(normalSampler, texcoord).xyz * 2.0 - 1.0);
	float depth = texture(depthSampler, texcoord).r;
	
	vec4 worldPos = camera.invViewProjectionMatrix * vec4(texcoord.x * 2.0 - 1.0, texcoord.y * 2.0 - 1.0, depth * 2.0 - 1.0, 1);
	worldPos.xyz /= worldPos.w;
	
	vec4 lightColor = CalcPointLight(pl.pointLights[vDrawIndex].position, pl.pointLights[vDrawIndex].color, pl.pointLights[vDrawIndex].intensity, pl.pointLights[vDrawIndex].range, worldPos.xyz, normal);

	outColor = color * lightColor;
}