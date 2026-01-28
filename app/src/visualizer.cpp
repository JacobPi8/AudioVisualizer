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

    int waveformHeight = (current_mode == BOTH) ? height / 2 : height;
    int waveformCenter = waveformHeight / 2;

    for(size_t i = 0; i < chunk.samples.size(); i++) {

        points[i].x = i * scale;
        points[i].y = waveformCenter - (chunk.samples[i] * (waveformHeight * 0.4f));
    }

    SDL_RenderLines(renderer, points.data(), (int)points.size());
}

void Visualizer::renderSpectrum(const AudioEngine::AudioChunk& chunk) {

    if(chunk.spectrum.empty()) return;

    int totalBars = 0;
    const float SAMPLE_RATE = static_cast<float>(chunk.sampleRate);
    const float FFT_SIZE = chunk.spectrum.size() * 2;
    const float MIN_DB = -90.0f;
    const float MAX_DB = -10.0f;

    std::vector<FrequencyBand> bands = {
        {"SUB-BASS",  20.0f,   60.0f,   16, {200, 0,   255, 255}}, // Purple
        {"BASS",      60.0f,  250.0f,   20, {255, 0,   127, 255}}, // Pink
        {"LOW MID",  250.0f,  500.0f,   12, {255, 127, 0,   255}}, // Orange
        {"MIDRANGE", 500.0f, 2000.0f,   24, {255, 255, 0,   255}}, // Yellow
        {"HIGH MID",2000.0f, 4000.0f,   20, {0,   255, 0,   255}}, // Green
        {"PRESENCE",4000.0f, 6000.0f,   16, {0,   255, 255, 255}}, // Cyan
        {"TREBLE",   6000.0f,20000.0f,  20, {0,   100, 255, 255}}  // Blue
    };

    for(auto& band : bands) totalBars += band.numBars;
    float barWidth = static_cast<float>(width) / totalBars;
    int currentBar = 0;

    for(size_t idx = 0; idx < bands.size(); idx++) {

        FrequencyBand& band = bands[idx];

        float logMin = std::log10(band.minFreq);
        float logMax = std::log10(band.maxFreq);
        float logStep = (logMax - logMin) / band.numBars;

        for(int i = 0; i < band.numBars; i++) {

            float freqMin = std::pow(10.0f, logMin + i * logStep);
            float freqMax = std::pow(10.0f, logMin + (i + 1) * logStep);

            int binMin = static_cast<int>((freqMin * FFT_SIZE) / SAMPLE_RATE);
            int binMax = static_cast<int>((freqMax * FFT_SIZE) / SAMPLE_RATE);

            binMin = std::max(0, std::min(binMin, static_cast<int>(chunk.spectrum.size())));
            binMax = std::max(0, std::min(binMax, static_cast<int>(chunk.spectrum.size())));

            float avgMagnitude = -140.0f;

            if(binMax > binMin) {

                float sum = 0.0f;
                int count = 0;

                for(int bin = binMin; bin < binMax; bin++) {

                    sum += chunk.spectrum[bin];
                    count++;
                }

                avgMagnitude = sum / count;
            }
            else if(binMin == binMax && binMin < chunk.spectrum.size()) avgMagnitude = chunk.spectrum[binMin];

            float normalized = (avgMagnitude - MIN_DB) / (MAX_DB - MIN_DB);
            normalized = std::max(0.0f, std::min(1.0f, normalized));
            normalized = std::pow(normalized, 1.5f);

            int barHeight = static_cast<int>(normalized * height * 0.45f);

            float x = currentBar * barWidth;
            int y = height - barHeight;

            float gradientFactor = static_cast<float>(i) / band.numBars;

            int r = static_cast<int>(band.color.r * (1.0f - gradientFactor * 0.3f));
            int g = static_cast<int>(band.color.g * (1.0f - gradientFactor * 0.3f));
            int b = static_cast<int>(band.color.b * (1.0f - gradientFactor * 0.3f));

            SDL_SetRenderDrawColor(renderer, r, g, b, 255);

            SDL_FRect barRect = {
                x + 1.0f,
                static_cast<float>(y),
                barWidth - 2.0f,
                static_cast<float>(barHeight)
            };

            SDL_RenderFillRect(renderer, &barRect);

            currentBar++;
        }
    }
}