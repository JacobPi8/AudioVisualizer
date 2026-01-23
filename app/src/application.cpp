#include <iostream>
#include "../include/application.h"
#include "visualizer.h"


Application::Application(int width, int height) : width(width), height(height),window(nullptr), renderer(nullptr),
audio_engine(nullptr), visualizer(nullptr), running(false) {}
Application::~Application() {cleanup();}

bool Application::init() {

    if(!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) return false;

    window = SDL_CreateWindow("Audio Visualizer", width, height, 0);
    if(!window) return false;

    renderer = SDL_CreateRenderer(window, NULL);
    if(!renderer) return false;

    audio_engine = new AudioEngine();
    visualizer = new Visualizer(renderer, width, height);

    return true;
}

void Application::run() {

    running = true;
    SDL_Event event;

    while(running) {

        while(SDL_PollEvent(&event)) {

            if(event.type == SDL_EVENT_QUIT) running = false;
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






