#pragma once
#include <algorithm>
#include <cmath>
#include <vector>

// Parameters are in natural units (not 0..1), the plugin wrapper maps them
// from its own parameter ranges.
struct Bit8FuzzParams {
    float driveDb  = 18.f;      // 0 .. 48    pre-clip gain
    float bias     = 0.10f;     // -1 .. 1    0 = symmetric; more = even harmonics, then gated "sputter"
    float hardness = 0.6f;      // 0 .. 1     0 = soft (tanh), 1 = hard clip (towards square wave)
    float bits     = 8.f;       // 1 .. 16    quantizer resolution (fractional values allowed)
    float rateHz   = 11025.f;   // 500 .. sr  sample-and-hold rate (aliasing is intentional)
    float toneHz   = 6000.f;    // 500 .. 20000  post low-pass to tame the fizz
    float mix      = 1.f;       // 0 .. 1     dry/wet
    float outDb    = -6.f;      // -24 .. 6   output level
};

class Bit8FuzzDSP {
public:
    Bit8FuzzParams p;

    void prepare(double sampleRate, int numChannels) {
        sr = (float) sampleRate;
        held.assign(numChannels, 0.f);
        dc.assign(numChannels, 0.f);
        lp.assign(numChannels, 0.f);
        phase = 1.f;                         // capture a sample right away
    }

    void process(float* const* io, int numCh, int n) {
        const float twoPi = 6.2831853f;

        const float g     = dbToGain(p.driveDb);
        const float outG  = dbToGain(p.outDb);
        const float step  = 2.f / std::exp2(std::max(1.f, p.bits));   // quantizer step over -1..1
        const float inv   = 1.f / step;
        const float inc   = std::min(1.f, std::max(100.f, p.rateHz) / sr);
        const float aDc   = 1.f - std::exp(-twoPi * 20.f / sr);
        const float tone  = std::min(std::max(p.toneHz, 200.f), 0.45f * sr);
        const float aLp   = 1.f - std::exp(-twoPi * tone / sr);

        for (int i = 0; i < n; ++i) {
            // Sample-and-hold clock, shared by all channels
            phase += inc;
            const bool grab = phase >= 1.f;
            if (grab) phase -= 1.f;

            for (int ch = 0; ch < numCh; ++ch) {
                const float dry = io[ch][i];

                // 1) fuzz: bias is added BEFORE the gain, so at high drive it
                //    starves one side of the wave (classic gated fuzz)
                float x = (dry + p.bias * 0.5f) * g;
                x = fuzzClip(x, p.hardness);

                // 2) remove the DC offset the bias just created
                dc[ch] += aDc * (x - dc[ch]);
                x -= dc[ch];

                // 3) bit crush + sample-rate reduction (mid-tread quantizer)
                if (grab) held[ch] = std::floor(x * inv + 0.5f) * step;
                float y = held[ch];

                // 4) tone
                lp[ch] += aLp * (y - lp[ch]);
                y = lp[ch];

                io[ch][i] = (dry * (1.f - p.mix) + y * p.mix) * outG;
            }
        }
    }

private:
    float sr = 44100.f, phase = 1.f;
    std::vector<float> held, dc, lp;

    static float dbToGain(float db) { return std::pow(10.f, db * 0.05f); }

    static float fuzzClip(float x, float hardness) {
        const float soft = std::tanh(x);
        const float hard = std::max(-1.f, std::min(1.f, x));
        return soft + hardness * (hard - soft);
    }
};
