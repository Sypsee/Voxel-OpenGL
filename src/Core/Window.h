#include <GLFW/glfw3.h>
#include <glm/glm.hpp>


class Window
{
public:
	Window(const char* title, const int width, const int height);
	~Window();

	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;

	bool shouldClose() { return glfwWindowShouldClose(m_Window); }
	glm::vec2 getWindowRes() const { return m_WindowRes; }
	GLFWwindow* getGLFWwindow() const { return m_Window; }

private:
	GLFWwindow* m_Window;
	glm::vec2 m_WindowRes;
};