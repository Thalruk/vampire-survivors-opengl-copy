#pragma once 
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "GameObject.h"
#include "../Graphics/Texture2D.h"

class Player : public GameObject{
public:
    float MoveSpeed;
    Texture2D Sprite;
    Player();

    ~Player();
    int HP;
    void Update(GLFWwindow* window, float dt) override;

    void Draw(SpriteRenderer& renderer) override;
};