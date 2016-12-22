#version 450 core

in vec2 outTexcoord;
in vec3 outNormal;

out vec3 color;

uniform vec3 meshColor;

void main()
{
	color = meshColor * (outNormal + 1.0) * 0.5;
}