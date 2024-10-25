#include <pch.h>
#include "Shader.h"
#include <glm/gtc/type_ptr.hpp> 
#include "Utils.h"
#include "DebugTools/DebugLogger/DebugLogger.h"

namespace Graphics {

	static std::unordered_set<std::string> missingUniforms;
	//creates a compute shader
	Shader::Shader(std::string const& shdrFile) : pgmHdl{} {
		CreateComputeShader(shdrFile);
	}
	/*  _________________________________________________________________________ */
	/*! Shader

	@param vertFile
	The path to the vertex shader file.

	@param fragFile
	The path to the fragment shader file.

	This constructor initializes the Shader object by creating a shader from the
	provided vertex and fragment shader files.

	*/
	Shader::Shader(std::string const& vertFile, std::string const& fragFile)
		:pgmHdl{}{
		CreateShaderFromFile(vertFile, fragFile);
	}

	Shader::Shader(std::string const& geomFile, std::string const& vertFile, std::string const& fragFile) :pgmHdl{} {
		CreateShaderFromFile(geomFile, vertFile, fragFile);
	}

	std::shared_ptr<Shader> Shader::Create(std::string const& vertFile, std::string const& fragFile) {
		return std::make_shared<Shader>(vertFile, fragFile);
	}

	/*  _________________________________________________________________________ */
	/*! ~Shader

	Destructor for the Shader class. It deletes the shader program from OpenGL.

	*/
	Shader::~Shader() {
		GLCALL(glDeleteProgram(pgmHdl));
	}

	void Shader::CreateComputeShader(std::string const& compute_file_path)
	{
		// On the C++ side, creating a compute shader works exactly like other shaders
		// Create shader, store reference
		GLuint ComputeShaderID = glCreateShader(GL_COMPUTE_SHADER);

		// Parse shader string
		std::string ComputeShaderCode;
		std::ifstream ComputeShaderStream(compute_file_path, std::ios::in);
		if (ComputeShaderStream.is_open())
		{
			std::stringstream sstr;
			sstr << ComputeShaderStream.rdbuf();
			ComputeShaderCode = sstr.str();
			ComputeShaderStream.close();
		}
		else
		{
			printf("Impossible to open %s. Are you in the right directory!\n", compute_file_path.c_str());
			int d = getchar();
			return;
		}

		// Init result variables to check return values
		GLint Result = GL_FALSE;
		int InfoLogLength;

		// Compile Compute Shader
		// Read shader as c_string
		char const* ComputeSourcePointer = ComputeShaderCode.c_str();
		// Read shader source into ComputeShaderID
		glShaderSource(ComputeShaderID, 1, &ComputeSourcePointer, NULL);
		// Compile shader
		glCompileShader(ComputeShaderID);

		// Check Compute Shader
		// These functions get the requested shader information
		GLCALL(glGetShaderiv(ComputeShaderID, GL_COMPILE_STATUS, &Result));
		GLCALL(glGetShaderiv(ComputeShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength));
		if (InfoLogLength > 0)
		{
			std::vector<char> ComputeShaderErrorMessage(InfoLogLength + 1);
			GLCALL(glGetShaderInfoLog(ComputeShaderID, InfoLogLength, NULL, &ComputeShaderErrorMessage[0]));
			printf("Compiling shader : %s\n", compute_file_path.c_str());
			printf("%s\n", &ComputeShaderErrorMessage[0]);
		}

		// Link the program
		GLuint ProgramID = glCreateProgram();
		GLCALL(glAttachShader(ProgramID, ComputeShaderID));
		GLCALL(glLinkProgram(ProgramID));

		// Check the program
		GLCALL(glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result));
		GLCALL(glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength));
		if (InfoLogLength > 0)
		{
			std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
			GLCALL(glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]));
			printf("Linking program\n");
			printf("%s\n", &ProgramErrorMessage[0]);
		}

		// Cleanup
		GLCALL(glDetachShader(ProgramID, ComputeShaderID));
		GLCALL(glDeleteShader(ComputeShaderID));

		pgmHdl = ProgramID;
	}

	/*  _________________________________________________________________________ */
	/*! CreateShaderFromString

	@param vertSrc
	The source code for the vertex shader as a string.

	@param fragSrc
	The source code for the fragment shader as a string.

	This function creates a shader program from the provided vertex and fragment
	shader source strings.

	*/
	void Shader::CreateGeomShaderFromString(std::string const& geomSrc, std::string const& vertSrc, std::string const& fragSrc, std::string const& geomName, std::string const& vertName, std::string const& fragName) {
		// Create an empty vertex shader handle
		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

		// Send the vertex shader source code to GL
		// Note that std::string's .c_str is NULL character terminated.
		const GLchar* source = vertSrc.c_str();
		GLCALL(glShaderSource(vertexShader, 1, &source, 0));

		// Compile the vertex shader
		GLCALL(glCompileShader(vertexShader));


		//Checks to see if compliation succeeded or failed
		GLint isCompiled = 0;
		GLCALL(glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isCompiled));
		if (isCompiled == GL_FALSE) {
			GLint maxLength = 0;
			GLCALL(glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength));
			std::vector<GLchar> infoLog(maxLength);
			GLCALL(glGetShaderInfoLog(vertexShader, maxLength, &maxLength, &infoLog[0]));
			GLCALL(glDeleteShader(vertexShader));

			Debug::DebugLogger::GetInstance().LogError("Vertex Shader: (" + vertSrc + ") Compilation Error:\n" + std::string(infoLog.data()));
			throw Debug::Exception<Shader>(Debug::LVL_ERROR, Msg("Vertex Shader: (" + vertSrc + ") Compilation Error:\n" + std::string(infoLog.data())));
			return;
		}

		// Create an empty fragment shader handle
		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

		// Send the fragment shader source code to GL
		// Note that std::string's .c_str is NULL character terminated.
		source = (const GLchar*)fragSrc.c_str();
		GLCALL(glShaderSource(fragmentShader, 1, &source, 0));

		// Compile the fragment shader
		GLCALL(glCompileShader(fragmentShader));

		GLCALL(glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isCompiled));
		if (isCompiled == GL_FALSE)
		{
			GLint maxLength = 0;
			GLCALL(glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength));

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			GLCALL(glGetShaderInfoLog(fragmentShader, maxLength, &maxLength, &infoLog[0]));

			// Failed to compile, delete both shaders to prevent leak
			GLCALL(glDeleteShader(fragmentShader));
			GLCALL(glDeleteShader(vertexShader));

			throw Debug::Exception<Shader>(Debug::LVL_ERROR, Msg("Fragment Shader: (" + fragSrc + ") Compilation Error:\n" + std::string(infoLog.data())));
			return;
		}

		// Create an empty geometry shader handle
		GLuint geometryShader = glCreateShader(GL_GEOMETRY_SHADER);

		// Send the geometry shader source code to GL
		// Note that std::string's .c_str is NULL character terminated.
		source = (const GLchar*)geomSrc.c_str();
		GLCALL(glShaderSource(geometryShader, 1, &source, 0));

		// Compile the geometry shader
		GLCALL(glCompileShader(geometryShader));

		GLCALL(glGetShaderiv(geometryShader, GL_COMPILE_STATUS, &isCompiled));
		if (isCompiled == GL_FALSE)
		{
			GLint maxLength = 0;
			GLCALL(glGetShaderiv(geometryShader, GL_INFO_LOG_LENGTH, &maxLength));

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			GLCALL(glGetShaderInfoLog(geometryShader, maxLength, &maxLength, &infoLog[0]));

			// Failed to compile, delete shaders to prevent leak
			GLCALL(glDeleteShader(geometryShader));
			GLCALL(glDeleteShader(fragmentShader));
			GLCALL(glDeleteShader(vertexShader));

			throw Debug::Exception<Shader>(Debug::LVL_ERROR, Msg("Geometry Shader: (" + geomSrc + ") Compilation Error : \n" + std::string(infoLog.data())));
			return;
		}

		// Vertex and fragment shaders are successfully compiled.
		// Now time to link them together into a program.
		// Get a program object.
		pgmHdl = glCreateProgram();

		throw Debug::Exception<Shader>(Debug::LVL_ERROR, Msg("Unable to create program handle"));

		// Attach our shaders to our program
		GLCALL(glAttachShader(pgmHdl, vertexShader));
		GLCALL(glAttachShader(pgmHdl, fragmentShader));
		GLCALL(glAttachShader(pgmHdl, geometryShader));

		// Link our program
		glLinkProgram(pgmHdl);

		// Check if program is linked successully
		GLint isLinked = 0;
		glGetProgramiv(pgmHdl, GL_LINK_STATUS, (int*)&isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			GLCALL(glGetProgramiv(pgmHdl, GL_INFO_LOG_LENGTH, &maxLength));

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			GLCALL(glGetProgramInfoLog(pgmHdl, maxLength, &maxLength, &infoLog[0]));

			// Link failed, delete program and both shaders
			GLCALL(glDeleteProgram(pgmHdl));
			GLCALL(glDeleteShader(geometryShader));
			GLCALL(glDeleteShader(fragmentShader));
			GLCALL(glDeleteShader(vertexShader));

			throw Debug::Exception<Shader>(Debug::LVL_ERROR, Msg("Shader Link Failure: " + std::string(infoLog.data()) + "Files: "));
			return;
		}

		// Always detach shaders after a successful link.
		GLCALL(glDetachShader(pgmHdl, vertexShader));
		GLCALL(glDetachShader(pgmHdl, fragmentShader));
		GLCALL(glDetachShader(pgmHdl, geometryShader));
		Debug::DebugLogger::GetInstance().LogInfo("Shader with files () () sucessfully compiled and linked.");
	}
	/*  _________________________________________________________________________ */
	/*! CreateShaderFromString

	@param vertSrc
	The source code for the vertex shader as a string.

	@param fragSrc
	The source code for the fragment shader as a string.

	This function creates a shader program from the provided vertex and fragment
	shader source strings.

	*/
	void Shader::CreateShaderFromString(std::string const& vertSrc, std::string const& fragSrc, std::string const& vertName, std::string const& fragName) {
		// Create an empty vertex shader handle
		GLuint vertexShader;
		GLCALL(vertexShader = glCreateShader(GL_VERTEX_SHADER));

		// Send the vertex shader source code to GL
		// Note that std::string's .c_str is NULL character terminated.
		const GLchar* source = vertSrc.c_str();
		glShaderSource(vertexShader, 1, &source, 0);

		// Compile the vertex shader
		glCompileShader(vertexShader);


		//Checks to see if compliation succeeded or failed
		GLint isCompiled = 0;
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(vertexShader, maxLength, &maxLength, &infoLog[0]);

			// Failed to compile, delete shader
			glDeleteShader(vertexShader);

			std::string str(infoLog.data());

			Debug::DebugLogger::GetInstance().LogError("Vertex Shader: " + vertName + " Compilation Error : \n" + std::string(infoLog.data()));
			//throw Debug::Exception<Shader>(Debug::LVL_ERROR, Msg("Vertex Shader: " + vertName + " Compilation Error : \n" + std::string(infoLog.data())));
			return;
		}

		// Create an empty fragment shader handle
		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

		// Send the fragment shader source code to GL
		// Note that std::string's .c_str is NULL character terminated.
		source = (const GLchar*)fragSrc.c_str();
		glShaderSource(fragmentShader, 1, &source, 0);

		// Compile the fragment shader
		glCompileShader(fragmentShader);

		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(fragmentShader, maxLength, &maxLength, &infoLog[0]);

			// Failed to compile, delete both shaders to prevent leak
			glDeleteShader(fragmentShader);
			glDeleteShader(vertexShader);

			std::string str(infoLog.data());

			Debug::DebugLogger::GetInstance().LogError("Fragment Shader: " + fragName + " Compilation Error:\n" + std::string(infoLog.data()));
			//throw Debug::Exception<Shader>(Debug::LVL_ERROR, Msg("Fragment Shader: " + fragName + " Compilation Error:\n" + std::string(infoLog.data())));
			return;
		}

		// Vertex and fragment shaders are successfully compiled.
		// Now time to link them together into a program.
		// Get a program object.
		pgmHdl = glCreateProgram();

		if (pgmHdl == 0) {
			throw Debug::Exception<Shader>(Debug::LVL_ERROR, Msg("Unable to create program handle"));
		}

		// Attach our shaders to our program
		glAttachShader(pgmHdl, vertexShader);
		glAttachShader(pgmHdl, fragmentShader);

		// Link our program
		glLinkProgram(pgmHdl);

		// Check if program is linked successully
		GLint isLinked = 0;
		glGetProgramiv(pgmHdl, GL_LINK_STATUS, (int*)&isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(pgmHdl, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(pgmHdl, maxLength, &maxLength, &infoLog[0]);

			// Link failed, delete program and both shaders
			glDeleteProgram(pgmHdl);
			glDeleteShader(vertexShader);
			glDeleteShader(fragmentShader);

			std::string str(infoLog.data());
			Debug::DebugLogger::GetInstance().LogError("Shader Link Failure: " + std::string(infoLog.data()) + "Files: " + vertName + ", " + fragName);
			//throw Debug::Exception<Shader>(Debug::LVL_ERROR, Msg("Shader Link Failure: " + std::string(infoLog.data()) + "Files: " + vertName + ", " + fragName));
			return;
		}

		// Always detach shaders after a successful link.
		glDetachShader(pgmHdl, vertexShader);
		glDetachShader(pgmHdl, fragmentShader);
		Debug::DebugLogger::GetInstance().LogInfo("Shader with files " + vertName + ", " + fragName + " sucessfully compiled and linked.");
	}

	/*  _________________________________________________________________________ */
	/*! CreateShaderFromFile

	@param vertFile
	The path to the vertex shader file.

	@param fragFile
	The path to the fragment shader file.

	This function reads the source code from the provided vertex and fragment shader
	files and creates a shader program.

	*/
	void Shader::CreateShaderFromFile(std::string const& vertFile, std::string const& fragFile) {
		std::ifstream inVertFile{ vertFile };

		if (!inVertFile) {
			Debug::DebugLogger::GetInstance().LogError("Unable to open Vertex File: (" + vertFile + "). Check the directory");
			//throw Debug::Exception<Shader>(Debug::LVL_ERROR, Msg("Unable to open Vertex File: (" + vertFile + "). Check the directory"));
		}
		std::stringstream vertSrc;
		vertSrc << inVertFile.rdbuf();
		inVertFile.close();

		std::ifstream inFragFile{ fragFile };

		if (!inFragFile) {
			Debug::DebugLogger::GetInstance().LogError("Unable to open Fragment File: (" + fragFile + "). Check the directory");
			//throw Debug::Exception<Shader>(Debug::LVL_ERROR, Msg("Unable to open Fragment File: (" + fragFile + "). Check the directory"));
		}

		std::stringstream fragSrc;
		fragSrc << inFragFile.rdbuf();
		inFragFile.close();

		CreateShaderFromString(vertSrc.str(), fragSrc.str(), vertFile, fragFile);
	}
	/*  _________________________________________________________________________ */
	/*! CreateShaderFromFile

	@param vertFile
	The path to the vertex shader file.

	@param fragFile
	The path to the fragment shader file.

	This function reads the source code from the provided vertex and fragment shader
	files and creates a shader program.

	*/
	void Shader::CreateShaderFromFile(std::string const& geomFile, std::string const& vertFile, std::string const& fragFile) {
		std::ifstream inVertFile{ vertFile };
		if (!inVertFile) {
			throw Debug::Exception<Shader>(Debug::LVL_ERROR, Msg("Unable to open Vertex File: (" + vertFile + "). Check the directory"));
		}
		std::stringstream vertSrc;
		vertSrc << inVertFile.rdbuf();
		inVertFile.close();

		std::ifstream inFragFile{ fragFile };
		if (!inFragFile) {
			throw Debug::Exception<Shader>(Debug::LVL_ERROR, Msg("Unable to open Fragment File: (" + fragFile + "). Check the directory"));
		}

		std::stringstream fragSrc;
		fragSrc << inFragFile.rdbuf();
		inFragFile.close();

		std::ifstream inGeomFile{ geomFile };
		if (!inGeomFile) {
			throw Debug::Exception<Shader>(Debug::LVL_ERROR, Msg("Unable to open Geom File: (" + geomFile + "). Check the directory"));
		}

		std::stringstream geomSrc;
		geomSrc << inGeomFile.rdbuf();
		inGeomFile.close();

		CreateGeomShaderFromString(geomSrc.str(), vertSrc.str(), fragSrc.str(), geomFile, vertFile, fragFile);
	}
	/*  _________________________________________________________________________ */
	/*! Use

	This function sets the current shader program to the one represented by this
	Shader object.

	*/
	void Shader::Use() const {
		GLCALL(glUseProgram(pgmHdl));
	}

	/*  _________________________________________________________________________ */
	/*! Unuse

	This function unsets the current shader program, effectively disabling any shaders.

	*/
	void Shader::Unuse() const {
		GLCALL(glUseProgram(0));
	}

	GLint Shader::GetUniformLocation(std::string const& name) {
		GLint loc = glGetUniformLocation(pgmHdl, name.c_str());
		if (loc < 0) {
			// If the uniform is not found, log only if it hasn't been logged before
			if (missingUniforms.find(name) == missingUniforms.end()) {
				Debug::DebugLogger::GetInstance().LogWarning("Uniform variable '" + name + "' does not exist.");
				missingUniforms.insert(name); // Mark as logged
			}
		}
		return loc;
	}

	/*  _________________________________________________________________________ */
	/*! SetUniform

	@param name
	The name of the uniform variable in the shader.

	@param val
	The boolean value to set the uniform variable to.

	This function sets the specified uniform variable in the shader program to the
	provided boolean value.

	*/
	void Shader::SetUniform(std::string const& name, GLboolean val) {
		GLint loc = GetUniformLocation(name);
		if (loc >= 0) {
			glUniform1i(loc, val);
		}
	}

	/*  _________________________________________________________________________ */
	/*! SetUniform

	@param name
	The name of the uniform variable in the shader.

	@param val
	The integer value to set the uniform variable to.

	This function sets the specified uniform variable in the shader program to the
	provided integer value.

	*/
	void Shader::SetUniform(std::string const& name, GLint val) {
		GLint loc = GetUniformLocation(name);
		if (loc >= 0) {
			glUniform1i(loc, val);
		}
	}


	/*  _________________________________________________________________________ */
	/*! SetUniform

	@param name
	The name of the uniform variable in the shader.

	@param val
	The float value to set the uniform variable to.

	This function sets the specified uniform variable in the shader program to the
	provided float value.

	*/
	void Shader::SetUniform(std::string const& name, GLfloat val) {
		GLint loc = GetUniformLocation(name);
		if (loc >= 0) {
			glUniform1f(loc, val);
		}
	}

	/*  _________________________________________________________________________ */
	/*! SetUniform

	@param name
	The name of the uniform variable in the shader.

	@param val
	The float value to set the uniform variable to.

	This function sets the specified uniform variable in the shader program to the
	provided double value.

	*/
	void Shader::SetUniform(std::string const& name, GLdouble val) {
		GLint loc = GetUniformLocation(name);
		if (loc >= 0) {
			glUniform1f(loc, static_cast<GLfloat>(val));
		}
	}


	/*  _________________________________________________________________________ */
	/*! SetUniform

	@param name
	The name of the uniform variable in the shader.

	@param x, y
	The x and y float values to set the uniform variable to.

	This function sets the specified uniform variable in the shader program to the
	provided 2D float values.

	*/
	void Shader::SetUniform(std::string const& name, GLfloat x, GLfloat y) {
		GLint loc = GetUniformLocation(name);
		if (loc >= 0) {
			glUniform2f(loc, x, y);
		}
	}

	/*  _________________________________________________________________________ */
	/*! SetUniform

	@param name
	The name of the uniform variable in the shader.

	@param x, y, z
	The x, y, and z float values to set the uniform variable to.

	This function sets the specified uniform variable in the shader program to the
	provided 3D float values.

	*/
	void Shader::SetUniform(std::string const& name, GLfloat x, GLfloat y, GLfloat z) {
		GLint loc = GetUniformLocation(name);
		if (loc >= 0) {
			glUniform3f(loc, x, y, z);
		}
	}

	/*  _________________________________________________________________________ */
	/*! SetUniform

	@param name
	The name of the uniform variable in the shader.

	@param x, y, z, w
	The x, y, z, and w float values to set the uniform variable to.

	This function sets the specified uniform variable in the shader program to the
	provided 4D float values.

	*/
	void Shader::SetUniform(std::string const& name, GLfloat x, GLfloat y, GLfloat z, GLfloat w) {
		GLint loc = GetUniformLocation(name);
		if (loc >= 0) {
			glUniform4f(loc, x, y, z, w);
		}
	}

	/*  _________________________________________________________________________ */
	/*! SetUniform

	@param name
	The name of the uniform variable in the shader.

	@param val
	The 2D vector value to set the uniform variable to.

	This function sets the specified uniform variable in the shader program to the
	provided 2D vector value.

	*/
	void Shader::SetUniform(std::string const& name, glm::vec2 const& val) {
		GLint loc = GetUniformLocation(name);
		if (loc >= 0) {
			glUniform2f(loc, val.x, val.y);
		}
	}

	/*  _________________________________________________________________________ */
	/*! SetUniform

	@param name
	The name of the uniform variable in the shader.

	@param val
	The 3D vector value to set the uniform variable to.

	This function sets the specified uniform variable in the shader program to the
	provided 3D vector value.

	*/
	void Shader::SetUniform(std::string const& name, glm::vec3 const& val) {
		GLint loc = GetUniformLocation(name);
		if (loc >= 0) {
			glUniform3f(loc, val.x, val.y, val.z);
		}
	}

	/*  _________________________________________________________________________ */
	/*! SetUniform

	@param name
	The name of the uniform variable in the shader.

	@param val
	The 4D vector value to set the uniform variable to.

	This function sets the specified uniform variable in the shader program to the
	provided 4D vector value.

	*/
	void Shader::SetUniform(std::string const& name, glm::vec4 const& val) {
		GLint loc = GetUniformLocation(name);
		if (loc >= 0) {
			glUniform4f(loc, val.x, val.y, val.z, val.w);
		}
	}

	/*  _________________________________________________________________________ */
	/*! SetUniform

	@param name
	The name of the uniform variable in the shader.

	@param val
	The 3x3 matrix value to set the uniform variable to.

	This function sets the specified uniform variable in the shader program to the
	provided 3x3 matrix value.

	*/
	void Shader::SetUniform(std::string const& name, glm::mat3 const& val) {
		GLint loc = GetUniformLocation(name);
		if (loc >= 0) {
			glUniformMatrix3fv(loc, 1, GL_FALSE, &val[0][0]);
		}
	}

	/*  _________________________________________________________________________ */
	/*! SetUniform

	@param name
	The name of the uniform variable in the shader.

	@param val
	The 4x4 matrix value to set the uniform variable to.

	This function sets the specified uniform variable in the shader program to the
	provided 4x4 matrix value.

	*/
	void Shader::SetUniform(std::string const& name, glm::mat4 const& val) {
		GLint loc = GetUniformLocation(name);
		if (loc >= 0) {
			glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(val));
		}
	}

	/*  _________________________________________________________________________ */
	/*! SetUniform

	@param name
	The name of the uniform variable in the shader.

	@param val
	The pointer to an array of integer values.

	@param count
	The number of integer values in the array.

	This function sets the specified uniform variable in the shader program to the
	provided array of integer values.

	*/
	void Shader::SetUniform(std::string const& name, int* val, unsigned int count) {
		GLint loc = GetUniformLocation(name);
		if (loc >= 0) {
			GLCALL(glUniform1iv(loc, count, val));
		}
	}

	void Shader::SetUniform(std::string const& name, GLuint64 bindlessHandle) {
		// Get the location of the uniform in the shader
		GLint loc = GetUniformLocation(name);
		if (loc != -1) {
			// Set the bindless texture handle as a uniform
			GLCALL(glUniformHandleui64ARB(loc, bindlessHandle));
		}
	}

	void Shader::SetUniform(std::string const& name, const GLuint64* bindlessHandles, unsigned int count) {
		// Get the location of the first element of the uniform array in the shader
		GLint loc = GetUniformLocation(name);
		if (loc != -1) {
			// Set the bindless texture handles as a uniform array
			GLCALL(glUniformHandleui64vARB(loc, count, bindlessHandles));
		}
	}

	void Shader::SetUniform(std::string const& name, Texture const& texture, unsigned int texUnit) {
		//if (texture->IsBindless()) {
		//	SetUniform(name, texture->GetBindlessHandle());
		//	return;
		//}
		// Activate the appropriate texture unit
		GLCALL(glActiveTexture(GL_TEXTURE0 + texUnit));

		// Bind the texture to that unit
		texture.Bind(texUnit);

		// Set the uniform in the shader to the correct texture unit
		GLint loc = GetUniformLocation(name);
		if (loc != -1) {
			GLCALL(glUniform1i(loc, texUnit));
		}
	}

	void Shader::SetUniform(std::string const& name, std::shared_ptr<Texture> texture, unsigned int texUnit) {
		//if (texture->IsBindless()) {
		//	SetUniform(name, texture->GetBindlessHandle());
		//	return;
		//}
		// Activate the appropriate texture unit
		GLCALL(glActiveTexture(GL_TEXTURE0 + texUnit));

		// Bind the texture to that unit
		texture->Bind(texUnit);

		// Set the uniform in the shader to the correct texture unit
		GLint loc = GetUniformLocation(name);
		if (loc != -1) {
			GLCALL(glUniform1i(loc, texUnit));
		}
	}
}