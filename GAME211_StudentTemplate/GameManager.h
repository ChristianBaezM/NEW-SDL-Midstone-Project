#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H
#include <SDL.h>
#include <iostream>
#include "Window.h"
#include "Timer.h"
#include "Scene.h"
#include "PlayerBody.h"
#include <SDL_mixer.h>


class GameManager {
private:
	/// These are called "forward declarations" The idea is that a pointer is 
	/// really just an unsigned int, so to create a pointer to it you really only
	/// need to tell the compiler that there is a class called Window and I need
	/// a pointer to it, thus the "class Window*" means trust me, there is a class 
	/// called Window, I just need a pointer to it.

	/// If that was too much for your brain, just #include "Window.h" and declare
	/// Window *ptr and don't use the word "class"  This is a second semester C++
	/// topic anyway
	class Window *windowPtr;
	class Timer *timer;
	bool isRunning;
	class Scene *currentScene;
	int applesCollected = 0;

	// This might be unfamiliar
    class PlayerBody *player;

	void handleEvents();
	bool ValidateCurrentScene();

	Mix_Music* bgMusic = nullptr;
	int musicVolume = MIX_MAX_VOLUME / 2;

	Mix_Chunk* buttonSfx = nullptr;
	Mix_Chunk* backSfx = nullptr;
	Mix_Chunk* hitSfx = nullptr;
	int sfxVolume = MIX_MAX_VOLUME;

	// pending scene change (deferred until safe time)
	int pendingScene = -1;

public:
	GameManager();
	~GameManager();
	bool OnCreate();
	void OnDestroy();

	// These might be unfamiliar
	float getSceneHeight();
	float getSceneWidth();
	Matrix4 getProjectionMatrix();
	PlayerBody* getPlayerBody();
	void RenderPlayer(float scale = 1.0f);
	SDL_Renderer* getRenderer();
	void collectApple();
	void LoadScene(int i);
	void Run();

	// request a scene change (does not delete currentScene immediately)
	void RequestLoadScene(int i) { pendingScene = i; }
    
	void SetMusicVolume(int volume);
	int GetMusicVolume() const;

	void SetSfxVolume(int volume);
	int GetSfxVolume() const { return sfxVolume; }

	void PlayButtonSfx();
	void PlayBackSfx();
	void PlayHitSfx();

};
#endif


