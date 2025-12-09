#include "Log.h"
#include "GridSettings.h" 

Log::Log(SDL_Renderer* renderer, const char* imagePath, int x, int y, float speed, bool moveRight, float tilesWide)
    : speed(speed), moveRight(moveRight), tilesWide(tilesWide) {
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
    /*SDL_Rect scaledRect = getRectScaled();
    SDL_RenderCopy(renderer, texture, NULL, &scaledRect);*/

    if (!texture) return;

    SDL_Rect src{ 0, 0, rect.w, rect.w }; // full 16x16 texture

    for (int i = 0; i < tilesWide; ++i) {
        SDL_Rect dst;
        dst.x = rect.x + i * CELL_SIZE;
        dst.y = rect.y;
        dst.w = CELL_SIZE;   // bigger than 16
        dst.h = CELL_SIZE;

        SDL_RenderCopy(renderer, texture, &src, &dst);
    }
}

SDL_Rect Log::getRectScaled() const {
    SDL_Rect r;
    r.x = rect.x;
    r.y = rect.y;
    r.w = tilesWide * CELL_SIZE;  // full visual width
    r.h = CELL_SIZE;              // full visual height
    return r;
}