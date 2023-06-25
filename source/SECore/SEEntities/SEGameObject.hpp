#pragma once

#include <cstdint>
#include <memory>

#include "SEMesh.hpp"

namespace SE {

	struct Transform2dComponent {
		glm::vec2 Translation{};
		glm::vec2 Scale{1.0f, 1.0f};
		float Rotation;

		glm::mat2 get_mat2() 
		{	
			const float s = glm::sin(Rotation);
			const float c = glm::cos(Rotation);
			glm::mat2 RotationMatrix{{c, s}, {-s, c}};

			glm::mat2 ScaleMatrix{{Scale.x, 0.0f}, { 0.0f, Scale.y }};
			return RotationMatrix * ScaleMatrix;
		}
	};


	class SEGameObject {

	public:
	using id_type = uint32_t;

#pragma region Lifecycle
	SEGameObject(id_type ObjectID) : m_Id(ObjectID) {}

	// SEGameObject(const SEGameObject&) = delete;
	// SEGameObject& operator=(const SEGameObject&) = delete;
	// SEGameObject(SEGameObject&&) = delete;
	// SEGameObject& operator=(SEGameObject&&) = delete;
#pragma endregion Lifecycle

	static SEGameObject create_game_object() { static id_type id = 0; return SEGameObject(id++); }

	uint32_t get_game_object_id() const { return m_Id; }


	std::shared_ptr<SEMesh> m_Mesh{};
	glm::vec3 m_Color{};
	Transform2dComponent m_Transform2d{};

	private:
	id_type m_Id;

	};

} //namespace SE