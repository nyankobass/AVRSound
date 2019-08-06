#include <Arduino.h>
#include <Wire.h>

#include "Register.h"
#include "ISoundControl.h"
#include "SquareSoundControl.h"

namespace{
    uint8_t KEY_TO_NOTE[28] = {
        0, 0, 0, 2, 1, 4, 5, 7, 10, 9, 11, 12, 0, 0, 0, 0, 0, 3, 0, 0, 8, 0, 0, 0, 6, 0
    };

    AVRSound::REGISTER sound_register;

    AVRSound::SquareSoundControl square_sound_control;

    AVRSound::ISoundControl* sound_control = &square_sound_control;
}

void setup() 
{
    Serial.begin(115200);

    Wire.begin();	

    sound_register.TOTAL.BIT.is_output_enable = 1;
    Wire.beginTransmission(0x30);

    Wire.write(AVRSound::REGISTER::TOTAL_ADDR);
    Wire.write(sound_register.TOTAL.BYTE, 1);

    Wire.endTransmission();


    sound_control->Initialize();
}

void loop() 
{
    static uint8_t shift = 0;
    static uint8_t oct = 48;

    if (Serial.available() > 0){
        uint8_t data = Serial.read();

        uint8_t note_number = 0;

        if (data == 'z'){
            oct = max(oct - 12, 36);
        }
        else if (data == 'x'){
            oct = min(oct + 12, 120);
        }

        else if (data == 'c'){
            shift = (shift - 1) & 0x0F; 
        }
        else if (data == 'v'){
            shift = (shift + 1) & 0x0F;
        }

        else if (data >= 'a' && data <= 'z'){
            note_number = KEY_TO_NOTE[data - 'a'] + oct + shift;
            sound_control->onKeyOn(note_number);
        }

        else if (data == ' '){
            sound_control->onKeyOff();
        }
    }
}
