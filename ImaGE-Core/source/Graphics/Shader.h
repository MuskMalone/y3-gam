/*!*********************************************************************
\file   Shader.h
\author k.choa\@digipen.edu
\date   22/09/2024
\brief  The Shader class encapsulates the process of creating, compiling, and linking OpenGL shaders. 
		It supports vertex, fragment, geometry, and compute shaders, and provides methods for setting 
		uniform variables of various types (e.g., integers, floats, vectors, matrices, textures).

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#pragma once
#include <string>
#include "Texture.h"

namespace Graphics {
	class Shader {
	public:
		Shader(std::string const& shdrFile);
		Shader(std::string const& vertFile, std::string const& fragFile);
		Shader(std::string const& geomFile, std::string const& vertFile, std::string const& fragFile);
		~Shader();

		static std::shared_ptr<Shader> Create(std::string const& vertFile, std::string const& fragFile);
		void CreateComputeShader(std::string const& shdrFile);
		void CreateShaderFromString(std::string const& vertSrc, std::string const& fragSrc, std::string const& vertName = "", std::string const& fragName = "");
		void CreateShaderFromFile(std::string const& vertFile, std::string const& fragFile);
		void CreateGeomShaderFromString(std::string const& geomSrc, std::string const& vertSrc, std::string const& fragSrc, std::string const& geomName ="", std::string const& vertName ="", std::string const& fragName = "");
		void CreateShaderFromFile(std::string const& geomFile, std::string const& vertFile, std::string const& fragFile);

		void Use() const;
		void Unuse() const;

		GLint GetUniformLocation(std::string const& name);

		void SetUniform(std::string const& name, GLboolean val);
		void SetUniform(std::string const& name, GLint val);
		void SetUniform(std::string const& name, GLfloat val);
		void SetUniform(std::string const& name, GLdouble val);
		void SetUniform(std::string const& name, GLfloat x, GLfloat y);
		void SetUniform(std::string const& name, GLfloat x, GLfloat y, GLfloat z);
		void SetUniform(std::string const& name, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
		void SetUniform(std::string const& name, glm::vec2 const& val);
		void SetUniform(std::string const& name, glm::vec3 const& val);
		void SetUniform(std::string const& name, glm::vec4 const& val);
		void SetUniform(std::string const& name, glm::mat3 const& val);
		void SetUniform(std::string const& name, glm::mat4 const& val);
		void SetUniform(std::string const& name, int const* val, unsigned int count);
		void SetUniform(std::string const& name, float const* val, unsigned int count);
		void SetUniform(std::string const& name, glm::vec3 const* val, unsigned int count);

		void SetUniform(std::string const& name, GLuint64 bindlessHandle);

		void SetUniform(std::string const& name, const GLuint64* bindlessHandles, unsigned int count);

		void SetUniform(std::string const& name, Texture const& texture/*, unsigned int texUnit*/); //TEXTURES
		void SetUniform(std::string const& name, std::shared_ptr<Texture> texture/*, unsigned int texUnit*/); //TEXTURES
		unsigned int PgmHdl() { return pgmHdl; };
	private:
		unsigned int pgmHdl;
	};

	class ShaderLibrary {
	public:
		inline static bool Add(const std::string& name, std::shared_ptr<Shader> shader) {
			if (mShaders.find(name) != mShaders.end()) {
				return false;  // Shader already exists
			}
			mShaders[name] = shader;  // Add shader if it doesn't exist
			return true;
		}

		inline static std::shared_ptr<Shader> Get(const std::string& name) {
			return mShaders.at(name);  // Handle exceptions as needed
		}
		static const std::string cShaderDirectory;  // Shader directory
	private:
		static std::unordered_map<std::string, std::shared_ptr<Shader>> mShaders;

	};
}	// namespace Graphics

