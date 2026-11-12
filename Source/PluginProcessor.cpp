#include "PluginProcessor.h"
#include "PluginEditor.h"

BitFuzzAudioProcessor::BitFuzzAudioProcessor()
    : AudioProcessor (BusesProperties()
                          .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                          .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "PARAMETERS", createLayout())
{
    driveP  = apvts.getRawParameterValue ("drive");
    bitsP   = apvts.getRawParameterValue ("bits");
    rateP   = apvts.getRawParameterValue ("rate");
    toneP   = apvts.getRawParameterValue ("tone");
    outputP = apvts.getRawParameterValue ("output");
}

juce::AudioProcessorValueTreeState::ParameterLayout BitFuzzAudioProcessor::createLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    for (const auto& p : kParams)
    {
        if (p.isChoice)
        {
            layout.add (std::make_unique<juce::AudioParameterChoice> (
                juce::ParameterID { p.id, 1 },
                p.name,
                juce::StringArray { "4 bit", "8 bit", "16 bit" },
                (int) p.def));
        }
        else
        {
            layout.add (std::make_unique<juce::AudioParameterFloat> (
                juce::ParameterID { p.id, 1 },          // version hint, needed for AU
                p.name,
                juce::NormalisableRange<float> (p.min, p.max, p.interval, p.skew),
                p.def,
                juce::AudioParameterFloatAttributes().withLabel (p.unit)));
        }
    }

    return layout;
}

void BitFuzzAudioProcessor::prepareToPlay (double sampleRate, int /*samplesPerBlock*/)
{
    preparedChannels = juce::jmax (1, getTotalNumOutputChannels());
    dsp.prepare (sampleRate, preparedChannels);
}

bool BitFuzzAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    const auto& out = layouts.getMainOutputChannelSet();

    if (out != juce::AudioChannelSet::mono() && out != juce::AudioChannelSet::stereo())
        return false;

    return out == layouts.getMainInputChannelSet();
}

void BitFuzzAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    // Clear any output channels that have no matching input
    for (int ch = getTotalNumInputChannels(); ch < getTotalNumOutputChannels(); ++ch)
        buffer.clear (ch, 0, buffer.getNumSamples());

    // Copy the current parameter values (already in real units) into the DSP core
    auto& p = dsp.p;
    p.driveDb = driveP->load();
    p.rateHz  = rateP->load();
    p.toneHz  = toneP->load();
    p.outDb   = outputP->load();

    const int bitsIndex = juce::jlimit (0, 2, (int) std::lround (bitsP->load()));
    p.bits = kBitDepths[bitsIndex];

    const int numCh = juce::jmin (buffer.getNumChannels(), preparedChannels);
    dsp.process (buffer.getArrayOfWritePointers(), numCh, buffer.getNumSamples());
}

juce::AudioProcessorEditor* BitFuzzAudioProcessor::createEditor()
{
    return new BitFuzzAudioProcessorEditor (*this);
}

void BitFuzzAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    if (auto xml = apvts.copyState().createXml())
        copyXmlToBinary (*xml, destData);
}

void BitFuzzAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    if (auto xml = getXmlFromBinary (data, sizeInBytes))
        if (xml->hasTagName (apvts.state.getType()))
            apvts.replaceState (juce::ValueTree::fromXml (*xml));
}

// This creates new instances of the plugin; required by JUCE.
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new BitFuzzAudioProcessor();
}
