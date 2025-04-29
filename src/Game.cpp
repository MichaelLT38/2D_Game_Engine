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
    SDL_DisplayMode current;
    SDL_GetCurrentDisplayMode(0, &current);
    windowWidth = 1280;
    windowHeight = 720;
    window = SDL_CreateWindow(
        "2D Game Engine",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        windowWidth,
        windowHeight,
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
    SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
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

glm::vec2 playerPosition;
glm::vec2 playerVelocity;

void Game::Setup() {
    playerPosition = glm::vec2(10.0, 20.0);
    playerVelocity = glm::vec2(30.0, 20.0);
}

void Game::Update() {

    /*int waitTime = MS_PER_FRAME - (milliseconds - SDL_GetTicks());
    if (waitTime > 0 && waitTime <= MS_PER_FRAME) {
        SDL_Delay(waitTime);
    }*/
    double deltaTime = (SDL_GetTicks() - milliseconds) / 1000.0;
    milliseconds = SDL_GetTicks();

    playerPosition.x += playerVelocity.x * deltaTime;
    playerPosition.y += playerVelocity.y * deltaTime;
}

void Game::Render() {
    SDL_SetRenderDrawColor(renderer, 25, 25, 25, 255);
    SDL_RenderClear(renderer);

    SDL_Surface* surface = IMG_Load("./assets/images/tank-tiger-right.png");
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    SDL_Rect dstrect = {
        static_cast<int>(playerPosition.x),
        static_cast<int>(playerPosition.y),
        32,
        32
    };
    SDL_RenderCopy(renderer, texture, NULL, &dstrect);
    SDL_DestroyTexture(texture);

    SDL_RenderPresent(renderer);
}

void Game::Run() {
    Setup();
    while (running == true) {
        ProcessInput();
        Update();
        Render();
    }

}

void Game::CleanUp() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

