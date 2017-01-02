#version 450 core
#extension GL_ARB_shader_draw_parameters : require
layout(location = 0) in vec3 inPosition;

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

out vec4 vPosition;
flat out uint vDrawIndex;

void main()
{
	vPosition = camera.viewProjectionMatrix * vec4(inPosition * pl.pointLights[gl_DrawIDARB].range + pl.pointLights[gl_DrawIDARB].position, 1);
	gl_Position = vPosition;
	vDrawIndex = gl_DrawIDARB;
}