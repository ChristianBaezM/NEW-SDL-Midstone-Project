#ifndef MENUSCENE_H
#define MENUSCENE_H

#include "Scene.h"
#include <SDL_mixer.h>

class MenuScene : public Scene {
private:
	GameManager* gameManager;

	SDL_Window* window;
	SDL_Renderer* renderer;

	SDL_Texture* bgTexture;
	SDL_Texture* playTexture;
	SDL_Texture* quitTexture;
	SDL_Texture* optionTexture;

	SDL_Rect playRect;
	SDL_Rect quitRect;
	SDL_Rect optionRect;

	bool startGame = false;
	bool quitGame = false;
	bool gameOption = false;

	bool hoverPlay = false;
	bool hoverQuit = false;
	bool hoverOption = false;

	float fadeAlpha = 0.0f;
	enum class MenuState { Normal, FadingToGame, FadingToQuit };
	MenuState state = MenuState::Normal;



	


public:
	MenuScene(SDL_Window* sdlWindow, GameManager* game_);
	~MenuScene();

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

	bool ShouldStartGame() const { return startGame; }
	bool ShouldQuitGame() const { return quitGame; }
	bool ShouldOpenOptions() const { return gameOption; }
};




#endif