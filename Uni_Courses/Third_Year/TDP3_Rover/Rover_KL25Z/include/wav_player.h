#pragma once
#include "mbed.h"
#include <cstdint>
#include <cstddef>

struct WavPlayer {
    // Call once at startup
    void init(PinName dac_pin, int sample_rate_hz = 8000);

    // Start playing an 8-bit unsigned PCM buffer (0..255)
    // If loop=true it will repeat until stop() is called.
    void play_u8(const uint8_t* data, size_t len, bool loop = false);

    // Stop playback (DAC returns to mid-scale)
    void stop();

    // True while playing (or looping)
    bool is_playing() const;

private:
    void on_tick();

    AnalogOut* _dac = nullptr;
    Ticker _tick;

    const uint8_t* _data = nullptr;
    size_t _len = 0;

    volatile size_t _idx = 0;
    volatile bool _playing = false;
    volatile bool _loop = false;

    int _fs = 8000;

    // Singleton-style ISR trampoline
    static WavPlayer* _self;
    static void _isr_trampoline();
};
