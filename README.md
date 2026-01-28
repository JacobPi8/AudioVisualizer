# Audio Visualizer FFT

Real-time audio visualizer written in **C++**, using **FFT (Fast Fourier Transform)** to analyze audio signals and generate graphical visualizations.

<p align="center">
<img width="80%" alt="Preview" src="https://github.com/JacobPi8/AudioVisualizer/assets/preview.gif">
</p>

## Features

- **7 frequency bands** with logarithmic scaling (Sub-Bass → Treble)
- **Dual visualization**: Waveform + Spectrum analyzer
- **Color-coded** frequency ranges (purple → blue)
- **Adjustable playback speed** (0.5x - 16x)

## Tools & Sources

- [Git](https://git-scm.com/)
- [Visual Studio (MSVC) 2022+](https://visualstudio.microsoft.com/)
- [CMake](https://cmake.org/)
- [SDL3](https://www.libsdl.org/)
- [FFT](https://rosettacode.org/wiki/Fast_Fourier_transform)

### Ubuntu / Debian
```sh
sudo apt update
sudo apt install build-essential cmake git libsdl3-dev
```

## Build Instructions

```sh
# Clone this repository
git clone https://github.com/JacobPi8/AudioVisualizer
cd AudioVisualizer

# Configure
cmake -S . -B build

# Build
cmake --build build --config Release

# Run
build\Release\AudioVisualizer.exe

# Run (Linux/macOS)
./build/AudioVisualizer
```

### ⚠️ Currently supports only .wav files (16-bit PCM).

## Controls

- #### SPACE | Pause / Resume
- #### ARROW ↑ (UP)	| Increase speed (decrease delay)
- #### ARROW ↓ (DOWN) | Decrease speed (increase delay)

## 🚧 **Work in Progress** 🚧