#ifndef APPLICATION_H
#define APPLICATION_H
#include <SDL3/SDL.h>
#include "audio_engine.h"

//forward declaration
class Visualizer;

class Application {
private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    AudioEngine* audio_engine;
    AudioEngine:: AudioChunk current_chunk;
    Visualizer* visualizer;
    bool running;
    bool paused;
    float speedMultiplier;
    int width, height;

public:
    Application(int w = 1280, int h = 720);
    ~Application();

    bool init();
    void run();
    void cleanup();

private:
    void handleEvents();
    void update(float dt);
    void render();
};

#endif //APPLICATION_H
