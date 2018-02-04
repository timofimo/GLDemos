#version 450 core

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexcoord;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec3 inTangent;

out vec2 vTexcoord;
out vec3 vNormal;

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

uniform mat4 modelMatrix;

void main()
{
	gl_Position = camera.viewProjectionMatrix * modelMatrix * vec4(inPosition, 1.0);
	vTexcoord = inTexcoord;
	vNormal = normalize(mat3(modelMatrix) * inNormal);
}