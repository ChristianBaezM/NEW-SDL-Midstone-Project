#include "Log.h"

Log::Log(SDL_Renderer* renderer, const char* imagePath, int x, int y, float speed, bool moveRight, float scale)
    : speed(speed), moveRight(moveRight), scale(scale) {
    SDL_Surface* surface = IMG_Load(imagePath);
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    rect.x = x;
    rect.y = y;
    SDL_QueryTexture(texture, NULL, NULL, &rect.w, &rect.h);
}

Log::~Log() {
    if (texture) {
        SDL_DestroyTexture(texture);
    }
}

void Log::Update(float deltaTime, int screenWidth) {
    if (moveRight) {
        rect.x += static_cast<int>(speed * deltaTime);
        if (rect.x > screenWidth) {
            rect.x = -rect.w;
        }
    }
    else {
        rect.x -= static_cast<int>(speed * deltaTime);
        if (rect.x < -rect.w) {
            rect.x = screenWidth;
        }
    }
}

void Log::Render(SDL_Renderer* renderer) {
    SDL_Rect scaledRect = getRectScaled();
    SDL_RenderCopy(renderer, texture, NULL, &scaledRect);
}

SDL_Rect Log::getRectScaled() const {
    SDL_Rect scaledRect;
    scaledRect.w = static_cast<int>(rect.w * scale);
    scaledRect.h = static_cast<int>(rect.h * scale);
    scaledRect.x = rect.x;
    scaledRect.y = rect.y;
    return scaledRect;
}