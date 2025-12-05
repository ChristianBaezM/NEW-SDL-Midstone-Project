#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include "Vector.h"

class Car {
private:
    SDL_Texture* texture;
    SDL_Rect rect;
    float speed;
    bool moveRight; // true: right->left, false: left->right
	float scale;
public:
    Car(SDL_Renderer* renderer, const char* imagePath, int x, int y, float spd, bool dir, float scale_);
    ~Car();
    void Update(float deltaTime, int screenWidth);
    void Render(SDL_Renderer* renderer, float scale = 1.0f);
    SDL_Rect getRect() const { return rect; }
    SDL_Rect getRectScaled() const;
};
