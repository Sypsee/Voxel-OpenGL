#version 450 core

layout(location = 0) in uint packedData;

out vec3 fragPos;
out vec3 normal;
out vec3 randomColor;
out float ambientLevel;

uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;
uniform vec3 viewPos;

const vec3 normals[6] = {
	vec3(0,  0, -1),
	vec3(0,  0,  1),
	vec3(-1,  0,  0),
	vec3(1,  0,  0),
	vec3(0, -1,  0),
	vec3(0,  1,  0)
};

const vec3 randomColors[5] = {
    vec3(1.0, 0.9, 0.5),  // Soft Yellow
    vec3(0.8, 0.5, 1.0),  // Light Lavender
    vec3(1.0, 0.7, 0.3),  // Warm Gold
    vec3(0.6, 0.3, 0.8),  // Soft Purple
    vec3(0.9, 0.6, 0.8)   // Pale Violet
};

void main()
{
    int x = int(packedData & 0x1F);
    int y = int((packedData >> 5) & 0xFF);
    int z = int((packedData >> 13) & 0x1F);
    uint colorIndex = (packedData >> 18) & 0x7;
    uint normalIndex = (packedData >> 21) & 0x7;
    uint unpackedAmbientLevel = (packedData >> 24) & 0x3;

	fragPos = vec3(model * vec4(float(x), float(y), float(z), 1.0));
	normal = mat3(transpose(inverse(model))) * normals[int(normalIndex)];
	randomColor = randomColors[int(colorIndex)];
	ambientLevel = float(unpackedAmbientLevel);
	
	gl_Position = proj * view * model * vec4(float(x), float(y), float(z), 1.0);
}