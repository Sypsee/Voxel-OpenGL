#include "Application.h"

#include <glad/glad.h>

Camera cam{glm::vec3(0, CHUNK_SIZE + 1, 0)};	// Global for a reason

void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
	cam.mouse_callback(xpos, ypos);
}

void framebuffer_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	cam.setAspectRatio((float)width/height);
}

void message_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, GLchar const* message, void const* user_param)
{
	auto const src_str = [source]() {
		switch (source)
		{
		case GL_DEBUG_SOURCE_API: return "API";
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM: return "WINDOW SYSTEM";
		case GL_DEBUG_SOURCE_SHADER_COMPILER: return "SHADER COMPILER";
		case GL_DEBUG_SOURCE_THIRD_PARTY: return "THIRD PARTY";
		case GL_DEBUG_SOURCE_APPLICATION: return "APPLICATION";
		case GL_DEBUG_SOURCE_OTHER: return "OTHER";
		}
		}();

	auto const type_str = [type]() {
		switch (type)
		{
		case GL_DEBUG_TYPE_ERROR: return "ERROR";
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "DEPRECATED_BEHAVIOR";
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: return "UNDEFINED_BEHAVIOR";
		case GL_DEBUG_TYPE_PORTABILITY: return "PORTABILITY";
		case GL_DEBUG_TYPE_PERFORMANCE: return "PERFORMANCE";
		case GL_DEBUG_TYPE_MARKER: return "MARKER";
		case GL_DEBUG_TYPE_OTHER: return "OTHER";
		}
		}();

	auto const severity_str = [severity]() {
		switch (severity) {
		case GL_DEBUG_SEVERITY_NOTIFICATION: return "NOTIFICATION";
		case GL_DEBUG_SEVERITY_LOW: return "LOW";
		case GL_DEBUG_SEVERITY_MEDIUM: return "MEDIUM";
		case GL_DEBUG_SEVERITY_HIGH: return "HIGH";
		}
		}();
	std::cout << src_str << ", " << type_str << ", " << severity_str << ", " << id << ": " << message << '\n';
}

Application::Application()
{
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
	}

	glViewport(0, 0, START_WIDTH, START_HEIGHT);

	glfwSetCursorPosCallback(m_Window.getGLFWwindow(), mouse_callback);
	glfwSetFramebufferSizeCallback(m_Window.getGLFWwindow(), framebuffer_callback);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_BLEND);
	/*glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);*/
	/*glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(message_callback, nullptr);*/

	world = new World();
	cam.setPosition({ 0, 50, 0 });
	//cloud = new Cloud();
}

Application::~Application()
{
	delete world;
	delete cloud;
}

void Application::run()
{
	float currentFrame = 0;
	float lastFrame = 0;
	float dt = 0;

	while (!m_Window.shouldClose())
	{
		glfwPollEvents();

		glClearColor(0.53f, 0.81f, 1.f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (glfwGetKey(m_Window.getGLFWwindow(), GLFW_KEY_O) == GLFW_PRESS)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else if (glfwGetKey(m_Window.getGLFWwindow(), GLFW_KEY_P) == GLFW_PRESS)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}

		currentFrame = glfwGetTime();
		dt = currentFrame - lastFrame;
		lastFrame = currentFrame;

		//system("cls");
		//std::cout << dt*1000 << "ms\n";

		cam.update(m_Window.getGLFWwindow());
		world->Draw(cam);

		/*cloud->transform.proj = cam.getProjMatrix();
		cloud->transform.view = cam.getViewMatrix();
		cloud->Draw(cam.getPosition());*/

		glfwSwapBuffers(m_Window.getGLFWwindow());
	}
}