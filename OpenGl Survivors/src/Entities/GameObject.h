#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include "../Graphics/Shader.h"

class GameObject
{
public:
    glm::vec2 Position;
    glm::vec2 Size;
    glm::vec3 Color;
    float Rotation;

    bool Destroyed;

    GameObject();
    GameObject(glm::vec2 pos, glm::vec2 size, glm::vec3 color = glm::vec3(1.0f));

    virtual ~GameObject();

    virtual void Update(GLFWwindow* window, float dt) = 0;
    virtual void Draw(Shader& shader) = 0;
};