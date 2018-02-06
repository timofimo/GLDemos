#pragma once

namespace GLR
{
	class Framebuffer;
	class Texture2D;
	class Image3D;
	class Shader;
	class Mesh;

	struct DrawElementsIndirectCommand
	{
		unsigned count;
		unsigned primCount;
		unsigned firstIndex;
		unsigned baseVertex;
		unsigned baseInstance;
	};

	enum class EAttributeLayout
	{
		FLOAT, VEC2, VEC3, VEC4,
		INT, IVEC2, IVEC3, IVEC4,
		UINT, UVEC2, UVEC3, UVEC4,
		DOUBLE, DVEC2, DVEC3, DVEC4,
		MAT3, MAT4,
	};

	namespace Internal
	{
		struct RendererState;
	}

	void Initialize();

	void Clear(GLbitfield mask);
	void DrawIndexed(unsigned count, unsigned offset = 0);
	void DrawIndexedInstanced(unsigned instances, unsigned count, unsigned offset = 0);
	void DrawLinesIndexed(unsigned count, unsigned offset = 0);
	void DrawPointsInstanced(unsigned count, unsigned offset, unsigned instances);
	void DrawIndexedIndirect(unsigned bufferIndex);

	unsigned CreateDrawCommandBuffer(const std::vector<DrawElementsIndirectCommand>& drawCommands);
	void DestroyDrawCommandBuffer(unsigned index);

	void BindMesh(const Mesh& mesh);
	void BindShader(const Shader& shader);
	void BindTexture(const Texture2D& texture, unsigned unit);
	void BindTexture(const Image3D& texture, unsigned unit);
	void BindImage(const Image3D& image, unsigned unit);
	void BindTextures(const Texture2D* textures, int count, unsigned unit);
	void BindTextures(const Image3D* textures, int count, unsigned unit);
	void BindImages(const Image3D* images, int count, unsigned unit);
	void BindFramebuffer(const Framebuffer& framebuffer);
	
	void UnbindFramebuffer();
	void UnbindShader();
	void UnbindMesh();

	void SetClearColor(float r, float g, float b, float a);
	void SetColorMask(bool r, bool g, bool b, bool a);
	void SetViewport(int x, int y, int width, int height);
	void SetBlendState(bool enabled, GLuint sFactor, GLuint dFactor);
	void SetDepthState(bool depthTest, bool depthWrite, GLuint depthCompareFunction);
	void SetStencilState(bool stencilTest, bool stencilWrite, GLuint compareFunc, GLuint compareRef, GLuint compareMask, GLuint testFail, GLuint testDepthFail, GLuint testPass);
	void SetRasterizationState(bool faceCulling, GLuint cullFace, GLuint windingOrder);
	void SetMSAA(bool enabled);

	GLuint GetMaxTextureUnit();
}
