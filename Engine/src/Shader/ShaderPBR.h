#pragma once

#include "Platform/OpenGL/OpenGLImaGEShader.h"


class ShaderPBR : public OpenGLImaGEShader
{
public:
	ShaderPBR();
	ShaderPBR(const char* vertexLocation, const char* fragmentLocation, bool forceCompile = false);
	~ShaderPBR();

	void SetLightPositions(glm::vec3* lightPositions, unsigned int lightCount);
	void SetLightColors(glm::vec3* lightColors, unsigned int lightCount);

private:
	virtual void GetUniformLocations() override;

private:
	static const unsigned int lightCount = 4;

	// lights
	GLint uniformLightPositions[lightCount];
	GLint uniformLightColors[lightCount];

};
