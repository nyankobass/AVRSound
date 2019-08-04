#include <Arduino.h>
#include <Wire.h>

#include "Register.h"

namespace{
    AVRSound::REGISTER sound_register;

    uint16_t FREQ_TABLE[12] = {
        1764,
        1795,
        1823,
        1835,
        1858,
        1879,
        1898,
        1906,
        1921,
        1935,
        1942, 1747,
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
}

void loop() 
{
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
            frequency_index = 11;
            break;
        case ' ':
            sound_register.SOUND1.set_is_start(false);
            UpdateSound1();
            return;
            break;
        default:
            return;
            break;
        }

        sound_register.SOUND1.set_is_start(true);
        sound_register.SOUND1.set_frequency(FREQ_TABLE[frequency_index]);
        UpdateSound1();
    }
}

namespace{
void UpdateSound1()
{
    Wire.beginTransmission(0x30);

    Wire.write(sound_register.SOUND1_ADDR);
    Wire.write(sound_register.SOUND1.BYTE, sound_register.SOUND1.BYTE_SIZE);

    Wire.endTransmission();
}
}