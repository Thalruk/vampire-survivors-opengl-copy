#pragma once
#include "GameObject.h"
#include "../Graphics/Texture2D.h"

enum class PickupType
{
    MultiShot
};

class Pickup : public GameObject
{
public:
    PickupType Type;
    Texture2D Sprite;
    bool Collected = false;

    Pickup(glm::vec2 pos, glm::vec2 size, PickupType type, Texture2D sprite);

    void Update(GLFWwindow* window, float dt) override;
    void Draw(SpriteRenderer& renderer) override;
};
