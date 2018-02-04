#version 450 core

out flat ivec3 oVoxel;
out flat int oCascade;

uniform VoxelGrid
{
	mat4 projectionMatrix[3][5];
	vec4 worldPosition[5];	// w == scale of a voxel
	uint resolution;
	uint cascadeLevels;
} voxel;

void main()
{
	oVoxel.x = int(gl_InstanceID % voxel.resolution);
	oVoxel.y = int((gl_InstanceID / voxel.resolution) % voxel.resolution);
	oVoxel.z = int((gl_InstanceID / voxel.resolution / voxel.resolution) % voxel.resolution);
	oCascade = int((gl_InstanceID / voxel.resolution / voxel.resolution / voxel.resolution) % voxel.cascadeLevels);
	
	gl_Position = vec4(voxel.worldPosition[oCascade].xyz + oVoxel * voxel.worldPosition[oCascade].w, 1.0);
}