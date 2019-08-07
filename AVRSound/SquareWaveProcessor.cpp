#include "SquareWaveProcessor.h"

#include <Arduino.h>
#include "Register.h"

namespace AVRSound {

const uint8_t SquareWaveProcessor::WAVE_PATTERN_TABLE[WAVE_PATTERN_MAX][8] = {
    {0,0,0,0,1,0,0,0},  /* duty : 12% */
    {0,0,0,0,1,1,0,0},  /* duty : 25% */
    {0,0,1,1,1,1,0,0},  /* duty : 50% */
    {1,1,1,1,0,0,1,1},  /* duty : 75% */
};

const uint8_t SquareWaveProcessor::VOLUME_TABLE[16] = {0, 8, 17, 25, 34, 42, 51, 59, 68, 76, 85, 93, 102, 110, 119, 127};

SquareWaveProcessor::SquareWaveProcessor(DAC& _dac, volatile REGISTER& _sound_register, uint8_t index)
    : sound_register(_sound_register) 
    , dac(_dac)
{
    if (index == 0){
        square_register = &(sound_register.SOUND1);
    }
    else {
        square_register = &(sound_register.SOUND2);
    }
}

SquareWaveProcessor::~SquareWaveProcessor()
{

}

void SquareWaveProcessor::Initialize()
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

void SquareWaveProcessor::onTimer1Event()
{
    static uint8_t wave_table_index = 0;
    static uint16_t pre_frequency = 0;

    dac.Output(output_volume_buffer);


    /* 停止指示が出ていれば終了 */
    if (square_register->BYTE[2] == 0x00) {
        output_volume_buffer = 0x80;
        sound_register.TOTAL.BIT.is_key_on_square1 = 0;
        return;
    }

    /* 長さ有効かつ長さ0なら終了 */
    if (square_register->is_enable_length() && (square_register->length() == 0)){
        output_volume_buffer = 0x80;
        sound_register.TOTAL.BIT.is_key_on_square1 = 0;
        return;
    }

    /* 再生開始処理 */
    if (sound_register.SOUND1.is_start()) {
        wave_table_index = 0;
        volume_index = square_register->init_volume();
        sound_register.SOUND1.set_is_start(false);
        sound_register.TOTAL.BIT.is_key_on_square1 = 1;
    }

    /* 周波数の変更が生じていた場合、割り込み時間を修正 */
    if (pre_frequency != square_register->frequency()){
        pre_frequency = square_register->frequency();
        FrequencyUpdate();
    }

    /* 出力バッファに格納 */
    if (WAVE_PATTERN_TABLE[square_register->wave_pattern()][wave_table_index] != 0) {
        output_volume_buffer = 0x80 + VOLUME_TABLE[volume_index];
    }
    else{
        output_volume_buffer = 0x80 - VOLUME_TABLE[volume_index];
    }

    wave_table_index = (wave_table_index + 1) & 0b0000111;
}

void SquareWaveProcessor::onTimer2Event()
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
        if (!square_register->is_enable_length()){
            return;
        }

        uint8_t length = square_register->length();
        if (length == 0){
            return;
        }

        square_register->set_length(length + 1);
    }

}

inline void SquareWaveProcessor::EnvelopeUpdate()
{
    static uint8_t tick = 0;

    if (square_register->envelope_step_time() == 0){
        return;
    }

    tick += 1;

    if (square_register->envelope_step_time() > tick) {
        return;
    }

    tick = 0;

    if (square_register->is_envelope_increment()) {
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
 

inline void SquareWaveProcessor::FrequencyUpdate()
{
    uint16_t ocr = (2048 - square_register->frequency()) << 1;

    OCR1A = ocr;
}

}