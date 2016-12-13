#version 450 core

in vec3 outColor;
out vec3 color;

layout(location = 0) uniform vec3 Color;

void main()
{
	color = Color * outColor;
}