#pragma once
#include "Vector.h"
#include <SDL.h>
#include <SDL_image.h>
#include "GameManager.h"

using namespace MATH;

class Food {

public:

	bool isCollected = false; //needs a default value
	Vec3 position; //
	SDL_Texture* texture;
	SDL_Rect hitBox;
	class GameManager* game;
	float scale = 2.5f;

	//public :
		//collistion method
		//load texture
		//populate hitbox (means give value)
		//set start position

	Food(Vec3 Pos_, GameManager* game_) { position = Pos_; game = game_; }

	bool OnCreate();
	// create a render method that receives an SDL_renderer variable and renders the texture
	void setPos(Vec3 Pos_) {
		position = Pos_;
		hitBox.x = position.x;
		hitBox.y = position.y;
	}

	Vec3 getPos() {
		return position;
	}

	void render(SDL_Renderer* renderer);

};

