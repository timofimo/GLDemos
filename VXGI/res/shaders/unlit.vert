#version 450 core

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexcoord;
layout(location = 2) in vec3 inNormal;

out vec2 oTexcoord;
out vec3 oNormal;

uniform CameraBlock
{
	mat4 viewProjectionMatrix;
} camera;

uniform mat4 modelMatrix;

void main()
{
	gl_Position = camera.viewProjectionMatrix * modelMatrix * vec4(inPosition, 1.0);
	oTexcoord = inTexcoord;
	oNormal = normalize(mat3(modelMatrix) * inNormal);
}