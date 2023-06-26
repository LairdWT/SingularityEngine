#include "SEApp.hpp"
#include <stdexcept>
#include <array>

#include "SERendering/SERenderSystems/SERenderSystem.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace SE {

#pragma region Lifecycle
	SEApp::SEApp()
	{
		load_game_objects();
	}

	SEApp::~SEApp()
	{

	}
#pragma endregion Lifecycle

void SEApp::run()
{
	SERenderSystem RenderSystem{m_GraphicsDevice, m_Renderer.get_swap_chain_render_pass()};

	while (!m_Window.should_close()) 
	{
		glfwPollEvents();
		if (VkCommandBuffer commandBuffer = m_Renderer.begin_frame())
		{
			m_Renderer.begin_swap_chain_render_pass(commandBuffer);
			RenderSystem.render_game_objects(commandBuffer, m_GameObjects);
			m_Renderer.end_swap_chain_render_pass(commandBuffer);
			m_Renderer.end_frame();
		}
	}

	vkDeviceWaitIdle(m_GraphicsDevice.device());
}

void SEApp::load_game_objects()
{
	std::vector<SEMesh::Vertex> verticies 
	{
		{ {0.0f, -0.5f},	{0.5f, 0.0f, 0.0f} },
		{ {0.5f, 0.5f},	{0.0f, 0.5f, 0.0f} },
		{ {-0.5f, 0.5f},	{0.0f, 0.0f, 0.5f} }
	};

	auto mesh = std::make_shared<SEMesh>(m_GraphicsDevice, verticies);

	auto TriangleGameObject = SEGameObject::create_game_object();
	TriangleGameObject.m_Mesh = mesh;
	TriangleGameObject.m_Color = { 0.1f, 0.12f, 0.25f };
	TriangleGameObject.m_Transform2d.Translation.x = 0.2f;
	TriangleGameObject.m_Transform2d.Scale = { 1.0f, 1.0f };
	TriangleGameObject.m_Transform2d.Rotation = 0.25f * glm::two_pi<float>();

	m_GameObjects.push_back(std::move(TriangleGameObject));
}

} // namespace SE