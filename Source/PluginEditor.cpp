#include "PluginEditor.h"

namespace
{
    const juce::Colour bgColour      { 0xff1e1a24 };
    const juce::Colour textColour    { 0xfff0c674 };

    // Dial colours
    const juce::Colour dialFill      { 0xffe53935 };   // value arc: red
    const juce::Colour dialThumb     { 0xffff6f61 };   // pointer: lighter red
    const juce::Colour dialTrack     { 0xff5a1a1a };   // empty part of the arc: dark red
}

BitFuzzAudioProcessorEditor::BitFuzzAudioProcessorEditor (BitFuzzAudioProcessor& p)
    : juce::AudioProcessorEditor (&p), audioProcessor (p)
{
    for (size_t i = 0; i < knobs.size(); ++i)
    {
        auto& k = knobs[i];
        const auto& info = kParams[i];

        k.label.setText (info.name, juce::dontSendNotification);
        k.label.setJustificationType (juce::Justification::centred);
        k.label.setColour (juce::Label::textColourId, textColour);
        addAndMakeVisible (k.label);

        k.slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 72, 18);
        k.slider.setColour (juce::Slider::textBoxTextColourId, textColour);
        k.slider.setColour (juce::Slider::rotarySliderFillColourId, dialFill);
        k.slider.setColour (juce::Slider::rotarySliderOutlineColourId, dialTrack);
        k.slider.setColour (juce::Slider::thumbColourId, dialThumb);
        addAndMakeVisible (k.slider);

        k.attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
            audioProcessor.apvts, info.id, k.slider);
    }

    setSize (460, 220);
}

void BitFuzzAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (bgColour);

    g.setColour (textColour);
    g.setFont (20.0f);
    g.drawText ("BitFuzz", getLocalBounds().removeFromTop (40).reduced (14, 0),
                juce::Justification::centredLeft);
}

void BitFuzzAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced (12);
    area.removeFromTop (30);                       // room for the title

    const int colWidth = area.getWidth() / (int) knobs.size();

    for (auto& k : knobs)
    {
        auto col = area.removeFromLeft (colWidth);
        k.label.setBounds (col.removeFromTop (20));
        k.slider.setBounds (col);
    }
}
