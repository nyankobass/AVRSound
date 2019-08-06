#include "WaveMemoryProcessor.h"

#include <Arduino.h>
#include "Register.h"

namespace AVRSound {


WaveMemoryProcessor::WaveMemoryProcessor(DAC& _dac, volatile REGISTER& _sound_register)
    : sound_register(_sound_register) 
    , dac(_dac)
{
}

WaveMemoryProcessor::~WaveMemoryProcessor()
{

}

void WaveMemoryProcessor::Initialize()
{
    /* Timer1 割り込み設定 */
    /* CTC 8分周 */
    TCCR1A = 0b00000000;
    TCCR1B = 0b00001010;
    TIMSK1 = 0b00000010;

    /* REGISTER 設定初期化 */
    /* ボリューム最大 */
    sound_register.SOUND3.set_volume(1);
    /* 無音 */
    sound_register.SOUND3.set_is_start(false);
    /* 発音持続 */
    sound_register.SOUND3.set_length(0);
    sound_register.SOUND3.set_is_enable_length(false);
    /* 最遅割り込みにとりあえず設定 */
    sound_register.SOUND3.set_frequency(0);
    /* 再生中フラグはオフ */
    sound_register.TOTAL.BIT.is_key_on_wave_memory = 0;
    
    /* 有効化 */
    sound_register.SOUND3.set_is_sound_on(1);

    static const uint8_t memory[32] = {
        15, 15, 14, 14, 13, 13, 12, 12, 11, 11, 10, 10,  9,  9,  8,  8,
         7,  7,  6,  6,  5,  5,  4,  4,  3,  3,  2,  2,  1,  1,  0,  0,
    };
    static const uint8_t memory2[32] = {
        15, 14, 13, 12, 11, 10,  9,  8,  7,  6,  5,  4,  3,  2,  1,  0, 
         0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
    };
    sound_register.WAVE.set_wave_memory(memory);

    /* レジスタ値に応じた割り込み時間を設定 */
    FrequencyUpdate();
}

void WaveMemoryProcessor::Update()
{
    static uint8_t wave_table_index = 0;
    static uint16_t pre_frequency = 0;

    dac.Output(output_volume_buffer);

    /* 出力が有効でなければ終了 */
    if (sound_register.TOTAL.BIT.is_output_enable == 0) {
        output_volume_buffer = 0;
        sound_register.TOTAL.BIT.is_key_on_square1 = 0;
        return;
    }

    /* 再生フラグが立っていなければ終了 */
    if (sound_register.SOUND3.is_start() == false) {
        output_volume_buffer = 0;
        sound_register.TOTAL.BIT.is_key_on_wave_memory = 0;
        return;
    }

    /* 再生開始処理 */
    if (sound_register.TOTAL.BIT.is_key_on_wave_memory == 0) {
        wave_table_index = 0;
        sound_register.TOTAL.BIT.is_key_on_wave_memory = 1;
    }

    /* 周波数の変更が生じていた場合、割り込み時間を修正 */
    if (pre_frequency != sound_register.SOUND3.frequency()){
        pre_frequency = sound_register.SOUND3.frequency();
        FrequencyUpdate();
    }

    if (sound_register.SOUND3.volume() == 0){
        output_volume_buffer = 0;
    }
    else {
        /* 出力バッファに格納 */
        uint8_t table_value= sound_register.WAVE.wave_memory(wave_table_index);
        output_volume_buffer = table_value >> (sound_register.SOUND3.volume() - 1);

        /* output_value_buffer を 8bit に拡張する */
        output_volume_buffer = output_volume_buffer << 4;
    }

    wave_table_index = (wave_table_index + 1) & 0b0011111;
}

void WaveMemoryProcessor::EnvelopeUpdate()
{

}

inline void WaveMemoryProcessor::FrequencyUpdate()
{
    uint16_t ocr = (2048 - sound_register.SOUND3.frequency());

    OCR1A = ocr;
}

}