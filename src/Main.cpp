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

#include "Game.h"

int main(int argc, char** argv) {
    Game game;

    game.Init();
    game.Run();
    game.Destroy();

    return 0;
}