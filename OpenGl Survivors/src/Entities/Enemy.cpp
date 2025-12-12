#include "Enemy.h"
#include "../Core/ResourceManager.h"
#include "../Graphics/SpriteRenderer.h"
#include <glm/glm.hpp>
#include "Player.h"


Enemy::Enemy(Player* target, glm::vec2 position, EnemyType type)
    : Target(target), Type(type) 
{
    this->Position = position;
    this->Sprite = ResourceManager::GetTexture("enemy"); // Wszyscy u¿ywaj¹ tej samej tekstury

    if (this->Type == EnemyType::Tank)
    {
        // TANK
        this->HP = 3;
        this->MaxHP = 3;
        this->MoveSpeed = 1.5f; 


        this->Size = glm::vec2(1.3f, 1.3f);

        this->Color = glm::vec3(0.4f, 0.6f, 0.4f);
    }
    else
    {
        // NORMALNY
        this->HP = 1;
        this->MaxHP = 1;
        this->MoveSpeed = 2.3f;
        this->Size = glm::vec2(1.0f, 1.0f);

        this->Color = glm::vec3(1.0f, 1.0f, 1.0f);
    }
}

Enemy::~Enemy() {}

void Enemy::Update(GLFWwindow* window, float dt)
{
    if (Target == nullptr) return;

    glm::vec2 direction = Target->Position - this->Position;
    if (glm::length(direction) > 0.0f)
        direction = glm::normalize(direction);

    this->Position += direction * this->MoveSpeed * dt;
}

void Enemy::Draw(SpriteRenderer& renderer)
{
    renderer.DrawSprite(this->Sprite, this->Position, this->Size, this->Rotation, this->Color);
}