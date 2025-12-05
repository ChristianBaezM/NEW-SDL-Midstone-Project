#pragma once
#include <SDL.h>
#include <SDL_image.h>

class Log {
private:
    SDL_Texture* texture;
    SDL_Rect rect;
    float speed;
    bool moveRight;
    float scale;

public:
    Log(SDL_Renderer* renderer, const char* imagePath, int x, int y, float speed, bool moveRight, float scale);
    ~Log();

    void Update(float deltaTime, int screenWidth);
    void Render(SDL_Renderer* renderer);
    SDL_Rect getRectScaled() const;
    float getSpeed() const { return moveRight ? speed : -speed; }
};