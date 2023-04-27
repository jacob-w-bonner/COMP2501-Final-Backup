#include "obstacle_game_object.h"
#include <glm/gtc/matrix_transform.hpp>

namespace game {

	/*
		ObstacleGameObject inherits from GameObject
		It overrides GameObject's update method, so that you can check for input to change the velocity of the player
	*/

	ObstacleGameObject::ObstacleGameObject(const glm::vec3& position, Geometry* geom, Shader* shader, GLuint texture, int t)
		: GameObject(position, geom, shader, texture) {
		hostile_ = false;
		type = t;
		velocity_ = glm::vec3(0.0f, -0.1f, 0.0f);
		if (type == 0) {
			mass = 2;
			health = 3;
		}
	}

	// Update function for moving the player object around
	void ObstacleGameObject::Update(double delta_time) {

		// Special player updates go here
		if (type == 0) {
			SetRotate(glm::rotate(GetRotate(), glm::radians(2.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
		}

		// Call the parent's update method to move the object in standard way, if desired
		GameObject::Update(delta_time);
	}

} // namespace game