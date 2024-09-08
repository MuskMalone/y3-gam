#pragma once

#include "Texture/ImaGETexture.h"

#include <map>
#include <string>


class TextureLoader
{
public:
	TextureLoader();
	static TextureLoader* Get();
	H2M::RefH2M<ImaGETexture> GetTexture(const char* fileLoc, bool flipVert, bool force);
	H2M::RefH2M<ImaGETexture> GetTexture(const char* fileLoc, bool flipVert, GLenum filter, bool force);
	void Print();
	void Clean();
	~TextureLoader();

private:
	static TextureLoader* s_Instance;
	std::map<std::string, H2M::RefH2M<ImaGETexture>> m_Textures;

};
