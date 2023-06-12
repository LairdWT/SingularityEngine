#include "SEWindow.hpp"
#include <stdexcept>

namespace SE {
	
SEWindow::SEWindow(uint32_t width, uint32_t height, std::string name) : m_WINDOWWIDTH{ width }, m_WINDOWHEIGHT{ height }, m_WindowName{ name } {
	init_window();
}

SEWindow::~SEWindow()
{
	glfwDestroyWindow(m_Window);
	glfwTerminate();
}

void SEWindow::init_window()
{
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	m_Window = glfwCreateWindow(m_WINDOWWIDTH, m_WINDOWHEIGHT, m_WindowName.c_str(), nullptr, nullptr);

}

void SEWindow::create_window_surface(VkInstance instance, VkSurfaceKHR* surface)
{
	if (glfwCreateWindowSurface(instance, m_Window, nullptr, surface) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create window surface.");
	}
}

} // end SE namespace