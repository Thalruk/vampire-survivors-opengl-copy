#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <glm/glm.hpp>

#include "../Graphics/Shader.h"
#include "../Graphics/SpriteRenderer.h"
#include "../Entities/Player.h"
#include "../Entities/Enemy.h"
#include "../Entities/Projectile.h"
#include "../Entities/Pickup.h"

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
    float SpawnInterval = 2.0f;  // Co ile sekund respiæ wroga
    float TotalGameTime = 0.0f; 
    bool IsGameOver;        
    float GameOverTimer;    
    float PlayerHitTimer;

    std::vector<Projectile*> Projectiles;
    float AttackTimer = 0.0f;
    float AttackInterval = 0.50f;   

    float MultiShotSpreadDeg = 10.0f;   // rozrzut w stopniach
    float ProjectileSpeed = 8.0f;
    float ProjectileLife = 2.0f;
    int ProjectileDamage = 1;
    glm::vec2 ProjectileSize = glm::vec2(0.35f, 0.35f);

    //Multi-shot power-up
    bool HasMultiShot = false;
    float MultiShotTimeLeft = 0.0f; // sekundy
    float MultiShotDuration = 8.0f; // ile trwa po podniesieniu
    int BaseShotCount = 1;          // domyœlna iloœæ pocisków
    int PowerShotCount = 3;         // power-up 3 pociski

    std::vector<Pickup*> Pickups;
    float PickupSpawnTimer = 0.0f;
    float PickupSpawnInterval = 10.0f;  // co ile sekund


    glm::mat4 BaseProjection = glm::mat4(1.0f);
    glm::vec2 CameraPos = glm::vec2(0.0f);

    float Zoom = 10.0f;

    // granice mapy (spawn jest mniej wiêcej -12..+12)
    glm::vec2 WorldMin = glm::vec2(-12.0f, -12.0f);
    glm::vec2 WorldMax = glm::vec2(12.0f, 12.0f);

};