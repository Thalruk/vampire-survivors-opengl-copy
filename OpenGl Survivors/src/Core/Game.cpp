#include "Game.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "ResourceManager.h"

Game::Game(int width, int height, const char* title)
{
	this->Width = width;
	this->Height = height;
	this->Title = title;
	this->Window = nullptr;
	this->MainShader = nullptr;
	this->MainPlayer = nullptr;
}

Game::~Game()
{
	delete(MainShader);
	delete(MainPlayer);
	glfwTerminate();
}

bool Game::Init()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	this->Window = glfwCreateWindow(Width, Height, Title, NULL, NULL);
	if (Window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return false;
	}

	glfwMakeContextCurrent(Window);
	glfwSetFramebufferSizeCallback(Window, framebuffer_size_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return false;
	}

	this->MainShader = new Shader("assets/shaders/default.vert", "assets/shaders/default.frag");
	this->MainPlayer = new Player();


	float zoom = 10.0f;
	float aspectRatio = (float)Width / (float)Height;

	glm::mat4 projection = glm::ortho(-zoom * aspectRatio, zoom * aspectRatio, -zoom, zoom, -1.0f, 1.0f);

	MainShader->Use();
	glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(projection));

	ResourceManager::LoadTexture("Assets/Textures/face.png", true, "face");
	return true;
}

void Game::Run()
{
	float deltaTime = 0.0f;
	float lastFrame = 0.0f;

	while (!glfwWindowShouldClose(Window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		ProcessInput(deltaTime);
		Update(deltaTime);
		Render(); 

		glfwSwapBuffers(Window);
		glfwPollEvents();
	}
}

void Game::ProcessInput(float dt)
{
	if (glfwGetKey(Window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(Window, true);
}

void Game::Update(float dt)
{
	if (MainPlayer != nullptr) 
	{
		MainPlayer->Update(Window, dt);
	}
}

void Game::Render()
{
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	if (MainPlayer != nullptr && MainShader != nullptr) {
		MainPlayer->Draw(*MainShader);
	}
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}