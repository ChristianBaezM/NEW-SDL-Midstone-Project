#include "Scene1.h"
#include <VMath.h>
#include "GridSettings.h"
#include <string>
#include <iomanip>
#include <sstream>
#include <iostream> // Make sure to include iostream for std::cout
#include <cstdlib>

// See notes about this constructor in Scene1.h.
Scene1::Scene1(SDL_Window* sdlWindow_, GameManager* game_) {
	window = sdlWindow_;
	game = game_;
	renderer = SDL_GetRenderer(window);
	xAxis = 25.0f;
	yAxis = 15.0f;
	showGrid = true;
}

Scene1::~Scene1() {
}

bool Scene1::OnCreate() {
	srand(time(0)); //this might cause a bug wiht the srandbellow!
	srand(static_cast<unsigned>(time(NULL)));
	int w;
	int h;
	SDL_GetWindowSize(window, &w, &h);

	Matrix4 ndc = MMath::viewportNDC(w, h);
	Matrix4 ortho = MMath::orthographic(0.0f, xAxis, 0.0f, yAxis, 0.0f, 1.0f);
	projectionMatrix = ndc * ortho;

	/// Turn on the SDL imaging subsystem
	IMG_Init(IMG_INIT_PNG);

	SDL_Surface* image;
	SDL_Texture* texture;

	// Road
	int laneCount = 4;
	int startLane = 8;

	carLaneBackgroundRect.x = 0;
	carLaneBackgroundRect.y = startLane * CELL_SIZE;
	carLaneBackgroundRect.w = w;    // full window width
	carLaneBackgroundRect.h = laneCount * CELL_SIZE;

	SDL_Surface* bgSurface = IMG_Load("road.png");
	if (!bgSurface) {
		std::cout << "Failed to load background image: " << IMG_GetError() << std::endl;
	}
	else {
		backgroundTexture = SDL_CreateTextureFromSurface(renderer, bgSurface);
		SDL_FreeSurface(bgSurface);
	}
	// River
	int riverLaneCount = 4;
	int riverStartLane = 3;

	riverBackgroundRect.x = 0;
	riverBackgroundRect.y = riverStartLane * CELL_SIZE;
	riverBackgroundRect.w = w;
	riverBackgroundRect.h = riverLaneCount * CELL_SIZE;

	SDL_Surface* riverSurface = IMG_Load("river.png");
	if (riverSurface) {
		riverTexture = SDL_CreateTextureFromSurface(renderer, riverSurface);
		SDL_FreeSurface(riverSurface);
	}

	// Player
	image = IMG_Load("raccoon.png");
	texture = SDL_CreateTextureFromSurface(renderer, image);
	game->getPlayerBody()->setImage(image);
	game->getPlayerBody()->setTexture(texture);

	// Cars
	const char* carImages[] = { "carRed.png", "carBlue.png" };
	int lanePositions[] = { 11, 10,9, 8 }; // Example lanes for more cars
	int numCars = 4; // Number of cars you want to create
	float carScale = 0.10f; // Scale to match player size

	for (int i = 0; i < numCars; i++) {
		const char* imagePath = carImages[i % 2];  // Cycle between two images
		int lane = lanePositions[i];
		float speed = 150.0f + i * 20;	// Varying speed
		bool moveRight = (i % 2 == 0);	// Alternate direction

		cars.push_back(new Car(renderer, imagePath, 0, lane * CELL_SIZE, speed, moveRight, carScale));
	}

	// Logs
	int logLanePositions[] = { 6, 5, 4, 3 };
	int numLogs = 4;
	float logScale = 2.0f;

	for (int i = 0; i < numLogs; i++) {
		int lane = logLanePositions[i];
		float speed = 100.0f + i * 15;
		bool moveRight = (i % 2 == 0);

		logs.push_back(new Log(renderer, "log.png", 0, lane * CELL_SIZE, speed, moveRight, logScale));
	}
	//spawns the nests and food
	 
	apple = new Food(Vec3(0, 0, 0), game);
	apple->OnCreate();
	//spawning the collectable food
	spawnFood();
	int startingX = 0;
	int startingY = 40; // this value spawns the nest right on the line
	for (int i = 0; i < numberOfNests; i++) {
		Nest* nest = new Nest(Vec3(startingX + i * 144.5f, startingY, 0.0f), game);
		nest->OnCreate();
		nests.push_back(nest);
	}

	// Load power‑up textures
	SDL_Surface* s = IMG_Load("freeze.png");
	if (!s) {
		std::cout << "Failed freeze.png: " << IMG_GetError() << std::endl;
		return false;
	}
	freezeTexture = SDL_CreateTextureFromSurface(renderer, s);
	int freezeW = s->w;
	int freezeH = s->h;
	SDL_FreeSurface(s);

	s = IMG_Load("extraLife.png");
	if (!s) {
		std::cout << "Failed extraLife.png: " << IMG_GetError() << std::endl;
		return false;
	}
	extraLifeTexture = SDL_CreateTextureFromSurface(renderer, s);
	int extraW = s->w;
	int extraH = s->h;
	SDL_FreeSurface(s);

	// Hearts UI
	s = IMG_Load("heart.png");
	if (!s) {
		std::cout << "Failed heart.png: " << IMG_GetError() << std::endl;
		return false;
	}
	heartTexture = SDL_CreateTextureFromSurface(renderer, s);
	int heartW = s->w;
	int heartH = s->h;
	SDL_FreeSurface(s);

	// Timer digits texture
	s = IMG_Load("digits.png");
	if (!s) {
		std::cout << "Failed digits.png: " << IMG_GetError() << std::endl;
		return false;
	}
	digitsTexture = SDL_CreateTextureFromSurface(renderer, s);
	SDL_FreeSurface(s);

	// Arrange hearts top‑left
	// Hearts UI - SCALED
	if (heartTexture) {
		int scaledHeartW = static_cast<int>(heartW * heartScale);
		int scaledHeartH = static_cast<int>(heartH * heartScale);

		int margin = 10;
		for (int i = 0; i < 3; ++i) {
			heartRects[i].x = margin + i * (scaledHeartW + 5);
			heartRects[i].y = margin;
			heartRects[i].w = scaledHeartW;
			heartRects[i].h = scaledHeartH;
		}
	}

	// Initial lives
	lives = 3;
	// Reset timer
	countdownTime = 120.0f;

	// Initial random spawn
	spawnFreezePowerUp();
	spawnExtraLifePowerUp();

	return true;
}


void Scene1::OnDestroy() {
	if (backgroundTexture) {
		SDL_DestroyTexture(backgroundTexture);
		backgroundTexture = nullptr;
	}

	if (freezeTexture) {
		SDL_DestroyTexture(freezeTexture);
		freezeTexture = nullptr;
	}
	if (extraLifeTexture) {
		SDL_DestroyTexture(extraLifeTexture);
		extraLifeTexture = nullptr;
	}
	if (heartTexture) {
		SDL_DestroyTexture(heartTexture);
		heartTexture = nullptr;
	}

	if (riverTexture) {
		SDL_DestroyTexture(riverTexture);
		riverTexture = nullptr;
	}

	if (digitsTexture) {
		SDL_DestroyTexture(digitsTexture);
		digitsTexture = nullptr;
	}

	/*delete game->getPlayerBody();*/

	for (auto car : cars) {
		delete car;
	}
	cars.clear();

	for (auto log : logs) {
		delete log;
	}
	logs.clear();

	// Reset for next game
	lives = 3;
}


void Scene1::Update(const float deltaTime) {
	// Freeze timer
	if (isFrozen) {
		freezeTimer -= deltaTime;
		if (freezeTimer <= 0.0f) {
			isFrozen = false;
		}
	}

	// Update player
	game->getPlayerBody()->Update(deltaTime);

	// Cars, logs, and countdown only update if not frozen
	if (!isFrozen) {
		// Update countdown timer
		if (countdownTime > 0.0f) {
			countdownTime -= deltaTime;
		}

		// Update cars
		for (auto& car : cars) {
			car->Update(deltaTime, SCREEN_WIDTH);
		}
		// Update logs
		for (auto& log : logs) {
			log->Update(deltaTime, SCREEN_WIDTH);
		}
	}

	// Check for collisions between player and cars
	const float playerScale = 0.10f;

	SDL_Rect playerRect = game->getPlayerBody()->getRect(playerScale);

	if (!apple->isCollected) {
		if (SDL_HasIntersection(&playerRect, &apple->hitBox)) { // This is where the collision for the car is being checked and where i think i should tick down health
			game->getPlayerBody()->setHasCollected(true);
			std::cout << "Player has collected the apple";
			apple->isCollected = true;

		}
	}
	//This code is for spawning the nests
	for (auto nest : nests) {
		if (nest->disabled)
			continue;
		if (SDL_HasIntersection(&playerRect, &nest->hitBox)) {
			if (game->getPlayerBody()->getHasCollected()) {
				nest->disableNest();
				srand(time(0));
				spawnFood();
				game->getPlayerBody()->setHasCollected(false);
				game->collectApple();
			}

		}
	}

	for (auto car : cars) {
		SDL_Rect carRect = car->getRectScaled();

		if (SDL_HasIntersection(&playerRect, &carRect)) {
			if (lives > 0) {
				lives--;
			}
			// Reset player to initial position - define as needed in game coords
			game->PlayHitSfx();
			Vec3 startPos = Vec3(11.6f, 0.5f, 0.0f);
			game->getPlayerBody()->setPosition(startPos);
			if (game->getPlayerBody()->getHasCollected()) {
				spawnFood();
			}
			game->getPlayerBody()->setHasCollected(false);
			game->getPlayerBody()->setVel(Vec3(0, 0, 0));
			break;
		}
	}

	// Update power-up cooldown timers
	if (freezeCooldownTimer > 0.0f) {
		freezeCooldownTimer -= deltaTime;
	}
	if (extraLifeCooldownTimer > 0.0f) {
		extraLifeCooldownTimer -= deltaTime;
	}

	// Freeze power‑up pickup
	if (freezeActive) {
		SDL_Rect freezeRectScreen = freezeRect;
		if (SDL_HasIntersection(&playerRect, &freezeRectScreen)) {
			isFrozen = true;
			freezeTimer = freezeDuration;
			freezeActive = false; // hide it until next spawn
			freezeCooldownTimer = POWERUP_COOLDOWN;  // Start cooldown NOW
		}
	}

	// Extra life pickup
	if (extraLifeActive) {
		SDL_Rect lifeRectScreen = extraLifeRect;
		if (SDL_HasIntersection(&playerRect, &lifeRectScreen)) {
			if (lives < 3) {
				lives++;
				extraLifeActive = false;
				extraLifeCooldownTimer = POWERUP_COOLDOWN;  // Start cooldown NOW
			}
		}
	}

	// Respawn power‑ups cooldown
	if (!freezeActive && freezeCooldownTimer <= 0.0f) {
		spawnFreezePowerUp();
		freezeCooldownTimer = POWERUP_COOLDOWN;  // Start 30s cooldown
	}
	if (!extraLifeActive && freezeCooldownTimer <= 0.0f) {
		spawnExtraLifePowerUp();
		extraLifeCooldownTimer = POWERUP_COOLDOWN;  // Start 30s cooldown
	}

	// Check for player interaction with logs and river
	bool onLog = false;
	for (auto log : logs) {
		SDL_Rect logRect = log->getRectScaled();
		if (SDL_HasIntersection(&playerRect, &logRect)) {
			onLog = true;
			if (!isFrozen) {
				Vec3 playerPos = game->getPlayerBody()->getPos();
				// The log's speed is in pixels per second. We need to convert it to grid units per second.
				float speedInGridUnits = log->getSpeed() / CELL_SIZE;
				playerPos.x += speedInGridUnits * deltaTime;
				game->getPlayerBody()->setPosition(playerPos);
			}
			break;
		}
	}

	if (!onLog && SDL_HasIntersection(&playerRect, &riverBackgroundRect)) {
		game->PlayHitSfx();
		Vec3 startPos = Vec3(11.6f, 0.5f, 0.0f);
		game->getPlayerBody()->setPosition(startPos);
		if (game->getPlayerBody()->getHasCollected()) {
			spawnFood();
		}
		game->getPlayerBody()->setHasCollected(false);
		lives--;
	}

	// GAME OVER CHECK
	if (lives <= 0 || countdownTime <= 0.0f) {
		if (game) {
			game->LoadScene(0);
		}
		return;  // Exit Update immediately
	}
}

void Scene1::spawnFreezePowerUp() {
	// x full width, y between lane 3 and 11
	int minYlane = 3;
	int maxYlane = 11;
	int minY = minYlane * CELL_SIZE;
	int maxY = (maxYlane + 1) * CELL_SIZE - 32; // 32 padding

	int minX = 0;
	int maxX = SCREEN_WIDTH - 32;

	freezeRect.w = 32;
	freezeRect.h = 32;
	freezeRect.x = rand() % (SCREEN_WIDTH - 32);
	freezeRect.y = (rand() % 8 + 3) * CELL_SIZE;

	freezeActive = true;
}

void Scene1::spawnExtraLifePowerUp() {
	int minYlane = 3;
	int maxYlane = 11;
	int minY = minYlane * CELL_SIZE;
	int maxY = (maxYlane + 1) * CELL_SIZE - 32;

	int minX = 0;
	int maxX = SCREEN_WIDTH - 32;

	extraLifeRect.w = 32;
	extraLifeRect.h = 32;
	extraLifeRect.x = minX + rand() % (maxX - minX + 1);
	extraLifeRect.y = minY + rand() % (maxY - minY + 1);

	extraLifeActive = true;
}

void Scene1::RenderText(std::string text, int x, int y, float scale) {
	if (!digitsTexture) return;

	// This string MUST match the order of characters in your digits.png file
	const std::string charMap = "0123456789.:"; // Added colon for timer display

	int currentX = x;
	int scaledWidth = static_cast<int>(DIGIT_SRC_WIDTH * scale);
	int scaledHeight = static_cast<int>(DIGIT_SRC_HEIGHT * scale);

	for (char const& c : text) {
		size_t charIndex = charMap.find(c);

		if (charIndex != std::string::npos) { // Check if the character was found
			SDL_Rect srcRect;
			srcRect.x = static_cast<int>(charIndex) * DIGIT_SRC_WIDTH;
			srcRect.y = 0;
			srcRect.w = DIGIT_SRC_WIDTH;
			srcRect.h = DIGIT_SRC_HEIGHT;

			SDL_Rect destRect = { currentX, y, scaledWidth, scaledHeight };
			SDL_RenderCopy(renderer, digitsTexture, &srcRect, &destRect);
		}

		currentX += scaledWidth; // Move to the next position
	}
}

void Scene1::Render() {
	SDL_SetRenderDrawColor(renderer, 0, 100, 0, 255); // Green for grass
	SDL_RenderClear(renderer);

	// customized road rendering
	if (backgroundTexture) {
		SDL_RenderCopy(renderer, backgroundTexture, nullptr, &carLaneBackgroundRect);
	}

	// customized river rendering
	if (riverTexture) {
		SDL_RenderCopy(renderer, riverTexture, nullptr, &riverBackgroundRect);
	}

	// Power‑ups
	if (freezeActive && freezeTexture) {
		SDL_RenderCopy(renderer, freezeTexture, nullptr, &freezeRect);
	}
	if (extraLifeActive && extraLifeTexture) {
		SDL_RenderCopy(renderer, extraLifeTexture, nullptr, &extraLifeRect);
	}

	// Hearts UI
	if (heartTexture) {
		for (int i = 0; i < lives; ++i) {
			SDL_RenderCopy(renderer, heartTexture, nullptr, &heartRects[i]);
		}
	}

	// Render Timer
	int minutes = static_cast<int>(countdownTime) / 60;
	int seconds = static_cast<int>(countdownTime) % 60;
	if (countdownTime < 0) { // Prevent negative display
		minutes = 0;
		seconds = 0;
	}

	std::stringstream ss;
	ss << minutes << ":" << std::setw(2) << std::setfill('0') << seconds;
	std::string timeStr = ss.str();

	// Calculate total width of the timer string to right-align it
	int totalWidth = static_cast<int>(timeStr.length() * (DIGIT_SRC_WIDTH * TIMER_SCALE));
	int xPos = SCREEN_WIDTH - totalWidth - 10; // 10 pixels margin
	int yPos = 10;

	RenderText(timeStr, xPos, yPos, TIMER_SCALE);


	//below is the code to display the grid lines on screen. im still working on how to toggle it with a single button.
	SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255); // gray lines

	//// Vertical lines
	//for (int x = 0; x <= GRID_COLS; ++x) {
	//	int xPos = x * CELL_SIZE;
	//	SDL_RenderDrawLine(renderer, xPos, 0, xPos, SCREEN_HEIGHT);
	//}

	//// Horizontal lines
	//for (int y = 0; y <= GRID_ROWS; ++y) {
	//	int yPos = y * CELL_SIZE;
	//	SDL_RenderDrawLine(renderer, 0, yPos, SCREEN_WIDTH, yPos);
	//}

	// Render cars
	for (auto car : cars) {
		car->Render(renderer, 0.10f); // scale to match player
	}

	// Render logs
	for (auto log : logs) {
		log->Render(renderer);
	}

	apple->render(renderer);

	for (auto nest : nests) {
		nest->render(renderer);
	}

	// render the player on top of the logs
	game->RenderPlayer(0.10f); // this number scales the player sprite

	SDL_RenderPresent(renderer);
}
void Scene1::HandleEvents(const SDL_Event& event)
{
	// send events to player as needed
	game->getPlayerBody()->HandleEvents(event);

	if (event.type == SDL_KEYDOWN)
	{
		switch (event.key.keysym.scancode)
		{
		case SDL_SCANCODE_2:
			spawnFood();

			break;
		}
	}

}

void Scene1::spawnFood() {
	srand(time(0));
	int X_Tile = rand() % 13;
	int Y_Tile = rand() % (11 - 5 + 1) + 5; // this number makes it so that the apple cant spawn on the player row
	apple->setPos(Vec3(X_Tile * 48, Y_Tile * 48, 0.0f));
	apple->isCollected = false;
}