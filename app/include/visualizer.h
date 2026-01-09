#ifndef VISUALIZER_H
#define VISUALIZER_H

#include <SDL3/SDL.h>
#include "audio_engine.h"

class Visualizer {
public:
    enum Mode {
        WAVEFORM,
        SPECTRUM,
    };

    Visualizer(SDL_Renderer* renderer, int width, int height);
    ~Visualizer();

    void setAudioSource(const AudioEngine* source);
    void setMode(Mode mode);
    void resize(int width, int height);

    void render();

    void setColor(SDL_Color color);
    void setBackground(SDL_Color color);

private:
    SDL_Renderer* renderer;
    const AudioEngine* audio_source;
    Mode current_mode;
    int width, height;
    SDL_Color line_color;
    SDL_Color backg_color;

    void renderWaveform();
    void renderSpectrum();

    void drawGrid();
    void drawAxes();
    void drawText(const char* text, int x, int y);
};

#endif //VISUALIZER_H
