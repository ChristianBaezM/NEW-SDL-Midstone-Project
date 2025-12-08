#include "GameManager.h"
#include "MenuScene.h"
#include "OptionScene.h"
#include "Scene1.h"
#include "WinScene.h"
#include "GridSettings.h" // you added this

GameManager::GameManager() {
	windowPtr = nullptr;
	timer = nullptr;
	isRunning = true;
	currentScene = nullptr;
    player = nullptr;
}

bool GameManager::OnCreate() {
    // My display is 1920 x 1080 but the following seems to work best to fill the screen.
    //const int SCREEN_WIDTH = 1540;
    //const int SCREEN_HEIGHT = 860;

    // Use 1000x600 for less than full screen

    // This is hard coded because this game window is almost perfectly matches https://happyhopper.org/. 
    const int SCREEN_WIDTH = 672; 
    const int SCREEN_HEIGHT = 624;

    //const int SCREEN_WIDTH = SCREEN_WIDTH; //you added this
    //const int SCREEN_HEIGHT = SCREEN_HEIGHT; // you added this

    windowPtr = new Window(SCREEN_WIDTH, SCREEN_HEIGHT);
	if (windowPtr == nullptr) {
		OnDestroy();
		return false;
	}
	if (windowPtr->OnCreate() == false) {
		OnDestroy();
		return false;
	}

	timer = new Timer();
	if (timer == nullptr) {
		OnDestroy();
		return false;
	}

    // select scene for specific assignment

    currentScene = new MenuScene(windowPtr->GetSDL_Window(), this);
    
    // create player
    float mass = 1.0f;
    float radius = 0.5f;
    float orientation = 0.0f;
    float rotation = 0.0f;
    float angular = 0.0f;
    Vec3 position(0.5f * currentScene->getxAxis(), 0.5f * currentScene->getyAxis(), 0.0f);
    Vec3 velocity(0.0f, 0.0f, 0.0f);
    Vec3 acceleration(0.0f, 0.0f, 0.0f);

    player = new PlayerBody
    (
        position,
        velocity,
        acceleration,
        mass,
        radius,
        orientation,
        rotation,
        angular,
        this
    );
    if ( player->OnCreate() == false ) {
        OnDestroy();
        return false;
    }

    // need to create Player before validating scene
    if (!ValidateCurrentScene()) {
        OnDestroy();
        return false;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        SDL_Log("SDL_mixer could not initialize! %s", Mix_GetError());
        return false;
    }

    bgMusic = Mix_LoadMUS("song8.mp3");
    if (!bgMusic) {
        SDL_Log("Failed to load music: %s", Mix_GetError());
    }
    else {
        Mix_VolumeMusic(musicVolume);
        Mix_PlayMusic(bgMusic, -1);
    }

    buttonSfx = Mix_LoadWAV("menu change.wav");
    if (!buttonSfx) {
        SDL_Log("Failed to load menu change.wav", Mix_GetError());
        return false;
    }

    backSfx = Mix_LoadWAV("back.wav");
    if (!backSfx) {
        SDL_Log("Failed to load back.wav", Mix_GetError());
        return false;
    }

    hitSfx = Mix_LoadWAV("enemy bump.wav");
    if (!hitSfx) {
        SDL_Log("Failed to load enemy bump.wav", Mix_GetError());
        return false;
    }

    SetSfxVolume(sfxVolume);

           
	return true;
}


/// Here's the whole game loop
void GameManager::Run() {
    
	timer->Start();
    
	while (isRunning) {
        
        handleEvents();
		timer->UpdateFrameTicks();
        currentScene->Update(timer->GetDeltaTime());
		currentScene->Render();

		/// Keep the event loop running at a proper rate
		SDL_Delay(timer->GetSleepTime(60)); ///60 frames per sec
	}
}

void GameManager::handleEvents() 
{
    SDL_Event event;

    // Let's add mouse movement and position
    // https://wiki.libsdl.org/SDL_GetMouseState

    SDL_PumpEvents();  // make sure we have the latest mouse state.

    //https://www.youtube.com/watch?v=SYrRMr4BaD4&list=PLM7LHX-clszBIGsrh7_3B2Pi74AhMpKhj&index=3

    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
        {
            isRunning = false;
        }
        else if (event.type == SDL_KEYDOWN)
        {
            switch (event.key.keysym.scancode)
            {
            case SDL_SCANCODE_ESCAPE:
                isRunning = false;
                break;
            case SDL_SCANCODE_Q:
                isRunning = false;
                break;
            case SDL_SCANCODE_DELETE:
                isRunning = false;
                break;
            case SDL_SCANCODE_1:
                LoadScene(1);
                break;
            default:
                break;
            }
        }
        currentScene->HandleEvents(event);
        if (auto* menu = dynamic_cast<MenuScene*>(currentScene)) {
            if (menu->ShouldStartGame()) {
                LoadScene(1);
                return;
            }
            if (menu->ShouldQuitGame()) {
                isRunning = false;
                return;
            }
            if (menu->ShouldOpenOptions()) {
                LoadScene(2);
                return;
            }
        }
        if (auto* opt = dynamic_cast<OptionScene*>(currentScene)) {
            if (opt->ShouldReturnToMenu()) {
                LoadScene(0);
                return;
            }
        }
    }
}

GameManager::~GameManager() {}

void GameManager::OnDestroy(){
	if (windowPtr) delete windowPtr;
	if (timer) delete timer;
	if (currentScene) delete currentScene;

    if (buttonSfx) { Mix_FreeChunk(buttonSfx); buttonSfx = nullptr; }
    if (backSfx) { Mix_FreeChunk(backSfx); backSfx = nullptr; }
    if (hitSfx) { Mix_FreeChunk(hitSfx); hitSfx = nullptr; }

    if (bgMusic) {
        Mix_HaltMusic();
        Mix_FreeMusic(bgMusic);
        bgMusic = nullptr;

    }

    Mix_CloseAudio();
}

// This might be unfamiliar
float GameManager::getSceneHeight() { return currentScene->getyAxis(); }

// This might be unfamiliar
float GameManager::getSceneWidth() { return currentScene->getxAxis(); }

// This might be unfamiliar
Matrix4 GameManager::getProjectionMatrix()
{ return currentScene->getProjectionMatrix(); }

PlayerBody* GameManager::getPlayerBody()
{
    return player;
}

// This might be unfamiliar
SDL_Renderer* GameManager::getRenderer()
{
    // [TODO] might be missing some SDL error checking
    SDL_Window* window = currentScene->getWindow();
    SDL_Renderer* renderer = SDL_GetRenderer(window);
    return renderer;
}

void GameManager::collectApple()
{
    applesCollected++;
    if (applesCollected >= 5) {
        std::cout << "YOU WIN";
        LoadScene(3);

    }

}

// This might be unfamiliar
void GameManager::RenderPlayer(float scale)
{
    player->Render(scale);
}

void GameManager::LoadScene( int i )
{
    // cleanup of current scene before loading another one
    if (currentScene) {
        currentScene->OnDestroy();
        delete currentScene;
        currentScene = nullptr;
    }
   
    switch ( i )
    {
    case 0:
        currentScene = new MenuScene(windowPtr->GetSDL_Window(), this);
        break;
    case 1:
        currentScene = new Scene1( windowPtr->GetSDL_Window(), this);
        break;
    case 2:
        currentScene = new OptionScene(windowPtr->GetSDL_Window(), this);
        break;
    case 3:
        currentScene = new WinScene(windowPtr->GetSDL_Window(), this);
        break;
    }

    // Validate and initialize new scene
    if (!ValidateCurrentScene())
    {
        isRunning = false;
    }
}

bool GameManager::ValidateCurrentScene()
{
    if (currentScene == nullptr) {
        return false;
    }
    if (currentScene->OnCreate() == false) {
        return false;
    }
    return true;
}

void GameManager::SetMusicVolume(int volume) {
    if (volume < 0) volume = 0;
    if (volume > MIX_MAX_VOLUME) volume = MIX_MAX_VOLUME;
    musicVolume = volume;
    Mix_VolumeMusic(volume);
}

int GameManager::GetMusicVolume() const {
    return musicVolume;
}


void GameManager::SetSfxVolume(int vol) {
    if (vol < 0) vol = 0;
    if (vol > MIX_MAX_VOLUME) vol = MIX_MAX_VOLUME;
    sfxVolume = vol;

    if (buttonSfx) { Mix_VolumeChunk(buttonSfx, sfxVolume); }
    if (backSfx) { Mix_VolumeChunk(backSfx, sfxVolume); }
    if (hitSfx) { Mix_VolumeChunk(hitSfx, sfxVolume); }
}




void GameManager::PlayButtonSfx() {
    if (buttonSfx) Mix_PlayChannel(-1, buttonSfx, 0);
}

void GameManager::PlayBackSfx() {
    if (backSfx) Mix_PlayChannel(-1, backSfx, 0);
}

void GameManager::PlayHitSfx() {
    if (hitSfx) Mix_PlayChannel(-1, hitSfx, 0);
}