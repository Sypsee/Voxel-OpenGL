#version 450 core

out vec4 FragColor;

uniform sampler2D texture;

// in vec3 normal;
in vec2 uv;

void main()
{
	FragColor = vec4(uv, 0.5, 1.0);
}