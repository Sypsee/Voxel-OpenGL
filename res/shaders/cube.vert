#version 450 core

layout(location = 0) in ivec3 aPos;
layout(location = 1) in int aUVIndex;
layout(location = 2) in int aNormalIndex;

out vec3 fragPos;
out vec3 normal;
out vec2 uv;

uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;
uniform vec3 viewPos;

vec3 normals[6] = {
	vec3(0,  0, -1),
	vec3(0,  0,  1),
	vec3(-1,  0,  0),
	vec3(1,  0,  0),
	vec3(0, -1,  0),
	vec3(0,  1,  0)
};

vec2 uvs[4] = {
	vec2(0, 0),
	vec2(1, 0),
	vec2(0, 1),
	vec2(1, 1),
};

void main()
{
	fragPos = vec3(model * vec4(aPos, 1.0));
	normal = mat3(transpose(inverse(model))) * normals[aNormalIndex];
	uv = uvs[aUVIndex];

	gl_Position = proj * view * model * vec4(aPos, 1.0);
}