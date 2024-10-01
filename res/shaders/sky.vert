#version 450 core

layout(location = 0) in vec3 aPos;

uniform mat4 inverseView;

out vec3 viewDir;

void main()
{
	vec4 end = inverseView * vec4(aPos.xy, 1.0, 1.0);
	viewDir = normalize(end.xyz / end.w);

	gl_Position = vec4(aPos, 1.0);
}