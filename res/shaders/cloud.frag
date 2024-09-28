#version 450 core

out vec4 FragColor;

struct Ray { vec3 origin; vec3 dir; };

vec2 BBoxIntersect(const vec3 boxMin, const vec3 boxMax, const Ray r) {
	vec3 t0 = (boxMin - r.origin) / r.dir;
	vec3 t1 = (boxMax - r.origin) / r.dir;
	vec3 tmin = min(t0, t1);
	vec3 tmax = max(t0, t1);
	
	float dstA = max(max(tmin.x, tmin.y), tmin.z);
	float dstB = min(tmax.x, min(tmax.y, tmax.z));
	
	float dstToBox = max(0.0, dstA);
	float dstInsideBox = max(0.0, dstB - dstToBox);
	return vec2(dstToBox, dstInsideBox);
}

uniform vec3 viewPos;

in vec3 FragPos;

vec3 base_color = vec3(1,1,1);
vec3 light_pos = vec3(1, 1, -1);

vec3 boxMin = vec3(-3.0, -1.0, -3.0);
vec3 boxMax = vec3(5.0, 3.0, 5.0);
const float ABSORPTION = 0.1;

void main()
{
	Ray ray;
	ray.origin = viewPos;
	ray.dir = normalize(light_pos - viewPos);

	vec2 t = BBoxIntersect(boxMin, boxMax, ray);
	float distance = length(t.y - t.x);

	float transmittance = exp(-distance * ABSORPTION);
	if (transmittance < 0.01) return;

	vec4 finalColor = vec4(base_color * transmittance, transmittance);
	if (finalColor.a > 0.1) discard;

	FragColor = finalColor;
}