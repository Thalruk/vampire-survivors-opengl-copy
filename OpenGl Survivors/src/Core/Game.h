#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>

#include "../Graphics/Shader.h"
#include "../Graphics/SpriteRenderer.h"
#include "../Entities/Player.h"
#include "../Entities/Enemy.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

class Game {
public:
    Game(int width, int height, const char* title);

    ~Game();

    bool Init();

    void Run();
    void ResetGame();
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

    std::vector<Enemy*> Enemies; 
    float SpawnTimer = 0.0f;     
    float SpawnInterval = 2.0f;  // Co ile sekund respiæ wroga?
    float TotalGameTime = 0.0f; 
    bool IsGameOver;        
    float GameOverTimer;    
    float PlayerHitTimer;
};