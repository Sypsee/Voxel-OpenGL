#pragma once

#include "FrustumCulling.h"

struct Transform
{
	glm::vec3 position{ 0.f };
	glm::vec3 camPos{ 0.f };
	
	glm::mat4 proj{ 1.f };
	glm::mat4 view{ 1.f };
	glm::mat4 model{ 1.f };

	glm::vec3 getRight() const
	{
		return model[0];
	}
	glm::vec3 getUp() const
	{
		return model[1];
	}

	glm::vec3 getBackward() const
	{
		return model[2];
	}

	glm::vec3 getForward() const
	{
		return -model[2];
	}
};

struct AABB
{
	glm::vec3 center{ 0.f, 0.f, 0.f };
	glm::vec3 extents{ 0.f, 0.f, 0.f };

	AABB(const glm::vec3& min, const glm::vec3& max)
		:center{(max + min) * 0.5f}, extents(max.x - center.x, max.y - center.y, max.z - center.z)
	{}

	AABB(const glm::vec3& inCenter, float iI, float iJ, float iK)
		:center{ inCenter }, extents{ iI, iJ, iK }
	{}

	bool isOnFrustum(const Frustum& frustum, Transform transform) const
	{
		const glm::vec3 globalCenter{ transform.model * glm::vec4(center, 1.f) };

		const glm::vec3 right = transform.getRight() * extents.x;
		const glm::vec3 up = transform.getUp() * extents.y;
		const glm::vec3 forward = transform.getForward() * extents.z;

		const float newIi = std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, right)) +
			std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, up)) +
			std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, forward));

		const float newIj = std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, right)) +
			std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, up)) +
			std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, forward));

		const float newIk = std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, right)) +
			std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, up)) +
			std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, forward));

		const AABB globalAABB(globalCenter, newIi, newIj, newIk);

		return (globalAABB.isOnOrForwardPlane(frustum.leftFace) &&
			globalAABB.isOnOrForwardPlane(frustum.rightFace) &&
			globalAABB.isOnOrForwardPlane(frustum.topFace) &&
			globalAABB.isOnOrForwardPlane(frustum.bottomFace) &&
			globalAABB.isOnOrForwardPlane(frustum.nearFace) &&
			globalAABB.isOnOrForwardPlane(frustum.farFace));
	}

	bool isOnOrForwardPlane(const Plane& plane) const
	{
		const float r = extents.x * std::abs(plane.normal.x) +
			extents.y * std::abs(plane.normal.y) + extents.z * std::abs(plane.normal.z);

		return -r <= plane.getSignedDistanceToPlane(center);
	}
};