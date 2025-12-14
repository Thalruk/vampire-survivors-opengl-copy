#include "Game.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "ResourceManager.h"
#include "../Entities/Enemy.h"
#include "../Entities/Pickup.h"
#include <ctime> 
#include <iostream>
#include <algorithm>
#include <cfloat> //FLT_MAX
#include <cmath>

static bool CheckAABB_Center(const glm::vec2& aPos, const glm::vec2& aSize, 
							 const glm::vec2& bPos, const glm::vec2& bSize)
{
	glm::vec2 aHalf = aSize * 0.5f;
	glm::vec2 bHalf = bSize * 0.5f;

	glm::vec2 aMin = aPos - aHalf;
	glm::vec2 aMax = aPos + aHalf;
	glm::vec2 bMin = bPos - bHalf;
	glm::vec2 bMax = bPos + bHalf;

	bool overlapX = aMin.x <= bMax.x && aMax.x >= bMin.x;
	bool overlapY = aMin.y <= bMax.y && aMax.y >= bMin.y;
	return overlapX && overlapY;
}

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
	delete Renderer;
	Renderer = nullptr;
	delete(MainShader);
	delete(MainPlayer);

	for (Projectile* p : Projectiles) delete p;
	Projectiles.clear();

	// Sprz¹tanie wrogów
	for (Enemy* enemy : Enemies)
	{
		delete enemy;
	}
	Enemies.clear();

	for (Pickup* pu : Pickups) delete pu;
	Pickups.clear();

	if (Window) glfwDestroyWindow(Window);
	Window = nullptr;
	glfwTerminate();

}

bool Game::Init()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
		
	this->Window = glfwCreateWindow(Width, Height, Title, NULL, NULL);

	if (!this->Window)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return false;
	}

	glfwMaximizeWindow(this->Window);
	glfwMakeContextCurrent(Window);
	glfwSetFramebufferSizeCallback(Window, framebuffer_size_callback);

	glfwSetInputMode(Window, GLFW_STICKY_KEYS, GLFW_TRUE);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return false;
	}

	glDisable(GL_DEPTH_TEST); // 2D sprite game
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	int fbW = 0, fbH = 0;
	glfwGetFramebufferSize(Window, &fbW, &fbH);
	glViewport(0, 0, fbW, fbH);

	this->Width = fbW;
	this->Height = fbH;

	framebuffer_size_callback(Window, this->Width, this->Height);

	srand(static_cast<unsigned int>(time(NULL)));

	ResourceManager::LoadTexture("Assets/Textures/face.png", true, "face");
	ResourceManager::LoadTexture("Assets/Textures/enemy.png", true, "enemy");
	ResourceManager::LoadTexture("Assets/Textures/bullet.png", true, "bullet");
	ResourceManager::LoadTexture("Assets/Textures/pickup.png", true, "pickup");

	this->MainShader = new Shader("assets/shaders/default.vert", "assets/shaders/default.frag");
	this->Renderer = new SpriteRenderer(*MainShader);
	this->MainPlayer = new Player();

	this->TotalGameTime = 0.0f;
	this->IsGameOver = false;
	this->GameOverTimer = 0.0f;
	this->PlayerHitTimer = 0.0f;
	this->SpawnTimer = 0.0f;

	Zoom = 10.0f;
	float aspect = (float)Width / (float)Height;

	BaseProjection = glm::ortho(
		-Zoom * aspect, Zoom * aspect,
		-Zoom, Zoom,
		-1.0f, 1.0f
	);

	MainShader->Use();
	glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(BaseProjection));

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
	}
}

void Game::ProcessInput(float dt)
{
	if (glfwGetKey(Window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(Window, true);

	static bool pWasDown = false;
	bool pDown = glfwGetKey(Window, GLFW_KEY_P) == GLFW_PRESS;

	if (pDown && !pWasDown)
	{
		HasMultiShot = true;
		MultiShotTimeLeft = MultiShotDuration;
	}

	pWasDown = pDown;
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

	if (MainPlayer == nullptr)
		return;

	// --- 2. AKTUALIZACJA GRACZA I CZASU ---
	MainPlayer->Update(Window, dt);

	float aspect = (float)Width / (float)Height;
	float halfW = Zoom * aspect;
	float halfH = Zoom;

	// margines = po³owa rozmiaru sprite'a, ¿eby nie ucina³o na krawêdzi
	glm::vec2 halfSize = MainPlayer->Size * 0.5f;

	auto clampf = [](float v, float a, float b) {
		return (v < a) ? a : (v > b) ? b : v;
		};

	MainPlayer->Position.x = clampf(MainPlayer->Position.x, -halfW + halfSize.x, halfW - halfSize.x);
	MainPlayer->Position.y = clampf(MainPlayer->Position.y, -halfH + halfSize.y, halfH - halfSize.y);

	TotalGameTime += dt;

	if (HasMultiShot)
	{
		MultiShotTimeLeft -= dt;
		if (MultiShotTimeLeft <= 0.0f)
		{
			HasMultiShot = false;
			MultiShotTimeLeft = 0.0f;
			std::cout << "PICKUP: MultiShot OFF\n";
		}
	}

	// Timer nietykalnoœci
	if (PlayerHitTimer > 0.0f)
	{
		PlayerHitTimer -= dt;
		if (PlayerHitTimer < 0.8f) MainPlayer->Color = glm::vec3(1.0f);
	}

	// --- 2.5 SPAWN POCISKU CO AttackInterval ---
	AttackTimer -= dt;
	if (AttackTimer <= 0.0f)
	{

		Texture2D projTex = ResourceManager::GetTexture("bullet");

		Enemy* nearest = nullptr;
		float bestD2 = FLT_MAX;

		for (Enemy* e : Enemies)
		{
			if (!e) continue;
			glm::vec2 d = e->Position - MainPlayer->Position;
			float d2 = d.x * d.x + d.y * d.y;
			if (d2 < bestD2)
			{
				bestD2 = d2;
				nearest = e;
			}
		}

		if (nearest != nullptr)
		{
			glm::vec2 dir = nearest->Position - MainPlayer->Position;
			float len = glm::length(dir);
			dir = (len > 0.001f) ? (dir / len) : glm::vec2(1.0f, 0.0f);

			int count = HasMultiShot ? PowerShotCount : BaseShotCount;
			if (count < 1) count = 1;

			float spreadRad = glm::radians(MultiShotSpreadDeg);

			// dla 1 pocisku: offset = 0
			// dla 3 pocisków: offsety = -1, 0, +1
			int mid = count / 2;

			for (int i = 0; i < count; i++)
			{
				int k = i - mid;

				float angle = k * spreadRad;
				
				// obrót wektora dir w 2D o k¹t angle
				glm::vec2 rotatedDir;
				rotatedDir.x = dir.x * std::cos(angle) - dir.y * std::sin(angle);
				rotatedDir.y = dir.x * std::sin(angle) + dir.y * std::cos(angle);

				Projectiles.push_back(new Projectile(
					MainPlayer->Position,
					ProjectileSize,
					rotatedDir * ProjectileSpeed,
					ProjectileLife,
					ProjectileDamage,
					projTex
				));
			}

			AttackTimer = AttackInterval; // dopiero po strzale
		}
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
	
	// --- 3.5. SPAWN PICKUPÓW (tylko 1 naraz) ---
	bool hasActivePickup = std::any_of(Pickups.begin(), Pickups.end(),
		[](Pickup* p) { return p && !p->Collected; });

	if (!hasActivePickup)
	{
		PickupSpawnTimer += dt;
		if (PickupSpawnTimer >= PickupSpawnInterval)
		{
			PickupSpawnTimer = 0.0f;

			glm::vec2 pickupSize(0.8f, 0.8f);
			glm::vec2 pickupHalf = pickupSize * 0.5f;

			auto randf = [](float a, float b) {
				return a + (b - a) * (rand() / (float)RAND_MAX);
				};

			float randomX = randf(-halfW + pickupHalf.x, halfW - pickupHalf.x);
			float randomY = randf(-halfH + pickupHalf.y, halfH - pickupHalf.y);

			glm::vec2 spawnPos(randomX, randomY);

			if (glm::distance(spawnPos, MainPlayer->Position) < 2.0f)
			{
				// nie spawnuj teraz; spróbuj ponownie prawie od razu
				PickupSpawnTimer = PickupSpawnInterval - 0.1f;
			}
			else
			{
				Texture2D pickupTex = ResourceManager::GetTexture("pickup");
				Pickups.push_back(new Pickup(
					spawnPos,
					pickupSize,
					PickupType::MultiShot,
					pickupTex
				));
			}
		}
	}
	else
	{
		PickupSpawnTimer = 0.0f;
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
	for (Projectile* p : Projectiles)
	{
		if (!p || p->Destroyed) continue;

		p->Update(Window, dt);

		//kolizja pocisk -> wróg
		for (Enemy* e : Enemies)
		{
			if (!e) continue;

			if (CheckAABB_Center(p->Position, p->Size, e->Position, e->Size))
			{
				e->HP -= p->Damage;
				p->Destroyed = true;	// pocisk znika po trafieniu
				break;
			}
		}
	}

	Projectiles.erase(
		std::remove_if(Projectiles.begin(), Projectiles.end(),
			[](Projectile* p)
			{
				if (!p) return true;

				if (p->Destroyed)
				{
					delete p;
					return true;
				}
				return false;
			}),
		Projectiles.end());

	Enemies.erase(
		std::remove_if(Enemies.begin(), Enemies.end(),
			[](Enemy* e)
			{
				if (!e) return true;

				if (e->HP <= 0)
				{
					delete e;
					return true;
				}

				return false;
			}),
		Enemies.end());

	for (Pickup* pu : Pickups)
	{
		if (!pu || pu->Collected) continue;

		if (CheckAABB_Center(MainPlayer->Position, MainPlayer->Size, pu->Position, pu->Size))
		{
			pu->Collected = true;

			//efekt power-up
			HasMultiShot = true;
			MultiShotTimeLeft = MultiShotDuration;
			std::cout << "PICKUP: MultiShot ON (" << MultiShotDuration << "s)\n";
		}
	}
	Pickups.erase(
		std::remove_if(Pickups.begin(), Pickups.end(),
			[](Pickup* pu)
			{
				if (!pu) return true;
				if (pu->Collected)
				{
					delete pu;
					return true;
				}
				return false;
			}),
		Pickups.end());
}

void Game::Render()
{
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	if (!MainShader || !Renderer) return;

	// Sta³a projekcja
	MainShader->Use();
	glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(BaseProjection));

	if (MainPlayer) MainPlayer->Draw(*Renderer);

	for (Projectile* p : Projectiles)
		if (p && !p->Destroyed) p->Draw(*Renderer);

	for (Pickup* pu : Pickups)
		if (pu && !pu->Collected) pu->Draw(*Renderer);

	for (Enemy* enemy : Enemies)
		if (enemy) enemy->Draw(*Renderer);
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void Game::ResetGame()
{
	for (Pickup* pu : Pickups) delete pu;
	Pickups.clear();
	PickupSpawnTimer = 0.0f;

	for (Projectile* p : Projectiles) delete p;
	Projectiles.clear();

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