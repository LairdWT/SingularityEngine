#include "SEMesh.hpp"
#include "SECore/SEUtilities/SEHashUtilities.hpp"

#include <cassert>
#include <unordered_map>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tinyobjloader.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

namespace std
{
	template <> 
	struct hash<SE::SEMesh::Vertex>
	{
		size_t operator()(SE::SEMesh::Vertex const& vertex) const
		{
			size_t seed = 0;
			SE::hash_combine(seed, vertex.position, vertex.color, vertex.normal, vertex.texCoord);
			return seed;
		}
	};
}


namespace SE {


SEMesh::SEMesh(SEGraphicsDevice& device, const SEMesh::Builder& builder) : m_GraphicsDevice(device)
{
	create_vertex_buffers(builder.vertices);
	create_index_buffers(builder.indices);
}

SEMesh::~SEMesh()
{

}

std::unique_ptr<SEMesh> SEMesh::create_model_from_file(SEGraphicsDevice& device, const std::string& filepath)
{
	Builder builder{};
	load_mesh_from_file(builder, filepath);
	return std::make_unique<SEMesh>(device, builder);
}

void SEMesh::load_mesh_from_file(Builder& builder, const std::string& filepath)
{
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn, err;

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filepath.c_str()))
	{
		throw std::runtime_error(warn + err);
	}

	builder.vertices.clear();
	builder.indices.clear();

	std::unordered_map<Vertex, uint32_t> uniqueVertices{};

	for (const auto& shape : shapes)
	{
		for (const auto& index : shape.mesh.indices)
		{
			Vertex vertex{};
	
			if (index.vertex_index >= 0)
			{
				vertex.position = 
				{
					attrib.vertices[3 * index.vertex_index + 0],
					attrib.vertices[3 * index.vertex_index + 1],
					attrib.vertices[3 * index.vertex_index + 2]
				};

				vertex.color =
				{
					attrib.colors[3 * index.vertex_index + 0],
					attrib.colors[3 * index.vertex_index + 1],
					attrib.colors[3 * index.vertex_index + 2]
				};
			}

			if (index.normal_index >= 0)
			{
				vertex.normal =
				{
					attrib.normals[3 * index.normal_index],
					attrib.normals[3 * index.normal_index + 1],
					attrib.normals[3 * index.normal_index + 2]
				};
			}

			if (index.texcoord_index >= 0)
			{
				vertex.texCoord =
				{
					attrib.texcoords[2 * index.texcoord_index],
					attrib.texcoords[2 * index.texcoord_index + 1]
				};
			}

			if (uniqueVertices.count(vertex) == 0)
			{
				uniqueVertices[vertex] = static_cast<uint32_t>(builder.vertices.size());
				builder.vertices.push_back(vertex);
			}
			builder.indices.push_back(uniqueVertices[vertex]);
		}
	}
}

void SEMesh::bind_command_buffer(VkCommandBuffer commandBuffer)
{
	VkBuffer buffers[] = { m_VertexBuffer->get_buffer() };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);

	if (m_HasIndexBuffer)
	{
		vkCmdBindIndexBuffer(commandBuffer, m_IndexBuffer->get_buffer(), 0, VK_INDEX_TYPE_UINT32);
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

	// Create staging buffer
	uint32_t vertexSize = sizeof(vertices[0]);
	SEBuffer stagingBuffer{ m_GraphicsDevice, vertexSize, m_VertexCount, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT };
	stagingBuffer.map();
	stagingBuffer.write_to_buffer((void*)vertices.data());

	// Create vertex buffer
	m_VertexBuffer = std::make_unique<SEBuffer>(m_GraphicsDevice, vertexSize, m_VertexCount, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	m_GraphicsDevice.copy_buffer(stagingBuffer.get_buffer(), m_VertexBuffer->get_buffer(), bufferSize);
}

void SEMesh::create_index_buffers(const std::vector<uint32_t>& indices)
{
	m_IndexCount = static_cast<uint32_t>(indices.size());
	m_HasIndexBuffer = (m_IndexCount > 0);
	if (!m_HasIndexBuffer) { return; }

	VkDeviceSize bufferSize = sizeof(indices[0]) * m_IndexCount;

	// Create staging buffer
	uint32_t indexSize = sizeof(indices[0]);
	SEBuffer stagingBuffer{ m_GraphicsDevice, indexSize, m_IndexCount, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT };
	stagingBuffer.map();
	stagingBuffer.write_to_buffer((void*)indices.data());

	// Create index buffer
	m_IndexBuffer = std::make_unique<SEBuffer>(m_GraphicsDevice, indexSize, m_IndexCount, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	m_GraphicsDevice.copy_buffer(stagingBuffer.get_buffer(), m_IndexBuffer->get_buffer(), bufferSize);
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
	std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};
	
	attributeDescriptions.push_back({0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position)});
	attributeDescriptions.push_back({ 1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color) });
	attributeDescriptions.push_back({ 2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal) });
	attributeDescriptions.push_back({ 3, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, texCoord) });

	return attributeDescriptions;
}

} // end namespace SE