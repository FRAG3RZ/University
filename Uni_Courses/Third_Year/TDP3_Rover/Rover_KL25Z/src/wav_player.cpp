#include "wav_player.h"

WavPlayer* WavPlayer::_self = nullptr;

void WavPlayer::_isr_trampoline() {
    if (_self) _self->on_tick();
}

void WavPlayer::init(PinName dac_pin, int sample_rate_hz) {
    _fs = sample_rate_hz;
    static AnalogOut dac_obj(dac_pin); // static so it stays alive
    _dac = &dac_obj;

    _dac->write(0.5f);

    _self = this;
    // Start ticker; if not playing it just outputs mid-scale (cheap)
    _tick.attach(&WavPlayer::_isr_trampoline, 1.0f / (float)_fs);
}

void WavPlayer::play_u8(const uint8_t* data, size_t len, bool loop) {
    _data = data;
    _len = len;
    _idx = 0;
    _loop = loop;
    _playing = (data != nullptr && len > 0);
}

void WavPlayer::stop() {
    _playing = false;
    _idx = 0;
    if (_dac) _dac->write(0.5f);
}

bool WavPlayer::is_playing() const {
    return _playing;
}

void WavPlayer::on_tick() {
    if (!_dac) return;

    if (!_playing || !_data || _len == 0) {
        _dac->write(0.5f);
        return;
    }

    // Fetch sample
    uint8_t s = _data[_idx++];

    // Convert u8 PCM 0..255 to 0.0..1.0 DAC
    _dac->write(s / 255.0f);

    if (_idx >= _len) {
        if (_loop) {
            _idx = 0;
        } else {
            _playing = false;
            _idx = 0;
            _dac->write(0.5f);
        }
    }
}
