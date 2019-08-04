#include "MainProcess.h"

#include <Arduino.h>
#include <Wire.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "Register.h"
#include "SquareWaveProcessor.h"
#include "WaveMemoryProcessor.h"

ISR(TIMER1_COMPA_vect) //timer1でのコンペアマッチAの割り込み関数
{
    AVRSound::onTimerEvent();
}

/* ローカル(?)変数定義 */
namespace {
    /* レジスタ */
    volatile AVRSound::REGISTER sound_register;

    /* DAC 制御 */
    AVRSound::DAC dac;

    /* 矩形波出力モジュール */
    AVRSound::SquareWaveProcessor square_wave1_processor(dac, sound_register, 0);
    AVRSound::SquareWaveProcessor square_wave2_processor(dac, sound_register, 1);
    AVRSound::WaveMemoryProcessor wave_memory_processor(dac, sound_register);

    uint8_t mode = 0;
}

namespace AVRSound{

void setup()
{
    /* dac 初期化 */
    dac.Initialize();

    /* 出力モジュール初期化 */
    switch (mode)
    {
    case 0:
        square_wave1_processor.Initialize();
        break;
    case 1:
        square_wave2_processor.Initialize();
        break;
    case 2:
        wave_memory_processor.Initialize();
        break;
    default:
        square_wave1_processor.Initialize();
        break;
    }

    /* デバッグ用出力ピン設定 */
    pinMode(2, OUTPUT);
    pinMode(3, OUTPUT);

    /* 出力ON */
    sound_register.TOTAL.BIT.is_output_enable = 1;

    /* CH1 設定 */
    sound_register.SOUND1.set_is_start(true);

    /* CH3 設定 */
    sound_register.SOUND3.set_is_start(true);
    sound_register.SOUND3.set_frequency(1910);

    Wire.begin(0x30);
    Wire.onReceive(onI2CReceived);
}

void loop()
{
}

/* 再生周波数に応じた周期で呼び出される */
void onTimerEvent()
{
    /* 処理時間計測用に割り込み中port2をHIGHにする  */
    PORTD |= 0b00000100;

    switch (mode)
    {
    case 0:
        square_wave1_processor.Update();
        break;
    case 1:
        square_wave2_processor.Update();
        break;
    case 2:
        wave_memory_processor.Update();
        break;
    default:
        square_wave1_processor.Update();
        break;
    }

    PORTD &= 0b11111011;
}

/* I2C 受信時 */
void onI2CReceived(int byte_num)
{
    /* @note 32 : wire の受信バッファのサイズ */
    uint8_t byte_data[32] = {0};
    
    /* @note そもそも Wire.readBytes に sound_register の 該当する BYTE を渡せば2重コピーしなくても済む */
    uint8_t read_size = Wire.readBytes(byte_data, byte_num);

    if (read_size < 2){
        return;
    }

    sound_register.write_byte(byte_data[0], &byte_data[1], read_size - 1);

    return;
}

}
