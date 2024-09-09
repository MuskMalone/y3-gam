#include "Texture/TextureSampler.h"


TextureSampler::TextureSampler() : OpenGLImaGETexture()
{
	m_Spec.IsSampler = true;
}

TextureSampler::TextureSampler(const char* fileLoc) : OpenGLImaGETexture(fileLoc, false, true)
{
}

TextureSampler::~TextureSampler()
{
}
