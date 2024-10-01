#pragma once

#include "../OpenGL/Shader.h"
#include "../OpenGL/Buffer.h"

#include "ObjectComponents.h"

class Sky
{
public:
	Sky();
	~Sky();
	
	void Draw();
	
	Transform transform;

private:
	const float vertices[18]{
		-1.0F, -1.0F, 0.0F,
		 1.0F, -1.0F, 0.0F,
		 1.0F,  1.0F, 0.0F,	
		 1.0F,  1.0F, 0.0F,	
		-1.0F,  1.0F, 0.0F,
		-1.0F, -1.0F, 0.0F,
	};

	Shader m_Shader;
	Buffer m_VBO{ {GL_ARRAY_BUFFER, NULL, sizeof(vertices), GL_STATIC_DRAW}};
	unsigned int m_VAO=0;
};