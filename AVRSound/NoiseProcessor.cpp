#include "NoiseProcessor.h"

#include <Arduino.h>
#include "DbgApi.h"

namespace AVRSound {

const uint8_t NoiseProcessor::VOLUME_TABLE[16] = {0, 17, 34, 51, 68, 85, 102, 119, 136, 153, 170, 187, 204, 221, 237, 255};

NoiseProcessor::NoiseProcessor(DAC& _dac, volatile REGISTER& _sound_register)
    : sound_register(_sound_register) 
    , dac(_dac)
{
    noise_register = &sound_register.SOUND4;
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
 
    /* レジスタ値に応じた割り込み時間を設定 */
    FrequencyUpdate();
    

    /* Timer2 割り込み設定 */
    /* CTC 1024分周 */
    TCCR2A = 0b00000010;
    TCCR2B = 0b00000111;
    TIMSK2 = 0b00000010;

    /* 1/256[s] 毎に割り込みが発生するように設定 */
    OCR2A = 61;
}

void NoiseProcessor::onTimer1Event()
{
    static uint16_t reg = 0xFFFF;
    static uint8_t output = 1;

    static uint8_t pre_freq_divider = 0;
    static uint8_t pre_freq_bit_shift = 0;

    dac.Output(output_volume_buffer);

    /* 停止指示が出ていれば終了 */
    if (noise_register->BYTE[2] == 0) {
        output_volume_buffer = 0x80;
        sound_register.TOTAL.BIT.is_key_on_noise = 0;
        return;
    }

    /* 長さ有効かつ長さ0なら終了 */
    if (noise_register->is_enable_length() && (noise_register->length() == 0)){
        output_volume_buffer = 0x80;
        sound_register.TOTAL.BIT.is_key_on_square1 = 0;
        return;
    }

    /* 再生開始処理 */
    if (noise_register->is_start()) {
        reg = 0xFF;
        output = 1;
        volume_index = noise_register->init_volume();
        noise_register->set_is_start(false);
        sound_register.TOTAL.BIT.is_key_on_noise = 1;
    }

    /* 周波数の変更が生じていた場合、割り込み時間を修正 */
    if ((pre_freq_divider != noise_register->freq_divider()) || (pre_freq_bit_shift != noise_register->freq_bit_shift())) {
        pre_freq_divider= noise_register->freq_divider();
        pre_freq_bit_shift = noise_register->freq_bit_shift();
        FrequencyUpdate();
    }

    {
        /* 線形帰還シフトレジスタ */
        bool short_freq = noise_register->is_short_freq();
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
void NoiseProcessor::onTimer2Event()
{
    /* 1/256[s] 毎に呼び出される */
    {
        static uint8_t envelope_tick = 0;
        envelope_tick++;

        /* 1/256 * 4 = 1/64[s] 毎に呼び出し */
        if(envelope_tick >= 4){
            EnvelopeUpdate();
            envelope_tick = 0;
        }
    }

    {
        /* 長さ有効なら lengthレジスタがオーバーフローするまでインクリメント */
        /* オーバーフローした場合 Update() 側で再生が停止される */
        if (!noise_register->is_enable_length()){
            return;
        }

        uint8_t length = noise_register->length();
        if (length == 0){
            return;
        }

        noise_register->set_length(length + 1);
    }
}

inline void NoiseProcessor::EnvelopeUpdate()
{
    static uint8_t tick = 0;

    if (noise_register->envelope_step_time() == 0){
        return;
    }

    tick += 1;

    if (noise_register->envelope_step_time() > tick) {
        return;
    }

    tick = 0;
    if (noise_register->is_envelope_increment()) {
        if (volume_index < 0x0F) {
            volume_index = volume_index + 1;
        }
    }
    else {
        if (volume_index != 0) {
            volume_index = volume_index - 1;
        }
    }
}
 
inline void NoiseProcessor::FrequencyUpdate()
{
    const uint8_t freq_bit_shift = noise_register->freq_bit_shift();
    const uint8_t freq_divider = noise_register->freq_divider();



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