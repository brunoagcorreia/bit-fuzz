# BitFuzz

A chiptune-style fuzz and bitcrusher effect plugin. It pushes a clean signal into
fuzzy clipping, then crushes it down to 4, 8 or 16 bits and a lower sample rate
with deliberate aliasing, for the gritty sound of old 8-bit hardware.

## Controls

| Control | Range |
|---|---|
| Drive | 0 to 24 dB |
| Bits | 4, 8 or 16 |
| Rate | 500 to 48000 Hz |
| Tone | 500 to 20000 Hz |
| Output | -6 to +3 dB |

- Formats: AU and VST3 (macOS)
- Built with [JUCE](https://juce.com) and CMake
- The DSP lives in a plain C++ class (`Bit8FuzzDSP.h`) with no JUCE dependency,
  so it can be reused in other plugin formats or platforms

## Build

    cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
    cmake --build build

The plugin is copied to `~/Library/Audio/Plug-Ins/` after a successful build.