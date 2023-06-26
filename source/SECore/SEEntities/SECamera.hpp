#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace SE
{
	class SECamera
	{
	public:
	
#pragma region Lifecycle
		SECamera();
		~SECamera();
#pragma endregion Lifecycle

	void set_perspective_projection(float fovy, float aspect, float near, float far);
	void set_orthographic_projection(float left, float right, float bottom, float top, float near, float far);
	
	const glm::mat4& get_projection_matrix() const { return m_ProjectionMatrix; }


	private:

	glm::mat4 m_ProjectionMatrix{1.0f};

	};
} // end namespace SE