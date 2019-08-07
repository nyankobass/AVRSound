#include "WaveMemoryControl.h"

#include <Wire.h>
#include "Register.h"

namespace AVRSound
{

WaveMemoryControl::WaveMemoryControl()
{

}

WaveMemoryControl::~WaveMemoryControl()
{
}

void WaveMemoryControl::Initialize()
{
    {
        Wire.beginTransmission(0x30);
        uint8_t BYTE[] = {0b10000100};
        Wire.write(REGISTER::TOTAL_ADDR);
        Wire.write(BYTE, 1);

        Wire.endTransmission();
    }

    setting.set_is_sound_on(false);
    setting.set_frequency(0);
    setting.set_is_enable_length(false);
    setting.set_is_start(false);
    setting.set_length(0);
    setting.set_volume(1);
    UpdateSound();
}

void WaveMemoryControl::onKeyOn(uint8_t note_num)
{
    setting.set_is_sound_on(true);
    setting.set_frequency(FREQ_TABLE[note_num]);
    setting.set_is_enable_length(true);
    setting.set_is_start(true);
    setting.set_length(1);
    setting.set_volume(1);
    UpdateSound();
}

void WaveMemoryControl::onKeyOff() 
{
    setting.set_is_sound_on(false);
    UpdateSound();
}

void WaveMemoryControl::onTimer()
{
    
}

void WaveMemoryControl::UpdateSound()
{
    Wire.beginTransmission(0x30);

    Wire.write(REGISTER::SOUND3_ADDR);
    Wire.write(setting.BYTE, setting.BYTE_SIZE);

    Wire.endTransmission();
}

const uint16_t WaveMemoryControl::FREQ_TABLE[0xFF] = {
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 22, 137, 244, 345, 441, 531, 616, 697, 772, 844, 912, 975, 1036, 1092, 1146, 1197, 1245, 1290, 1332, 1372, 1410, 1446, 1480, 1512, 1542, 1570, 1597, 1622, 1646, 1669, 1690, 1710, 1729, 1747, 1764, 1780, 1795, 1809, 1823, 1835, 1847, 1858, 1869, 1879, 1889, 1898, 1906, 1914, 1921, 1929, 1935, 1942, 1948, 1953, 1959, 1964, 1968, 1973, 1977, 1981, 1985, 1988, 1992, 1995, 1998, 2001, 2003, 2006, 2008, 2010, 2012, 2014, 2016, 2018, 2020, 2021, 2023, 2024, 2026, 2027, 2028, 2029, 2030, 2031, 2032, 2033, 2034, 2035, 2035, 2036, 2037, 2037, 2038
};

}