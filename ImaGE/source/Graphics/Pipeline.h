#pragma once
#include "RenderCommand.h"
namespace Graphics {
	class Pipeline {
	public:
		Pipeline();
		~Pipeline();

		void Init();

		void RenderFrame();

		void ShadowPass();
		void GeometryPass();
		void LightingPass();
		void PostProcessingPass();

		//void SetCamera()

		void RenderDebugLines();

	private:
		//camera
		std::vector<RenderCommand> mCommandList;
		//framebuffer
		void CollectRenderCommands();
		void ExecuteRenderCommands();
	};
}