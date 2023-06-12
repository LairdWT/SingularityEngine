#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <cstdlib>
#include <string>

namespace SE {

class SEWindow {

public:
	SEWindow(uint32_t width, uint32_t height, std::string name);
	~SEWindow();

	SEWindow(const SEWindow&) = delete;
	SEWindow& operator=(const SEWindow&) = delete;

	bool should_close() { return glfwWindowShouldClose(m_Window); };
	void create_window_surface(VkInstance instance, VkSurfaceKHR* surface);
	VkExtent2D get_window_extent() { return { static_cast<uint32_t>(m_WINDOWWIDTH), static_cast<uint32_t>(m_WINDOWHEIGHT) }; };
private:

	void init_window();

	const uint32_t m_WINDOWHEIGHT;
	const uint32_t m_WINDOWWIDTH;

	std::string m_WindowName;
	GLFWwindow* m_Window = nullptr;
};

} // end SE namespace