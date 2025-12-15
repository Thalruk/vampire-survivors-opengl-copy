#include "Player.h"
#include "../Core/ResourceManager.h"
#include "../Graphics/SpriteRenderer.h"

Player::Player()
{
    this->Position = glm::vec2(0.0f, 0.0f);
    this->Size = glm::vec2(1.0f, 1.0f);
    this->Color = glm::vec3(1.0f, 1.0f, 1.0f);
    this->MoveSpeed = 3.0f;
    this->Sprite = ResourceManager::GetTexture("face");
    this->HP = 5;
}

Player::~Player()
{
}


void Player::Update(GLFWwindow* window, float dt)
{
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        Position.x -= MoveSpeed * dt;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        Position.x += MoveSpeed * dt;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        Position.y += MoveSpeed * dt;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        Position.y -= MoveSpeed * dt;
}

void Player::Draw(SpriteRenderer& renderer)
{
    renderer.DrawSprite(this->Sprite, this->Position, this->Size, this->Rotation, this->Color);
}