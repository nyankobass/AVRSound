#include "MainProcess.h"

#include <Arduino.h>
#include <Wire.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "DbgApi.h"
#include "Register.h"
#include "SquareWaveProcessor.h"
#include "WaveMemoryProcessor.h"
#include "NoiseProcessor.h"
#include "RegisterClientSynchronizer.h"

ISR(TIMER1_COMPA_vect) //timer1でのコンペアマッチAの割り込み関数
{
    AVRSound::onTimerEvent();
}

ISR(TIMER2_COMPA_vect) //timer2でのコンペアマッチAの割り込み関数
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
    /* レジスタ同期モジュール駆動 */
    RegisterClientSynchronizer::Begin(sound_register, 0x30, onRegisterChanged);

    /* dac 初期化 */
    dac.Initialize();

    /* 出力モジュール初期化 */
    processor->Initialize();

    /* デバッグ用出力ピン設定 */
    pinMode(2, OUTPUT);
    pinMode(3, OUTPUT);
}

void loop()
{
    /* 基本的な処理はすべて割り込みにて行う */
}

void onTimerEvent()
{
    /* 処理時間計測用に割り込み中port2をHIGHにする  */
    DbgPin2High();

    processor->onTimer1Event();

    DbgPin2Low();
}

void onTimer2Event()
{
    processor->onTimer2Event();

}

void onRegisterChanged(uint8_t addr, uint8_t byte_num)
{
    if (addr == REGISTER::TOTAL_ADDR){
        ChangeProcess();
    }
    
    return;
}

}
