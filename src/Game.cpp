//
// Created by mltac on 4/22/2025.
//



#include "Game.h"

Game::Game() {
    running = false;
}

Game::~Game() {

}

void Game::Init() {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return;
    }
    window = SDL_CreateWindow(
        "2D Game Engine",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        1280,
        720,
        SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        return;
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == nullptr) {
        std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
        return;
    }
    running = true;
}

void Game::ProcessInput() {  // TODO: May need work. I recall uses switch is slow. COuld be wrong.
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                running = false;
                break;
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE ) {
                    running = false;
                }
                break;
            default:
                continue;
        }
    }
}

void Game::Update() {}

void Game::Render() {
    SDL_SetRenderDrawColor(renderer, 255, 55, 255, 255);
    SDL_RenderClear(renderer);

    // TODO: Render game objects

    SDL_RenderPresent(renderer);
}

void Game::Run() {
    while (running == true) {
        ProcessInput();
        Update();
        Render();
    }

}

void Game::Destroy() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

