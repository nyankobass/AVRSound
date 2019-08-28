#include <Arduino.h>
#include <Wire.h>
#include <MIDI.h>
#include <MsTimer2.h>

#include "Register.h"
#include "ISoundControl.h"
#include "SquareSoundControl.h"
#include "WaveMemoryControl.h"
#include "NoiseSoundControl.h"
#include "DbgApi.h"

MIDI_CREATE_DEFAULT_INSTANCE();

namespace{
    AVRSound::REGISTER sound_register;

    AVRSound::SquareSoundControl square_sound_control;
    AVRSound::WaveMemoryControl wave_memory_control;
    AVRSound::NoiseSoundControl noise_sound_control;

    /* コントローラ */ 
    AVRSound::ISoundControl* sound_control = &square_sound_control;

    /* 現在発音中のノート番号 */
    uint8_t note_num = 0xff;
    uint8_t pre_note_num = 0xff;

    /* KeyOn中bitが立つ */
    uint8_t key_on_flag[32] = { 0 };

    /* タイマー割り込み検知用フラグ */
    volatile bool timer_flag = false;

    /* ================================================= */
    /* イベントハンドラ一覧 */
    /* ================================================= */
    void handleNoteOn(byte channel, byte pitch, byte velocity)
    {
        sound_control->onKeyOn(pitch);
        
        pre_note_num = note_num;
        note_num = pitch;
    }

    void handleNoteOff(byte channel, byte pitch, byte velocity)
    {
        /* 一音前までは保存しておきKeyOff時に復帰する */
        /* @note 5音ぐらい保存しておきたいが、リスト構造が必要で実装が面倒なので保留 */
        if (pre_note_num == pitch){
            pre_note_num = 0xff;
        }
        
        if (note_num == pitch){
            sound_control->onKeyOff();
        
            note_num = 0xff;

            if (pre_note_num != 0xff){
                sound_control->onKeyOn(pre_note_num);
                note_num = pre_note_num;
                pre_note_num = 0xff;
            }   
        }   
    }

    void handleControlChange(byte channel , byte number , byte value )
    {
        /* エクスプレッション */
        if (number == 11){
            sound_control->onChangeExpression(value);
        }

        /* リリースタイム */
        else if (number == 72){
            sound_control->onChangeRelease(value); 
        }

        /* ビブラートレイト */
        else if (number == 76){
            sound_control->onChangeVibRate(value);
        }
        /* ビブラートデプス */
        else if (number == 77){
            sound_control->onChangeVibDepth(value);
        }

        /* ビブラートディレイ */
        else if (number == 78){
            sound_control->onChangeVibDelay(value);
        }
    
        /* リバーブセンド */
        else if (number == 91){
            sound_control->onChangeReverbSend(value);
        }
        /* リバーブタイム */
        else if (number == 12){
            sound_control->onChangeReverbTime(value);
        }
    }

    void onTimer()
    {
        timer_flag = true;
    }

    /* ================================================= */
    /* 初期化関数一覧 */
    /* ================================================= */
    /* Midi関連初期化 */
    void MIDIInit()
    {
        MIDI.setHandleNoteOn(handleNoteOn);
        MIDI.setHandleNoteOff(handleNoteOff);
        MIDI.setHandleControlChange(handleControlChange);
        MIDI.begin(MIDI_CHANNEL_OMNI); 
    }

    /* APU との通信確立 */
    void APUInit()
    {
        constexpr uint8_t APU_ADDR = 0x30;

        /* 再生を有効化 */
        sound_register.TOTAL.BIT.is_output_enable = 1;
        
        /* APUとの接続を確立 */
        Wire.begin();	
        Wire.beginTransmission(APU_ADDR);
        Wire.write(AVRSound::REGISTER::TOTAL_ADDR);
        Wire.write(sound_register.TOTAL.BYTE, 1);

        Wire.endTransmission();
    }
}

/* 初期化 */
void setup() 
{
    /* デバッグ用 */
    AVRSound::DbgInit();

    /* APU の初期化 */
    APUInit();

    /* MIDI コールバック設定 */
    MIDIInit();

    /* 1ms 毎にタイマー呼出し */
    MsTimer2::set(1, onTimer);
    MsTimer2::start();

    /* コントローラ初期化 */
    sound_control->Initialize();
}

/* 無限ループ */
void loop() 
{
    /* タイマー呼出し */
    /* 割り込みの文脈で呼び出すとI2C関係との衝突が起こるため、呼び出しはメインスレッドで */
    if (timer_flag == true){
        sound_control->onTimer();
        timer_flag = false;
    }

    /* コールバック呼出し */
    MIDI.read();
}
