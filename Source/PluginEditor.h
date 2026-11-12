#pragma once

#include <array>
#include <memory>
#include "PluginProcessor.h"

class BitFuzzAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit BitFuzzAudioProcessorEditor (BitFuzzAudioProcessor&);
    ~BitFuzzAudioProcessorEditor() override = default;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    struct Knob
    {
        juce::Slider slider { juce::Slider::RotaryHorizontalVerticalDrag,
                              juce::Slider::TextBoxBelow };
        juce::Label label;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attachment;
    };

    BitFuzzAudioProcessor& audioProcessor;
    std::array<Knob, kNumParams> knobs;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BitFuzzAudioProcessorEditor)
};
