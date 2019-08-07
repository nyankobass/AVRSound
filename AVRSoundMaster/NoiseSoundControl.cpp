#include "NoiseSoundControl.h"

#include <Wire.h>
#include "Register.h"

namespace AVRSound
{

NoiseSoundControl::NoiseSoundControl()
{

}

NoiseSoundControl::~NoiseSoundControl()
{
}

void NoiseSoundControl::Initialize()
{
    {
        Wire.beginTransmission(0x30);
        uint8_t BYTE[] = {0b10001000};
        Wire.write(REGISTER::TOTAL_ADDR);
        Wire.write(BYTE, 1);

        Wire.endTransmission();
    }

    setting.set_envelope_step_time(0);
    setting.set_freq_bit_shift(0xFF);
    setting.set_freq_divider(0xFF);
    setting.set_init_volume(0x00);
    setting.set_is_enable_length(false);
    setting.set_is_envelope_increment(false);
    setting.set_is_short_freq(false);
    setting.set_is_start(false);
    setting.set_length(0);
    UpdateSound();
}

void NoiseSoundControl::onKeyOn(uint8_t note_num)
{
    setting.set_is_start(true);
    setting.set_is_envelope_increment(false);
    setting.set_envelope_step_time(1);
    setting.set_freq_bit_shift(16 - ((note_num >> 3) & 0x0F));
    setting.set_freq_divider(8 - note_num & 0b00000111);
    setting.set_init_volume(0xFF);
    UpdateSound();
}

void NoiseSoundControl::onKeyOff() 
{
    UpdateSound();
}

void NoiseSoundControl::onTimer()
{
    
}

void NoiseSoundControl::UpdateSound()
{
    Wire.beginTransmission(0x30);

    Wire.write(REGISTER::SOUND4_ADDR);
    Wire.write(setting.BYTE, setting.BYTE_SIZE);

    Wire.endTransmission();
}

}