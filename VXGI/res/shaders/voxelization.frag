#version 450 core

#extension GL_ARB_bindless_texture : require

in vec2 gsTexcoord;
in flat int gsAxis;
in flat int gsInvocation;
in flat uint gsNormalMask;

writeonly restrict uniform image3D voxelDataAlbedo[5];
layout(r32ui) restrict uniform uimage3D voxelDataFaces;

uniform VoxelGrid
{
	mat4 projectionMatrix[3][5];
	vec4 worldPosition[5];	// w == scale of a voxel
	uint resolution;
	uint cascadeLevels;
} voxel;

uniform Material
{
	sampler2D albedoTexture, normalTexture, specularTexture, opacityTexture;
	vec3 ambient;
	uint textureMask;
	vec4 diffuse;
	vec3 specular;
	float specularExponent;
	vec4 emissive;
} material;

void main()
{
	vec4 color = vec4(material.ambient, 0.0) + mix(material.diffuse, texture(material.albedoTexture, gsTexcoord), (material.textureMask & 1));
	
	ivec3 fragmentPos = ivec3(gl_FragCoord.xy, (1 - gl_FragCoord.z) * voxel.resolution);
	
	if(gsAxis == 0)
		fragmentPos = fragmentPos.zxy;
	else if(gsAxis == 1)
		fragmentPos = fragmentPos.yzx;
	
	imageStore(voxelDataAlbedo[gsInvocation], fragmentPos, color);
	imageAtomicOr(voxelDataFaces, fragmentPos, gsNormalMask << (gsInvocation * 6));
}