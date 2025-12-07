#include "OptionScene.h"
#include "GameManager.h"
#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_image.h>


OptionScene::OptionScene(SDL_Window* sdlWindow, GameManager* gm) {
	window = sdlWindow;
	gameManager = gm;
	renderer = SDL_GetRenderer(window);
}

bool OptionScene::OnCreate() {
	
	int w, h;
	SDL_GetWindowSize(window, &w, &h);
	SDL_Log("OptionScene::OnCreate Start");

	SDL_Renderer* renderer = gameManager->getRenderer();
	if (!renderer) return false;

	//current volume
	int musicVol = gameManager->GetMusicVolume();
	volume01 = musicVol / float(MIX_MAX_VOLUME);

	int sfxVol = gameManager->GetSfxVolume();
	sfx01 = sfxVol / float(MIX_MAX_VOLUME);
	

	SDL_Surface* bgSurf = IMG_Load("SlyRaccoonMainMenu.png");
	if (!bgSurf) {
		return false;
	}
	bgTexture = SDL_CreateTextureFromSurface(renderer, bgSurf);
	SDL_FreeSurface(bgSurf);
	if (!bgTexture) { 
		return false; 
	}

	bgRect = { 0, 0 ,w, h };


	SDL_Surface* backSurf = IMG_Load("BackButton.png");
	if (!backSurf) return false;
	backTexture = SDL_CreateTextureFromSurface(renderer, backSurf);
	SDL_FreeSurface(backSurf);
	if (!backTexture) return false;

	int bw, bh;
	SDL_QueryTexture(backTexture, nullptr, nullptr, &bw, &bh);
	backRect = { 300, 450, bw * 2, bh * 2 };

	int barWidth = w / 3;
	int barHeight = 8;

	sliderBar = { w / 2 - barWidth / 2, h / 2, barWidth, barHeight };
	int knobW = 20;
	sliderKnob = { sliderBar.x + int(sliderBar.w * volume01) - knobW / 2, sliderBar.y - 6, knobW, 20 };

	sfxSliderBar = sliderBar;
	sfxSliderBar.y += 60;

	sfxSliderKnob = { sfxSliderBar.x + int(sfxSliderBar.w * sfx01) - knobW / 2, sfxSliderBar.y - 6, knobW, 20 };

	dragging = false;
	hoverBack = false;

	SDL_Log("OptionScene::OnCreate END (forced success)");
	return true;
}

void OptionScene::OnDestroy() {
	if (bgTexture) SDL_DestroyTexture(bgTexture);
	if (backTexture) SDL_DestroyTexture(backTexture);
}

void OptionScene::HandleEvents(const SDL_Event& e) {
	if (e.type == SDL_QUIT) return;

	if (e.type == SDL_MOUSEMOTION) {
		int mx = e.motion.x;
		int my = e.motion.y;

		hoverBack = (mx >= backRect.x && mx <= backRect.x + backRect.w && my >= backRect.y && my <= backRect.y + backRect.h);
		
	}

	if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
		int mx = e.button.x;
		int my = e.button.y;
		SDL_Point p{ mx, my };



		if (SDL_PointInRect(&p, &sliderBar) || SDL_PointInRect(&p, &sliderKnob)) {
			dragging = true;
		}

		if (SDL_PointInRect(&p, &sfxSliderBar) || SDL_PointInRect(&p, &sfxSliderKnob)) {
			draggingSfx = true;
		}

		if (SDL_PointInRect(&p, &backRect)) {
			gameManager->PlayBackSfx();
			backToMenu = true;
		}
	}



	if (e.type == SDL_MOUSEBUTTONUP && e.button.button == SDL_BUTTON_LEFT) {
		dragging = false;
		draggingSfx = false;
	}


	if (e.type == SDL_MOUSEMOTION) {
		int mx = e.motion.x;

		if (dragging) {
			float t = (mx - sliderBar.x) / float(sliderBar.w);
			if (t < 0.0f) t = 0.0f;
			if (t > 1.0f) t = 1.0f;
			volume01 = t;

			sliderKnob.x = sliderBar.x + int(sliderBar.w * volume01) - sliderKnob.w / 2;
			int vol = int(volume01 * MIX_MAX_VOLUME);
			gameManager->SetMusicVolume(vol);
		}
		
		if (draggingSfx) {
			float y = (mx - sfxSliderBar.x) / float(sfxSliderBar.w);
			if (y < 0.0f) y = 0.0f;
			if (y > 1.0f) y = 1.0f;
			sfx01 = y;

			sfxSliderKnob.x = sfxSliderBar.x + int(sfxSliderBar.w * sfx01) - sfxSliderKnob.w / 2;
			int sfxVol = int(sfx01 * MIX_MAX_VOLUME);
			gameManager->SetSfxVolume(sfxVol);
		}


	}
}

void OptionScene::Update(float) {}

void OptionScene::Render() {

	

	auto drawButton = [&](SDL_Texture* tex, const SDL_Rect& baseRect, bool hovered) {
		float scale = hovered ? 1.15f : 1.0f; //button will be 15% bigger when hovered

		SDL_Rect dst;
		dst.w = int(baseRect.w * scale);
		dst.h = int(baseRect.h * scale);
		dst.x = baseRect.x - (dst.w - baseRect.w) / 2;
		dst.y = baseRect.y - (dst.h - baseRect.h) / 2;

		if (hovered) SDL_SetTextureColorMod(tex, 255, 255, 255);
		else SDL_SetTextureColorMod(tex, 190, 190, 190);

		SDL_RenderCopy(renderer, tex, nullptr, &dst);

		};

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);

	//draw background
	SDL_RenderCopy(renderer, bgTexture, nullptr, &bgRect);

	//Render slider bar
	SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
	SDL_RenderFillRect(renderer, &sliderBar);

	//render slider knob
	SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
	SDL_RenderFillRect(renderer, &sliderKnob);


	//render sfx slider bar
	SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
	SDL_RenderFillRect(renderer, &sfxSliderBar);

	// render sfx slider knob
	SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
	SDL_RenderFillRect(renderer, &sfxSliderKnob);
	
	
	////render back button
	drawButton(backTexture, backRect, hoverBack);
	

	SDL_RenderPresent(renderer);
}