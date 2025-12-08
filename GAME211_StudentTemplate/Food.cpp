#include "Food.h"
bool Food::OnCreate() {

    SDL_Surface* image = IMG_Load("apple.png");
    if (image == nullptr) {
        std::cerr << "Can't open the image" << std::endl;
        return false;
    }
    SDL_Renderer* renderer = game->getRenderer();
    texture = SDL_CreateTextureFromSurface(renderer, image);
    if (texture == nullptr) {
        std::cerr << "Can't open the image" << std::endl;
        return false;
    }
    hitBox.x = position.x;
    hitBox.y = position.y;
    SDL_QueryTexture(texture, NULL, NULL, &hitBox.w, &hitBox.h);
    hitBox.w *= scale;
    hitBox.h *= scale;
    return true;
}

void Food::render(SDL_Renderer* renderer) {
    if (!isCollected)
        SDL_RenderCopy(renderer, texture, nullptr, &hitBox);
}

