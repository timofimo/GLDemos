#version 450 core

layout(invocations = 5) in;
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vec2 oTexcoord[];
in vec3 oNormal[];

out vec2 gsTexcoord;
out flat int gsAxis;
out flat int gsInvocation;
out flat uint gsNormalMask;

uniform VoxelGrid
{
	mat4 projectionMatrix[3][5];
	vec4 worldPosition[5];	// w == scale of a voxel
	uint resolution;
	uint cascadeLevels;
} voxel;

int CalculateAxis()
{
	vec3 p1 = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;
	vec3 p2 = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;
	vec3 normal = abs(cross(p1, p2));

	if(normal.x > normal.y && normal.x > normal.z)
    {
		return 0;
	}
	else if(normal.y > normal.x && normal.y > normal.z)
    {
	    return 1;
    }
	else
    {
	    return 2;
	}
}

void main()
{
	gsAxis = CalculateAxis();
	gsInvocation = gl_InvocationID;
	gsNormalMask = 0;
	
	for(int i = 0; i < 3; i++)
	{
		gsNormalMask |= int(oNormal[i].x < -0.5);
		gsNormalMask |= int(oNormal[i].x > 0.5) << 1;
		gsNormalMask |= int(oNormal[i].y < -0.5) << 2;
		gsNormalMask |= int(oNormal[i].y > 0.5) << 3;
		gsNormalMask |= int(oNormal[i].z < -0.5) << 4;
		gsNormalMask |= int(oNormal[i].z > 0.5) << 5;
	
		gsTexcoord = oTexcoord[i];
		gl_Position = voxel.projectionMatrix[gsAxis][gl_InvocationID] * vec4(gl_in[i].gl_Position.xyz, 1.0);
		EmitVertex();
	}
	EndPrimitive();
}