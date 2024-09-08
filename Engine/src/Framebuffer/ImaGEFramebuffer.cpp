#include "Framebuffer/ImaGEFramebuffer.h"

#include "Platform/OpenGL/OpenGLImaGEFramebuffer.h"
//#include "Platform/Vulkan/VulkanImaGEFramebuffer.h"
//#include "Platform/DX11/DX11ImaGEFramebuffer.h"


H2M::RefH2M<ImaGEFramebuffer> ImaGEFramebuffer::Create(uint32_t width, uint32_t height)
{
	switch (H2M::RendererAPI_H2M::Current())
	{
		case H2M::RendererAPITypeH2M::None: return H2M::RefH2M<ImaGEFramebuffer>();
		case H2M::RendererAPITypeH2M::OpenGL: return H2M::RefH2M<OpenGLImaGEFramebuffer>::Create(width, height);
		//case H2M::RendererAPITypeH2M::Vulkan: return H2M::RefH2M<VulkanImaGEFramebuffer>::Create(width, height);
		//case H2M::RendererAPITypeH2M::DX11: return H2M::RefH2M<DX11ImaGEFramebuffer>::Create(width, height);
	}

	Log::GetLogger()->error("Unknown RendererAPI");
	H2M_CORE_ASSERT(false, "Unknown RendererAPI");
	return H2M::RefH2M<ImaGEFramebuffer>();
}

H2M::RefH2M<ImaGEFramebuffer> ImaGEFramebuffer::Create(FramebufferSpecification spec)
{
	switch (H2M::RendererAPI_H2M::Current())
	{
		case H2M::RendererAPITypeH2M::None: return H2M::RefH2M<ImaGEFramebuffer>();
		case H2M::RendererAPITypeH2M::OpenGL: return H2M::RefH2M<OpenGLImaGEFramebuffer>::Create(spec);
		//case H2M::RendererAPITypeH2M::Vulkan: return H2M::RefH2M<VulkanImaGEFramebuffer>::Create(spec);
		//case H2M::RendererAPITypeH2M::DX11: return H2M::RefH2M<DX11ImaGEFramebuffer>::Create(spec);
	}

	Log::GetLogger()->error("Unknown RendererAPI");
	H2M_CORE_ASSERT(false, "Unknown RendererAPI");
	return H2M::RefH2M<ImaGEFramebuffer>();
}
