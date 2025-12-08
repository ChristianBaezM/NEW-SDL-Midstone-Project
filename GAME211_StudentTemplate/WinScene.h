#pragma once
#include "Scene.h"
#include "GameManager.h"
#include <SDL.h>
#include <SDL_image.h>

class WinScene : public Scene
{
private:
	SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;

	GameManager* gameManager = nullptr;

	SDL_Texture* winTexture = nullptr;
	SDL_Rect winRect{ 0, 0, 0, 0 };


public:
	WinScene(SDL_Window* window, GameManager* gm);
	~WinScene() override = default;

	bool OnCreate() override;
	void OnDestroy() override;
	void Update(const float time) override;
	void Render() override;
	void HandleEvents(const SDL_Event& event) override;

	float getxAxis() override { return 0.0f; }
	float getyAxis() override { return 0.0f; }
	SDL_Window* getWindow() override { return window; }
	Matrix4 getProjectionMatrix() override { return Matrix4(); }
	Matrix4 getInverseMatrix() override { return Matrix4(); }
};