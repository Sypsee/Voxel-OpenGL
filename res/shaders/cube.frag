#version 450 core

out vec4 FragColor;

uniform sampler2D tex;
uniform vec3 viewPos;
uniform float ExpFogDensity = 1;
uniform float FogEnd = 2000;
uniform vec3 FogColor = vec3(0.68, 0.86, 0.9);
uniform float time;

in vec3 fragPos;
in vec3 normal;
in vec3 randomColor;
in float ambientLevel;

const vec3 lightPos = vec3(0, 1, -1);
const vec3 lightColor = vec3(1, 1, 1);
const vec3 specularReflectionColor = vec3(1, 1, 0.8);
const float lightIntensity = 1;
const float ambientIntensity = 0.5;
const float ao_values[4] = float[4](0.1, 0.25, 0.5, 1);

// Function to calculate the sky color based on time
vec3 CalculateSkyColor(float time) {
    vec3 dayColor = vec3(0.5, 0.7, 1.0); // Light blue for day
    vec3 duskColor = vec3(1.0, 0.5, 0.3); // Warm orange for dusk
    vec3 nightColor = vec3(0.1, 0.1, 0.2); // Dark blue for night
    
    // Simple time-based color calculation
    if (time < 0.5) {
        return mix(dayColor, duskColor, time * 2.0); // Transition from day to dusk
    } else {
        return mix(duskColor, nightColor, (time - 0.5) * 2.0); // Transition from dusk to night
    }
}

float CalcFogFactor()
{
    float camToPixelDst = length(fragPos - viewPos);
    float dstRatio = 4 * camToPixelDst / FogEnd;
    float FogFactor = exp(-dstRatio * ExpFogDensity);

    return FogFactor;
}

void main()
{
    float sunAngle = time * (3.14159) * 2; // Full circle (0 to 2π)
    const vec3 sunDirection = normalize(vec3(sin(sunAngle), cos(sunAngle), -1.0));

    const vec3 lightDir = normalize(-sunDirection - fragPos);
    const vec3 viewDir = normalize(viewPos - fragPos);

    // Calculate diffuse color based on normal and light direction
    const vec3 diffuseColor = max(dot(normalize(normal), normalize(lightDir)), ambientIntensity) * randomColor;

    // Calculate the sky color based on time
    const vec3 skyColor = CalculateSkyColor(time);

    // Adjust voxel color based on the calculated sky color
    const vec3 adjustedColor = mix(diffuseColor, skyColor, 0.2); // Blend with sky color

    vec3 outputColor = (lightIntensity * adjustedColor) * lightColor;

    const vec3 fogColor = mix(FogColor, skyColor, 0.65 * pow(max(0.0, dot(viewDir, lightDir)), 2.0) + (time * 0.3));
    const float FogFactor = CalcFogFactor();
    outputColor = mix(fogColor, outputColor, FogFactor);

    FragColor = vec4(outputColor * ao_values[int(ambientLevel)], 1.0);
}