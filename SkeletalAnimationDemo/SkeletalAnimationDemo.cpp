#include <PCH.h>
#include "GLRenderer.h"
#include "Shader.h"
#include "Mesh.h"
#include <ExampleBase/ExampleBase.h>
#include "ResourceLoader.h"
#include "ExampleBase/Camera.h"
#include "Framebuffer.h"
#include "Light.h"
#include <AssimpCompilerStructures.h>
#include <glm/gtc/random.hpp>

#define DRAW_INDIRECT
#define ADVANCED_CULLING
#define ANIMATED_LIGHTS

// Quad
const float vertexData[] =
{
	-1.0f, 1.0f, 0.0f,
	-1.0f, -1.0f, 0.0f,
	1.0f, -1.0f, 0.0f,
	1.0f, 1.0f, 0.0f,
};

const unsigned indexData[] =
{
	0, 1, 2,
	0, 2, 3,
};

void LoadSkeletalMesh(const char* file);
void GetBoneMatrices(const std::string& animationName, float time, std::vector<glm::mat4>& boneMatrices);
std::vector<Animation> animatedMeshAnimations;
std::vector<Mesh> animatedMeshes;
std::vector<Skeleton> animatedMeshSkeletons;
GLuint animatedMeshVAO, animatedMeshVBO[10], animatedMeshIBO, animatedMeshIndexCount;
unsigned currentAnimation = 0;

class SkeletalAnimationDemo : public ExampleBase
{
public:
	SkeletalAnimationDemo(unsigned width, unsigned height, const char* title, bool borderless)
		: ExampleBase(width, height, title, borderless), m_camera(glm::radians(60.0f), float(width) / float(height), 0.1f, 2000.0f)
	{
		m_meshes.push_back(GLR::Mesh("Quad", vertexData, 12, indexData, 6, std::vector<GLenum>{ GL_FLOAT_VEC3}));
		GLR::ResourceLoader::LoadMeshes("D:/Programming/GLDemos/Resources/primitives.fbx", m_meshes, GLR::EBatchType::PerFile);
		GLR::ResourceLoader::LoadMeshes("D:/Programming/GLDemos/Resources/sponza/sponza.fbx", m_meshes, GLR::EBatchType::PerFile);

		GLR::Shader::AddGlobalUniformBlock("CameraBlock");
		m_shaders[0] = std::make_unique<GLR::Shader>("geometryShaderSkeleton", "D:/Programming/GLDemos/SkeletalAnimationDemo/res/shaders/geometryPassSkeleton.vert", "D:/Programming/GLDemos/SkeletalAnimationDemo/res/shaders/geometryPass.frag");
		m_shaders[1] = std::make_unique<GLR::Shader>("geometryShader", "D:/Programming/GLDemos/SkeletalAnimationDemo/res/shaders/geometryPass.vert", "D:/Programming/GLDemos/SkeletalAnimationDemo/res/shaders/geometryPass.frag");
		m_shaders[2] = std::make_unique<GLR::Shader>("lightShader", "D:/Programming/GLDemos/SkeletalAnimationDemo/res/shaders/lightPassAdvanced.vert", "D:/Programming/GLDemos/SkeletalAnimationDemo/res/shaders/lightPassAdvanced.frag");

		GLR::SetClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		GLR::SetViewport(0, 0, width, height);
		GLR::SetDepthState(true, true, GL_LESS);
		GLR::SetRasterizationState(true, GL_BACK, GL_CCW);

		m_camera.SetPosition(glm::vec3(0.0f, 1.0f, 0.0f));

		m_framebuffer = std::make_unique<GLR::Framebuffer>("TestFBO", width, height, 0, std::vector<GLR::ColorAttachmentDescription>{ {3, GL_UNSIGNED_BYTE}, { 3, GL_UNSIGNED_BYTE } }, GL_DEPTH_COMPONENT32F);

		m_pointLights.reserve(1024);
		for (unsigned i = 0; i < 1024; i++)
			m_pointLights.emplace_back(glm::linearRand(glm::vec3(-15.0f, 0.0f, -8.0f), glm::vec3(15.0f, 14.0f, 8.0f)), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 2.0f);

		std::vector<GLR::DrawElementsIndirectCommand> drawCommands;
		drawCommands.reserve(m_pointLights.size());
		for (unsigned i = 0; i < unsigned(m_pointLights.size()); i++)
			drawCommands.push_back({ m_meshes[2].GetIndexCount(), 1, m_meshes[2].GetIndexOffset(), 0, 0 });
		m_commandBuffers.push_back(GLR::CreateDrawCommandBuffer(drawCommands));

		drawCommands.clear();
		drawCommands.reserve(m_meshes.size());
		GLuint vao = 0;
		for (unsigned i = 0; i < unsigned(m_meshes.size()); i++)
		{
			if (vao != m_meshes[i].GetVertexArray())
			{
				if (!drawCommands.empty())
				{
					m_commandBuffers.push_back(GLR::CreateDrawCommandBuffer(drawCommands));
					drawCommands.clear();
				}
				GLR::BindMesh(m_meshes[i]);
				vao = m_meshes[i].GetVertexArray();
			}
			drawCommands.push_back({ m_meshes[i].GetIndexCount(), 1, m_meshes[i].GetIndexOffset(), 0, 0 });
			if (i == unsigned(m_meshes.size()) - 1)
				m_commandBuffers.push_back(GLR::CreateDrawCommandBuffer(drawCommands));
		}

		LoadSkeletalMesh("D:/Programming/GLDemos/SkeletalAnimationDemo/Golem.jkl");
	}
	~SkeletalAnimationDemo()
	{
		if(animatedMeshVAO != 0)
		{
			glDeleteBuffers(10, animatedMeshVBO);
			glDeleteBuffers(1, &animatedMeshIBO);
			glDeleteVertexArrays(1, &animatedMeshVAO);
			animatedMeshVAO = 0;
		}
	}

	virtual void Update(float deltaTime) override
	{
		m_camera.Update(deltaTime);
	}

	virtual void Render() override
	{
		// Geometry pass
		GLR::BindShader(*m_shaders[1]);
		GLR::BindFramebuffer(*m_framebuffer);

		struct CameraBlock
		{
			glm::mat4 viewProjectionMatrix;
			glm::mat4 invViewProjectionMatrix;
			glm::vec3 position;
			float padding;
		} cameraBlock;
		cameraBlock.viewProjectionMatrix = m_camera.GetViewProjectionMatrix();
		cameraBlock.invViewProjectionMatrix = glm::inverse(cameraBlock.viewProjectionMatrix);
		cameraBlock.position = m_camera.GetWorldPosition();
		m_shaders[1]->GetUniformBlock("CameraBlock")->UpdateContents(reinterpret_cast<char*>(&cameraBlock), sizeof(CameraBlock), 0);

		GLR::Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		const GLR::InputParameter* modelMatrix = m_shaders[1]->GetUniform("modelMatrix");
		const GLR::InputParameter* meshColor = m_shaders[1]->GetUniform("meshColor");
		meshColor->Set(glm::vec3(1.0f, 0.9f, 0.9f));

		for (unsigned j = 0; j < 1; j++)
		{
			modelMatrix->Set(glm::translate(glm::vec3(j * 64.0f, 0.0f, 0.0f)));
			GLR::BindMesh(m_meshes[3]);
			GLR::DrawIndexedIndirect(m_commandBuffers[3]);
		}

		GLR::BindShader(*m_shaders[0]);
		glBindVertexArray(animatedMeshVAO);
		std::vector<glm::mat4> boneMatrices;
		if(Input::GetKeyDown(GLFW_KEY_UP))
			currentAnimation = (currentAnimation + 1) % unsigned(animatedMeshAnimations.size());
		auto animIt = animatedMeshAnimations.begin() + currentAnimation;
		GetBoneMatrices(animIt->name, float(glfwGetTime()) * animIt->ticksPerSecond, boneMatrices);
		m_shaders[0]->GetUniformBlock("BoneMatrixBlock")->UpdateContents(reinterpret_cast<char*>(boneMatrices.data()), sizeof(glm::mat4) * unsigned(boneMatrices.size()), 0);
		m_shaders[0]->GetUniform("meshColor")->Set(glm::vec3(1.0f));
		m_shaders[0]->GetUniform("modelMatrix")->Set(glm::translate(glm::vec3(0.0f, 0.65f, 0.0f)) * glm::rotate(glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)) * glm::scale(glm::vec3(0.05f)));
		GLR::DrawIndexed(animatedMeshIndexCount);
		GLR::UnbindMesh();
		GLR::UnbindShader();

		// Light pass
		GLR::UnbindFramebuffer();
		GLR::SetBlendState(true, GL_ONE, GL_ONE);
		glBlendEquation(GL_FUNC_ADD);
		GLR::SetDepthState(false, true, GL_LESS);
		GLR::BindShader(*m_shaders[2]);

		m_shaders[2]->GetUniform("colorSampler")->Set(*GLR::Texture2D::GetItem("TestFBO_0"));
		m_shaders[2]->GetUniform("normalSampler")->Set(*GLR::Texture2D::GetItem("TestFBO_1"));
		m_shaders[2]->GetUniform("depthSampler")->Set(*GLR::Texture2D::GetItem("TestFBO_depth"));

		std::shared_ptr<char> pointLightBuffer;
		unsigned pointLightBufferSize;
		GLR::Light<GLR::PointLight>::GetBuffer(pointLightBuffer, pointLightBufferSize);
		unsigned plSize = sizeof(GLR::PointLight) + ((16 - (sizeof(GLR::PointLight) % 16)) % 16);
		for (unsigned i = 0; i < (pointLightBufferSize / plSize); i++)
		{
			GLR::PointLight* pl = reinterpret_cast<GLR::PointLight*>(&pointLightBuffer.get()[i * plSize]);
			pl->SetPosition(pl->GetPosition() + glm::vec3(glm::sin(glfwGetTime()) * glm::sign(int(i % 6) - 3), 0.0f, glm::cos(glfwGetTime()) * glm::sign(int(i % 12) - 4)) * 4.0f);
		}

		GLR::SetRasterizationState(true, GL_FRONT, GL_CCW);
		GLR::BindMesh(m_meshes[2]);

		unsigned maxBufferSize = (unsigned(sizeof(GLR::PointLight)) + ((16 - (unsigned(sizeof(GLR::PointLight)) % 16)) % 16)) * 1024;
		for (unsigned i = 0; i < unsigned(ceil(float(pointLightBufferSize) / float(maxBufferSize))); i++)
		{
			m_shaders[2]->GetUniformBlock("PointLightBlock")->UpdateContents(&pointLightBuffer.get()[i * maxBufferSize], glm::min(maxBufferSize, pointLightBufferSize - maxBufferSize * i), 0);
			GLR::DrawIndexedIndirect(m_commandBuffers[0]);
		}

		GLR::SetRasterizationState(true, GL_BACK, GL_CCW);
		GLR::SetDepthState(true, true, GL_LESS);
		GLR::SetBlendState(false, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		GLR::UnbindShader();
	}

private:
	std::vector<GLR::Mesh> m_meshes;
	std::unique_ptr<GLR::Shader> m_shaders[3];
	Camera m_camera;
	std::unique_ptr<GLR::Framebuffer> m_framebuffer;
	std::vector<unsigned> m_commandBuffers;
	std::vector<GLR::PointLight> m_pointLights;
};

int main()
{
	SkeletalAnimationDemo SkeletalAnimationDemo(640, 480, "SkeletalAnimationDemo", false);
	SkeletalAnimationDemo.StartGameLoop();

	return 0;
}

void LoadSkeletalMesh(const char* file)
{
	std::vector<Material> materials;
	std::vector<std::string> textures;
	Import(file, animatedMeshes, animatedMeshSkeletons, animatedMeshAnimations, materials, textures);

	if(animatedMeshes.size() > 0)
	{
		glGenVertexArrays(1, &animatedMeshVAO);
		glBindVertexArray(animatedMeshVAO);

		glGenBuffers(10, animatedMeshVBO);
		glBindBuffer(GL_ARRAY_BUFFER, animatedMeshVBO[0]);
		glBufferData(GL_ARRAY_BUFFER, animatedMeshes[0].vertexPositions.size() * sizeof(float), animatedMeshes[0].vertexPositions.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

		if((animatedMeshes[0].attributes & TextureCoordinates) > 0)
		{
			glBindBuffer(GL_ARRAY_BUFFER, animatedMeshVBO[1]);
			glBufferData(GL_ARRAY_BUFFER, animatedMeshes[0].textureCoordinates.size() * sizeof(float), animatedMeshes[0].textureCoordinates.data(), GL_STATIC_DRAW);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
		}

		if ((animatedMeshes[0].attributes & Normals) > 0)
		{
			glBindBuffer(GL_ARRAY_BUFFER, animatedMeshVBO[2]);
			glBufferData(GL_ARRAY_BUFFER, animatedMeshes[0].vertexNormals.size() * sizeof(float), animatedMeshes[0].vertexNormals.data(), GL_STATIC_DRAW);
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
		}

		if ((animatedMeshes[0].attributes & Tangents) > 0)
		{
			glBindBuffer(GL_ARRAY_BUFFER, animatedMeshVBO[3]);
			glBufferData(GL_ARRAY_BUFFER, animatedMeshes[0].vertexTangents.size() * sizeof(float), animatedMeshes[0].vertexTangents.data(), GL_STATIC_DRAW);
			glEnableVertexAttribArray(3);
			glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
		}

		if ((animatedMeshes[0].attributes & Bones) > 0)
		{
			glBindBuffer(GL_ARRAY_BUFFER, animatedMeshVBO[4]);
			std::unique_ptr<unsigned> boneIDs(new unsigned[animatedMeshes[0].vertexBoneWeights.size()]);
			for(unsigned i = 0; i < unsigned(animatedMeshes[0].vertexBoneWeights.size()); i++)
			{
				BoneWeight& weight = animatedMeshes[0].vertexBoneWeights[i];
				boneIDs.get()[i] = weight.boneIndex;
			}
			glBufferData(GL_ARRAY_BUFFER, animatedMeshes[0].vertexBoneWeights.size() * sizeof(unsigned), boneIDs.get(), GL_STATIC_DRAW);
			glEnableVertexAttribArray(4);
			glVertexAttribIPointer(4, 4, GL_UNSIGNED_INT, 0, nullptr);

			glBindBuffer(GL_ARRAY_BUFFER, animatedMeshVBO[5]);
			std::unique_ptr<float> boneWeights(new float[animatedMeshes[0].vertexBoneWeights.size()]);
			for (unsigned i = 0; i < unsigned(animatedMeshes[0].vertexBoneWeights.size()); i++)
			{
				BoneWeight& weight = animatedMeshes[0].vertexBoneWeights[i];
				boneWeights.get()[i] = weight.weight;
			}
			glBufferData(GL_ARRAY_BUFFER, animatedMeshes[0].vertexBoneWeights.size() * sizeof(float), boneWeights.get(), GL_STATIC_DRAW);
			glEnableVertexAttribArray(5);
			glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
		}

		if ((animatedMeshes[0].attributes & Indices) > 0)
		{
			glGenBuffers(1, &animatedMeshIBO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, animatedMeshIBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned) * animatedMeshes[0].indices.size(), animatedMeshes[0].indices.data(), GL_STATIC_DRAW);
			animatedMeshIndexCount = unsigned(animatedMeshes[0].indices.size());
		}

		GLR::UnbindMesh();
	}
}

void CalculateBoneMatrixRecursive(Animation& anim, Skeleton& skeleton, unsigned index, glm::mat4 parentMatrix, float time, std::vector<glm::mat4>& boneMatrices)
{
	Bone& bone = skeleton.bones[index];

	// Find the name of the channel
	std::string boneName;
	for(auto it = skeleton.boneNames.begin(); it != skeleton.boneNames.end(); ++it)
	{
		if(it->second == index)
		{
			boneName = it->first;
			break;
		}
	}

	// Find the channel
	unsigned channelIndex = 0;
	for(; channelIndex < unsigned(anim.channels.size()); channelIndex++)
	{
		if(anim.channels[channelIndex].boneName == boneName)
			break;
	}
	AnimationChannel& channel = anim.channels[channelIndex];

	// Find the right frame
	glm::vec3 position;
	glm::quat rotation;
	glm::vec3 scale;

	position = channel.positionKeys[0].vector;
	for(unsigned i = 0; i < unsigned(channel.positionKeys.size()) - 1; i++)
	{
		if(time < channel.positionKeys[i + 1].time)
		{
			float beginTime = channel.positionKeys[i].time;
			float endTime = channel.positionKeys[i + 1].time;
			float delta = (time - beginTime) / (endTime - beginTime);
			position = channel.positionKeys[i].vector * (1.0f - delta) + channel.positionKeys[i + 1].vector * delta;
			break;
		}
	}

	rotation = channel.rotationKeys[0].quat;
	for(unsigned i = 0; i < unsigned(channel.rotationKeys.size()) - 1; i++)
	{
		if(time < channel.rotationKeys[i + 1].time)
		{
			float beginTime = channel.rotationKeys[i].time;
			float endTime = channel.rotationKeys[i + 1].time;
			float delta = (time - beginTime) / (endTime - beginTime);
			rotation = glm::slerp(channel.rotationKeys[i].quat, channel.rotationKeys[i + 1].quat, delta);
			break;
		}
	}

	scale = channel.scaleKeys[0].vector;
	for(unsigned i = 0; i < unsigned(channel.scaleKeys.size()) - 1; i++)
	{
		if(time < channel.scaleKeys[i + 1].time)
		{
			float beginTime = channel.scaleKeys[i].time;
			float endTime = channel.scaleKeys[i + 1].time;
			float delta = (time - beginTime) / (endTime - beginTime);
			scale = channel.scaleKeys[i].vector * (1.0f - delta) + channel.scaleKeys[i + 1].vector * delta;
			break;
		}
	}

	glm::mat4 matrix = parentMatrix * (glm::translate(position) * glm::mat4(rotation) * glm::scale(scale));
	boneMatrices[index] = matrix * bone.transform;

	for (unsigned i = 0; i < unsigned(bone.children.size()); i++)
		CalculateBoneMatrixRecursive(anim, skeleton, bone.children[i], matrix, time, boneMatrices);

}

void GetBoneMatrices(const std::string& animationName, float time, std::vector<glm::mat4>& boneMatrices)
{
	// Find the right animation
	unsigned animationIndex = 0;
	for(; animationIndex < unsigned(animatedMeshAnimations.size()); animationIndex++)
	{
		if (animatedMeshAnimations[animationIndex].name == animationName)
			break;
	}

	Animation& anim = animatedMeshAnimations[animationIndex];
	time = fmodf(time, anim.duration);
	Skeleton& skel = animatedMeshSkeletons[0];

	boneMatrices.resize(skel.bones.size());

	// Find the root and calculate the bone matrices
	for(unsigned i = 0; i < unsigned(skel.bones.size()); i++)
	{
		if(skel.bones[i].parent == UINT_MAX)
		{
			CalculateBoneMatrixRecursive(anim, skel, i, glm::mat4(), time, boneMatrices);
			break;
		}
	}
}