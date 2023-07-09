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
	
	void set_view_direction(const glm::vec3& position, const glm::vec3& direction, const glm::vec3& up = glm::vec3{ 0.0f, -1.0f, 0.0f });
	void set_view_target(const glm::vec3& position, const glm::vec3& target, const glm::vec3& up = glm::vec3{ 0.0f, -1.0f, 0.0f });
	void set_view_yxz(const glm::vec3& position, const glm::vec3& rotation);

	const glm::mat4& get_projection_matrix() const { return m_ProjectionMatrix; }
	const glm::mat4& get_view_matrix() const { return m_ViewMatrix; }

	private:

	glm::mat4 m_ProjectionMatrix{1.0f};
	glm::mat4 m_ViewMatrix{1.0f};

	};
} // end namespace SE