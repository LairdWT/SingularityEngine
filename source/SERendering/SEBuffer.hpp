#pragma once

#include "SERendering/SEGraphicsDevice/SEGraphicsDevice.hpp"

namespace SE {

class SEBuffer {

public:
#pragma region Lifecycle
	SEBuffer(SEGraphicsDevice& device, VkDeviceSize instanceSize, uint32_t instanceCount, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryPropertyFlags, VkDeviceSize minOffsetAlignment = 1);
	~SEBuffer();

	SEBuffer(const SEBuffer&) = delete;
	SEBuffer& operator=(const SEBuffer&) = delete;
#pragma endregion Lifecycle

	VkResult map(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
	void unmap();
	void write_to_buffer(void* data, VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
	VkResult flush(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
	VkResult invalidate(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

	// Index management
	void write_to_index(void* data, int index);
	VkResult flush_index(int index);
	VkDescriptorBufferInfo descriptor_info_for_index(int index);
	VkResult invalidate_index(int index);

	// Getters
	VkDescriptorBufferInfo get_descriptor_info(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
	VkBuffer get_buffer() const { return m_Buffer; }
	void* get_mapped_memory() const { return m_MappedData; }
	uint32_t get_instance_count() const { return m_InstanceCount; }
	VkDeviceSize get_instance_size() const { return m_InstanceSize; }
	VkDeviceSize get_alignment_size() const { return m_InstanceSize; }
	VkBufferUsageFlags get_usage_flags() const { return m_UsageFlags; }
	VkMemoryPropertyFlags get_memory_property_flags() const { return m_MemoryPropertyFlags; }
	VkDeviceSize get_bufferSize() const { return m_BufferSize; }

private:
	static VkDeviceSize get_alignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment);

	SEGraphicsDevice& m_GraphicsDevice;
	void* m_MappedData = nullptr;
	VkBuffer m_Buffer = VK_NULL_HANDLE;
	VkDeviceMemory m_Memory = VK_NULL_HANDLE;

	VkDeviceSize m_BufferSize;
	uint32_t m_InstanceCount;
	VkDeviceSize m_InstanceSize;
	VkDeviceSize m_AlignmentSize;
	VkBufferUsageFlags m_UsageFlags;
	VkMemoryPropertyFlags m_MemoryPropertyFlags;
};

}  // namespace SE