#version 450 core

in vec2 outTexcoord;
out vec3 color;

uniform sampler2D textureSampler;

void main()
{
	color = texture(textureSampler, outTexcoord).rgb;
}