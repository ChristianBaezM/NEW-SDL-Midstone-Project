#pragma once
#include "Scene.h"
#include "GameManager.h"
#include <SDL.h>

class GameManager;

class OptionScene : public Scene {

private:
	GameManager* gameManager = nullptr;

	SDL_Window* window;
	SDL_Renderer* renderer;

	SDL_Texture* bgTexture = nullptr;
	SDL_Texture* backTexture = nullptr;

	SDL_Texture* musicTexture = nullptr;
	SDL_Texture* sfxTexture = nullptr;
	

	bool backToMenu = false;

	bool hoverBack = false;

	SDL_Rect sliderBar{};
	SDL_Rect sliderKnob{};
	SDL_Rect backButton{};
	bool dragging = false;
	float volume01 = 0.5f;

	SDL_Rect sfxSliderBar{};
	SDL_Rect sfxSliderKnob{};
	bool draggingSfx = false;
	float sfx01 = 0.5f;

	SDL_Rect backRect{};
	SDL_Rect bgRect{};

	SDL_Rect musicRect;
	SDL_Rect sfxRect;

public:
	OptionScene(SDL_Window* sdlWindow, GameManager* gm);
	bool OnCreate() override;
	void OnDestroy() override;
	void HandleEvents(const SDL_Event& e) override;
	void Update(float deltaTime) override;
	void Render() override;


	float getxAxis() override { return 0.0f; }
	float getyAxis() override { return 0.0f; }
	SDL_Window* getWindow() override { return window; }
	Matrix4 getProjectionMatrix() override { return Matrix4(); }
	Matrix4 getInverseMatrix() override { return Matrix4(); }


	bool ShouldReturnToMenu() const { return backToMenu; }

};