#include "visualizer.h"
#include <algorithm>


Visualizer::Visualizer(SDL_Renderer* renderer, int width, int height) :
renderer(renderer), audio_source(nullptr), current_mode(WAVEFORM), width(width), height(height){

    line_color = {255, 225, 32, 255};
    backg_color = {18, 18, 18, 255};
}

Visualizer::~Visualizer() {}
void Visualizer::setAudioSource(const AudioEngine* source){ audio_source = source; }
void Visualizer::setMode(Mode mode){ current_mode = mode; }
void Visualizer::setColor(SDL_Color color){ line_color = color; }
void Visualizer::setBackground(SDL_Color color){ backg_color = color; }

void Visualizer::resize(int w, int h){

    width = w;
    height = h;
}

void Visualizer::render(const AudioEngine::AudioChunk& chunk){

    SDL_SetRenderDrawColor(renderer, backg_color.r, backg_color.g, backg_color.b, backg_color.a);
    SDL_RenderClear(renderer);

    drawGrid();
    drawAxes();

    if(current_mode == WAVEFORM) renderWaveform(chunk);
    else if(current_mode == SPECTRUM) renderSpectrum(chunk);
    else if(current_mode == BOTH) {

        renderWaveform(chunk);
        renderSpectrum(chunk);
    }

    SDL_RenderPresent(renderer);
}

void Visualizer::drawGrid(){

    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);

    int stepY = std::max(1, height/10);

    for(int y = 0; y < height; y += stepY)
        SDL_RenderLine(renderer, 0, y, width, y);

    int stepX = std::max(1, width/20);

    for(int x = 0; x < width; x += stepX)
        SDL_RenderLine(renderer, x, 0, x, height);

}

void Visualizer::drawAxes(){

    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);

    int axisX = height / 2;
    SDL_RenderLine(renderer, 0, axisX, width, axisX);

    int axisY = width / 2;
    SDL_RenderLine(renderer, axisY, 0, axisY, height);
}

void Visualizer::renderWaveform(const AudioEngine::AudioChunk& chunk) {

    if(chunk.samples.empty()) return;

    SDL_SetRenderDrawColor(renderer, line_color.r, line_color.g, line_color.b, line_color.a);

    float scale = static_cast<float>(width) / (chunk.samples.size() - 1);
    std::vector<SDL_FPoint> points(chunk.samples.size());

    for(size_t i = 0; i < chunk.samples.size(); i++) {

        points[i].x = i * scale;
        points[i].y = (height / 2.0f) - (chunk.samples[i] * (height * 0.4f));
    }

    SDL_RenderLines(renderer, points.data(), (int)points.size());
}

void Visualizer::renderSpectrum(const AudioEngine::AudioChunk& chunk) {

    if(chunk.spectrum.empty()) return;

    SDL_SetRenderDrawColor(renderer, line_color.r, line_color.g, line_color.b, line_color.a);

    float scale = static_cast<float>(width) / chunk.spectrum.size();
    int barWidth = std::max(1, static_cast<int>(scale));

    for(size_t i = 0; i < chunk.spectrum.size(); i++) {

        float magnitude = chunk.spectrum[i];
        int barHeight = static_cast<int>((magnitude + 60.0f) / 60.0f * height * 0.8f);
        barHeight = std::max(0, std::min(barHeight, height));

        int x = static_cast<int>(i * scale);
        int y = height - barHeight;

        int r = std::min(255, static_cast<int>(255.0f * i / chunk.spectrum.size()));
        int g = 255 - r;
        int b = 50;

        SDL_SetRenderDrawColor(renderer, r, g, b, 255);

        SDL_FRect bar = {
            static_cast<float>(x),
            static_cast<float>(y),
            static_cast<float>(barWidth),
            static_cast<float>(barHeight)
        };

        SDL_RenderFillRect(renderer, &bar);
    }
}