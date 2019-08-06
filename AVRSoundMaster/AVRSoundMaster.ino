#include <Arduino.h>
#include <Wire.h>

#include "Register.h"

namespace{
    AVRSound::REGISTER sound_register;

    uint16_t FREQ_TABLE[12] = {
        1764 << 2,
        1795 << 2,
        1823 << 2,
        1835 << 2,
        1858 << 2,
        1879 << 2,
        1898 << 2,
        1906 << 2,
        1921 << 2,
        1935 << 2,
        1942 << 2,
        1747 << 2,
    };

    void UpdateSound1();
}

void setup() 
{
    Serial.begin(115200);

    Wire.begin();	

    sound_register.SOUND1.set_is_start(true);
    sound_register.SOUND1.set_wave_pattern(1);
    sound_register.SOUND1.set_is_enable_length(false);
    sound_register.SOUND1.set_is_envelope_increment(false);
    sound_register.SOUND1.set_frequency(1000);
    sound_register.SOUND1.set_init_volume(15);

    sound_register.SOUND3.set_is_start(true);
    sound_register.SOUND3.set_frequency(1000);
    sound_register.SOUND3.set_volume(1);

    sound_register.SOUND4.set_freq_bit_shift(8);
    sound_register.SOUND4.set_freq_divider(3);
    sound_register.SOUND4.set_is_start(1);
    sound_register.SOUND4.set_init_volume(15);
    sound_register.SOUND4.set_is_short_freq(false);
}

void loop() 
{
    static uint8_t pattern = 0;
    if (Serial.available() > 0){
        uint8_t data = Serial.read();

        uint8_t frequency_index = 0;
        switch (data)
        {
        case 'a':
            frequency_index = 0;
            break;
        case 's':
            frequency_index = 1;
            break;
        case 'd':
            frequency_index = 2;
            break;
        case 'f':
            frequency_index = 3;
            break;
        case 'g':
            frequency_index = 4;
            break;
        case 'h':
            frequency_index = 5;
            break;
        case 'j':
            frequency_index = 6;
            break;
        case 'k':
            frequency_index = 7;
            break;        
        case 'l':
            frequency_index = 8;
            break;
        case ';':
            frequency_index = 9;
            break;
        case ':':
            frequency_index = 10;
            break;        
        case 'q':
            //  sound_register.SOUND4.set_is_short_freq(!sound_register.SOUND4.is_short_freq());
            pattern = (pattern + 1) & 0b00000011;
            sound_register.SOUND1.set_wave_pattern(pattern);
            UpdateSound1();            return;
            break;
        case ' ':
            sound_register.SOUND3.set_is_start(false);
            UpdateSound1();
            return;
            break;
        default:
            return;
            break;
        }

        sound_register.SOUND3.set_is_start(true);
        // sound_register.SOUND1.set_freq_bit_shift(frequency_index);
        sound_register.SOUND3.set_frequency(FREQ_TABLE[frequency_index]);
        UpdateSound1();
    }
}

namespace{
void UpdateSound1()
{
    Wire.beginTransmission(0x30);

    Wire.write(sound_register.SOUND3_ADDR);
    Wire.write(sound_register.SOUND3.BYTE, sound_register.SOUND3.BYTE_SIZE);

    Wire.endTransmission();
}
}