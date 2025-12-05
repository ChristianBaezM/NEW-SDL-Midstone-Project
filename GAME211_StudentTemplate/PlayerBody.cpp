//
//  PlayerBody.cpp
//  DemoAI
//
//  Created by Gail Harris on 2021-Dec-23.
//

#include "PlayerBody.h"

bool PlayerBody::OnCreate()
{
    image = IMG_Load( "Pacman.png" );
    SDL_Renderer *renderer = game->getRenderer();
    texture = SDL_CreateTextureFromSurface( renderer, image );
    if (image == nullptr) {
        std::cerr << "Can't open the image" << std::endl;
        return false;
    }
	pos = Vec3(12.5f, 0.5f, 0.0f);
    return true;
}

void PlayerBody::Render( float scale )
{
    // This is why we need game in the constructor, to get the renderer, etc.
    SDL_Renderer *renderer = game->getRenderer();
    Matrix4 projectionMatrix = game->getProjectionMatrix();

    // square represents the position and dimensions for where to draw the image
    SDL_Rect square;
    Vec3 screenCoords;
    float    w, h;

    // convert the position from game coords to screen coords.
    screenCoords = projectionMatrix * pos;

    // Scale the image, in case the .png file is too big or small
    w = image->w * scale;
    h = image->h * scale;

    // The square's x and y values represent the top left corner of 
    // where SDL will draw the .png image.
    // The 0.5f * w/h offset is to place the .png so that pos represents the center
    // (Note the y axis for screen coords points downward, hence subtraction!!!!)
    square.x = static_cast<int>(screenCoords.x - 0.5f * w);
    square.y = static_cast<int>(screenCoords.y - 0.5f * h);
    square.w = static_cast<int>(w);
    square.h = static_cast<int>(h);

    // Convert character orientation from radians to degrees.
    float orientationDegrees = orientation * 180.0f / M_PI ;

    SDL_RenderCopyEx( renderer, texture, nullptr, &square,
        orientationDegrees, nullptr, SDL_FLIP_NONE );
}

// As of now this is how im getting sly to move, but we need to figure out how to move him in between each space.
void PlayerBody::HandleEvents(const SDL_Event& event) {
    float maxSpeed = 2.5f;
    if (event.type == SDL_KEYDOWN && event.key.repeat == 0) {
        float maxSpeed = 2.5f;
            switch (event.key.keysym.scancode) {
                case SDL_SCANCODE_W:
                    vel.y = maxSpeed * 1.0f;
                    break;
                case SDL_SCANCODE_A:
                    vel.x = maxSpeed * -1.0f;
                    break;
                case SDL_SCANCODE_S:
                    vel.y = maxSpeed * -1.0f;
                    break;
                case SDL_SCANCODE_D:
                    vel.x = maxSpeed * 1.0f;
                    break;
            }
    }
    if (event.type == SDL_KEYUP && event.key.repeat == 0) {
        switch (event.key.keysym.scancode) {
        case SDL_SCANCODE_W:
            vel.y = 0.0f;
            break;
        case SDL_SCANCODE_A:
            vel.x = 0.0f;
            break;
        case SDL_SCANCODE_S:
            vel.y = 0.0f;
            break;
        case SDL_SCANCODE_D:
            vel.x = 0.0f;
            break;
        }
    }

    if (VMath::mag(vel) > VERY_SMALL) {
        vel = VMath::normalize(vel) * maxSpeed;
    }
}
void PlayerBody::Update( float deltaTime )
{
    // Update position, call Update from base class
    // Note that would update velocity too, and rotation motion
    Body::Update( deltaTime );

}

SDL_Rect PlayerBody::getRect(float scale) const
{
    SDL_Rect rect;

    // Calculate width and height scaled
    int w = static_cast<int>(image->w * scale);
    int h = static_cast<int>(image->h * scale);

    // Convert game position to screen coordinates as in Render
    Vec3 screenCoords = game->getProjectionMatrix() * pos;

    rect.x = static_cast<int>(screenCoords.x - 0.5f * w);
    rect.y = static_cast<int>(screenCoords.y - 0.5f * h);
    rect.w = w;
    rect.h = h;

    return rect;
}

void PlayerBody::setPosition(const Vec3& newPos)
{
    pos = newPos;
}

