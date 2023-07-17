#pragma once

#include "SERendering/SEGraphicsDevice/SEGraphicsDevice.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vector>

namespace SE {

	class SEMesh {

	public:
		struct Vertex {
			glm::vec3 position;
			glm::vec3 color;

			static std::vector<VkVertexInputBindingDescription> get_binding_descriptions();
			static std::vector<VkVertexInputAttributeDescription> get_attribute_descriptions();
		};

		struct Builder {
			std::vector<Vertex> vertices;
			std::vector<uint32_t> indices;
		};

#pragma region Lifecycle
		SEMesh(SEGraphicsDevice& device, const SEMesh::Builder& builder);
		~SEMesh();
		SEMesh(const SEMesh&) = delete;
		SEMesh& operator=(const SEMesh&) = delete;
#pragma endregion Lifecycle

		void bind_command_buffer(VkCommandBuffer commandBuffer);
		void draw(VkCommandBuffer commandBuffer);


	private:

		void create_vertex_buffers(const std::vector<Vertex>& verticies);
		void create_index_buffers(const std::vector<uint32_t>& indicies);


		// Graphics Device
		SEGraphicsDevice m_GraphicsDevice;

		// Vertex Buffer
		VkBuffer m_VertexBuffer;
		VkDeviceMemory m_VertexBufferMemory;
		uint32_t m_VertexCount;

		// Index Buffer
		bool m_HasIndexBuffer{false};
		VkBuffer m_IndexBuffer;
		VkDeviceMemory m_IndexBufferMemory;
		uint32_t m_IndexCount;
	};
} // end namespace SE