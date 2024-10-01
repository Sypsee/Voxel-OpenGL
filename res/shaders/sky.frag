#version 450 core

out vec4 fragColor;

in vec3 viewDir;

// Uniform for time (0.0 to 1.0)
uniform float time;

void main()
{
    vec3 view = normalize(viewDir);
    
    // Calculate the sun direction based on time
    float sunAngle = time * (3.14159) * 2; // Full-circle (0 to 2π)
    vec3 sunDirection = normalize(vec3(sin(sunAngle), cos(sunAngle), -1.0));

    // Sky colors for different times of the day
    vec3 dayColor = vec3(0.5, 0.7, 1.0); // Light blue for day
    vec3 duskColor = vec3(1.0, 0.5, 0.3); // Bright warm orange for dusk
    vec3 nightColor = vec3(0.1, 0.1, 0.2); // Dark blue for night

    // Lerp between day, dusk, and night based on time
    vec3 skyColor;
    if (time < 0.5) {
        skyColor = mix(dayColor, duskColor, time * 2.0); // Transition from day to dusk
    }
    else {
        skyColor = mix(duskColor, nightColor, (time - 0.5) * 2.0); // Transition from dusk to night
    }

    // Apply a base blue color for the bottom of the sky
    vec3 baseColor = vec3(0.2, 0.3, 0.8); // Set a consistent blue color
    float verticalFactor = smoothstep(-0.5, 0.5, view.y);
    
    // Mix the sky color with the base blue color to ensure the bottom is not black
    skyColor = mix(baseColor, skyColor, verticalFactor);

    // Set the final sky color
    fragColor = vec4(skyColor, 1.0); // Alpha set to 1.0
    
    // Calculate sunlight intensity based on the sun direction
    float sunIntensity = pow(max(0.0, dot(view, sunDirection)), 1000.0); // Reduced power for gradual brightness
    fragColor.xyz += sunIntensity * vec3(1.0, 1.0, 0.0); // Sun color (yellow)
    fragColor.xyz += 0.25 * sunIntensity * vec3(1.0, 1.0, 0.0); // Sunlight effect
}