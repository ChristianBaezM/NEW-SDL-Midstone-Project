#ifndef SCENE1_H
#define SCENE1_H

#include <MMath.h>
#include <vector>
#include "Scene.h"
#include "Car.h"
#include "Log.h"

using namespace MATH;
class Scene1 : public Scene {
private:
	float xAxis;	// scene width, in game coords, set in constructor
	float yAxis;	// scene height, in game coords, set in constructor
	SDL_Window* window;		// an SDL window with a SDL renderer
	SDL_Renderer* renderer;	// the renderer associated with SDL window
	Matrix4 projectionMatrix;	// set in OnCreate()
	Matrix4     inverseProjection;	// set in OnCreate()
	bool showGrid;
	std::vector <Car*> cars;
	std::vector <Log*> logs;
	SDL_Texture* backgroundTexture = nullptr;
	SDL_Rect carLaneBackgroundRect;
	SDL_Texture* riverTexture = nullptr;
	SDL_Rect riverBackgroundRect;

	// Power‑ups
	SDL_Texture* freezeTexture = nullptr;
	SDL_Texture* extraLifeTexture = nullptr;
	SDL_Rect freezeRect{};
	SDL_Rect extraLifeRect{};
	bool freezeActive = false;
	bool extraLifeActive = false;

	// Freeze effect
	bool isFrozen = false;
	float freezeTimer = 0.0f;
	float freezeDuration = 3.0f; // seconds

	// Lives
	float heartScale = 0.05f;
	int lives = 3;
	SDL_Texture* heartTexture = nullptr;
	SDL_Rect heartRects[3]; // fixed 3 hearts max

	// Cooldown timers
	float freezeCooldownTimer = 0.0f;
	float extraLifeCooldownTimer = 0.0f;
	const float POWERUP_COOLDOWN = 30.0f;  // 30 seconds

	// Timer
	float countdownTime = 50.0f; // Start at 1 minute
	SDL_Texture* digitsTexture = nullptr;
	const int DIGIT_SRC_WIDTH = 153;
	const int DIGIT_SRC_HEIGHT = 256;
	const float TIMER_SCALE = 0.125f;

public:
	// This constructor may be different from what you've seen before
	// Notice the second parameter, and look in GameManager.cpp
	// to see how this constructor is called.
	Scene1(SDL_Window* sdlWindow, GameManager* game_);
	~Scene1();
	bool OnCreate();
	void OnDestroy();
	void Update(const float time);
	void Render();
	void HandleEvents(const SDL_Event& event);
	float getxAxis() { return xAxis; }
	float getyAxis() { return yAxis; }
	SDL_Window* getWindow() { return window; }
	Matrix4 getProjectionMatrix() { return projectionMatrix; }
	Matrix4 getInverseMatrix() { return inverseProjection; }
	void spawnFreezePowerUp();
	void spawnExtraLifePowerUp();
	void RenderText(std::string text, int x, int y, float scale);
};

#endif