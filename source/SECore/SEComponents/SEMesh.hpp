#pragma once

#include "SERendering/SEGraphicsDevice/SEGraphicsDevice.hpp"
#include "SERendering/SEBuffer.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vector>
#include <memory>

namespace SE {

	class SEMesh {

	public:
		struct Vertex {
			glm::vec3 position{};
			glm::vec3 color{};
			glm::vec3 normal{};
			glm::vec2 texCoord{};

			static std::vector<VkVertexInputBindingDescription> get_binding_descriptions();
			static std::vector<VkVertexInputAttributeDescription> get_attribute_descriptions();

			// Equality operator
			bool operator==(const Vertex& other) const 
			{
				return position == other.position && color == other.color && normal == other.normal && texCoord == other.texCoord;
			}
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

		// Load a model from a file
		static std::unique_ptr<SEMesh> create_model_from_file(SEGraphicsDevice& device, const std::string& filepath);
		static void load_mesh_from_file(Builder& builder, const std::string& filepath);

		void bind_command_buffer(VkCommandBuffer commandBuffer);
		void draw(VkCommandBuffer commandBuffer);


	private:

		void create_vertex_buffers(const std::vector<Vertex>& verticies);
		void create_index_buffers(const std::vector<uint32_t>& indicies);


		// Graphics Device
		SEGraphicsDevice m_GraphicsDevice;

		// Vertex Buffer
		std::unique_ptr<SEBuffer> m_VertexBuffer;
		uint32_t m_VertexCount;

		// Index Buffer
		bool m_HasIndexBuffer{false};
		std::unique_ptr<SEBuffer> m_IndexBuffer;
		uint32_t m_IndexCount;
	};
} // end namespace SE