#version 450 core

layout(points) in;
layout(triangle_strip, max_vertices = 36) out;

in flat ivec3 oVoxel[];
in flat int oCascade[];

out vec4 oColor;

uniform CameraBlock
{
	mat4 viewProjectionMatrix;
} camera;

uniform VoxelGrid
{
	mat4 projectionMatrix[3][5];
	vec4 worldPosition[5];	// w == scale of a voxel
	uint resolution;
	uint cascadeLevels;
} voxel;

layout(rgba8)readonly restrict uniform image3D voxelDataAlbedo[5];
layout(r32ui)readonly restrict uniform uimage3D voxelDataFaces;

const vec3 CascadeColor[5] = vec3[5](
	vec3(1, 0, 0), vec3(0, 1, 0), vec3(0, 0, 1), vec3(1, 1, 0), vec3(1, 0, 1)
);

void main()
{
	oColor = imageLoad(voxelDataAlbedo[oCascade[0]], oVoxel[0].xyz);
	oColor.rgb *= CascadeColor[oCascade[0]];
	uint faces = imageLoad(voxelDataFaces, oVoxel[0].xyz).r;
	
	vec3 min = voxel.worldPosition[oCascade[0] - 1].xyz;
	vec3 max = min + voxel.worldPosition[oCascade[0] - 1].w * voxel.resolution;
	vec3 pos = gl_in[0].gl_Position.xyz;
	
	bool test = oCascade[0] == 0 ||
	pos.x < min.x || pos.x >= max.x ||
	pos.y < min.y || pos.y >= max.y ||
	pos.z < min.z || pos.z >= max.z;
	
	if(faces != 0 && test && oColor.a > 0.01)
	{
		// Front
		//oColor = vec4(0, 0, 1, 1);
		gl_Position = camera.viewProjectionMatrix * (gl_in[0].gl_Position + vec4(1.0, 0.0, 0.0, 0.0) * voxel.worldPosition[oCascade[0]].w);
		EmitVertex();
		gl_Position = camera.viewProjectionMatrix * (gl_in[0].gl_Position + vec4(0.0, 0.0, 0.0, 0.0) * voxel.worldPosition[oCascade[0]].w);
		EmitVertex();
		gl_Position = camera.viewProjectionMatrix * (gl_in[0].gl_Position + vec4(0.0, 1.0, 0.0, 0.0) * voxel.worldPosition[oCascade[0]].w);
		EmitVertex();
		EndPrimitive();
		
		gl_Position = camera.viewProjectionMatrix * (gl_in[0].gl_Position + vec4(1.0, 1.0, 0.0, 0.0) * voxel.worldPosition[oCascade[0]].w);
		EmitVertex();
		gl_Position = camera.viewProjectionMatrix * (gl_in[0].gl_Position + vec4(1.0, 0.0, 0.0, 0.0) * voxel.worldPosition[oCascade[0]].w);
		EmitVertex();
		gl_Position = camera.viewProjectionMatrix * (gl_in[0].gl_Position + vec4(0.0, 1.0, 0.0, 0.0) * voxel.worldPosition[oCascade[0]].w);
		EmitVertex();
		EndPrimitive();
		
		// Back
		gl_Position = camera.viewProjectionMatrix * (gl_in[0].gl_Position + vec4(0.0, 0.0, 1.0, 0.0) * voxel.worldPosition[oCascade[0]].w);
		EmitVertex();
		gl_Position = camera.viewProjectionMatrix * (gl_in[0].gl_Position + vec4(1.0, 0.0, 1.0, 0.0) * voxel.worldPosition[oCascade[0]].w);
		EmitVertex();
		gl_Position = camera.viewProjectionMatrix * (gl_in[0].gl_Position + vec4(0.0, 1.0, 1.0, 0.0) * voxel.worldPosition[oCascade[0]].w);
		EmitVertex();
		EndPrimitive();
		
		gl_Position = camera.viewProjectionMatrix * (gl_in[0].gl_Position + vec4(1.0, 0.0, 1.0, 0.0) * voxel.worldPosition[oCascade[0]].w);
		EmitVertex();
		gl_Position = camera.viewProjectionMatrix * (gl_in[0].gl_Position + vec4(1.0, 1.0, 1.0, 0.0) * voxel.worldPosition[oCascade[0]].w);
		EmitVertex();
		gl_Position = camera.viewProjectionMatrix * (gl_in[0].gl_Position + vec4(0.0, 1.0, 1.0, 0.0) * voxel.worldPosition[oCascade[0]].w);
		EmitVertex();
		EndPrimitive();	
		
		// Left
		//oColor = vec4(1, 0, 0, 1);
		gl_Position = camera.viewProjectionMatrix * (gl_in[0].gl_Position + vec4(0.0, 1.0, 0.0, 0.0) * voxel.worldPosition[oCascade[0]].w);
		EmitVertex();
		gl_Position = camera.viewProjectionMatrix * (gl_in[0].gl_Position + vec4(0.0, 0.0, 0.0, 0.0) * voxel.worldPosition[oCascade[0]].w);
		EmitVertex();
		gl_Position = camera.viewProjectionMatrix * (gl_in[0].gl_Position + vec4(0.0, 0.0, 1.0, 0.0) * voxel.worldPosition[oCascade[0]].w);
		EmitVertex();
		EndPrimitive();
		
		gl_Position = camera.viewProjectionMatrix * (gl_in[0].gl_Position + vec4(0.0, 1.0, 1.0, 0.0) * voxel.worldPosition[oCascade[0]].w);
		EmitVertex();
		gl_Position = camera.viewProjectionMatrix * (gl_in[0].gl_Position + vec4(0.0, 1.0, 0.0, 0.0) * voxel.worldPosition[oCascade[0]].w);
		EmitVertex();
		gl_Position = camera.viewProjectionMatrix * (gl_in[0].gl_Position + vec4(0.0, 0.0, 1.0, 0.0) * voxel.worldPosition[oCascade[0]].w);
		EmitVertex();
		EndPrimitive();
		
		// Right
		gl_Position = camera.viewProjectionMatrix * (gl_in[0].gl_Position + vec4(1.0, 0.0, 0.0, 0.0) * voxel.worldPosition[oCascade[0]].w);
		EmitVertex();
		gl_Position = camera.viewProjectionMatrix * (gl_in[0].gl_Position + vec4(1.0, 1.0, 0.0, 0.0) * voxel.worldPosition[oCascade[0]].w);
		EmitVertex();
		gl_Position = camera.viewProjectionMatrix * (gl_in[0].gl_Position + vec4(1.0, 0.0, 1.0, 0.0) * voxel.worldPosition[oCascade[0]].w);
		EmitVertex();
		EndPrimitive();
		
		gl_Position = camera.viewProjectionMatrix * (gl_in[0].gl_Position + vec4(1.0, 1.0, 0.0, 0.0) * voxel.worldPosition[oCascade[0]].w);
		EmitVertex();
		gl_Position = camera.viewProjectionMatrix * (gl_in[0].gl_Position + vec4(1.0, 1.0, 1.0, 0.0) * voxel.worldPosition[oCascade[0]].w);
		EmitVertex();
		gl_Position = camera.viewProjectionMatrix * (gl_in[0].gl_Position + vec4(1.0, 0.0, 1.0, 0.0) * voxel.worldPosition[oCascade[0]].w);
		EmitVertex();
		EndPrimitive();
		
		// Bottom
		//oColor = vec4(0, 1, 0, 1);
		gl_Position = camera.viewProjectionMatrix * (gl_in[0].gl_Position + vec4(0.0, 0.0, 0.0, 0.0) * voxel.worldPosition[oCascade[0]].w);
		EmitVertex();
		gl_Position = camera.viewProjectionMatrix * (gl_in[0].gl_Position + vec4(1.0, 0.0, 0.0, 0.0) * voxel.worldPosition[oCascade[0]].w);
		EmitVertex();
		gl_Position = camera.viewProjectionMatrix * (gl_in[0].gl_Position + vec4(0.0, 0.0, 1.0, 0.0) * voxel.worldPosition[oCascade[0]].w);
		EmitVertex();
		EndPrimitive();
		
		gl_Position = camera.viewProjectionMatrix * (gl_in[0].gl_Position + vec4(1.0, 0.0, 0.0, 0.0) * voxel.worldPosition[oCascade[0]].w);
		EmitVertex();
		gl_Position = camera.viewProjectionMatrix * (gl_in[0].gl_Position + vec4(1.0, 0.0, 1.0, 0.0) * voxel.worldPosition[oCascade[0]].w);
		EmitVertex();
		gl_Position = camera.viewProjectionMatrix * (gl_in[0].gl_Position + vec4(0.0, 0.0, 1.0, 0.0) * voxel.worldPosition[oCascade[0]].w);
		EmitVertex();
		EndPrimitive();
		
		// Top
		gl_Position = camera.viewProjectionMatrix * (gl_in[0].gl_Position + vec4(1.0, 1.0, 0.0, 0.0) * voxel.worldPosition[oCascade[0]].w);
		EmitVertex();
		gl_Position = camera.viewProjectionMatrix * (gl_in[0].gl_Position + vec4(0.0, 1.0, 0.0, 0.0) * voxel.worldPosition[oCascade[0]].w);
		EmitVertex();
		gl_Position = camera.viewProjectionMatrix * (gl_in[0].gl_Position + vec4(0.0, 1.0, 1.0, 0.0) * voxel.worldPosition[oCascade[0]].w);
		EmitVertex();
		EndPrimitive();
		
		gl_Position = camera.viewProjectionMatrix * (gl_in[0].gl_Position + vec4(1.0, 1.0, 1.0, 0.0) * voxel.worldPosition[oCascade[0]].w);
		EmitVertex();
		gl_Position = camera.viewProjectionMatrix * (gl_in[0].gl_Position + vec4(1.0, 1.0, 0.0, 0.0) * voxel.worldPosition[oCascade[0]].w);
		EmitVertex();
		gl_Position = camera.viewProjectionMatrix * (gl_in[0].gl_Position + vec4(0.0, 1.0, 1.0, 0.0) * voxel.worldPosition[oCascade[0]].w);
		EmitVertex();
		EndPrimitive();
	}
}