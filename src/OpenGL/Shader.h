#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>


class Shader
{
public:
	Shader(const char* fragPath, const char* vertPath);
	void Bind() const;
	unsigned int GetUniformLocation(const char* u_name);

	void setF4(const char* u_name, float v1, float v2, float v3, float v4);
	void setF(const char* u_name, float v1);
	void setI(const char* u_name, float v1);
	void setVec3(const char* u_name, glm::vec3 val);
	void setMat4(const char* u_name, glm::mat4 val);

private:
	unsigned int shaderId;
};