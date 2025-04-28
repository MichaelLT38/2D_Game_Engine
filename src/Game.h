//
// Created by mltac on 4/22/2025.
//

#ifndef GAME_H
#define GAME_H

#ifdef _WIN32
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <glm/glm.hpp>
#include <imgui.h>
#include <sol/sol.hpp>

#elif defined(__linux__)
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <glm/glm.hpp>
#include <imgui/imgui.h>
#include <sol/sol.hpp>

#else
#error "Unsupported platform"
#endif

class Game {
    private:
    bool running;
    SDL_Window* window;
    SDL_Renderer* renderer;

    public:
        Game();
        ~Game();
        void Init();
        void Run();
        void Update();
        void Render();
        void ProcessInput();
        void Destroy();
};



#endif //GAME_H
