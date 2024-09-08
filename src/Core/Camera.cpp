#include "Camera.h"

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <iostream>

Camera::Camera(glm::vec3 camPos, float FOV, float nearClip, float farClip)
	:FOV(FOV), nearClip(nearClip), farClip(farClip), cameraPos(camPos)
{
	proj = glm::perspective(glm::radians(FOV), 800.f / 600.f, nearClip, farClip);
}

void Camera::update(GLFWwindow *window)
{
	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	process_inputs(window);

	cameraDirection.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraDirection.y = sin(glm::radians(pitch));
	cameraDirection.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

	cameraFront = glm::normalize(cameraDirection);

	view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
}

void Camera::process_inputs(GLFWwindow* window)
{
	float cameraSpeed = 5.f * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPos += cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraUp;
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		cameraPos += cameraSpeed * cameraUp;

	if (glfwGetMouseButton(window, 1) == GLFW_PRESS)
	{
		mouse_hidden = true;
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}
	else if (glfwGetMouseButton(window, 1) == GLFW_RELEASE)
	{
		mouse_hidden = false;
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
}

void Camera::mouse_callback(double xpos, double ypos)
{
	if (first_mouse)
	{
		lastX = xpos;
		lastY = ypos;
		first_mouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates range from bottom to top
	lastX = xpos;
	lastY = ypos;

	const float sensitivity = 2.f * deltaTime;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	if (!mouse_hidden) return;

	yaw += xoffset;
	pitch += yoffset;

	pitch = glm::clamp(pitch, -89.9f, 89.f);
}

void Camera::setAspectRatio(float aspect_ratio)
{
	proj = glm::perspective(glm::radians(FOV), aspect_ratio, nearClip, farClip);
}

glm::vec3 Camera::getPosition() const
{
	return cameraPos;
}

glm::mat4 Camera::getProjMatrix() const
{
	return proj;
}

glm::mat4 Camera::getViewMatrix() const
{
	return view;
}