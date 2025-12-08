#pragma once
#include "Vector.h"
#include <SDL.h>
#include <SDL_image.h>
#include "GameManager.h"

using namespace MATH;

class Nest {
private:
	Vec3 position;

public:

	bool disabled = false;
	SDL_Texture* texture;
	SDL_Rect hitBox;
	class GameManager* game;
	float scale = 3.0f;

	bool OnCreate();

	void render(SDL_Renderer* renderer);

	void setPos(Vec3 Pos_) {
		position = Pos_;
		hitBox.x = position.x;
		hitBox.y = position.y;
	}

	Nest(Vec3 Pos_, GameManager* game_) { position = Pos_; game = game_; }

	void disableNest();
};

