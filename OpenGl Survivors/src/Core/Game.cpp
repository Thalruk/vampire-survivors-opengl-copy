#include "Game.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "ResourceManager.h"
#include "../Entities/Enemy.h"
#include <ctime> 
#include <iostream>

Game::Game(int width, int height, const char* title)
{
	this->Width = width;
	this->Height = height;
	this->Title = title;
	this->Window = nullptr;
	this->MainShader = nullptr;
	this->MainPlayer = nullptr;
	this->Renderer = nullptr;       

	this->IsGameOver = false;       
	this->GameOverTimer = 0.0f;     
	this->PlayerHitTimer = 0.0f;    
	this->TotalGameTime = 0.0f;
	this->SpawnTimer = 0.0f;
}

Game::~Game()
{
	delete(MainShader);
	delete(MainPlayer);
	glfwTerminate();

	// Sprz¹tanie wrogów
	for (Enemy* enemy : Enemies)
	{
		delete enemy;
	}
	Enemies.clear();
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

	glfwSetInputMode(Window, GLFW_STICKY_KEYS, GLFW_TRUE);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return false;
	}

	srand(static_cast<unsigned int>(time(NULL)));

	ResourceManager::LoadTexture("Assets/Textures/face.png", true, "face");
	ResourceManager::LoadTexture("Assets/Textures/enemy.png", true, "enemy");

	this->MainShader = new Shader("assets/shaders/default.vert", "assets/shaders/default.frag");
	this->Renderer = new SpriteRenderer(*MainShader);
	this->MainPlayer = new Player();

	this->TotalGameTime = 0.0f;
	this->IsGameOver = false;
	this->GameOverTimer = 0.0f;
	this->PlayerHitTimer = 0.0f;
	this->SpawnTimer = 0.0f;

	float zoom = 10.0f;
	float aspectRatio = (float)Width / (float)Height;

	glm::mat4 projection = glm::ortho(-zoom * aspectRatio, zoom * aspectRatio, -zoom, zoom, -1.0f, 1.0f);

	MainShader->Use();
	glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(projection));

	return true;
}

void Game::Run()
{
	float deltaTime = 0.0f;
	float lastFrame = glfwGetTime();

	while (!glfwWindowShouldClose(Window))
	{
		glfwPollEvents();

		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		if (deltaTime > 0.05f) deltaTime = 0.05f;

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
	// --- 1. GAME OVER ---
	if (IsGameOver)
	{
		GameOverTimer += dt;
		if (GameOverTimer >= 3.0f) ResetGame();
		return;
	}

	// --- 2. AKTUALIZACJA GRACZA I CZASU ---
	if (MainPlayer != nullptr) MainPlayer->Update(Window, dt);

	TotalGameTime += dt;

	// Timer nietykalnoœci
	if (PlayerHitTimer > 0.0f)
	{
		PlayerHitTimer -= dt;
		if (PlayerHitTimer < 0.8f) MainPlayer->Color = glm::vec3(1.0f);
	}

	// --- 3. SYSTEM FAL I SPAWNER (CYKLICZNY) ---
	float currentInterval = 2.0f;
	int enemiesToSpawn = 1;

	// Obliczamy sekundê w bie¿¹cej minucie (0-59)
	int secondsInMinute = (int)TotalGameTime % 60;

	// ZMIANA: Rój w³¹cza siê, gdy sekunda jest miêdzy 50 a 55 (w ka¿dej minucie)
	bool isSwarmEvent = (secondsInMinute >= 50 && secondsInMinute <= 55);

	if (isSwarmEvent)
	{
		currentInterval = 0.2f; // RÓJ: Bardzo szybki spawn (co 0.2s)
		enemiesToSpawn = 1;
	}
	else
	{
		// Tryb normalny: Trudniej z ka¿d¹ minut¹
		// Dzielimy ca³kowity czas przez 30s, ¿eby poziom rós³
		int difficultyLevel = (int)(TotalGameTime / 30.0f);
		enemiesToSpawn = 1 + difficultyLevel;
		currentInterval = 2.0f;
	}

	SpawnTimer += dt;

	if (SpawnTimer >= currentInterval)
	{
		SpawnTimer = 0.0f;

		for (int i = 0; i < enemiesToSpawn; i++)
		{
			glm::vec2 spawnPosition;
			float safeDistance = 5.0f;
			bool positionFound = false;

			// Szukanie bezpiecznej pozycji
			for (int attempt = 0; attempt < 10; attempt++)
			{
				float randomX = (rand() % 24) - 12.0f;
				float randomY = (rand() % 24) - 12.0f;
				glm::vec2 candidate = glm::vec2(randomX, randomY);

				if (glm::distance(candidate, MainPlayer->Position) > safeDistance)
				{
					spawnPosition = candidate;
					positionFound = true;
					break;
				}
			}

			if (positionFound)
			{
				EnemyType typeToSpawn = EnemyType::Normal;
				if (rand() % 100 < 20) typeToSpawn = EnemyType::Tank; // 20% na tanka

				Enemies.push_back(new Enemy(MainPlayer, spawnPosition, typeToSpawn));
			}
		}
	}

	// --- 4. RUCH WROGÓW I KOLIZJE MIÊDZY NIMI ---
	for (Enemy* enemy : Enemies)
	{
		enemy->Update(Window, dt);
	}

	for (size_t i = 0; i < Enemies.size(); i++)
	{
		for (size_t j = i + 1; j < Enemies.size(); j++)
		{
			Enemy* e1 = Enemies[i];
			Enemy* e2 = Enemies[j];

			glm::vec2 diff = e1->Position - e2->Position;
			float dist = glm::length(diff);
			float minDistance = 0.8f;

			if (dist < minDistance && dist > 0.001f)
			{
				float overlap = minDistance - dist;
				glm::vec2 pushDir = glm::normalize(diff);
				glm::vec2 pushVector = pushDir * (overlap * 0.5f);

				e1->Position += pushVector;
				e2->Position -= pushVector;
			}
		}
	}

	// --- 5. KOLIZJE GRACZ vs WROGOWIE ---
	for (Enemy* enemy : Enemies)
	{
		glm::vec2 direction = MainPlayer->Position - enemy->Position;
		float dist = glm::length(direction);
		float hitRadius = 0.9f;

		if (dist < hitRadius && dist > 0.001f)
		{
			// FIZYKA
			float overlap = hitRadius - dist;
			glm::vec2 pushDir = glm::normalize(direction);
			float pushForce = 5.0f;
			MainPlayer->Position += pushDir * overlap * pushForce;

			// WALKA
			if (PlayerHitTimer <= 0.0f)
			{
				MainPlayer->HP -= 1;
				PlayerHitTimer = 1.0f;
				MainPlayer->Color = glm::vec3(1.0f, 1.0f, 0.0f);
				std::cout << "AUA! HP: " << MainPlayer->HP << std::endl;

				if (MainPlayer->HP <= 0)
				{
					IsGameOver = true;
					GameOverTimer = 0.0f;
					std::cout << "KONIEC GRY!" << std::endl;
				}
			}
		}
	}
}

void Game::Render()
{
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	if (MainPlayer != nullptr && MainShader != nullptr) MainPlayer->Draw(*Renderer);
	if (MainShader != nullptr)
	{
		for (Enemy* enemy : Enemies) enemy->Draw(*Renderer);
	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void Game::ResetGame()
{
	for (Enemy* enemy : Enemies) delete enemy;
	Enemies.clear();

	if (MainPlayer != nullptr)
	{
		MainPlayer->Position = glm::vec2(0.0f, 0.0f);
		MainPlayer->HP = 5;
		MainPlayer->Color = glm::vec3(1.0f);
	}

	SpawnTimer = 0.0f;
	TotalGameTime = 0.0f;
	GameOverTimer = 0.0f;
	PlayerHitTimer = 0.0f;
	IsGameOver = false;

	std::cout << "GRA ZRESETOWANA!" << std::endl;
}