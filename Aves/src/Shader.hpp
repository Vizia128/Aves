#pragma once

#include <glad/glad.h>

//#include <string>
//#include <fstream>
//#include <sstream>
#include <filesystem>
#include <limits>
#include <fstream>
#include <iterator>

#include "Log.hpp"

namespace Aves {

	class Shader
	{
	public:
		Shader() : shaderID(0) {}
		void Init(const char *shaderPath, const char *shaderType);
		void Init(Shader vertex, Shader fragment);
		~Shader();

		unsigned int getID() { return shaderID; }

		void use() { glUseProgram(shaderID); }

		GLuint CreateUniformBlock(const char* uniformBlockName, int size);
		GLuint CreateShaderStorageBuffer(const char* bufferName, int size, int bindingNum);

	private:
		void checkCompileErrors(unsigned int shader, std::string type);
		static std::string read_shader_file(const char* shaderFile);//std::filesystem::path::value_type* shader_file);

	private:
		unsigned int shaderID;
	};
} //namespace Aves
