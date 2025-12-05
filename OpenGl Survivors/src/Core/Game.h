#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "../Graphics/Shader.h"
#include "../Graphics/SpriteRenderer.h"
#include "../Entities/Player.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

class Game {
public:
    Game(int width, int height, const char* title);

    ~Game();

    bool Init();

    void Run();

private:
    int Width, Height;
    const char* Title;
    GLFWwindow* Window;
    SpriteRenderer* Renderer;
    Shader* MainShader;
    Player* MainPlayer;

    void ProcessInput(float dt);
    void Update(float dt);
    void Render();
};