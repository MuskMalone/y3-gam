#include <pch.h>
#include "Shader.h"
#include <glm/gtc/type_ptr.hpp> 
#include "Utils.h"

//TEMP??

namespace Graphics {
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
		:pgmHdl{} {
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
		glDeleteProgram(pgmHdl);
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
		glGetShaderiv(ComputeShaderID, GL_COMPILE_STATUS, &Result);
		glGetShaderiv(ComputeShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if (InfoLogLength > 0)
		{
			std::vector<char> ComputeShaderErrorMessage(InfoLogLength + 1);
			glGetShaderInfoLog(ComputeShaderID, InfoLogLength, NULL, &ComputeShaderErrorMessage[0]);
			printf("Compiling shader : %s\n", compute_file_path.c_str());
			printf("%s\n", &ComputeShaderErrorMessage[0]);
		}

		// Link the program
		GLuint ProgramID = glCreateProgram();
		glAttachShader(ProgramID, ComputeShaderID);
		glLinkProgram(ProgramID);

		// Check the program
		glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
		glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if (InfoLogLength > 0)
		{
			std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
			glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
			printf("Linking program\n");
			printf("%s\n", &ProgramErrorMessage[0]);
		}

		// Cleanup
		glDetachShader(ProgramID, ComputeShaderID);
		glDeleteShader(ComputeShaderID);

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
	void Shader::CreateShaderFromString(std::string const& geomSrc, std::string const& vertSrc, std::string const& fragSrc) {
		// Create an empty vertex shader handle
		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

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
			std::cout << "Vertex Shader Compliation Error!!!" << std::endl;
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
			std::cout << "Fragment Shader Compliation Error!!!" << std::endl;
			return;
		}

		// Create an empty geometry shader handle
		GLuint geometryShader = glCreateShader(GL_GEOMETRY_SHADER);

		// Send the geometry shader source code to GL
		// Note that std::string's .c_str is NULL character terminated.
		source = (const GLchar*)geomSrc.c_str();
		glShaderSource(geometryShader, 1, &source, 0);

		// Compile the geometry shader
		glCompileShader(geometryShader);

		glGetShaderiv(geometryShader, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(geometryShader, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(geometryShader, maxLength, &maxLength, &infoLog[0]);

			// Failed to compile, delete shaders to prevent leak
			glDeleteShader(geometryShader);
			glDeleteShader(fragmentShader);
			glDeleteShader(vertexShader);

			std::string str(infoLog.data());
			std::cout << "Geometry Shader Compliation Error!!!" << std::endl;
			return;
		}

		// Vertex and fragment shaders are successfully compiled.
		// Now time to link them together into a program.
		// Get a program object.
		pgmHdl = glCreateProgram();

		std::cout << "Unable to create program handle!!!" << std::endl;

		// Attach our shaders to our program
		glAttachShader(pgmHdl, vertexShader);
		glAttachShader(pgmHdl, fragmentShader);
		glAttachShader(pgmHdl, geometryShader);

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
			glDeleteShader(geometryShader);
			glDeleteShader(fragmentShader);
			glDeleteShader(vertexShader);

			std::string str(infoLog.data());
			std::cout << "Shader link failure!!!" << std::endl;
			return;
		}

		// Always detach shaders after a successful link.
		glDetachShader(pgmHdl, vertexShader);
		glDetachShader(pgmHdl, fragmentShader);
		glDetachShader(pgmHdl, geometryShader);
		std::cout << "Successfully Compiled and Linked!!!" << std::endl;
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
	void Shader::CreateShaderFromString(std::string const& vertSrc, std::string const& fragSrc) {
		// Create an empty vertex shader handle
		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

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
			std::cout << "Vertex Compliation error!!!" << std::endl;
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

			std::cout << "Fragment shader compliation error!!!" << std::endl;
			return;
		}

		// Vertex and fragment shaders are successfully compiled.
		// Now time to link them together into a program.
		// Get a program object.
		pgmHdl = glCreateProgram();

		if (pgmHdl == 0) {
			std::cout << "Unable to create program handle!!!!" << std::endl;
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
			std::cerr << "Shader Linking Error: " << &infoLog[0] << std::endl;
			return;
		}

		// Always detach shaders after a successful link.
		glDetachShader(pgmHdl, vertexShader);
		glDetachShader(pgmHdl, fragmentShader);
		std::cout << "Successfully compiled and linked!!!" << std::endl;
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
			std::cout << "Cannot open vertex file " << vertFile << std::endl;
		}
		std::stringstream vertSrc;
		vertSrc << inVertFile.rdbuf();
		inVertFile.close();

		std::ifstream inFragFile{ fragFile };

		if (!inFragFile) {
			std::cout << "Cannot open frag file " << fragFile << std::endl;
		}

		std::stringstream fragSrc;
		fragSrc << inFragFile.rdbuf();
		inFragFile.close();

		CreateShaderFromString(vertSrc.str(), fragSrc.str());
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
			std::cout << "Cannot open vertex file: " << vertFile << std::endl;
		}
		std::stringstream vertSrc;
		vertSrc << inVertFile.rdbuf();
		inVertFile.close();

		std::ifstream inFragFile{ fragFile };
		if (!inFragFile) {
			std::cout << "Cannot open frag file " << fragFile << std::endl;
		}

		std::stringstream fragSrc;
		fragSrc << inFragFile.rdbuf();
		inFragFile.close();

		std::ifstream inGeomFile{ geomFile };
		if (!inGeomFile) {
			std::cout << "Cannot open geom file " << geomFile << std::endl;
		}

		std::stringstream geomSrc;
		geomSrc << inGeomFile.rdbuf();
		inGeomFile.close();

		CreateShaderFromString(geomSrc.str(), vertSrc.str(), fragSrc.str());
	}
	/*  _________________________________________________________________________ */
	/*! Use

	This function sets the current shader program to the one represented by this
	Shader object.

	*/
	void Shader::Use() const {
		glUseProgram(pgmHdl);
	}

	/*  _________________________________________________________________________ */
	/*! Unuse

	This function unsets the current shader program, effectively disabling any shaders.

	*/
	void Shader::Unuse() const {
		glUseProgram(0);
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
		GLint loc{ glGetUniformLocation(pgmHdl, name.c_str()) };
		if (loc >= 0) {
			glUniform1i(loc, val);
		}
		else {
			std::cout << "Uniform variable " << name << " doesn't exist" << std::endl;
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
		GLint loc{ glGetUniformLocation(pgmHdl, name.c_str()) };
		if (loc >= 0) {
			glUniform1i(loc, val);
		}
		else {
			std::cout << "Uniform variable " << name << " doesn't exist" << std::endl;
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
		GLint loc{ glGetUniformLocation(pgmHdl, name.c_str()) };
		if (loc >= 0) {
			glUniform1f(loc, val);
		}
		else {
			std::cout << "Uniform variable " << name << " doesn't exist" << std::endl;
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
		GLint loc{ glGetUniformLocation(pgmHdl, name.c_str()) };
		if (loc >= 0) {
			glUniform1f(loc, static_cast<GLfloat>(val));
		}
		else {
			std::cout << "Uniform variable " << name << " doesn't exist" << std::endl;
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
		GLint loc{ glGetUniformLocation(pgmHdl, name.c_str()) };
		if (loc >= 0) {
			glUniform2f(loc, x, y);
		}
		else {
			std::cout << "Uniform variable " << name << " doesn't exist" << std::endl;
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
		GLint loc{ glGetUniformLocation(pgmHdl, name.c_str()) };
		if (loc >= 0) {
			glUniform3f(loc, x, y, z);
		}
		else {
			std::cout << "Uniform variable " << name << " doesn't exist" << std::endl;
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
		GLint loc{ glGetUniformLocation(pgmHdl, name.c_str()) };
		if (loc >= 0) {
			glUniform4f(loc, x, y, z, w);
		}
		else {
			std::cout << "Uniform variable " << name << " doesn't exist" << std::endl;
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
		GLint loc{ glGetUniformLocation(pgmHdl, name.c_str()) };
		if (loc >= 0) {
			glUniform2f(loc, val.x, val.y);
		}
		else {
			std::cout << "Uniform variable " << name << " doesn't exist" << std::endl;
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
		GLint loc{ glGetUniformLocation(pgmHdl, name.c_str()) };
		if (loc >= 0) {
			glUniform3f(loc, val.x, val.y, val.z);
		}
		else {
			std::cout << "Uniform variable " << name << " doesn't exist" << std::endl;
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
		GLint loc{ glGetUniformLocation(pgmHdl, name.c_str()) };
		if (loc >= 0) {
			glUniform4f(loc, val.x, val.y, val.z, val.w);
		}
		else {
			std::cout << "Uniform variable " << name << " doesn't exist" << std::endl;
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
		GLint loc{ glGetUniformLocation(pgmHdl, name.c_str()) };
		if (loc >= 0) {
			glUniformMatrix3fv(loc, 1, GL_FALSE, &val[0][0]);
		}
		else {
			std::cout << "Uniform variable " << name << " doesn't exist" << std::endl;
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
		GLint loc{ glGetUniformLocation(pgmHdl , name.c_str()) };
		if (loc >= 0) {
			glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(val));
		}
		else {
			std::cout << "Uniform variable " << name << " doesn't exist" << std::endl;
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
		GLint loc{ glGetUniformLocation(pgmHdl , name.c_str()) };
		if (loc >= 0) {
			glUniform1iv(loc, count, val);
		}
		else {
			std::cout << "Uniform variable " << name << " doesn't exist" << std::endl;
		}
	}

	void Shader::SetUniform(std::string const& name, std::shared_ptr<Texture> texture, unsigned int texUnit) {
		// Activate the appropriate texture unit
		GLCALL(glActiveTexture(GL_TEXTURE0 + texUnit));

		// Bind the texture to that unit
		texture->Bind(texUnit);

		// Set the uniform in the shader to the correct texture unit
		GLint location = glGetUniformLocation(pgmHdl, name.c_str());
		if (location != -1) {
			GLCALL(glUniform1i(location, texUnit));
		}
		else {
			// Log or handle the error if the uniform isn't found
			std::cerr << "Warning: Uniform '" << name << "' not found in shader!" << std::endl;
		}
	}
}