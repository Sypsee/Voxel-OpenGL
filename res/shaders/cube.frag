#version 450 core

layout(location = 0) out vec4 outColor;

in vec3 normal;
in vec2 uv;

void main()
{
	outColor = vec4(1.0, 0.0, 0.0, 1.0);
}