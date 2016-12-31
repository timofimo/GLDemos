#version 450 core

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexcoord;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec3 inTangent;
layout(location = 4) in uvec4 inBoneIDs;
layout(location = 5) in vec4 inBoneWeights;

out vec2 vTexcoord;
out vec3 vNormal;

uniform CameraBlock
{
	mat4 viewProjectionMatrix;
	mat4 invViewProjectionMatrix;
	vec3 position;
} camera;

uniform BoneMatrixBlock
{
	mat4 boneMatrix[256];
};

uniform mat4 modelMatrix;

void main()
{
	mat4 bMatrix = boneMatrix[inBoneIDs[0]] * inBoneWeights[0];
	bMatrix += boneMatrix[inBoneIDs[1]] * inBoneWeights[1];
	bMatrix += boneMatrix[inBoneIDs[2]] * inBoneWeights[2];
	bMatrix += boneMatrix[inBoneIDs[3]] * inBoneWeights[3];
	
	vec4 pos = bMatrix * vec4(inPosition, 1.0);
	gl_Position = (camera.viewProjectionMatrix * modelMatrix) * pos;
	vTexcoord = inTexcoord;
	vNormal = normalize(mat3(modelMatrix) * (mat3(bMatrix) * inNormal));
}