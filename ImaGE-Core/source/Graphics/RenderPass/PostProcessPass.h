#pragma once
#include "RenderPass.h"

namespace Graphics {
	class PostProcessingPass : public RenderPass {
	public:
		PostProcessingPass(const RenderPassSpec& spec);
		void Render(CameraSpec const& cam, std::vector<ECS::Entity> const& entities) override;
	private:
		std::shared_ptr<Graphics::Framebuffer> mPingPongBuffer;
	};
}