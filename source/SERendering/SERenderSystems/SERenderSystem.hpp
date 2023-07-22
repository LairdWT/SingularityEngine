#pragma once

#include "SERendering/SERenderPipeline/SERenderPipeline.hpp"
#include "SERendering/SEGraphicsDevice/SEGraphicsDevice.hpp"
#include "SECore/SEEntities/SEGameObject.hpp"
#include "SECore/SEEntities/SECamera.hpp"
#include "SERendering/SEFrameInfo.hpp"

#include <memory>
#include <vector>

namespace SE {

	class SERenderSystem {

	public:

#pragma region Lifecycle
		SERenderSystem(SEGraphicsDevice& graphicsDevice, VkRenderPass renderPass);
		~SERenderSystem();

		SERenderSystem(const SERenderSystem&) = delete;
		SERenderSystem& operator=(const SERenderSystem&) = delete;
#pragma endregion Lifecycle

		void render_game_objects(FFrameInfo& frameInfo, std::vector<SEGameObject>& gameObjects);


	private:

		void create_pipeline_layout();
		void create_pipeline(VkRenderPass renderPass);


		SEGraphicsDevice& m_GraphicsDevice;
		VkPipelineLayout m_PipelineLayout;

		std::unique_ptr<SERenderPipeline> m_Pipeline;
	};

} // end SE namespace