#include "SEWindow.hpp"
#include <stdexcept>
#include <iostream>

namespace SE {
	
SEWindow::SEWindow(uint32_t width, uint32_t height, std::string name) : m_WindowWidth{ width }, m_WindowHeight{ height }, m_WindowName{ name } {
	init_window();
}

SEWindow::~SEWindow()
{
	glfwDestroyWindow(m_Window);
	glfwTerminate();
}

void SEWindow::framebuffer_resize_callback(GLFWwindow* window, int width, int height)
{
	auto seWindow = reinterpret_cast<SEWindow*>(glfwGetWindowUserPointer(window));
	seWindow->m_FramebufferResized = true;
	seWindow->m_WindowWidth = width;
	seWindow->m_WindowHeight = height;
}

void SEWindow::init_window()
{
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

	m_Window = glfwCreateWindow(m_WindowWidth, m_WindowHeight, m_WindowName.c_str(), nullptr, nullptr);
	glfwSetWindowUserPointer(m_Window, this);
	glfwSetFramebufferSizeCallback(m_Window, framebuffer_resize_callback);
}

void SEWindow::create_window_surface(VkInstance instance, VkSurfaceKHR* surface)
{
	if (glfwCreateWindowSurface(instance, m_Window, nullptr, surface) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create window surface.");
	}
}

} // end SE namespace