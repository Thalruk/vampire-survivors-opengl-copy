#pragma once 
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Shader.h" 

class Player {
public:
    float PosX, PosY;
    float Speed;

    Player();

    ~Player();

    void Update(GLFWwindow* window, float dt);

    void Draw(Shader& shader);

private:
    unsigned int VAO, VBO, EBO;
    void initRenderData(); 
};