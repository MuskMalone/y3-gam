#pragma once

#include "H2M/Core/LayerH2M.h"

#include "Core/Window.h"
#include "Scene/Scene.h"


/***
 * Slightly modified version of H2M::Layer to be used in ImaGEEngine environment
 */
class ImaGELayer : public H2M::LayerH2M
{
public:
	ImaGELayer(const std::string& name = "ImaGELayer");
	virtual ~ImaGELayer();

	virtual void OnAttach() = 0;
	virtual void OnDetach() = 0;
	virtual void OnUpdate(H2M::TimestepH2M ts) = 0;
	virtual void OnEvent(H2M::EventH2M& event) = 0;
	virtual void OnRender() override {}; // Hazel version
	virtual void OnRender(Window* mainWindow, Scene* scene) = 0; // ImaGEEngine version
	virtual void OnImGuiRender() override {}; // Hazel version
	virtual void OnImGuiRender(Window* mainWindow, Scene* scene) = 0; // ImaGEEngine version

};
