#include "Pipeline.h"
#include <pch.h>

namespace Graphics {
	Pipeline::Pipeline(){}
	Pipeline::~Pipeline(){}

	void Pipeline::Init() {}

	void Pipeline::RenderFrame() {}

	void Pipeline::ShadowPass(){}
	void Pipeline::GeometryPass() {}
	void Pipeline::LightingPass() {}
	void Pipeline::PostProcessingPass() {}

	//void SetCamera()

	void Pipeline::RenderDebugLines() {}

	void Pipeline::CollectRenderCommands() {}
	void Pipeline::ExecuteRenderCommands() {}
}