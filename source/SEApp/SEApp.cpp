#include "SEApp.hpp"
#include <stdexcept>
#include <array>
#include <iostream>
#include <sstream>
#include <iomanip>

#include "SERendering/SERenderSystems/SERenderSystem.hpp"
#include "SECore/SEEntities/SECamera.hpp"
#include "SECore/SEInput/SEKeyboardInputController.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace SE {

#pragma region Lifecycle
	SEApp::SEApp()
	{
		load_game_objects();
		m_TimeManager = std::make_unique<SETimeManager>(m_FixedTimeStep);
	}

	SEApp::~SEApp()
	{

	}
#pragma endregion Lifecycle

void SEApp::run()
{
	SERenderSystem RenderSystem{m_GraphicsDevice, m_Renderer.get_swap_chain_render_pass()};
	SECamera camera{};
	SEGameObject viewerObject = SEGameObject::create_game_object();
	SEKeyboardInputController cameraInputController{};

	// Look at cube
	camera.set_view_target(glm::vec3{-1.0f, -2.0f, 2.0f}, glm::vec3{0.0f, 0.0f, 2.5f});

	m_TickDelegate = m_TimeManager->add_tick_delegate(std::bind(&SEApp::on_tick, this));
	m_TimeManager->update();

	while (!m_Window.should_close()) 
	{
		glfwPollEvents();

		// Update Time
		m_TimeManager->update();

		// Update Camera
		cameraInputController.move_in_xz_plane(m_Window.get_window(), viewerObject, m_TimeManager->get_delta_time());
		camera.set_view_yxz(viewerObject.m_TransformComponent.Translation, viewerObject.m_TransformComponent.Rotation);

		float aspectRatio = m_Renderer.get_swap_chain_aspect_ratio();
		camera.set_perspective_projection(glm::radians(60.0f), aspectRatio, 0.01f, 100.0f);

		if (VkCommandBuffer commandBuffer = m_Renderer.begin_frame())
		{
			m_Renderer.begin_swap_chain_render_pass(commandBuffer);
			RenderSystem.render_game_objects(commandBuffer, m_GameObjects, camera);
			m_Renderer.end_swap_chain_render_pass(commandBuffer);
			m_Renderer.end_frame();
		}
	}
	// m_TickCounter++;

	vkDeviceWaitIdle(m_GraphicsDevice.device());
}

void SEApp::on_tick()
{
	std::ostringstream ss;
	ss << "\rCurrent Tick Time: " << std::setw(3) << m_TimeManager->get_fixed_time_step()
		<< "   Current FPS: " << std::setw(3) << m_TimeManager->get_fps()
		<< "   ";
	std::cout << ss.str() << std::flush;
}


std::unique_ptr<SEMesh> createCubeModel(SEGraphicsDevice& device, glm::vec3 offset) 
{
	SEMesh::Builder modelBuilder{};
	modelBuilder.vertices = 
		{
			// left face (white)
			{{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
			{{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
			{{-.5f, -.5f, .5f}, {.9f, .9f, .9f}},
			{{-.5f, .5f, -.5f}, {.9f, .9f, .9f}},

			// right face (yellow)
			{{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
			{{.5f, .5f, .5f}, {.8f, .8f, .1f}},
			{{.5f, -.5f, .5f}, {.8f, .8f, .1f}},
			{{.5f, .5f, -.5f}, {.8f, .8f, .1f}},

			// top face (orange, y axis points down)
			{{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
			{{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
			{{-.5f, -.5f, .5f}, {.9f, .6f, .1f}},
			{{.5f, -.5f, -.5f}, {.9f, .6f, .1f}},

			// bottom face (red)
			{{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
			{{.5f, .5f, .5f}, {.8f, .1f, .1f}},
			{{-.5f, .5f, .5f}, {.8f, .1f, .1f}},
			{{.5f, .5f, -.5f}, {.8f, .1f, .1f}},

			// nose face (blue)
			{{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
			{{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
			{{-.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
			{{.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},

			// tail face (green)
			{{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
			{{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
			{{-.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
			{{.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
		};

	for (auto& v : modelBuilder.vertices) {
		v.position += offset;
	}

	modelBuilder.indices = { 0,  1,  2,  0,  3,  1,  4,  5,  6,  4,  7,  5,  8,  9,  10, 8,  11, 9,
						12, 13, 14, 12, 15, 13, 16, 17, 18, 16, 19, 17, 20, 21, 22, 20, 23, 21 };

	return std::make_unique<SEMesh>(device, modelBuilder);
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