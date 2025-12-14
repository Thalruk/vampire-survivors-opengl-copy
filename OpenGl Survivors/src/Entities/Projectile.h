#pragma once
#include "GameObject.h"
#include "../Graphics/Texture2D.h"

class Projectile : public GameObject
{
public:
	glm::vec2 Velocity;
	float LifeTime;
	int Damage;
	Texture2D Sprite;

	Projectile(glm::vec2 pos, glm::vec2 size, glm::vec2 vel, float lifeTime, int dmg, Texture2D sprite);

	void Update(GLFWwindow* window, float dt) override;
	void Draw(SpriteRenderer& renderer) override;
};
