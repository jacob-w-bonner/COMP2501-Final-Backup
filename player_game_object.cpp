#include <iostream>
#include "player_game_object.h"

namespace game {

/*
	PlayerGameObject inherits from GameObject
	It overrides GameObject's update method, so that you can check for input to change the velocity of the player
*/

PlayerGameObject::PlayerGameObject(const glm::vec3 &position, Geometry *geom, Shader *shader, GLuint texture, bool invulnerable_)
: GameObject(position, geom, shader, texture) {
	hostile_ = false;
	fuel = 10.0f;
	health = 10;
	mass = 2;
}

// Accounting for enemy collisions
void PlayerGameObject::registerCollision() {

	// Subtracting the appropriate health
	if (shield > 0) {
		shield -= 1;
	}
	else {
		health -= 1;
	}

	// Adjusting red colour
	hit += 0.05f;
}

// Update function for moving the player object around
void PlayerGameObject::Update(double delta_time) {

	// Draining player fuel
	if (fuel > 0.0f && !verified) {
		fuel -= 0.001;
	}
	//std::cout << fuel << std::endl;

	// Defining edges
	float leftEdge = -3.5f;
	float rightEdge = 3.5f;
	float topEdge = 3.5f;
	float bottomEdge = -3.5f;

	// Update object position with Euler integration
	position_ += velocity_ * ((float)delta_time);

	// Checking upper bound
	if (position_[1] > topEdge) {
		position_[1] = topEdge;
		//velocity_ = glm::vec3(0.0f, 0.0f, 0.0f);
		velocity_[1] = 0.0f;
	}

	// Checking lower bound
	if (position_[1] < bottomEdge) {
		position_[1] = bottomEdge;
		//velocity_ = glm::vec3(0.0f, 0.0f, 0.0f);
		velocity_[1] = 0.0f;
	}

	// Checking left bound
	if (position_[0] < leftEdge) {
		position_[0] = leftEdge;
		//velocity_ = glm::vec3(0.0f, 0.0f, 0.0f);
		velocity_[0] = 0.0f;
	}

	// Checking right bound
	if (position_[0] > rightEdge) {
		position_[0] = rightEdge;
		//velocity_ = glm::vec3(0.0f, 0.0f, 0.0f);
		velocity_[0] = 0.0f;
	}

}

} // namespace game
