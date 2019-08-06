#include "MainProcess.h"

#include <Arduino.h>
#include <Wire.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "Register.h"
#include "SquareWaveProcessor.h"
#include "WaveMemoryProcessor.h"
#include "NoiseProcessor.h"

ISR(TIMER1_COMPA_vect) //timer1でのコンペアマッチAの割り込み関数
{
    AVRSound::onTimerEvent();
}

ISR(TIMER2_COMPA_vect) //timer1でのコンペアマッチAの割り込み関数
{
    AVRSound::onTimer2Event();
}


/* ローカル(?)変数定義 */
namespace {
    /* レジスタ */
    volatile AVRSound::REGISTER sound_register;

    /* DAC 制御 */
    AVRSound::DAC dac;

    /* 各種波形生成プロセッサー一覧 */
    AVRSound::SquareWaveProcessor square_wave1_processor(dac, sound_register, 0);
    AVRSound::SquareWaveProcessor square_wave2_processor(dac, sound_register, 1);
    AVRSound::WaveMemoryProcessor wave_memory_processor(dac, sound_register);
    AVRSound::NoiseProcessor      noise_processor(dac, sound_register);

    /* 各種プロセッサーを扱うインターフェース */
    AVRSound::IProcessor* processor = &square_wave1_processor;

    inline void ChangeProcess(){
        static uint8_t pre_total_byte = sound_register.TOTAL.BYTE[0];
        
        if (pre_total_byte == sound_register.TOTAL.BYTE[0]){
            return;
        }

        pre_total_byte = sound_register.TOTAL.BYTE[0];

        if (sound_register.TOTAL.BYTE[0] == 0b10000001){
            processor = &square_wave1_processor;
        }
        else if (sound_register.TOTAL.BYTE[0] == 0b10000010){
            processor = &square_wave2_processor;
        }
        else if (sound_register.TOTAL.BYTE[0] == 0b10000100){
            processor = &wave_memory_processor;
        }
        else if (sound_register.TOTAL.BYTE[0] == 0b10001000){
            processor = &noise_processor;
        }
        else {
            return;
        }

        processor->Initialize();
    }
}

namespace AVRSound{

void setup()
{
    /* dac 初期化 */
    dac.Initialize();

    /* 出力モジュール初期化 */
    processor->Initialize();

    /* デバッグ用出力ピン設定 */
    pinMode(2, OUTPUT);
    pinMode(3, OUTPUT);

    /* I2C 初期化 */
    Wire.begin(0x30);
    Wire.onReceive(onI2CReceived);
}

void loop()
{
    /* 基本的な処理はすべて割り込みにて行う */
}

/* 再生周波数に応じた周期で呼び出される */
void onTimerEvent()
{
    /* 処理時間計測用に割り込み中port2をHIGHにする  */
    PORTD |= 0b00000100;

    processor->Update();

    PORTD &= 0b11111011;
}

void onTimer2Event()
{
    processor->EnvelopeUpdate();

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

    const uint8_t addr = byte_data[0];
    sound_register.write_byte(addr, &byte_data[1], read_size - 1);

    /* 全体設定を弄った時だけ、プロセス変更の可能性がある */
    if (addr == REGISTER::TOTAL_ADDR){
        ChangeProcess();
    }

    return;
}

}
