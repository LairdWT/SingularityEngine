#include "SEKeyboardInputController.hpp"

namespace SE 
{

SEKeyboardInputController::SEKeyboardInputController()
{

}

SEKeyboardInputController::~SEKeyboardInputController()
{

}

void SEKeyboardInputController::move_in_xz_plane(GLFWwindow* window, SEGameObject& gameObject, float deltaTime)
{
	rotate_in_xz_plane(window, gameObject, deltaTime);
	translate_in_xz_plane(window, gameObject, deltaTime);
}

void SEKeyboardInputController::rotate_in_xz_plane(GLFWwindow* window, SEGameObject& gameObject, float deltaTime)
{
	glm::vec3 rotation{0.0f};

	rotation.y += static_cast<float>(glfwGetKey(window, m_KeyMappings.RotateRight) == GLFW_PRESS);
	rotation.y -= static_cast<float>(glfwGetKey(window, m_KeyMappings.RotateLeft) == GLFW_PRESS);
	rotation.x += static_cast<float>(glfwGetKey(window, m_KeyMappings.RotateUp) == GLFW_PRESS);
	rotation.x -= static_cast<float>(glfwGetKey(window, m_KeyMappings.RotateDown) == GLFW_PRESS);

	if (glm::dot(rotation, rotation) > std::numeric_limits<float>::epsilon())
	{
		rotation = m_RotationSpeed * deltaTime * glm::normalize(rotation);
		gameObject.rotate_using_delta(rotation);
	}
}

void SEKeyboardInputController::translate_in_xz_plane(GLFWwindow* window, SEGameObject& gameObject, float deltaTime)
{
	const glm::vec3 forwardVector = gameObject.get_forward_vector();
	const glm::vec3 rightVector = gameObject.get_right_vector();
	const glm::vec3 upVector{0.0f, -1.0f, 0.0f};

	glm::vec3 movementVector{0.0f};
	movementVector += forwardVector * static_cast<float>(glfwGetKey(window, m_KeyMappings.MoveForward) == GLFW_PRESS);
	movementVector -= forwardVector * static_cast<float>(glfwGetKey(window, m_KeyMappings.MoveBackward) == GLFW_PRESS);
	movementVector += rightVector * static_cast<float>(glfwGetKey(window, m_KeyMappings.MoveRight) == GLFW_PRESS);
	movementVector -= rightVector * static_cast<float>(glfwGetKey(window, m_KeyMappings.MoveLeft) == GLFW_PRESS);
	movementVector += upVector * static_cast<float>(glfwGetKey(window, m_KeyMappings.MoveUp) == GLFW_PRESS);
	movementVector -= upVector * static_cast<float>(glfwGetKey(window, m_KeyMappings.MoveDown) == GLFW_PRESS);

	if (glm::dot(movementVector, movementVector) > std::numeric_limits<float>::epsilon())
	{
		movementVector = m_MovementSpeed * deltaTime * glm::normalize(movementVector);
		gameObject.translate_using_delta(movementVector);
	}
}

} // namespace SE