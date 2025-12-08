#include "WinScene.h"

WinScene::WinScene(SDL_Window* win, GameManager* gm)
{
	window = win;
	gameManager = gm;
	renderer = SDL_GetRenderer(window);
}

bool WinScene::OnCreate()
{
	int w, h;
	SDL_GetWindowSize(window, &w, &h);

	SDL_Surface* surf = IMG_Load("SlyRaccoonWin.png");
	if (!surf) return false;
	winTexture = SDL_CreateTextureFromSurface(renderer, surf);
	SDL_FreeSurface(surf);
	if (!winTexture) return false;

	winRect = { 0, 0, w, h };
	return true;
}

void WinScene::OnDestroy()
{
	if (winTexture) {
		SDL_DestroyTexture(winTexture);
		winTexture = nullptr;
	}
}

void WinScene::Update(const float deltaTime)
{

}

void WinScene::HandleEvents(const SDL_Event& e)
{

}


void WinScene::Render()
{
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);

	SDL_RenderCopy(renderer, winTexture, nullptr, &winRect);

	SDL_RenderPresent(renderer);
}