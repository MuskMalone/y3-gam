#pragma once

#include "Texture/ImaGETexture.h"


/**
 * Based on Procedural Landmass Generation series by Sebastian Lague
 */
class TextureGenerator
{
public:
	static H2M::RefH2M<ImaGETexture> TextureFromColorMap(glm::vec4* colorMap, const char* fileLocation, int width, int height);
	static H2M::RefH2M<ImaGETexture> TextureFromHeightMap(float** noiseMap, const char* fileLocation, int width, int height);

};
