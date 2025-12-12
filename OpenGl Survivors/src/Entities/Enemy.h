
#pragma once
#include "GameObject.h"
#include "Player.h" 
#include "../Graphics/Texture2D.h"


enum class EnemyType {
    Normal,
    Tank
};
class Enemy : public GameObject {
public:
    float MoveSpeed;
    Texture2D Sprite;
    Player* Target; 
    int HP;
    int MaxHP;
    EnemyType Type;

    Enemy(Player* target, glm::vec2 position, EnemyType type);
    ~Enemy();

    void Update(GLFWwindow* window, float dt) override;
    void Draw(SpriteRenderer& renderer) override;
};