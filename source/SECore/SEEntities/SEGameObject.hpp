#pragma once

#include <cstdint>
#include <memory>

#include "SECore/SEComponents/SEMesh.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/euler_angles.hpp"

namespace SE {

	struct TransformComponent {
		glm::vec3 Translation{};
		glm::vec3 Scale{1.0f, 1.0f, 1.0f};
		glm::vec3 Rotation{};

		glm::mat4 get_transform_matrix() const 
		{
			const float c3 = glm::cos(Rotation.z);
			const float s3 = glm::sin(Rotation.z);
			const float c2 = glm::cos(Rotation.x);
			const float s2 = glm::sin(Rotation.x);
			const float c1 = glm::cos(Rotation.y);
			const float s1 = glm::sin(Rotation.y);

			return glm::mat4
			{
				{
					Scale.x* (c1* c3 + s1 * s2 * s3),
					Scale.x* (c2* s3),
					Scale.x* (c1* s2* s3 - c3 * s1),
					0.0f,
				},
				{
					Scale.y * (c3 * s1 * s2 - c1 * s3),
					Scale.y * (c2 * c3),
					Scale.y * (c1 * c3 * s2 + s1 * s3),
					0.0f,
				},
				{
					Scale.z * (c2 * s1),
					Scale.z * (-s2),
					Scale.z * (c1 * c2),
					0.0f,
				},
				{ Translation.x, Translation.y, Translation.z, 1.0f }
			};
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

	// Getters
	glm::vec3 get_forward_vector() const;
	glm::vec3 get_right_vector() const;
	glm::vec3 get_up_vector() const;

	// Movement
	void rotate_using_delta(const glm::vec3& rotationDelta);
	void translate_using_delta(const glm::vec3& translationDelta);

	std::shared_ptr<SEMesh> m_Mesh{};
	glm::vec3 m_Color{};
	TransformComponent m_TransformComponent{};

	private:
	id_type m_Id;

	};

} //namespace SE