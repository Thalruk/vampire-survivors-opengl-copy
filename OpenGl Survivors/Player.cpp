#include "Player.h"

Player::Player()
{
    this->PosX = 0.0f;
    this->PosY = 0.0f;
    this->Speed = 1.0f;
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
        PosX -= Speed * dt;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        PosX += Speed * dt;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        PosY += Speed * dt;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        PosY -= Speed * dt;
}

void Player::Draw(Shader& shader)
{
    shader.Use();

    int moveLoc = glGetUniformLocation(shader.ID, "uMove");
    glUniform2f(moveLoc, PosX, PosY);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0); 
}

void Player::initRenderData()
{
    float vertices[] = {
         0.5f, 0.5f, 0.0f,  // Prawy górny
         0.5f, -0.5f, 0.0f,  // Prawy dolny
        -0.5f, -0.5f, 0.0f,  // Lewy dolny
        -0.5f,  0.5f, 0.0f   // Lewy górny
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

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
