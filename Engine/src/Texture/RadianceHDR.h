#pragma once

#include "Platform/OpenGL/OpenGLImaGETexture.h"

#include <GL/glew.h>


class RadianceHDR : public OpenGLImaGETexture
{
public:
	RadianceHDR();
	RadianceHDR(const char* fileLoc);
	bool Load();
	~RadianceHDR();

private:
	float* m_Buffer;

};
