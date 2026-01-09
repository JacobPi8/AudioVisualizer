#include "audio_engine.h"
#include <SDL3/SDL.h>
#include <iostream>
#include <cmath>

constexpr double PI = 3.14159265358979323846;

AudioEngine::AudioEngine() {}
AudioEngine::~AudioEngine() { a_samples.clear(); }

bool AudioEngine::loadFromFile(const std::string& filepath) {

    a_samples.clear();
    a_loaded = false;
    a_cursor = 0;

    SDL_AudioSpec spec;
    Uint8* buffer = nullptr;
    Uint32 length = 0;

    if(!SDL_LoadWAV(filepath.c_str(), &spec, &buffer, &length)) {

        std::cerr << "Cannot load WAV: " << SDL_GetError() << std::endl;
        return false;
    }

    // zakładamy 16-bit PCM
    if(spec.format != SDL_AUDIO_S16) {

        std::cerr << "Only 16-bit PCM WAV files are supported" << std::endl;
        SDL_free(buffer);
        return false;
    }

    a_sampleRate = spec.freq;
    a_channels = spec.channels;

    Sint16* ptr = reinterpret_cast<Sint16*>(buffer);
    size_t sampleCount = length / (spec.channels * sizeof(Sint16));
    a_samples.reserve(sampleCount);

    // mono
    for(size_t i = 0; i < sampleCount * a_channels; i += a_channels) {

        float sum = 0;
        for(int c = 0; c < a_channels; c++)
            sum += ptr[i + c] / 32768.0f;
        a_samples.push_back(sum / a_channels);
    }

    SDL_free(buffer);
    a_cursor = 0;
    a_loaded = true;
    return true;
}

void AudioEngine::setFFTSize(size_t size) {

    size_t pow2 = 1;
    while(pow2 < size) pow2 <<= 1;
    a_fftSize = pow2;
}

// overlap < fftsize
void AudioEngine::setOverlap(size_t overlap) {

    a_overlap = std::min(overlap, a_fftSize - 1);
}

void AudioEngine::applyWindow(std::vector<float>& samples) {

    size_t N = samples.size();

    for(size_t i = 0; i < samples.size(); i++)
        if(N > 1) samples[i] *= 0.5f * (1.0f - std::cos(2.0f * PI * i / (static_cast<float>(N) - 1.0f)));
}

void AudioEngine::fft(std::vector<Complex>& x){

    size_t N = x.size(), k = N, n;
    double thetaT = PI / static_cast<double>(N);
    Complex phiT = Complex(std::cos(thetaT), -std::sin(thetaT)), T;

    while(k > 1){

        n = k;
        k >>= 1;
        phiT *= phiT;
        T = Complex(1.0f, 0.0f);

        for(size_t l = 0; l < k; l++){

            for(size_t a = l; a < N; a += n){

                size_t b = a + k;
                Complex t = x[a] - x[b];
                x[a] += x[b];
                x[b] = t * T;
            }
            T *= phiT;
        }
    }

    size_t m = static_cast<size_t>(std::log2(N));
    for(size_t a = 0; a < N; a++){

        size_t b = a;
        b = ((b & 0xaaaaaaaa) >> 1) | ((b & 0x55555555) << 1);
        b = ((b & 0xcccccccc) >> 2) | ((b & 0x33333333) << 2);
        b = ((b & 0xf0f0f0f0) >> 4) | ((b & 0x0f0f0f0f) << 4);
        b = ((b & 0xff00ff00) >> 8) | ((b & 0x00ff00ff) << 8);
        b = ((b >> 16) | (b << 16)) >> (32 - m);
        if(b > a){

            Complex t = x[a];
            x[a] = x[b];
            x[b] = t;
        }
    }

}



std::vector<float> AudioEngine::computeMagnitudes(const std::vector<Complex>& fft) {

    std::vector<float> magDB(fft.size() / 2);
    const float eps = 1e-9f;
    const float scale = 2.0f / fft.size();

    for(size_t i = 0; i < fft.size() / 2; i++) {

        float mag = std::abs(fft[i]) * scale;
        magDB[i] = 20.0f * std::log10(mag + eps);
    }

    return magDB;
}

bool AudioEngine::getNextChunk(AudioChunk& outChunk) {

    if(!a_loaded || a_cursor >= a_samples.size()) return false;

    size_t end = std::min(a_cursor + a_fftSize, a_samples.size());
    outChunk.samples.assign(a_samples.begin() + a_cursor, a_samples.begin() + end);

    if(outChunk.samples.size() < a_fftSize)
        outChunk.samples.resize(a_fftSize, 0.0f);

    applyWindow(outChunk.samples);
    outChunk.fft.resize(a_fftSize);

    for(size_t i = 0; i < a_fftSize; i++)
        outChunk.fft[i] = Complex(outChunk.samples[i], 0);

    fft(outChunk.fft);
    outChunk.spectrum = computeMagnitudes(outChunk.fft);
    a_cursor += (a_fftSize - a_overlap);

    return true;
}

bool AudioEngine::hasAudio() const { return a_loaded && !a_samples.empty(); }
int AudioEngine::getSampleRate() const { return a_sampleRate; }
int AudioEngine::getChannels() const { return a_channels; }
