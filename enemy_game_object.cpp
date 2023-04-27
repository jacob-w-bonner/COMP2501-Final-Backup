#include "enemy_game_object.h"
#include "math_object.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

namespace game {

	/*
		Enemyobject inherits from GameObject
	*/

	EnemyGameObject::EnemyGameObject(const glm::vec3& position, Geometry* geom, Shader* shader, GLuint texture)
		: GameObject(position, geom, shader, texture) {
		myHandler = NULL;
		initialBehaviour = Idle;
		currentBehaviour = initialBehaviour;
		proximitySensor = 1;
		leashRange = proximitySensor * 2;
		proximityBehaviour = Idle;
		centerPos = position;
		timeAlive = 0.0f;
		done = false;
		Initialize();
	}

	EnemyGameObject::EnemyGameObject(EnemyHandler* myNewHandler, const glm::vec3& position, Geometry* geom, Shader* shader, GLuint texture, float newSpeed, behaviourType startBehaviour, float startProximity, behaviourType proxBehaviour, bool doesShoot, float attackSpeed, glm::vec3 playerPosition, GLuint newBulletTexture, shotType newShot, bool twitter)
		: GameObject(position, geom, shader, texture) {
		myHandler = myNewHandler;
		initialBehaviour = startBehaviour;
		currentBehaviour = initialBehaviour;
		proximitySensor = startProximity;
		leashRange = proximitySensor * 2;
		proximityBehaviour = proxBehaviour;
		timeAlive = 0.0f;
		moveSpeed = newSpeed;
		doesAttack = doesShoot;
		attackTimer = 0;
		attackTimerMax = attackSpeed;
		playerPos = playerPosition;
		bulletTexture = newBulletTexture;
		myShotType = newShot;
		verified = twitter;
		done = false;
		Initialize();
	}


	void EnemyGameObject::Initialize()
	{
		centerPos = position_;
		switch (currentBehaviour)
		{
		case Idle:
			velocity_ = glm::vec3(0.0f, 0.0f, 0.0f);
			break;
		case Circle:
			break;
		case MoveLeft:
			velocity_ = glm::vec3(-moveSpeed, 0, 0);
			//rotate_[0][0] = 3.14 / 2;
			rotate_ = glm::rotate(GetRotate(), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			break;
		case MoveRight:
			velocity_ = glm::vec3(moveSpeed, 0, 0);
			//rotate_[0][0] = 3 * 3.14 / 2;
			rotate_ = glm::rotate(GetRotate(), glm::radians(270.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			break;
		case Chase:
			break;
		case Flee:
			break;
		case Charge:
			break;
		}
	}

	void EnemyGameObject::rotateBehaviour(int type=0) {

		// Updating rotate value
		rotate_ = glm::mat4(1.0f);
		//std::cout << glm::to_string(rotate_) << std::endl;
		// Getting the direction vector
		glm::vec3 dirVec;
		if (type == 0) {
			dirVec = position_ - GetRotation();
		}
		else if (type == 1 || type == 2) {
			dirVec = myHandler->GetPlayer()->GetPosition() - position_;
		}
		glm::vec3 axisVec = glm::vec3(1.0f, 0.0f, 0.0f);
		float dirLength = glm::length(dirVec);
		float axisLength = glm::length(axisVec);
		float theta = acos(glm::dot(axisVec, dirVec) / (axisLength * dirLength)) * 180 / 3.14159265358979323846;

		// Ensuring enemy turns in the right direction
		if (type == 0) {
			theta = rotAngle + 1.0f;
			if (rotAngle > theta) {
				rotAngle = theta;
				theta *= -1.0f;
			}
			else {
				rotAngle = theta;
			}
		}
		else if (type == 1 || type == 2) {
			theta += 90.0f;
			if (myHandler->GetPlayer()->GetPosition()[1] > position_[1]) {
				theta = 180 + theta;
			}
			else {
				theta *= -1.0f;
			}
		}
		
		SetRotate(glm::rotate(GetRotate(), theta, glm::vec3(0.0f, 0.0f, 1.0f)));
		//std::cout << glm::to_string(rotate_) << std::endl;
		//std::cout << glm::radians(theta) << std::endl;
	}

	void EnemyGameObject::setBehaviour(behaviourType newBehaviour)
	{
		currentBehaviour = newBehaviour;
	}

	float EnemyGameObject::getSpeedMultiplier()
	{
		switch (currentBehaviour)
		{
		case Circle:
			return 1.0f;
		case Idle:
			return 1.0f;
		case Chase:
			return 4.0f;
		case Flee:
			return 1.25f;
		case MoveLeft:
			return 1.0f;
		case MoveRight:
			return 1.0f;
		case Charge:
			return 4.0f;
		default:
			return 1.0f;
		}
	}


	// Update function for the enemy object
	void EnemyGameObject::Update(double delta_time) {
		float adjustedSpeed = moveSpeed * getSpeedMultiplier();
		timeAlive += delta_time;

		switch (currentBehaviour)
		{
		case Idle:
			velocity_ = glm::vec3(0.0f, -0.8f, 0.0f);
			//rotate_[0][0] = 3.14 / 2 + MathObject::getAngleToPoint(position_, playerPos);
			rotateBehaviour(0);
			break;
		case Circle:
			this->doesAttack = false;
			rotateBehaviour(0);
			position_ = centerPos + glm::vec3(cos(adjustedSpeed * timeAlive), sin(adjustedSpeed * timeAlive), 0);
			if (!done) {
				centerPos[1] -= 0.01f;
			}
			timeAlive += delta_time;
			//rotateBehaviour(0);
			break;
		case MoveLeft:
			position_ = glm::vec3(position_.x, centerPos.y + sin(adjustedSpeed * timeAlive), 0);
			//rotate_[0][0] = 3.14 / 2 + 0.7 * sin(timeAlive);
			rotateBehaviour(1);
			break;
		case MoveRight:
			position_ = glm::vec3(position_.x, centerPos.y - sin(adjustedSpeed * timeAlive), 0);
			//rotate_[0][0] = 3 * 3.14 / 2 + 0.7 * sin(timeAlive);
			rotateBehaviour(1);
			break;
		case Chase:
			velocity_ = MathObject::findVelocity(position_, playerPos, adjustedSpeed);
			//rotate_[0][0] = 3.14 / 2 + MathObject::getAngleToPoint(position_, playerPos);
			rotateBehaviour(1);
			break;
		case Charge:
			velocity_ = MathObject::findVelocity(position_, playerPos, adjustedSpeed);
			//rotate_[0][0] = 3.14 / 2 + MathObject::getAngleToPoint(position_, playerPos);
			rotateBehaviour(1);
			break;
		case Flee:
			velocity_ = MathObject::findVelocity(position_, playerPos, adjustedSpeed);
			//rotate_[0][0] = 3 * 3.14 / 2 + MathObject::getAngleToPoint(position_, playerPos);
			this->doesAttack = true;
			rotateBehaviour(2);
			velocity_ = velocity_ *= -1;
			break;
		case Tes:
			if (position_[1] > 0.0f) {
				position_[1] -= 0.01f;
			}
			else {
				position_[1] = 0.0f;
			}
			//std::cout << position_[1] << std::endl;
			rotateBehaviour(0);
			break;
		}

		handleProximity();

		handleAttack(delta_time);

		//handleCollision is called by the EnemyHandler

		// Setting velocity to 0 if game is over
		if (done) {
			velocity_ = glm::vec3(0.0f, 0.0f, 0.0f);
		}
		
		// Call the parent's update method
		GameObject::Update(delta_time);

	}

	void EnemyGameObject::Render(glm::mat4 view_matrix, double current_time)
	{
		GameObject::Render(view_matrix, current_time);
	}

	void EnemyGameObject::handleProximity()
	{
		if (proximitySensor != -1)
		{
			float currentDistance = glm::length(position_ - playerPos);
			if (currentDistance < proximitySensor)
			{
				currentBehaviour = proximityBehaviour;
				if (currentBehaviour != initialBehaviour)
				{

				}
			}
			else if (currentDistance > leashRange)
			{
				if (currentBehaviour != initialBehaviour)
				{
					currentBehaviour = initialBehaviour;
					if (currentBehaviour == Circle)
					{
						centerPos = position_;
					}
				}
			}

		}
	}

	bool EnemyGameObject::handleCollision(glm::vec3 targetPos, glm::vec3 otherScale)
	{
		if (abs(position_.x - targetPos.x) < 0.75f * scale_.x * otherScale.x)
		{
			if (abs(position_.y - targetPos.y) < 0.75f * scale_.y * otherScale.y)
			{
				return true;
			}
		}
		return false;
	}

	void EnemyGameObject::handleAttack(float deltatime)
	{
		if (doesAttack)
		{
			attackTimer -= deltatime;
			if (attackTimer <= 0)
			{
				shoot();
				attackTimer = attackTimerMax;
			}
		}
	}

	void EnemyGameObject::shoot()
	{

		EnemyBulletObject* newBullet;
		float targetAngle = 0;
		float speed = 0;
		if (playerPos.y > position_.y)
		{
			targetAngle = 3.14 / 2;
		}
		else
		{
			targetAngle = 3 * 3.14 / 2;
		}
		switch (myShotType)
		{
		case Single:
			speed = 4.0f;
			newBullet = new EnemyBulletObject(position_, geometry_, shader_, bulletTexture, targetAngle, speed, 5.0f - (speed / 2));
			newBullet->SetScale(glm::vec3(0.25f, 0.25f, 0.0f));
			myHandler->addBulletToList(newBullet);
			break;
		case SingleTrack:
			speed = 4.0f;
			newBullet = new EnemyBulletObject(position_, geometry_, shader_, bulletTexture, MathObject::getAngleToPoint(playerPos, position_), speed, 5.0f - (speed / 2));
			newBullet->SetScale(glm::vec3(0.25f, 0.25f, 0.0f));
			myHandler->addBulletToList(newBullet);
			break;
		case Tri:
			speed = 3.0f;
			for (int i = 0;i < 3;i++)
			{
				newBullet = new EnemyBulletObject(position_, geometry_, shader_, bulletTexture, targetAngle - (3.14 / 6) + (i * 3.14 / 6), speed, 4.0f - (speed / 2));
				newBullet->SetScale(glm::vec3(0.25f, 0.25f, 0.0f));
				myHandler->addBulletToList(newBullet);
			}
			break;
		case Quad:
			speed = 3.0f;
			for (int i = 0;i < 4;i++)
			{
				newBullet = new EnemyBulletObject(position_, geometry_, shader_, bulletTexture, targetAngle - (1.5 * 3.14 / 6) + (i * 3.14 / 6), speed, 4.0f - (speed / 2));
				newBullet->SetScale(glm::vec3(0.25f, 0.25f, 0.0f));
				myHandler->addBulletToList(newBullet);
			}
			break;
		case Burst:
			speed = 4.0f;
			for (int i = 0;i < 8;i++)
			{
				newBullet = new EnemyBulletObject(position_, geometry_, shader_, bulletTexture, targetAngle - (3 * 3.14 / 12) + (i * 3.14 / 12), speed, 4.5f - speed);
				newBullet->SetScale(glm::vec3(0.25f, 0.25f, 0.0f));
				myHandler->addBulletToList(newBullet);
			}
		default:
			break;
		}
	}

	bool EnemyGameObject::checkIsOffScreen()
	{
		if (currentBehaviour != Tes) {
			if (position_.x < -7 || position_.x > 7)
			{
				return true;
			}
			else if (position_.y < -7 || position_.y > 7) {
				return true;
			}
			return false;
		}
	}

} // namespace game