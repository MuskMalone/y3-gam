#include <pch.h>
#include "ParticlePass.h"

namespace Graphics {
	ParticlePass::ParticlePass(const RenderPassSpec& spec) : RenderPass{ spec }
	{
	}
	void ParticlePass::Render(CameraSpec const& cam, std::vector<ECS::Entity> const& entities)
	{
	}
}