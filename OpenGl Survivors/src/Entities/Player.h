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

    void Update(GLFWwindow* window, float dt) override;

    void Draw(Shader& shader) override;

private:
    unsigned int VAO, VBO, EBO;
    void initRenderData(); 
};