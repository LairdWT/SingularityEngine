#pragma once

#include "SERendering/SEGraphicsDevice/SEGraphicsDevice.hpp"
#include <vulkan/vulkan.h>
#include <string>
#include <vector>
#include <memory>

namespace SE {

	class SESwapChain {
	public:
		static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

#pragma region Lifecycle
		SESwapChain(SEGraphicsDevice& deviceRef, VkExtent2D windowExtent);
		SESwapChain(SEGraphicsDevice& deviceRef, VkExtent2D windowExtent, std::shared_ptr<SESwapChain> previousSwapChain);
		~SESwapChain();

		SESwapChain(const SESwapChain&) = delete;
		SESwapChain& operator=(const SESwapChain&) = delete;
#pragma endregion Lifecycle

		VkFramebuffer get_frame_buffer(int index) { return m_SwapChainFramebuffers[index]; }
		VkRenderPass get_render_pass() { return m_RenderPass; }
		VkImageView get_image_view(int index) { return m_SwapChainImageViews[index]; }
		size_t get_image_count() { return m_SwapChainImages.size(); }
		VkFormat get_swapchain_image_format() { return m_SwapChainImageFormat; }
		VkExtent2D get_spawchain_extent() { return m_SwapChainExtent; }
		uint32_t get_swapchain_width() { return m_SwapChainExtent.width; }
		uint32_t get_swapchain_height() { return m_SwapChainExtent.height; }

		float get_extent_aspect_ratio() { return static_cast<float>(m_SwapChainExtent.width) / static_cast<float>(m_SwapChainExtent.height); }
		VkFormat find_depth_format();

		VkResult acquire_next_image(uint32_t* imageIndex);
		VkResult submit_command_buffers(const VkCommandBuffer* buffers, uint32_t* imageIndex);

	private:
		void initialize();
		void create_swapchain();
		void create_image_views();
		void create_depth_resources();
		void create_render_pass();
		void create_frame_buffers();
		void create_sync_object();

		// Helper functions
		VkSurfaceFormatKHR choose_swap_surface_format(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		VkPresentModeKHR choose_swap_present_mode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		VkExtent2D choose_swap_extent(const VkSurfaceCapabilitiesKHR& capabilities);

		VkFormat m_SwapChainImageFormat;
		VkExtent2D m_SwapChainExtent;

		std::vector<VkFramebuffer> m_SwapChainFramebuffers;
		VkRenderPass m_RenderPass;

		std::vector<VkImage> m_DepthImages;
		std::vector<VkDeviceMemory> m_DepthImageMemorys;
		std::vector<VkImageView> m_DepthImageViews;
		std::vector<VkImage> m_SwapChainImages;
		std::vector<VkImageView> m_SwapChainImageViews;

		SEGraphicsDevice& m_GraphicsDevice;
		VkExtent2D m_WindowExtent;

		VkSwapchainKHR m_SwapChain;
		std::shared_ptr<SESwapChain> m_PreviousSwapChain;

		std::vector<VkSemaphore> m_ImageAvailableSemaphores;
		std::vector<VkSemaphore> m_RenderFinishedSemaphores;
		std::vector<VkFence> m_InFlightFences;
		std::vector<VkFence> m_ImagesInFlight;
		size_t m_CurrentFrame = 0;
	};

}  // namespace lve