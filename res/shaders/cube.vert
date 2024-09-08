#version 450 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUv;

out vec3 normal;
out vec2 uv;

uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;

void main()
{
	normal = aNormal;
	uv = aUv;

	gl_Position = proj * view * model * vec4(aPosition, 1.0);
}