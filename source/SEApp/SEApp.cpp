#include "SEApp.hpp"
#include <stdexcept>
#include <array>
#include <iostream>
#include <sstream>
#include <iomanip>

#include "SERendering/SERenderSystems/SERenderSystem.hpp"
#include "SECore/SEEntities/SECamera.hpp"
#include "SECore/SEInput/SEKeyboardInputController.hpp"
#include "SERendering/SEBuffer.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace SE {

struct FGlobalUniformBufferObject 
{
	glm::mat4 projectionView{1.0f};
	glm::vec3 lightDirection{glm::normalize(glm::vec3{1.0f, -3.0f, -1.0f})};
};

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
	std::vector<std::unique_ptr<SEBuffer>> uniformBuffers(SESwapChain::MAX_FRAMES_IN_FLIGHT);

	for (int i = 0; i < uniformBuffers.size(); i++)
	{
		uniformBuffers[i] = std::make_unique<SEBuffer>(m_GraphicsDevice, sizeof(FGlobalUniformBufferObject), 1, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_GraphicsDevice.properties.limits.minUniformBufferOffsetAlignment);
		uniformBuffers[i]->map();
	}

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
			uint32_t currentFrameIndex = m_Renderer.get_current_frame_index();
			FFrameInfo frameInfo{currentFrameIndex, m_TimeManager->get_delta_time(), commandBuffer, camera};

			// update global uniform buffer
			FGlobalUniformBufferObject uniformBufferObject{};
			uniformBufferObject.projectionView = camera.get_projection_matrix() * camera.get_view_matrix();
			uniformBuffers[currentFrameIndex]->write_to_buffer(&uniformBufferObject);
			uniformBuffers[currentFrameIndex]->flush();

			// rendering
			m_Renderer.begin_swap_chain_render_pass(commandBuffer);
			RenderSystem.render_game_objects(frameInfo, m_GameObjects);
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

void SEApp::load_game_objects()
{
	std::shared_ptr<SEMesh> seMesh = SEMesh::create_model_from_file(m_GraphicsDevice, "content/models/starter/sm_gadgetbot.obj");
	
	SEGameObject gameObject = SEGameObject::create_game_object();
	gameObject.m_Mesh = seMesh;
	gameObject.m_TransformComponent.Translation = { 0.0f, 0.0f, 2.5f };
	gameObject.m_TransformComponent.Rotation = { 0.0f, 0.0f, 0.0f };
	gameObject.m_TransformComponent.Scale = { 0.5f, 0.5f, 0.5f };
	m_GameObjects.push_back(std::move(gameObject));

	SEGameObject gameObjectTwo = SEGameObject::create_game_object();
	gameObjectTwo.m_Mesh = seMesh;
	gameObjectTwo.m_TransformComponent.Translation = { 1.0f, 0.0f, 2.5f };
	gameObjectTwo.m_TransformComponent.Rotation = { 0.0f, 0.0f, 0.0f };
	gameObjectTwo.m_TransformComponent.Scale = { 0.7f, 0.7f, 0.7f };
	m_GameObjects.push_back(std::move(gameObjectTwo));
}

} // namespace SE