#pragma once

#include <glm/glm.hpp>
#include "../Core/Camera.h"

struct Plane
{
	glm::vec3 normal = { 0.f, 1.f, 0.f };
	float distance = 0.f;	// Distance from origin

	Plane() = default;

	Plane(const glm::vec3& p1, const glm::vec3& norm)
		: normal(glm::normalize(norm)),
		distance(glm::dot(normal, p1))
	{}

	float getSignedDistanceToPlane(const glm::vec3& point) const
	{
		return glm::dot(normal, point) - distance;
	}
};

struct Frustum
{
	Plane topFace;
	Plane bottomFace;

	Plane leftFace;
	Plane rightFace;

	Plane nearFace;
	Plane farFace;
};

class FrustumCulling
{
public:
	FrustumCulling() = default;

	inline FrustumCulling(const Camera& cam)
	{
		Update(cam);
	}

	void Update(const Camera& cam)
	{
		const float halfVSide = cam.getFarClip() * tanf(cam.getFOV() * .5f);
		const float halfHSide = halfVSide * cam.getAspectRatio();
		const glm::vec3 frontMultFar = cam.getFarClip() * cam.getFrontDirection();

		frustum.nearFace = { cam.getPosition() + cam.getNearClip() * cam.getFrontDirection(), cam.getFrontDirection() };
		frustum.farFace = { cam.getPosition() + frontMultFar, -cam.getFrontDirection() };
		frustum.rightFace = { cam.getPosition(),
								glm::cross(frontMultFar - cam.getRightDirection() * halfHSide, cam.getUpDirection()) };
		frustum.leftFace = { cam.getPosition(),
								glm::cross(cam.getUpDirection(), frontMultFar + cam.getRightDirection() * halfHSide) };
		frustum.topFace = { cam.getPosition(),
								glm::cross(cam.getRightDirection(), frontMultFar - cam.getUpDirection() * halfVSide) };
		frustum.bottomFace = { cam.getPosition(),
								glm::cross(frontMultFar + cam.getUpDirection() * halfVSide, cam.getRightDirection()) };
	}

	Frustum frustum;
};