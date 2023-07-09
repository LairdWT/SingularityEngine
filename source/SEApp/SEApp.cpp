#include "SEApp.hpp"
#include <stdexcept>
#include <array>


#include "SERendering/SERenderSystems/SERenderSystem.hpp"
#include "SECore/SEEntities/SECamera.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace SE {

#pragma region Lifecycle
	SEApp::SEApp()
	{
		load_game_objects();
		m_TimeManager = std::make_unique<SETimeManager>();
		m_TimeManager->initialize();
	}

	SEApp::~SEApp()
	{

	}
#pragma endregion Lifecycle

void SEApp::run()
{
	SERenderSystem RenderSystem{m_GraphicsDevice, m_Renderer.get_swap_chain_render_pass()};
	SECamera camera{};
	// camera.set_view_direction(glm::vec3{0.0f}, glm::vec3{0.5f, 0.0f, 1.0f});
	camera.set_view_target(glm::vec3{-1.0f, -2.0f, 2.0f}, glm::vec3{0.0f, 0.0f, 2.5f});

	m_TimeManager->update();

	while (!m_Window.should_close()) 
	{
		glfwPollEvents();

		m_TimeManager->update();

		float aspectRatio = m_Renderer.get_swap_chain_aspect_ratio();
		// camera.set_orthographic_projection(-aspectRatio, aspectRatio, -1.0f, 1.0f, -1.0f, 1.0f);
		camera.set_perspective_projection(glm::radians(60.0f), aspectRatio, 0.01f, 100.0f);

		if (VkCommandBuffer commandBuffer = m_Renderer.begin_frame())
		{
			m_Renderer.begin_swap_chain_render_pass(commandBuffer);
			RenderSystem.render_game_objects(commandBuffer, m_GameObjects, camera);
			m_Renderer.end_swap_chain_render_pass(commandBuffer);
			m_Renderer.end_frame();
		}
	}

	vkDeviceWaitIdle(m_GraphicsDevice.device());
}

std::unique_ptr<SEMesh> createCubeModel(SEGraphicsDevice& device, glm::vec3 offset) {
	std::vector<SEMesh::Vertex> vertices{

		// left face (white)
		{{-.5f, -.5f, -.5f}, { .9f, .9f, .9f }},
		{ {-.5f, .5f, .5f}, {.9f, .9f, .9f} },
		{ {-.5f, -.5f, .5f}, {.9f, .9f, .9f} },
		{ {-.5f, -.5f, -.5f}, {.9f, .9f, .9f} },
		{ {-.5f, .5f, -.5f}, {.9f, .9f, .9f} },
		{ {-.5f, .5f, .5f}, {.9f, .9f, .9f} },

			// right face (yellow)
		{ {.5f, -.5f, -.5f}, {.8f, .8f, .1f} },
		{ {.5f, .5f, .5f}, {.8f, .8f, .1f} },
		{ {.5f, -.5f, .5f}, {.8f, .8f, .1f} },
		{ {.5f, -.5f, -.5f}, {.8f, .8f, .1f} },
		{ {.5f, .5f, -.5f}, {.8f, .8f, .1f} },
		{ {.5f, .5f, .5f}, {.8f, .8f, .1f} },

			// top face (orange, remember y axis points down)
		{ {-.5f, -.5f, -.5f}, {.9f, .6f, .1f} },
		{ {.5f, -.5f, .5f}, {.9f, .6f, .1f} },
		{ {-.5f, -.5f, .5f}, {.9f, .6f, .1f} },
		{ {-.5f, -.5f, -.5f}, {.9f, .6f, .1f} },
		{ {.5f, -.5f, -.5f}, {.9f, .6f, .1f} },
		{ {.5f, -.5f, .5f}, {.9f, .6f, .1f} },

			// bottom face (red)
		{ {-.5f, .5f, -.5f}, {.8f, .1f, .1f} },
		{ {.5f, .5f, .5f}, {.8f, .1f, .1f} },
		{ {-.5f, .5f, .5f}, {.8f, .1f, .1f} },
		{ {-.5f, .5f, -.5f}, {.8f, .1f, .1f} },
		{ {.5f, .5f, -.5f}, {.8f, .1f, .1f} },
		{ {.5f, .5f, .5f}, {.8f, .1f, .1f} },

			// nose face (blue)
		{ {-.5f, -.5f, 0.5f}, {.1f, .1f, .8f} },
		{ {.5f, .5f, 0.5f}, {.1f, .1f, .8f} },
		{ {-.5f, .5f, 0.5f}, {.1f, .1f, .8f} },
		{ {-.5f, -.5f, 0.5f}, {.1f, .1f, .8f} },
		{ {.5f, -.5f, 0.5f}, {.1f, .1f, .8f} },
		{ {.5f, .5f, 0.5f}, {.1f, .1f, .8f} },

			// tail face (green)
		{ {-.5f, -.5f, -0.5f}, {.1f, .8f, .1f} },
		{ {.5f, .5f, -0.5f}, {.1f, .8f, .1f} },
		{ {-.5f, .5f, -0.5f}, {.1f, .8f, .1f} },
		{ {-.5f, -.5f, -0.5f}, {.1f, .8f, .1f} },
		{ {.5f, -.5f, -0.5f}, {.1f, .8f, .1f} },
		{ {.5f, .5f, -0.5f}, {.1f, .8f, .1f} },

	};
	for (auto& v : vertices) {
		v.position += offset;
	}
	return std::make_unique<SEMesh>(device, vertices);
}

void SEApp::load_game_objects()
{
	std::shared_ptr<SEMesh> cubeMesh = createCubeModel(m_GraphicsDevice, glm::vec3{ 0, 0, 0 });
	SEGameObject cube = SEGameObject::create_game_object();

	cube.m_Mesh = cubeMesh;
	cube.m_TransformComponent.Translation = { 0.0f, 0.0f, 2.5f };
	cube.m_TransformComponent.Rotation = { 0.0f, 0.0f, 0.0f };
	cube.m_TransformComponent.Scale = { 0.5f, 0.5f, 0.5f };

	m_GameObjects.push_back(std::move(cube));
}

} // namespace SE