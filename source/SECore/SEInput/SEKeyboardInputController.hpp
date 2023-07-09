#pragma once

#include "SECore/SEEntities/SEGameObject.hpp"
#include "SERendering/SEWindow/SEWindow.hpp"

namespace SE
{

class SEKeyboardInputController
{
public:

	struct FKeyMappings {

		// Translation
		uint16_t MoveLeft = GLFW_KEY_A;
		uint16_t MoveRight = GLFW_KEY_D;
		uint16_t MoveForward = GLFW_KEY_W;
		uint16_t MoveBackward = GLFW_KEY_S;
		uint16_t MoveUp = GLFW_KEY_E;
		uint16_t MoveDown = GLFW_KEY_Q;

		// Rotation
		uint16_t RotateLeft = GLFW_KEY_LEFT;
		uint16_t RotateRight = GLFW_KEY_RIGHT;
		uint16_t RotateUp = GLFW_KEY_UP;
		uint16_t RotateDown = GLFW_KEY_DOWN;
	};

	FKeyMappings m_KeyMappings;
	float m_MovementSpeed = 3.0f;
	float m_RotationSpeed = 1.5f;

#pragma region Lifecycle
	SEKeyboardInputController();
	~SEKeyboardInputController();
#pragma endregion Lifecycle

	// void update();

	// Movement
	void move_in_xz_plane(GLFWwindow* window, SEGameObject& gameObject, float deltaTime);

private:

	void rotate_in_xz_plane(GLFWwindow* window, SEGameObject& gameObject, float deltaTime);
	void translate_in_xz_plane(GLFWwindow* window, SEGameObject& gameObject, float deltaTime);

};
}