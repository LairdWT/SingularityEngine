#include "SEBuffer.hpp"

// std
#include <cassert>
#include <cstring>

namespace SE {

SEBuffer::SEBuffer(SEGraphicsDevice& device, VkDeviceSize instanceSize, uint32_t instanceCount, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryPropertyFlags, VkDeviceSize minOffsetAlignment)
    : m_GraphicsDevice{ device }, m_InstanceSize{ instanceSize }, m_InstanceCount{ instanceCount }, m_UsageFlags{ usageFlags }, m_MemoryPropertyFlags{ memoryPropertyFlags } 
{
    m_AlignmentSize = get_alignment(instanceSize, minOffsetAlignment);
    m_BufferSize = m_AlignmentSize * instanceCount;
    device.create_buffer(m_BufferSize, usageFlags, memoryPropertyFlags, m_Buffer, m_Memory);
}

SEBuffer::~SEBuffer() 
{
    unmap();
    vkDestroyBuffer(m_GraphicsDevice.device(), m_Buffer, nullptr);
    vkFreeMemory(m_GraphicsDevice.device(), m_Memory, nullptr);
}

// Returns the minimum instance size required to be compatible with devices minOffsetAlignment
VkDeviceSize SEBuffer::get_alignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment)
{
    if (minOffsetAlignment > 0)
    {
        return (instanceSize + minOffsetAlignment - 1) & ~(minOffsetAlignment - 1);
    }
    return instanceSize;
}

// Map a memory range of this buffer. If successful, mapped points to the specified buffer range.
VkResult SEBuffer::map(VkDeviceSize size, VkDeviceSize offset)
{
    return vkMapMemory(m_GraphicsDevice.device(), m_Memory, offset, size, 0, &m_MappedData);
}

// Unmap a mapped memory range
void SEBuffer::unmap()
{
    if (m_MappedData) 
    {
        vkUnmapMemory(m_GraphicsDevice.device(), m_Memory);
        m_MappedData = nullptr;
    }
}

// Copies the specified data to the mapped buffer. Default value writes whole buffer range
void SEBuffer::write_to_buffer(void* data, VkDeviceSize size, VkDeviceSize offset)
{
    assert(m_MappedData && "Cannot copy to unmapped buffer");

    if (size == VK_WHOLE_SIZE) 
    {
        memcpy(m_MappedData, data, m_BufferSize);
    } else {
        char* memOffset = (char*)m_MappedData;
        memOffset += offset;
        memcpy(memOffset, data, size);
    }
}

// Flush a memory range of the buffer to make it visible to the device. Only required for non-coherent memory
VkResult SEBuffer::flush(VkDeviceSize size, VkDeviceSize offset)
{
    VkMappedMemoryRange mappedRange = {};
    mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
    mappedRange.memory = m_Memory;
    mappedRange.offset = offset;
    mappedRange.size = size;
    return vkFlushMappedMemoryRanges(m_GraphicsDevice.device(), 1, &mappedRange);
}

// Invalidate a memory range of the buffer to make it visible to the host. Only required for non-coherent memory
VkResult SEBuffer::invalidate(VkDeviceSize size, VkDeviceSize offset)
{
    VkMappedMemoryRange mappedRange = {};
    mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
    mappedRange.memory = m_Memory;
    mappedRange.offset = offset;
    mappedRange.size = size;
    return vkInvalidateMappedMemoryRanges(m_GraphicsDevice.device(), 1, &mappedRange);
}

// Create a buffer info descriptor
VkDescriptorBufferInfo SEBuffer::get_descriptor_info(VkDeviceSize size, VkDeviceSize offset)
{
    return VkDescriptorBufferInfo{ m_Buffer, offset, size };
}

// Copies "instanceSize" bytes of data to the mapped buffer at an offset of index * alignmentSize
void SEBuffer::write_to_index(void* data, int index)
{
    write_to_buffer(data, m_InstanceSize, index * m_AlignmentSize);
}

// Flush the memory range at index * alignmentSize of the buffer to make it visible to the device
VkResult SEBuffer::flush_index(int index) 
{ 
    return flush(m_AlignmentSize, index * m_AlignmentSize);
}

// Create a buffer info descriptor
VkDescriptorBufferInfo SEBuffer::descriptor_info_for_index(int index)
{
    return get_descriptor_info(m_AlignmentSize, index * m_AlignmentSize);
}

// Invalidate a memory range of the buffer to make it visible to the host. Only required for non-coherent memory
VkResult SEBuffer::invalidate_index(int index)
{
    return invalidate(m_AlignmentSize, index * m_AlignmentSize);
}

}  // namespace SE