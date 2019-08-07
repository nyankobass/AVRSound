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

const uint16_t NoiseSoundControl::FREQ_TABLE[0xFF] = {
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 22, 137, 244, 345, 441, 531, 616, 697, 772, 844, 912, 975, 1036, 1092, 1146, 1197, 1245, 1290, 1332, 1372, 1410, 1446, 1480, 1512, 1542, 1570, 1597, 1622, 1646, 1669, 1690, 1710, 1729, 1747, 1764, 1780, 1795, 1809, 1823, 1835, 1847, 1858, 1869, 1879, 1889, 1898, 1906, 1914, 1921, 1929, 1935, 1942, 1948, 1953, 1959, 1964, 1968, 1973, 1977, 1981, 1985, 1988, 1992, 1995, 1998, 2001, 2003, 2006, 2008, 2010, 2012, 2014, 2016, 2018, 2020, 2021, 2023, 2024, 2026, 2027, 2028, 2029, 2030, 2031, 2032, 2033, 2034, 2035, 2035, 2036, 2037, 2037, 2038
};

}