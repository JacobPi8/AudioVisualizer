#ifndef AUDIOENGINE_H
#define AUDIOENGINE_H

#include <vector>
#include <complex>
#include <string>
#include <SDL3/SDL.h>

class AudioEngine {
public:
    using Complex = std::complex<float>;

    struct AudioChunk {

        std::vector<float> samples;
        std::vector<Complex> fft;
        std::vector<float> spectrum;
        int sampleRate;
    };

    AudioEngine();
    ~AudioEngine();

    bool loadFromFile(const std::string& filepath);

    void setFFTSize(size_t size);
    void setOverlap(size_t overlap);

    bool getNextChunk(AudioChunk& outChunk);

    bool hasAudio() const;
    int getSampleRate() const;
    int getChannels() const;

    bool startSound();
    void stopSound();

private:
    std::vector<float> a_samples;
    size_t a_cursor = 0;
    int a_sampleRate = 0;
    int a_channels = 0;
    size_t a_fftSize = 1024;
    size_t a_overlap = 512;

    void applyWindow(std::vector<float>& samples);
    void fft(std::vector<Complex>& data);
    std::vector<float> computeMagnitudes(const std::vector<Complex>& fft);

    bool a_loaded = false;

    SDL_AudioStream* audioStream = nullptr;
    SDL_AudioDeviceID audioDevice = 0;
    size_t soundCursor = 0;
};

#endif