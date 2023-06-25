#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <cstdlib>
#include <string>
#include <atomic>

namespace SE {

class SEWindow {

public:
#pragma region Lifecycle
	SEWindow(uint32_t width, uint32_t height, std::string name);
	~SEWindow();
	SEWindow(const SEWindow&) = delete;
	SEWindow& operator=(const SEWindow&) = delete;
#pragma endregion Lifecycle

	bool should_close() { return glfwWindowShouldClose(m_Window); };
	void create_window_surface(VkInstance instance, VkSurfaceKHR* surface);
	VkExtent2D get_window_extent() { return { static_cast<uint32_t>(m_WindowWidth), static_cast<uint32_t>(m_WindowHeight) }; };
	bool was_window_resized() { return m_FramebufferResized; };
	void reset_window_resized_flag() { m_FramebufferResized = false; };

private:

	static void framebuffer_resize_callback(GLFWwindow* window, int width, int height);

	void init_window();

	uint32_t m_WindowHeight;
	uint32_t m_WindowWidth;
	bool m_FramebufferResized = false;
	std::string m_WindowName;
	GLFWwindow* m_Window = nullptr;
};

} // end SE namespace