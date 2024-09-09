#include "Texture/ImaGETexture.h"

#include "H2M/Core/AssertH2M.h"
#include "H2M/Core/RefH2M.h"
#include "H2M/Renderer/RendererAPI_H2M.h"

#include "Platform/OpenGL/OpenGLImaGETexture.h"
//#include "Platform/Vulkan/VulkanImaGETexture.h"
//#include "Platform/DX11/DX11ImaGETexture.h"


H2M::RefH2M<ImaGETexture> ImaGETexture::Create(const char* fileLoc, bool flipVert, bool isSampler, int filter)
{
	switch (H2M::RendererAPI_H2M::Current())
	{
		case H2M::RendererAPITypeH2M::None:   return H2M::RefH2M<ImaGETexture>();
		case H2M::RendererAPITypeH2M::OpenGL: return H2M::RefH2M<OpenGLImaGETexture>::Create(fileLoc, flipVert, isSampler, filter);
		//case H2M::RendererAPITypeH2M::Vulkan: return H2M::RefH2M<VulkanImaGETexture>::Create(fileLoc, flipVert, isSampler, filter);
		//case H2M::RendererAPITypeH2M::DX11:   return H2M::RefH2M<DX11ImaGETexture>::Create(fileLoc, flipVert, isSampler, filter);
	}

	Log::GetLogger()->error("Unknown RendererAPI");
	H2M_CORE_ASSERT(false, "Unknown RendererAPI");
	return H2M::RefH2M<ImaGETexture>();
}

H2M::RefH2M<ImaGETexture> ImaGETexture::Create(const char* fileLoc, uint32_t width, uint32_t height, bool isSampler, int filter)
{
	switch (H2M::RendererAPI_H2M::Current())
	{
		case H2M::RendererAPITypeH2M::None:   return H2M::RefH2M<ImaGETexture>();
		case H2M::RendererAPITypeH2M::OpenGL: return H2M::RefH2M<OpenGLImaGETexture>::Create(fileLoc, width, height, isSampler, filter);
		//case H2M::RendererAPITypeH2M::Vulkan: return H2M::RefH2M<VulkanImaGETexture>::Create(fileLoc, width, height, isSampler, filter);
		//case H2M::RendererAPITypeH2M::DX11:   return H2M::RefH2M<DX11ImaGETexture>::Create(fileLoc, width, height, isSampler, filter);
	}

	Log::GetLogger()->error("Unknown RendererAPI");
	H2M_CORE_ASSERT(false, "Unknown RendererAPI");
	return H2M::RefH2M<ImaGETexture>();
}

H2M::RefH2M<ImaGETexture> ImaGETexture::Create(const char* fileLoc, Specification spec)
{
	switch (H2M::RendererAPI_H2M::Current())
	{
		case H2M::RendererAPITypeH2M::None:   return H2M::RefH2M<ImaGETexture>();
		case H2M::RendererAPITypeH2M::OpenGL: return H2M::RefH2M<OpenGLImaGETexture>::Create(fileLoc, spec);
		//case H2M::RendererAPITypeH2M::Vulkan: return H2M::RefH2M<VulkanImaGETexture>::Create(fileLoc, spec);
		//case H2M::RendererAPITypeH2M::DX11:   return H2M::RefH2M<DX11ImaGETexture>::Create(fileLoc, spec);
	}

	Log::GetLogger()->error("Unknown RendererAPI");
	H2M_CORE_ASSERT(false, "Unknown RendererAPI");
	return H2M::RefH2M<ImaGETexture>();
}

void ImaGETexture::Unbind()
{
	Log::GetLogger()->error("Method ImaGETexture::Unbind() should not be called directly!");
}

void ImaGETexture::Clear()
{
	Log::GetLogger()->error("Method ImaGETexture::Clear() should not be called directly!");
}

uint32_t ImaGETexture::CalculateMipMapCount(uint32_t width, uint32_t height)
{
	Log::GetLogger()->error("Method ImaGETexture::CalculateMipMapCount() should not be called directly!");
	return uint32_t();
}

uint32_t ImaGETexture::GetMipLevelCount()
{
	Log::GetLogger()->error("Method ImaGETexture::GetMipLevelCount() should not be called directly!");
	return uint32_t();
}

std::pair<uint32_t, uint32_t> ImaGETexture::GetMipSize(uint32_t mip) const
{
	Log::GetLogger()->error("Method ImaGETexture::GetMipSize() should not be called directly!");
	return std::pair<uint32_t, uint32_t>();
}

int ImaGETexture::GetRed(int x, int z)
{
	Log::GetLogger()->error("Method ImaGETexture::GetRed() should not be called directly!");
	return 0;
}

int ImaGETexture::GetGreen(int x, int z)
{
	Log::GetLogger()->error("Method ImaGETexture::GetGreen() should not be called directly!");
	return 0;
}

int ImaGETexture::GetBlue(int x, int z)
{
	Log::GetLogger()->error("Method ImaGETexture::GetBlue() should not be called directly!");
	return 0;
}

int ImaGETexture::GetAlpha(int x, int z)
{
	Log::GetLogger()->error("Method ImaGETexture::GetAlpha() should not be called directly!");
	return 0;
}

float ImaGETexture::GetFileSize(const char* filename)
{
	Log::GetLogger()->error("Method ImaGETexture::GetFileSize() should not be called directly!");
	return 0.0f;
}

void ImaGETexture::SetPixel(int x, int z, glm::ivec4 pixel)
{
	Log::GetLogger()->error("Method ImaGETexture::SetPixel() should not be called directly!");
}

void ImaGETexture::SetRed(int x, int z, int value)
{
	Log::GetLogger()->error("Method ImaGETexture::SetRed() should not be called directly!");
}

void ImaGETexture::SetGreen(int x, int z, int value)
{
	Log::GetLogger()->error("Method ImaGETexture::SetGreen() should not be called directly!");
}

void ImaGETexture::SetBlue(int x, int z, int value)
{
	Log::GetLogger()->error("Method ImaGETexture::SetBlue() should not be called directly!");
}

void ImaGETexture::SetAlpha(int x, int z, int value)
{
	Log::GetLogger()->error("Method ImaGETexture::SetAlpha() should not be called directly!");
}
