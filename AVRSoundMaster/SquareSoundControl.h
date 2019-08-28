#ifndef __SQUARE_SOUND_CONTROL__
#define __SQUARE_SOUND_CONTROL__

#include "ISoundControl.h"
#include "SquareSetting.h"

namespace AVRSound 
{

class SquareSoundControl : public ISoundControl
{
public:
    SquareSoundControl();
    ~SquareSoundControl();

    /* ISoundControl 継承 */
    void Initialize() override;
    void onKeyOn(uint8_t note_num) override;
    void onKeyOff() override;
    void onTimer() override;
    void onChangeRelease(uint8_t value);
    void onChangeExpression(uint8_t value);
    void onChangeReverbSend(uint8_t value);
    void onChangeReverbTime(uint8_t value);
    void onChangeVibRate(uint8_t value);
    void onChangeVibDepth(uint8_t value);
    void onChangeVibDelay(uint8_t value);

private:
    void onVibTimer();

    void UpdateSound();


    SQUARE_SETTING setting;

    static const uint16_t FREQ_TABLE[0xFF];

    /* 基本情報 */
    uint16_t freqency = 0;
    uint8_t release_time = 0;
    
    bool is_reset_vib = false;
    uint16_t key_on_tick = 0;
    bool is_key_on = false;

    /* ======================== */
    /* パラメータ群 */ 
    /* ======================== */
    /* ADSR */
    uint8_t attack_time = 0;
    uint8_t decay_time = 0;
    uint8_t sustain_level = 0;
    uint8_t release_time = 0;

    /* ビブラート */
    uint8_t vib_rate = 0;
    uint8_t vib_depth = 0;
    uint16_t vib_delay = 0;

    /* リバーブ */
    uint8_t reverb_send = 0;
    uint8_t reverb_time = 0;
};

}

#endif
