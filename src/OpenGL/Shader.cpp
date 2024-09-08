#include "Shader.h"
#include <vector>


void CheckCompileError(unsigned int shader, const char* shaderType)
{
	GLint isCompiled = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);

	if (isCompiled == GL_FALSE)
	{
		int maxLength = 512;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<char> errorLog(maxLength);
		glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);

        std::cout << shaderType << " COMPILATION ERROR : " << std::endl;

        for (char log : errorLog)
        {
            std::cout << log;
        }

        std::cout << std::endl;

		// Exit with failure.
		glDeleteShader(shader); // Don't leak the shader.
	}
}


Shader::Shader(const char* fragPath, const char* vertPath)
{
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    // ensure ifstream objects can throw exceptions:
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        // open files
        vShaderFile.open(vertPath);
        fShaderFile.open(fragPath);
        std::stringstream vShaderStream, fShaderStream;
        // read file's buffer contents into streams
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        // close file handlers
        vShaderFile.close();
        fShaderFile.close();
        // convert stream into string
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch (std::ifstream::failure e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
    }
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();


    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vShaderCode, NULL);
    glCompileShader(vertexShader);

    CheckCompileError(vertexShader, "Vertex Shader");

    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fShaderCode, NULL);
    glCompileShader(fragmentShader);

    CheckCompileError(fragmentShader, "Fragment Shader");

    shaderId = glCreateProgram();
    glAttachShader(shaderId, vertexShader);
    glAttachShader(shaderId, fragmentShader);
    glLinkProgram(shaderId);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void Shader::Bind() const
{
    glLinkProgram(shaderId);
    glUseProgram(shaderId);
}

void Shader::setF4(const char* u_name, float v1, float v2, float v3, float v4)
{
	glUseProgram(shaderId);
	glUniform4f(GetUniformLocation(u_name), v1, v2, v3, v4);
}

void Shader::setF(const char* u_name, float v1)
{
    glUseProgram(shaderId);
    glUniform1f(GetUniformLocation(u_name), v1);
}

void Shader::setI(const char* u_name, float v1)
{
    glUseProgram(shaderId);
    glUniform1i(GetUniformLocation(u_name), v1);
}

void Shader::setVec3(const char* u_name, glm::vec3 val)
{
    glUseProgram(shaderId);
    glUniform3fv(GetUniformLocation(u_name), 1, glm::value_ptr(val));
}

void Shader::setMat4(const char* u_name, glm::mat4 val)
{
    glUseProgram(shaderId);
    glUniformMatrix4fv(GetUniformLocation(u_name), 1, GL_FALSE, glm::value_ptr(val));
}


unsigned int Shader::GetUniformLocation(const char* u_name)
{
    unsigned int loc = glGetUniformLocation(shaderId, u_name);
    return loc;
}