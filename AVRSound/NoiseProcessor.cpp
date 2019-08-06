#include "NoiseProcessor.h"

#include <Arduino.h>

namespace AVRSound {

const uint8_t NoiseProcessor::VOLUME_TABLE[16] = {0, 17, 34, 51, 68, 85, 102, 119, 136, 153, 170, 187, 204, 221, 237, 255};

NoiseProcessor::NoiseProcessor(DAC& _dac, volatile REGISTER& _sound_register)
    : sound_register(_sound_register) 
    , dac(_dac)
{
}

NoiseProcessor::~NoiseProcessor()
{

}

void NoiseProcessor::Initialize()
{
    /* Timer1 割り込み設定 */
    /* CTC 8分周 */
    TCCR1A = 0b00000000;
    TCCR1B = 0b00001010;
    TIMSK1 = 0b00000010;
    
 
    /* 再生中フラグはオフ */
    sound_register.TOTAL.BIT.is_key_on_noise = 0;

    sound_register.TOTAL.BIT.is_output_enable = 1;
    sound_register.SOUND4.set_freq_bit_shift(8);
    sound_register.SOUND4.set_freq_divider(1);
    sound_register.SOUND4.set_is_start(1);
    sound_register.SOUND4.set_init_volume(15);

    /* レジスタ値に応じた割り込み時間を設定 */
    FrequencyUpdate();
}

void NoiseProcessor::Update()
{
    static uint16_t reg = 0xFFFF;
    static uint8_t output = 1;

    static uint8_t pre_freq_divider = 0;
    static uint8_t pre_freq_bit_shift = 0;

    dac.Output(output_volume_buffer);

    /* 出力が有効でなければ終了 */
    if (sound_register.TOTAL.BIT.is_output_enable == 0) {
        output_volume_buffer = 0x80;
        sound_register.TOTAL.BIT.is_key_on_noise = 0;
        return;
    }

    /* 再生フラグが立っていなければ終了 */
    if (sound_register.SOUND4.is_start() == false) {
        output_volume_buffer = 0x80;
        sound_register.TOTAL.BIT.is_key_on_noise = 0;
        return;
    }

    /* 再生開始処理 */
    if (sound_register.TOTAL.BIT.is_key_on_noise == 0) {
        reg = 0xFF;
        output = 1;
        volume_index = sound_register.SOUND4.init_volume();
        sound_register.TOTAL.BIT.is_key_on_noise = 1;
    }

    /* 周波数の変更が生じていた場合、割り込み時間を修正 */
    if ((pre_freq_divider != sound_register.SOUND4.freq_divider()) || (pre_freq_bit_shift != sound_register.SOUND4.freq_bit_shift())) {
        pre_freq_divider= sound_register.SOUND4.freq_divider();
        pre_freq_bit_shift = sound_register.SOUND4.freq_bit_shift();
        FrequencyUpdate();
    }

    {
        /* 線形帰還シフトレジスタ */
        bool short_freq = sound_register.SOUND4.is_short_freq();
        if(reg == 0)reg = 1; //一応
        reg += reg + (((reg >> (short_freq ? 6 : 14)) ^ (reg >> (short_freq ? 5 : 13))) & 1);
        output ^= reg & 1;

        output_volume_buffer = 0;
        if (output != 0){
            output_volume_buffer = VOLUME_TABLE[volume_index];
        }
        else{

        }
    }

}
void NoiseProcessor::EnvelopeUpdate()
{

}

inline void NoiseProcessor::FrequencyUpdate()
{
    const uint8_t freq_bit_shift = sound_register.SOUND4.freq_bit_shift();
    const uint8_t freq_divider = sound_register.SOUND4.freq_divider();



    uint16_t ocr = 0;
    if (freq_bit_shift < 8){
        /* CTC 8分周 */
        TCCR1B = 0b00001010;

        uint16_t ocr = (freq_divider + 1) << (freq_bit_shift + 2);
        if (ocr <= 100){
            ocr = 100;
        }
        OCR1A = ocr;
    }
    else{
        /* CTC 16分周 */
        TCCR1B = 0b00001011;

        uint16_t ocr = (freq_divider + 1) << (freq_bit_shift - 1);
        if (ocr < 15){
            ocr = 15;
        }
        OCR1A = ocr;       
    }
}

}