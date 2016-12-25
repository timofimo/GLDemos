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
};

uniform PointLightBlock
{
	PointLight pointLights[2048];
} pl;

/*buffer PointLightBlock
{
	PointLight pointLights[];
} pl;*/

struct FlatNode
{
	uint start;
	float min_x, min_y, min_z;
	float max_x, max_y, max_z;
	uint rightOffset;
};

uniform FlatNodeBlock
{
	FlatNode flatNodes[2048];
} bvh;

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

bool Contains(vec3 min, vec3 max, vec3 p)
{
	return p.x >= min.x && p.x <= max.x &&
		p.y >= min.y && p.y <= max.y &&
		p.z >= min.z && p.z <= max.z;
}

void main()
{
	vec4 color = texture(colorSampler, vTexcoord);
	vec3 normal = normalize(texture(normalSampler, vTexcoord).xyz * 2.0 - 1.0);
	float depth = texture(depthSampler, vTexcoord).r;
	
	vec4 worldPos = camera.invViewProjectionMatrix * vec4(vTexcoord.x * 2.0 - 1.0, vTexcoord.y * 2.0 - 1.0, depth * 2.0 - 1.0, 1);
	worldPos.xyz /= worldPos.w;
	
	uint todo[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	int stackptr = 0;
	
	vec4 lightColor = vec4(0);
	
	while(stackptr >= 0)
	{
		uint ni = todo[stackptr];
		--stackptr;
		
		FlatNode node = bvh.flatNodes[ni];
		if(node.rightOffset == 0)
		{
			if(Contains(vec3(node.min_x, node.min_y, node.min_z), vec3(node.max_x, node.max_y, node.max_z), worldPos.xyz))
				lightColor += CalcPointLight(pl.pointLights[node.start].position, pl.pointLights[node.start].color, pl.pointLights[node.start].intensity, pl.pointLights[node.start].range, worldPos.xyz, normal);
		}
		else
		{
			FlatNode n = bvh.flatNodes[ni + 1];
			if(Contains(vec3(n.min_x, n.min_y, n.min_z), vec3(n.max_x, n.max_y, n.max_z), worldPos.xyz))
				todo[++stackptr] = ni + 1;
				
			n = bvh.flatNodes[ni + node.rightOffset];
			if(Contains(vec3(n.min_x, n.min_y, n.min_z), vec3(n.max_x, n.max_y, n.max_z), worldPos.xyz))
				todo[++stackptr] = ni + node.rightOffset;
		}
	}
	
	outColor = color * lightColor;
}