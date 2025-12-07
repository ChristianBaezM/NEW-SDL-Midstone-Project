#include "MenuScene.h"
#include "GameManager.h"
#include <SDL_image.h>

MenuScene::MenuScene(SDL_Window* sdlWindow, GameManager* game_) {
	window = sdlWindow;
	gameManager = game_;
	renderer = SDL_GetRenderer(window);

	bgTexture = nullptr;
	playTexture = nullptr;
	

	playRect = { 0, 0, 0, 0 };
}

MenuScene::~MenuScene() {}

bool MenuScene::OnCreate() {
	

	int w, h;
	SDL_GetWindowSize(window, &w, &h);

	SDL_Surface* bgSurf = IMG_Load("SlyRaccoonMainMenu.png");
	if (!bgSurf) return false;
	bgTexture = SDL_CreateTextureFromSurface(renderer, bgSurf);
	SDL_FreeSurface(bgSurf);
	if (!bgTexture) return false;


	SDL_Surface* playSurf = IMG_Load("PlayButton.png");
	if (!playSurf) return false;
	playTexture = SDL_CreateTextureFromSurface(renderer, playSurf);
	SDL_FreeSurface(playSurf);
	if (!playTexture) return false;

	SDL_Surface* quitSurf = IMG_Load("ExitButton.png");
	if (!quitSurf) return false;
	quitTexture = SDL_CreateTextureFromSurface(renderer, quitSurf);
	SDL_FreeSurface(quitSurf);
	if (!quitTexture) return false;

	SDL_Surface* optionSurf = IMG_Load("OptionButton.png");
	if (!optionSurf) return false;
	optionTexture = SDL_CreateTextureFromSurface(renderer, optionSurf);
	SDL_FreeSurface(optionSurf);
	if (!optionTexture) return false;


	int bw, bh;
	SDL_QueryTexture(playTexture, nullptr, nullptr, &bw, &bh);

	playRect.w = bw * 1.5;
	playRect.h = bh * 1.5;
	playRect.x = w / 2 - bw / 2;
	playRect.y = h * 2 / 3 - bh / 2;


	int qw, qh;
	SDL_QueryTexture(quitTexture, nullptr, nullptr, &qw, &qh);

	quitRect.w = qw * 1.5;
	quitRect.h = qh * 1.5;
	quitRect.x = w / 2 - qw / 2;
	quitRect.y = playRect.y + playRect.h + 20;

	int ow, oh;
	SDL_QueryTexture(optionTexture, nullptr, nullptr, &ow, &oh);

	optionRect.w = ow * 1.5;
	optionRect.h = oh * 1.5;
	optionRect.x = w / 2 - ow / 2;
	optionRect.y = quitRect.y + quitRect.h + 20;

	return true;
}


void MenuScene::OnDestroy() {
	if (bgTexture) SDL_DestroyTexture(bgTexture);
	if (playTexture) SDL_DestroyTexture(playTexture);
	if (quitTexture) SDL_DestroyTexture(quitTexture);
	if (optionTexture) SDL_DestroyTexture(optionTexture);

	
}

void MenuScene::Update(const float deltaTime) {
	const float fadeSpeed = 1.0f;

	if (state == MenuState::FadingToGame || state == MenuState::FadingToQuit) {
		fadeAlpha += fadeSpeed * deltaTime;
		if (fadeAlpha >= 1.0f) {
			fadeAlpha = 1.0f;
			if (state == MenuState::FadingToGame) startGame = true;
			if (state == MenuState::FadingToQuit) quitGame = true;
		}
	}
}

void MenuScene::Render() {

	auto drawButton = [&](SDL_Texture* tex, const SDL_Rect& baseRect, bool hovered) {
		float scale = hovered ? 1.15f : 1.0f; //button will be 15% bigger when hovered

		SDL_Rect dst;
		dst.w = static_cast<int>(baseRect.w * scale);
		dst.h = static_cast<int>(baseRect.h * scale);
		dst.x = baseRect.x - (dst.w - baseRect.w) / 2;
		dst.y = baseRect.y - (dst.h - baseRect.h) / 2;

		if (hovered) SDL_SetTextureColorMod(tex, 255, 255, 255);
		else SDL_SetTextureColorMod(tex, 190, 190, 190);

		SDL_RenderCopy(renderer, tex, nullptr, &dst);

		};

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);

	int w, h;
	SDL_GetWindowSize(window, &w, &h);
	SDL_Rect dst = { 0, 0, w, h };
	SDL_RenderCopy(renderer, bgTexture, nullptr, &dst);


	drawButton(playTexture, playRect, hoverPlay);
	drawButton(quitTexture, quitRect, hoverQuit);
	drawButton(optionTexture, optionRect, hoverOption);

	if (fadeAlpha > 0.0f) {
		SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
		Uint8 a = static_cast<Uint8>(fadeAlpha * 255);
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, a);
		SDL_Rect full = { 0, 0, w, h };
		SDL_RenderFillRect(renderer, &full);
	}

	

	SDL_RenderPresent(renderer);



}

void MenuScene::HandleEvents(const SDL_Event& event) {
	if (event.type == SDL_MOUSEMOTION) {
		int mx = event.motion.x;
		int my = event.motion.y;

		hoverPlay = (mx >= playRect.x && mx <= playRect.x + playRect.w && my >= playRect.y && my <= playRect.y + playRect.h);
		

		hoverQuit = (mx >= quitRect.x && mx <= quitRect.x + quitRect.w && my >= quitRect.y && my <= quitRect.y + quitRect.h);
		

		hoverOption = (mx >= optionRect.x && mx <= optionRect.x + optionRect.w && my >= optionRect.y && my <= optionRect.y + optionRect.h);
		
	}

	if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT && state == MenuState::Normal) {
		int mx = event.button.x;
		int my = event.button.y;

		if (mx >= playRect.x && mx <= playRect.x + playRect.w && my >= playRect.y && my <= playRect.y + playRect.h) {
			
			gameManager->PlayButtonSfx();
			state = MenuState::FadingToGame;
		}

		if (mx >= quitRect.x && mx <= quitRect.x + quitRect.w && my >= quitRect.y && my <= quitRect.y + quitRect.h) {
			
			gameManager->PlayButtonSfx();
			state = MenuState::FadingToQuit;
		}

		if (mx >= optionRect.x && mx <= optionRect.x + optionRect.w && my >= optionRect.y && my <= optionRect.y + optionRect.h) {
			gameManager->PlayButtonSfx();
			gameOption = true;
		}
	}

	

}