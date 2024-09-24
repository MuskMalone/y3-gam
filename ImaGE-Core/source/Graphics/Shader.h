#pragma once

namespace Graphics {
	class Shader {
	public:
		Shader(std::string const& shdrFile);
		Shader(std::string const& vertFile, std::string const& fragFile);
		Shader(std::string const& geomFile, std::string const& vertFile, std::string const& fragFile);
		~Shader();

		void CreateComputeShader(std::string const& shdrFile);
		void CreateShaderFromString(std::string const& vertSrc, std::string const& fragSrc);
		void CreateShaderFromFile(std::string const& vertFile, std::string const& fragFile);
		void CreateShaderFromString(std::string const& geomSrc, std::string const& vertSrc, std::string const& fragSrc);
		void CreateShaderFromFile(std::string const& geomFile, std::string const& vertFile, std::string const& fragFile);

		void Use() const;
		void Unuse() const;

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
		void SetUniform(std::string const& name, int* val, unsigned int count);

		unsigned int PgmHdl() { return pgmHdl; };
	private:
		unsigned int pgmHdl;
	};
}