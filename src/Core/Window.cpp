#include "Window.h"

#include <iostream>

Window::Window(const char* title, const int width, const int height)
	:m_WindowRes{width, height}
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	m_Window = glfwCreateWindow(width, height, title, NULL, NULL);
	if (m_Window == NULL)
	{
		std::cerr << "Error while creating glfwWindow!" << std::endl;
		glfwTerminate();
	}

	glfwMakeContextCurrent(m_Window);
}

Window::~Window()
{
	glfwDestroyWindow(m_Window);
	glfwTerminate();
}
