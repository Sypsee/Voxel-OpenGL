#version 450 core

out vec4 FragColor;

uniform sampler2D tex;
uniform vec3 viewPos;
uniform float ExpFogDensity = 1;
uniform float FogEnd = 1500;
uniform vec3 FogColor = vec3(0.53, 0.81, 1);

in vec3 fragPos;
in vec3 normal;
in vec2 uv;

vec3 lightPos = vec3(0, 1, -1);
vec3 lightColor = vec3(1,1,1);
vec3 specularReflectionColor = vec3(1,1,1);
float lightIntensity = 1;
float ambientIntensity = .6;
float shininess = .02;
float alpha = 100;

vec3 baseColor = vec3(0.09, 0.8, 0.23);

float CalcFogFactor()
{
	float camToPixelDst = length(fragPos - viewPos);
	float dstRatio = 4 * camToPixelDst / FogEnd;
	float FogFactor = exp(-dstRatio * ExpFogDensity);

	return FogFactor;
}

void main()
{
	vec3 lightDir = normalize(lightPos - fragPos);
	vec3 viewDir = normalize(viewPos - fragPos);

	vec3 diffuseColor = max(dot(normalize(normal), normalize(lightDir)), ambientIntensity) * texture(tex, uv).xyz;
	vec3 reflectedVector = -reflect(normalize(lightDir), normalize(normal));
	vec3 specularReflection = max(pow(dot(reflectedVector, normalize(viewDir)), alpha), shininess) * specularReflectionColor;
	
	vec3 outputColor = (lightIntensity * (diffuseColor + specularReflection)) * lightColor;

	float FogFactor = CalcFogFactor();
	outputColor = mix(FogColor, outputColor, FogFactor);

	FragColor = vec4(outputColor, 1.0);
}