#version 450 core

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexcoord;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec3 inTangent;

out vec2 outTexcoord;

uniform CameraBlock
{
	mat4 viewProjectionMatrix;
} camera;

uniform mat4 modelMatrix;

void main()
{
	gl_Position = camera.viewProjectionMatrix * modelMatrix * vec4(inPosition, 1.0);
	outTexcoord = inTexcoord;
}