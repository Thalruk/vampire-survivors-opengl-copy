#include "Projectile.h"
#include "../Graphics/SpriteRenderer.h"

Projectile::Projectile(glm::vec2 pos, glm::vec2 size, glm::vec2 vel, float lifeTime, int dmg, Texture2D sprite) 
{
	Position = pos;
	Size = size;
	Velocity = vel;
	LifeTime = lifeTime;
	Damage = dmg;
	Sprite = sprite;

	Color = glm::vec3(1.0f);
	Rotation = 0.0f;
	Destroyed = false;
}

void Projectile::Update(GLFWwindow* /*window*/, float dt)
{
	Position += Velocity * dt;

	LifeTime -= dt;
	if (LifeTime <= 0.0f)
		Destroyed = true;
}

void Projectile::Draw(SpriteRenderer& renderer) 
{
	renderer.DrawSprite(Sprite, Position, Size, Rotation, Color);
}
