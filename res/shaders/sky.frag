#version 450 core

out vec4 FragColor;

uniform vec3 viewPos;

in vec3 FragPos;

void main()
{
	FragColor = vec4(1, 0, 0, 1.0);
}