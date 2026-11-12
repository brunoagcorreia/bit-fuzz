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
    float       min, max;  // real-world range (unused for the choice parameter)
    float       def;       // default (for the choice parameter: index into kBitDepths)
    float       skew;      // 1 = linear, <1 = more resolution at the low end
    float       interval;  // 0 = continuous, 1 = whole steps
    bool        isChoice;  // true = list of options instead of a range
};

inline constexpr int kNumParams = 5;
inline constexpr ParamInfo kParams[kNumParams] = {
    //  id         name       unit    min     max      def      skew  interval  isChoice
    { "drive",  "Drive",   " dB",   0.f,    24.f,    18.f,    1.0f, 1.f,      false },
    { "bits",   "Bits",    "",      0.f,    2.f,     1.f,     1.0f, 1.f,      true  },
    { "rate",   "Rate",    " Hz",   500.f,  48000.f, 11025.f, 0.3f, 1.f,      false },
    { "tone",   "Tone",    " Hz",   500.f,  20000.f, 6000.f,  0.3f, 1.f,      false },
    { "output", "Output",  " dB",  -6.f,    3.f,    -6.f,     1.0f, 0.f,      false },
};

// The three choices of the Bits parameter, in order.
inline constexpr float kBitDepths[3] = { 4.f, 8.f, 16.f };

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
    std::atomic<float>* driveP  = nullptr;
    std::atomic<float>* bitsP   = nullptr;   // index 0..2 into kBitDepths
    std::atomic<float>* rateP   = nullptr;
    std::atomic<float>* toneP   = nullptr;
    std::atomic<float>* outputP = nullptr;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BitFuzzAudioProcessor)
};
