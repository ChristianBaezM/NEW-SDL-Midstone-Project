#include "Car.h"

Car::Car(SDL_Renderer* renderer, const char* imagePath, int x, int y, float spd, bool dir, float scale_)
    : speed(spd), moveRight(dir), scale(scale_) {
    SDL_Surface* surface = IMG_Load(imagePath);
    if (!surface) {
        std::cout << "IMG_Load failed for " << imagePath << ": " << IMG_GetError() << std::endl;
        texture = nullptr;
        rect = { x, y, 0, 0 };
        return;
    }
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    rect = { x, y, surface->w, surface->h };
    SDL_FreeSurface(surface);
}

Car::~Car() {
    SDL_DestroyTexture(texture);
}

void Car::Update(float deltaTime, int screenWidth) {
    if (moveRight)
        rect.x += static_cast<int>(speed * deltaTime);
    else
        rect.x -= static_cast<int>(speed * deltaTime);

    // Loop car to the other side when off screen
    if (moveRight && rect.x > screenWidth)
        rect.x = -rect.w;
    if (!moveRight && rect.x + rect.w < 0)
        rect.x = screenWidth;
}

void Car::Render(SDL_Renderer* renderer, float scale) {
	if (!texture) return; // Safety check
    SDL_Rect dst = { 
        rect.x, rect.y, 
        static_cast<int>(rect.w * scale), 
        static_cast<int>(rect.h * scale) 
    };
    SDL_RenderCopy(renderer, texture, nullptr, &dst);
}

SDL_Rect Car::getRectScaled() const
{
    SDL_Rect scaledRect;
    scaledRect.x = rect.x;
    scaledRect.y = rect.y;
    scaledRect.w = static_cast<int>(rect.w * scale);
    scaledRect.h = static_cast<int>(rect.h * scale);

    return scaledRect;
}
