#version 450 core

layout(location = 0) in vec3 inPosition;

out vec2 vTexcoord;

void main()
{
	gl_Position = vec4(inPosition, 1);
	vTexcoord = (inPosition.xy + 1) * 0.5;
}