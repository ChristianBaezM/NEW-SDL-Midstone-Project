#include "Nest.h"
bool Nest::OnCreate() {

    SDL_Surface* image = IMG_Load("Nest.png");
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
void Nest::render(SDL_Renderer* renderer) {
    SDL_RenderCopy(renderer, texture, nullptr, &hitBox);
}

void Nest::disableNest() {
    disabled = true;
    SDL_DestroyTexture(texture);

    SDL_Surface* image = IMG_Load("Nest_Collected.png");

    SDL_Renderer* renderer = game->getRenderer();
    texture = SDL_CreateTextureFromSurface(renderer, image);

    SDL_QueryTexture(texture, NULL, NULL, &hitBox.w, &hitBox.h);
    hitBox.w *= scale;
    hitBox.h *= scale;
}

