#version 450 core

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec3 inColor;

out vec3 outColor;

void main()
{
	gl_Position.xy = inPosition;
	gl_Position.zw = vec2(0.0, 1.0) ;
	outColor = inColor;

}