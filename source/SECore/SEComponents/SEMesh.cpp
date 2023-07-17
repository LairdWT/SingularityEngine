#include "SEMesh.hpp"
#include <cassert>

namespace SE {


SEMesh::SEMesh(SEGraphicsDevice& device, const SEMesh::Builder& builder) : m_GraphicsDevice(device)
{
	create_vertex_buffers(builder.vertices);
	create_index_buffers(builder.indices);
}

SEMesh::~SEMesh()
{
	vkDestroyBuffer(m_GraphicsDevice.device(), m_VertexBuffer, nullptr);
	vkFreeMemory(m_GraphicsDevice.device(), m_VertexBufferMemory, nullptr);

	if (m_HasIndexBuffer)
	{
		vkDestroyBuffer(m_GraphicsDevice.device(), m_IndexBuffer, nullptr);
		vkFreeMemory(m_GraphicsDevice.device(), m_IndexBufferMemory, nullptr);
	}
}

void SEMesh::bind_command_buffer(VkCommandBuffer commandBuffer)
{
	VkBuffer buffers[] = { m_VertexBuffer };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);

	if (m_HasIndexBuffer)
	{
		vkCmdBindIndexBuffer(commandBuffer, m_IndexBuffer, 0, VK_INDEX_TYPE_UINT32);
	}
}

void SEMesh::draw(VkCommandBuffer commandBuffer)
{
	if (m_HasIndexBuffer)
	{
		vkCmdDrawIndexed(commandBuffer, m_IndexCount, 1, 0, 0, 0);
		return;
	}
	vkCmdDraw(commandBuffer, m_VertexCount, 1, 0, 0);
}

void SEMesh::create_vertex_buffers(const std::vector<Vertex>& vertices)
{
	m_VertexCount = static_cast<uint32_t>(vertices.size());
	assert(m_VertexCount >= 3 && "Vertex count must be at least 3");

	VkDeviceSize bufferSize = sizeof(vertices[0]) * m_VertexCount;

	// Staging Buffer
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;

	m_GraphicsDevice.create_buffer(
		bufferSize, 
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
		stagingBuffer, 
		stagingBufferMemory);

	void* vertexBufferData;
	vkMapMemory(m_GraphicsDevice.device(), stagingBufferMemory, 0, bufferSize, 0, &vertexBufferData);
	memcpy(vertexBufferData, vertices.data(), static_cast<size_t>(bufferSize));
	vkUnmapMemory(m_GraphicsDevice.device(), stagingBufferMemory);

	// Vertex Buffer
	m_GraphicsDevice.create_buffer(
		bufferSize, 
		VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, 
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
		m_VertexBuffer, 
		m_VertexBufferMemory);

	m_GraphicsDevice.copy_buffer(stagingBuffer, m_VertexBuffer, bufferSize);

	vkDestroyBuffer(m_GraphicsDevice.device(), stagingBuffer, nullptr);
	vkFreeMemory(m_GraphicsDevice.device(), stagingBufferMemory, nullptr);
}

void SEMesh::create_index_buffers(const std::vector<uint32_t>& indices)
{
	m_IndexCount = static_cast<uint32_t>(indices.size());
	m_HasIndexBuffer = (m_IndexCount > 0);
	if (!m_HasIndexBuffer) { return; }

	VkDeviceSize bufferSize = sizeof(indices[0]) * m_IndexCount;

	// Staging Buffer
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;

	m_GraphicsDevice.create_buffer(
		bufferSize,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		stagingBuffer,
		stagingBufferMemory);

	void* indexBufferData;
	vkMapMemory(m_GraphicsDevice.device(), stagingBufferMemory, 0, bufferSize, 0, &indexBufferData);
	memcpy(indexBufferData, indices.data(), static_cast<size_t>(bufferSize));
	vkUnmapMemory(m_GraphicsDevice.device(), stagingBufferMemory);

	// Vertex Buffer
	m_GraphicsDevice.create_buffer(
		bufferSize,
		VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		m_IndexBuffer,
		m_IndexBufferMemory);

	m_GraphicsDevice.copy_buffer(stagingBuffer, m_IndexBuffer, bufferSize);

	vkDestroyBuffer(m_GraphicsDevice.device(), stagingBuffer, nullptr);
	vkFreeMemory(m_GraphicsDevice.device(), stagingBufferMemory, nullptr);
}

std::vector<VkVertexInputBindingDescription> SEMesh::Vertex::get_binding_descriptions()
{
	std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
	bindingDescriptions[0].binding = 0;
	bindingDescriptions[0].stride = sizeof(Vertex);
	bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	return bindingDescriptions;
}

std::vector<VkVertexInputAttributeDescription> SEMesh::Vertex::get_attribute_descriptions()
{
	std::vector<VkVertexInputAttributeDescription> attributeDescriptions(2);
	
	// Vertex Position
	attributeDescriptions[0].binding = 0;
	attributeDescriptions[0].location = 0;
	attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[0].offset = offsetof(Vertex, position);

	// Vertex Color
	attributeDescriptions[1].binding = 0;
	attributeDescriptions[1].location = 1;
	attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[1].offset = offsetof(Vertex, color);

	return attributeDescriptions;
}

} // end namespace SE