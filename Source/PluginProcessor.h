#pragma once

#include <array>
#include <atomic>
#include <juce_audio_processors/juce_audio_processors.h>
#include "Bit8FuzzDSP.h"

// Single source of truth for the parameters. The editor builds its knobs from
// this table, the processor builds its parameter layout from it.
struct ParamInfo
{
    const char* id;        // stable ID, never rename once released (breaks saved projects)
    const char* name;      // display name
    const char* unit;      // shown after the value ("" = none)
    float       min, max;  // real-world range
    float       def;       // default, in real-world units
    float       skew;      // 1 = linear, <1 = more resolution at the low end
    float       interval;  // 0 = continuous
};

inline constexpr int kNumParams = 8;
inline constexpr ParamInfo kParams[kNumParams] = {
    //  id          name        unit   min     max      def      skew  interval
    { "drive",    "Drive",     " dB",  0.f,    48.f,    18.f,    1.0f, 0.f },
    { "bias",     "Bias",      "",    -1.f,    1.f,     0.1f,    1.0f, 0.f },
    { "hardness", "Hardness",  "",     0.f,    1.f,     0.6f,    1.0f, 0.f },
    { "bits",     "Bits",      " bit", 1.f,    16.f,    8.f,     1.0f, 1.f },
    { "rate",     "Rate",      " Hz",  500.f,  48000.f, 11025.f, 0.3f, 0.f },
    { "tone",     "Tone",      " Hz",  500.f,  20000.f, 6000.f,  0.3f, 0.f },
    { "mix",      "Mix",       "",     0.f,    1.f,     1.f,     1.0f, 0.f },
    { "output",   "Output",    " dB", -24.f,   6.f,    -6.f,     1.0f, 0.f },
};

class BitFuzzAudioProcessor : public juce::AudioProcessor
{
public:
    BitFuzzAudioProcessor();
    ~BitFuzzAudioProcessor() override = default;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return JucePlugin_Name; }
    bool acceptsMidi() const override  { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState apvts;

private:
    static juce::AudioProcessorValueTreeState::ParameterLayout createLayout();

    Bit8FuzzDSP dsp;
    int preparedChannels = 2;

    // Cached raw pointers: reading them on the audio thread is lock-free.
    std::atomic<float>* driveP    = nullptr;
    std::atomic<float>* biasP     = nullptr;
    std::atomic<float>* hardnessP = nullptr;
    std::atomic<float>* bitsP     = nullptr;
    std::atomic<float>* rateP     = nullptr;
    std::atomic<float>* toneP     = nullptr;
    std::atomic<float>* mixP      = nullptr;
    std::atomic<float>* outputP   = nullptr;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BitFuzzAudioProcessor)
};
