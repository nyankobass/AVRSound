#include "WaveMemoryProcessor.h"

#include <Arduino.h>
#include "Register.h"
#include "DbgApi.h"

namespace AVRSound {


WaveMemoryProcessor::WaveMemoryProcessor(DAC& _dac, volatile REGISTER& _sound_register)
    : sound_register(_sound_register) 
    , dac(_dac)
{
    wave_memory_register = &sound_register.SOUND3;
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

    
    /* Timer2 割り込み設定 */
    /* CTC 1024分周 */
    TCCR2A = 0b00000010;
    TCCR2B = 0b00000111;
    TIMSK2 = 0b00000010;

    /* 1/256[s] 毎に割り込みが発生するように設定 */
    OCR2A = 61;

    static const uint8_t memory[32] = {
        15, 15, 14, 14, 13, 13, 12, 12, 11, 11, 10, 10,  9,  9,  8,  8,
         7,  7,  6,  6,  5,  5,  4,  4,  3,  3,  2,  2,  1,  1,  0,  0,
    };
    static const uint8_t memory2[32] = {
        15, 14, 13, 12, 11, 10,  9,  8,  7,  6,  5,  4,  3,  2,  1,  0, 
         0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
    };
    sound_register.WAVE.set_wave_memory(memory);

    wave_memory_register->set_is_sound_on(false);
    wave_memory_register->set_is_start(false);
    wave_memory_register->set_frequency(0);

    /* レジスタ値に応じた割り込み時間を設定 */
    FrequencyUpdate();
}

void WaveMemoryProcessor::onTimer1Event()
{
    static uint8_t wave_table_index = 0;
    static uint16_t pre_frequency = 0;

    dac.Output(output_volume_buffer);

    /* 停止指示が出ていれば終了 */
    if (!wave_memory_register->is_sound_on()) {
        output_volume_buffer = 0x80;
        sound_register.TOTAL.BIT.is_key_on_wave_memory = 0;
        return;
    }

    /* 再生フラグが立っていなければ終了 */
    if (wave_memory_register->is_enable_length() && (wave_memory_register->length() == 0)) {
        output_volume_buffer = 0x80;
        sound_register.TOTAL.BIT.is_key_on_wave_memory = 0;
        return;
    }

    /* 再生開始処理 */
    if (wave_memory_register->is_start()) {
        wave_table_index = 0;
        wave_memory_register->set_is_start(false);
        sound_register.TOTAL.BIT.is_key_on_wave_memory = 1;
    }

    /* 周波数の変更が生じていた場合、割り込み時間を修正 */
    if (pre_frequency != wave_memory_register->frequency()){
        pre_frequency = wave_memory_register->frequency();
        FrequencyUpdate();
    }

    if (wave_memory_register->volume() == 0){
        output_volume_buffer = 0;
    }
    else {
        /* 出力バッファに格納 */
        uint8_t table_value= sound_register.WAVE.wave_memory(wave_table_index);
        output_volume_buffer = table_value >> (wave_memory_register->volume() - 1);

        /* output_value_buffer を 8bit に拡張する */
        output_volume_buffer = output_volume_buffer << 4;
    }

    wave_table_index = (wave_table_index + 1) & 0b0011111;
}

void WaveMemoryProcessor::onTimer2Event()
{
    /* 長さ有効なら lengthレジスタがオーバーフローするまでインクリメント */
    /* オーバーフローした場合 Update() 側で再生が停止される */
    if (!wave_memory_register->is_enable_length()){
        return;
    }

    uint8_t length = wave_memory_register->length();
    if (length == 0){
        return;
    }

    wave_memory_register->set_length(length + 1);
}

inline void WaveMemoryProcessor::FrequencyUpdate()
{
    uint16_t ocr = (2048 - wave_memory_register->frequency());

    OCR1A = ocr;
}

}