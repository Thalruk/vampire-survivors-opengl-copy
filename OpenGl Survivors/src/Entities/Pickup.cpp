#include "Pickup.h"
#include "../Graphics/SpriteRenderer.h"

Pickup::Pickup(glm::vec2 pos, glm::vec2 size, PickupType type, Texture2D sprite)
{
    Position = pos;
    Size = size;
    Type = type;
    Sprite = sprite;

    Color = glm::vec3(1.0f);
    Rotation = 0.0f;
    Destroyed = false;
    Collected = false;
}

void Pickup::Update(GLFWwindow* /*window*/, float /*dt*/)
{
    // na razie brak logiki - pickup siê nie rusza
}

void Pickup::Draw(SpriteRenderer& renderer)
{
    renderer.DrawSprite(Sprite, Position, Size, Rotation, Color);
}
