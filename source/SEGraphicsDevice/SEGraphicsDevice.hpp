#pragma once

#include "vulkan/vulkan.h"
#include "SEWindow/SEWindow.hpp"
#include <string>
#include <vector>

namespace SE {

	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	struct QueueFamilyIndices {
		uint32_t graphicsFamily;
		uint32_t presentFamily;
		bool graphicsFamilyHasValue = false;
		bool presentFamilyHasValue = false;
		bool complete() { return graphicsFamilyHasValue && presentFamilyHasValue; }
	};

	class SEGraphicsDevice {

	public:

#pragma region Lifecycle
		SEGraphicsDevice(SEWindow& window);
		~SEGraphicsDevice();
		SEGraphicsDevice(const SEGraphicsDevice&) = delete;
		void operator=(const SEGraphicsDevice&) = delete;
		SEGraphicsDevice(SEGraphicsDevice&&) = delete;
		SEGraphicsDevice& operator=(SEGraphicsDevice&&) = delete;
#pragma endregion Lifecycle

		VkCommandPool get_command_pool() { return m_CommandPool; }
		VkDevice device() { return m_GraphicsDevice; }
		VkSurfaceKHR surface() { return m_Surface; }
		VkQueue graphicsQueue() { return m_GraphicsQueue; }
		VkQueue presentQueue() { return m_PresentQueue; }
		QueueFamilyIndices find_physical_queue_families() { return find_queue_families(m_PhysicalDevice); }
		SwapChainSupportDetails get_swap_chain_support() { return query_swap_chain_support(m_PhysicalDevice); }
		uint32_t find_memory_type(uint32_t typeFilter, VkMemoryPropertyFlags properties);
		VkFormat find_supported_format(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

		// Vertex Buffer
		VkCommandBuffer begin_single_time_commands();
		void end_single_time_commands(VkCommandBuffer commandBuffer);
		void create_vertex_buffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
		void copy_buffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
		void copy_buffer_to_image(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layerCount);
		void create_image_with_info(const VkImageCreateInfo& imageInfo, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);

		VkPhysicalDeviceProperties properties;

#ifdef NDEBUG
		const bool ENABLE_VALIDATION_LAYERS = false;
#else
		const bool ENABLE_VALIDATION_LAYERS = true;
#endif

	private:
		void create_instance();
		void setup_debug_messenger();
		void create_surface();
		void pick_physical_device();
		void create_logical_device();
		void create_command_pool();

		bool check_device_suitability(VkPhysicalDevice device);
		std::vector<const char*> get_required_extensions();
		bool check_validation_layer_support();
		QueueFamilyIndices find_queue_families(VkPhysicalDevice device);
		void populate_debug_messenger_create_info(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
		void has_gflw_required_instance_extensions();
		bool check_device_extensions_support(VkPhysicalDevice device);
		SwapChainSupportDetails query_swap_chain_support(VkPhysicalDevice device);

		VkInstance m_Instance;
		VkDebugUtilsMessengerEXT m_DebugMessenger;
		VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
		SEWindow& m_Window;
		VkCommandPool m_CommandPool;

		VkDevice m_GraphicsDevice;
		VkSurfaceKHR m_Surface;
		VkQueue m_GraphicsQueue;
		VkQueue m_PresentQueue;

		const std::vector<const char*> m_ValidationLayers = { "VK_LAYER_KHRONOS_validation" };
		const std::vector<const char*> m_DeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
	};

}  // end SE namespace