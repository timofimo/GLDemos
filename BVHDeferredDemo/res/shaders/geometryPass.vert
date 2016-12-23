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
	vec3 position;
} camera;

uniform mat4 modelMatrix;

void main()
{
	gl_Position = camera.viewProjectionMatrix * modelMatrix * vec4(inPosition, 1.0);
	vTexcoord = inTexcoord;
	vNormal = mat3(modelMatrix) * inNormal;
}