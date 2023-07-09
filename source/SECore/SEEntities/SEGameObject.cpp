#include "SEGameObject.hpp"

#include <limits>

namespace SE {

glm::vec3 SEGameObject::get_forward_vector() const
{
	return { static_cast<float>(sin(m_TransformComponent.Rotation.y)), 0.0f, static_cast<float>(cos(m_TransformComponent.Rotation.y)) };
}

glm::vec3 SEGameObject::get_right_vector() const
{
	glm::vec3 forwardVector = get_forward_vector();
	return { forwardVector.z, 0.0f, -forwardVector.x };
}

glm::vec3 SEGameObject::get_up_vector() const
{
	return glm::cross(get_right_vector(), get_forward_vector());
}

void SEGameObject::rotate_using_delta(const glm::vec3& rotationDelta)
{
	m_TransformComponent.Rotation += rotationDelta;
	m_TransformComponent.Rotation.x = glm::clamp(m_TransformComponent.Rotation.x, -1.5f, 1.5f);
	m_TransformComponent.Rotation.y = glm::mod(m_TransformComponent.Rotation.y, glm::two_pi<float>());
}

void SEGameObject::translate_using_delta(const glm::vec3& translationDelta)
{
	m_TransformComponent.Translation += translationDelta;
}

} // namespace SE