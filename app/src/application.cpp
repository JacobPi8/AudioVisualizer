#include <iostream>
#include "../include/application.h"
#include "visualizer.h"


Application::Application(int width, int height) : width(width), height(height),window(nullptr), renderer(nullptr),
audio_engine(nullptr), visualizer(nullptr), running(false), paused(false), speedMultiplier(1.0f) {}
Application::~Application() {cleanup();}

bool Application::init() {

    if(!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) return false;

    window = SDL_CreateWindow("Audio Visualizer", width, height, 0);
    if(!window) return false;

    renderer = SDL_CreateRenderer(window, NULL);
    if(!renderer) return false;
    SDL_SetRenderVSync(renderer, 1);

    audio_engine = new AudioEngine();
    audio_engine->loadFromFile("../assets/audio2.wav");
    visualizer = new Visualizer(renderer, width, height);
    visualizer->setMode(Visualizer::BOTH);

    return true;
}

void Application::run() {

    running = true;
    SDL_Event event;

    float delayMS = (1024.0f - 512.0f) / audio_engine->getSampleRate() * 1000.0f;

    while(running) {

        while(SDL_PollEvent(&event)) {

            if(event.type == SDL_EVENT_QUIT) running = false;
            else if(event.type == SDL_EVENT_KEY_DOWN)
                if(event.key.key == SDLK_SPACE) paused = !paused;
                else if(event.key.key == SDLK_UP) {

                    speedMultiplier -= 0.5f;
                    if(speedMultiplier < 0.5f) speedMultiplier = 0.5f;
                }
                else if(event.key.key == SDLK_DOWN) {

                    speedMultiplier += 0.5f;
                    if(speedMultiplier > 16.0f) speedMultiplier = 16.0f;

                }
        }

        if(!paused) {

            if(audio_engine->getNextChunk(current_chunk)) {

                visualizer->render(current_chunk);
                SDL_Delay(static_cast<Uint32>(delayMS * speedMultiplier));
            }
            else running = false;
        }
        else {

            visualizer->render(current_chunk);
            SDL_Delay(16);
        }

    }
}

void Application::cleanup() {

    delete visualizer;
    delete audio_engine;

    if(renderer) SDL_DestroyRenderer(renderer);
    if(window) SDL_DestroyWindow(window);

    SDL_Quit();
}






