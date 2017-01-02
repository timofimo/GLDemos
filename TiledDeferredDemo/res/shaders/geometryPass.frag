#version 450 core

in vec2 vTexcoord;
in vec3 vNormal;

layout(location = 0) out vec3 outColor;
layout(location = 1) out vec3 outNormal;

uniform vec3 meshColor;

void main()
{
	outColor = meshColor * (vNormal + 1.0) * 0.5;
	outNormal = (vNormal + 1.0) * 0.5;
}