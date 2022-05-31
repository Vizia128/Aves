#include "Shader.hpp"

namespace Aves {

	void Shader::Init(const char* shaderPath, const char* shaderType)
	{
		std::string shaderCode;

        shaderCode = read_shader_file(shaderPath);
        const char* shaderCodeChar = shaderCode.c_str();

        if(shaderType == "VERTEX")
        {
            shaderID = glCreateShader(GL_VERTEX_SHADER);
        }
        else if (shaderType == "FRAGMENT")
        {
            shaderID = glCreateShader(GL_FRAGMENT_SHADER);
        }
        else
        {
            AVES_WARN("Invalad shader type!");
        }
        
        glShaderSource(shaderID, 1, &shaderCodeChar, NULL);
        glCompileShader(shaderID);
        checkCompileErrors(shaderID, shaderType);
	}

    void Shader::Init(Shader vertex, Shader fragment)
    {
        shaderID = glCreateProgram();
        glAttachShader(shaderID, vertex.getID());
        glAttachShader(shaderID, fragment.getID());
        glLinkProgram(shaderID);
        Shader::checkCompileErrors(shaderID, "PROGRAM");

        // delete the shaders as they're linked into our program now and no longer necessary
        vertex.~Shader();
        fragment.~Shader();
    }

	Shader::~Shader()
	{
        glDeleteShader(shaderID);
	}

    void Shader::checkCompileErrors(unsigned int shader, std::string type) 
    {
        int success;
        char infoLog[1024];

        if (type != "PROGRAM") 
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

            if (!success) 
            {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                AVES_ERROR("SHADER_COMPILATION_ERROR of type : ", type, infoLog);
                AVES_ERROR(type);
                AVES_ERROR(infoLog);
            }
        }
        else 
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);

            if (!success) 
            {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                AVES_ERROR("PROGRAM_LINKING_ERROR of type : ", type, infoLog);
                AVES_ERROR(type);
                AVES_ERROR(infoLog);
            }
        }
    }

    std::string Shader::read_shader_file(const char* shader_file)//std::filesystem::path::value_type* shader_file)
    {
        std::ifstream ifs;

        auto ex = ifs.exceptions();
        ex |= std::ios_base::badbit | std::ios_base::failbit;
        ifs.exceptions(ex);
        try
        {
            ifs.open(shader_file);
            ifs.ignore(std::numeric_limits<std::streamsize>::max());
            auto size = ifs.gcount();

            if (size > 0x10000) // 64KiB sanity check:
                return "";
        }
        catch (std::ifstream::failure& e)
        {
            AVES_ERROR(e.what());
            return "";
        }

        ifs.clear();
        ifs.seekg(0, std::ios_base::beg);

        return std::string{ std::istreambuf_iterator<char> {ifs}, {} };
    }

    GLuint Shader::CreateUniformBlock(const char* uniformBlockName, int size) {
        GLuint UniformBlockIndex = glGetUniformBlockIndex(shaderID, uniformBlockName);
        glUniformBlockBinding(shaderID, UniformBlockIndex, 0);

        GLuint uboStruct;
        glGenBuffers(1, &uboStruct);
        glBindBuffer(GL_UNIFORM_BUFFER, uboStruct);
        glBufferData(GL_UNIFORM_BUFFER, size, NULL, GL_STATIC_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
        // define the range of the buffer that links to a uniform binding point
        glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboStruct, 0, size);
        return uboStruct;
    }

};