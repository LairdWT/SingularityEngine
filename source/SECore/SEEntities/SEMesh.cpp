#include "SEMesh.hpp"
#include <cassert>

namespace SE {


SEMesh::SEMesh(SEGraphicsDevice& device, const std::vector<Vertex>& verticies) : m_GraphicsDevice(device)
{
	create_vertex_buffers(verticies);
}

SEMesh::~SEMesh()
{
	vkDestroyBuffer(m_GraphicsDevice.device(), m_VertexBuffer, nullptr);
	vkFreeMemory(m_GraphicsDevice.device(), m_VertexBufferMemory, nullptr);
}

void SEMesh::bind_command_buffer(VkCommandBuffer commandBuffer)
{
	VkBuffer buffers[] = { m_VertexBuffer };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
}

void SEMesh::draw(VkCommandBuffer commandBuffer)
{
	vkCmdDraw(commandBuffer, m_VertexCount, 1, 0, 0);
}

void SEMesh::create_vertex_buffers(const std::vector<Vertex>& verticies)
{
	m_VertexCount = static_cast<uint32_t>(verticies.size());
	assert(m_VertexCount >= 3 && "Vertex count must be at least 3");

	VkDeviceSize bufferSize = sizeof(verticies[0]) * m_VertexCount;
	m_GraphicsDevice.create_vertex_buffer(bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_VertexBuffer, m_VertexBufferMemory);

	void* vertexBufferData;
	vkMapMemory(m_GraphicsDevice.device(), m_VertexBufferMemory, 0, bufferSize, 0, &vertexBufferData);
	memcpy(vertexBufferData, verticies.data(), static_cast<size_t>(bufferSize));
	vkUnmapMemory(m_GraphicsDevice.device(), m_VertexBufferMemory);
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