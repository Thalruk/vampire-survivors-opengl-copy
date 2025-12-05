#include "Player.h"
#include "../Core/ResourceManager.h"

Player::Player()
{
    this->Position = glm::vec2(0.0f, 0.0f);
    this->Size = glm::vec2(1.0f, 1.0f);
    this->Color = glm::vec3(1.0f, 1.0f, 1.0f);
    this->MoveSpeed = 5.0f;
    this->Sprite = ResourceManager::GetTexture("face");

    initRenderData(); 
}

Player::~Player()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
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

void Player::Draw(Shader& shader)
{
    shader.Use();

    glUniform2f(10, this->Position.x, this->Position.y);
    glUniform3f(11, this->Color.x, this->Color.y, this->Color.z);

    this->Sprite.Bind();

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Player::initRenderData()
{
    float vertices[] = {
         0.5f,  0.5f, 0.0f,   1.0f, 1.0f,
          0.5f, -0.5f, 0.0f,   1.0f, 0.0f,
          -0.5f, -0.5f, 0.0f,   0.0f, 0.0f,
          -0.5f,  0.5f, 0.0f,   0.0f, 1.0f
    };
    unsigned int indices[] = {
        0, 1, 3,
        1, 2, 3
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
