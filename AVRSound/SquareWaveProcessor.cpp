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

const uint8_t SquareWaveProcessor::VOLUME_TABLE[16] = {0, 17, 34, 51, 68, 85, 102, 119, 136, 153, 170, 187, 204, 221, 237, 255};

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

    /* REGISTER 設定初期化 */
    /* 矩形波に設定 */
    /* ボリューム最大 */
    square_register->set_init_volume(15);
    /* 無音 */
    square_register->set_is_start(false);
    /* 発音持続 */
    square_register->set_length(0);
    square_register->set_is_enable_length(false);
    /* エンベロープ無効 */
    square_register->set_envelope_step_time(0);
    /* 最遅割り込みにとりあえず設定 */
    square_register->set_frequency(0);
    /* 再生中フラグはオフ */
    sound_register.TOTAL.BIT.is_key_on_square1 = 0;

    /* レジスタ値に応じた割り込み時間を設定 */
    FrequencyUpdate();
}

void SquareWaveProcessor::Update()
{
    static uint8_t wave_table_index = 0;
    static uint16_t pre_frequency = 0;

    dac.Output(output_volume_buffer);

    /* 出力が有効でなければ終了 */
    if (sound_register.TOTAL.BIT.is_output_enable == 0) {
        output_volume_buffer = 0x80;
        sound_register.TOTAL.BIT.is_key_on_square1 = 0;
        return;
    }

    /* 再生フラグが立っていなければ終了 */
    if (square_register->is_start() == false) {
        output_volume_buffer = 0x80;
        sound_register.TOTAL.BIT.is_key_on_square1 = 0;
        return;
    }

    /* 再生開始処理 */
    if (sound_register.TOTAL.BIT.is_key_on_square1 == 0) {
        wave_table_index = 0;
        volume_index = square_register->init_volume();
        sound_register.TOTAL.BIT.is_key_on_square1 = 1;
    }

    /* 周波数の変更が生じていた場合、割り込み時間を修正 */
    if (pre_frequency != square_register->frequency()){
        pre_frequency = square_register->frequency();
        FrequencyUpdate();
    }

    /* 出力バッファに格納 */
    output_volume_buffer = 0;
    if (WAVE_PATTERN_TABLE[square_register->wave_pattern()][wave_table_index] != 0) {
        output_volume_buffer = VOLUME_TABLE[volume_index];
    }

    wave_table_index = (wave_table_index + 1) & 0b0000111;
}

void SquareWaveProcessor::EnvelopeUpdate()
{

}

inline void SquareWaveProcessor::FrequencyUpdate()
{
    uint16_t ocr = (2048 - square_register->frequency()) << 1;

    OCR1A = ocr;
}

}