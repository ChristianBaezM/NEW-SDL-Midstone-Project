#include "Scene1.h"
#include <VMath.h>
#include "GridSettings.h"

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
	int w, h;
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

	return true;
}


void Scene1::OnDestroy() {
	if (backgroundTexture) {
		SDL_DestroyTexture(backgroundTexture);
		backgroundTexture = nullptr;
	}
	if (riverTexture) {
		SDL_DestroyTexture(riverTexture);
		riverTexture = nullptr;
	}

	delete game->getPlayerBody();

	for (auto car : cars) {
		delete car;
	}
	cars.clear();

	for (auto log : logs) {
		delete log;
	}
	logs.clear();
}


void Scene1::Update(const float deltaTime) {

	// Update player
	game->getPlayerBody()->Update(deltaTime);

	// Update cars
	for (auto& car : cars) {
		car->Update(deltaTime, SCREEN_WIDTH);
	}
	// Update logs
	for (auto& log : logs) {
		log->Update(deltaTime, SCREEN_WIDTH);
	}

	// Check for collisions between player and cars
	const float playerScale = 0.10f;

	SDL_Rect playerRect = game->getPlayerBody()->getRect(playerScale);

	for (auto car : cars) {
		SDL_Rect carRect = car->getRectScaled();

		if (SDL_HasIntersection(&playerRect, &carRect)) {
			// Reset player to initial position - define as needed in game coords
			Vec3 startPos = Vec3(12.5f, 0.5f, 0.0f);
			game->getPlayerBody()->setPosition(startPos);
			break;
		}
	}
	// Check for player interaction with logs and river
	bool onLog = false;
	for (auto log : logs) {
		SDL_Rect logRect = log->getRectScaled();
		if (SDL_HasIntersection(&playerRect, &logRect)) {
			onLog = true;
			Vec3 playerPos = game->getPlayerBody()->getPos();
			playerPos.x += log->getSpeed() * deltaTime / CELL_SIZE; // Adjust for grid coordinates
			game->getPlayerBody()->setPosition(playerPos);
			break;
		}
	}

	if (!onLog && SDL_HasIntersection(&playerRect, &riverBackgroundRect)) {
		Vec3 startPos = Vec3(12.5f, 0.5f, 0.0f);
		game->getPlayerBody()->setPosition(startPos);
	}
}

void Scene1::Render() {
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	SDL_RenderClear(renderer);

	// customized road rendering
	if (backgroundTexture) {
		SDL_RenderCopy(renderer, backgroundTexture, nullptr, &carLaneBackgroundRect);
	}

	// customized river rendering
	if (riverTexture) {
		SDL_RenderCopy(renderer, riverTexture, nullptr, &riverBackgroundRect);
	}

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

	// render the player on top of the logs
	game->RenderPlayer(0.10f); // this number scales the player sprite

	SDL_RenderPresent(renderer);
}
void Scene1::HandleEvents(const SDL_Event& event)
{
	// send events to player as needed
	game->getPlayerBody()->HandleEvents(event);
}