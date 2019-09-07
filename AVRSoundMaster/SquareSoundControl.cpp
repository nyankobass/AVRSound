#include "SquareSoundControl.h"

#include <Wire.h>
#include "Register.h"
#include "DbgApi.h"

namespace AVRSound
{

/* Attack 設定用テーブル */
static const struct AttackData {
    uint8_t init_value;
    uint16_t attack_time;
    uint8_t attack_envelope;
} ATTACK_DATA_TABLE[16] = {
    { 0x0F,    0,  0 },
    { 0x0E,   31,  1 },
    { 0x0B,   63,  1 },
    { 0x07,  125,  1 },
    { 0x00,  250,  1 },
    { 0x00,  500,  2 },
    { 0x00,  750,  3 },
    { 0x00, 1000,  4 },
    { 0x00, 1250,  5 },
    { 0x00, 1500,  6 },
    { 0x00, 1750,  7 },
    { 0x00, 1750,  7 },
    { 0x00, 1750,  7 },
    { 0x00, 1750,  7 },
    { 0x00, 1750,  7 },
};


SquareSoundControl::SquareSoundControl()
{

}

SquareSoundControl::~SquareSoundControl()
{
    
}

void SquareSoundControl::Initialize()
{
    {
        Wire.beginTransmission(0x30);
        uint8_t BYTE[] = {0b10000001};
        Wire.write(REGISTER::TOTAL_ADDR);
        Wire.write(BYTE, 1);

        Wire.endTransmission();
    }

    setting.set_envelope_step_time(0);
    setting.set_frequency(0);
    setting.set_is_enable_length(0);
    setting.set_init_volume(0);
    setting.set_is_envelope_increment(false);
    setting.set_length(0);
    setting.set_is_start(false);
    setting.set_wave_pattern(2);

    UpdateSound();
}

void SquareSoundControl::onKeyOn(uint8_t note_num)
{
    freqency = FREQ_TABLE[note_num];

    setting.set_frequency(freqency);
    setting.set_is_start(true);
    setting.set_init_volume(ATTACK_DATA_TABLE[attack].init_value);
    setting.set_is_envelope_increment(true);
    setting.set_envelope_step_time(ATTACK_DATA_TABLE[attack].attack_envelope);
    setting.set_is_enable_length(false);
    setting.set_length(0);

    UpdateSound();

    /* ビブラート初期化 */
    is_reset_vib = true;
    is_key_on = true;
    key_on_tick = 0;

    adsr_mode = ATTACK_MODE;
}

void SquareSoundControl::onKeyOff() 
{
    is_key_on = false;

    setting.set_is_envelope_increment(false);
    setting.set_frequency(freqency);
    
    /* リバーブ処理 */ 
    if (reverb_send != 0 && reverb_time != 0){
        setting.set_is_start(true);
        setting.set_init_volume(reverb_send < sustain ? reverb_send : sustain);
        setting.set_envelope_step_time(reverb_time);
        adsr_mode = REVERB_MODE;
    }

    /* リバーブがかかっていなければリリースの処理 */
    else {
        setting.set_is_start(false);
    
        if (release == 0){
            setting.set_init_volume(0x00);
            setting.set_envelope_step_time(0);
            adsr_mode = KEY_OFF;
        }
        else{
            setting.set_init_volume(0xFF);
            setting.set_envelope_step_time(release);
            adsr_mode = RELEASE_MODE;
        }
    }

    UpdateSound();
}

void SquareSoundControl::onChangeRelease(uint8_t value)
{
    release = value >> 4;
}

void SquareSoundControl::onChangeAttack(uint8_t value)
{
    attack = value >> 3;
}

void SquareSoundControl::onChangeSustain(uint8_t value)
{
    sustain = value >> 3;
    decay.time = (uint16_t)(decay.envelope * (0x0F - sustain)) << 4;
}

void SquareSoundControl::onChangeDecay(uint8_t value)
{
    decay.envelope = value >> 4;
    decay.time = ((uint16_t)(decay.envelope * (0x0F - sustain))) << 4;
}

void SquareSoundControl::onChangeExpression(uint8_t value)
{
    uint8_t wave_pattern = value >> 5;
    setting.set_wave_pattern(wave_pattern);
}

void SquareSoundControl::onChangeReverbSend(uint8_t value)
{
    reverb_send = value >> 3;
}

void SquareSoundControl::onChangeReverbTime(uint8_t value)
{
    reverb_time = value >> 4;
}

void SquareSoundControl::onChangeVibRate(uint8_t value)
{
    vib_rate = value;
}

void SquareSoundControl::onChangeVibDepth(uint8_t value)
{
    vib_depth = value >> 3;
}

void SquareSoundControl::onChangeVibDelay(uint8_t value)
{
    vib_delay = ((uint16_t)value) << 2;
}

void SquareSoundControl::onTimer()
{
    /* KeyOn 時間の計測 */
    key_on_tick = key_on_tick == 0xFFFF ? 0xFFFF : key_on_tick + 1;

    /* ビブラート処理 */
    if (is_key_on){
        onVibTimer();
    }

    if (adsr_mode == ATTACK_MODE ){
        if (key_on_tick < ATTACK_DATA_TABLE[attack].attack_time) {
            return;
        }

        if (decay.time == 0) {
            adsr_mode = DECAY_MODE;
            return;
        }

        /* Decay 動作を指示 */
        setting.set_frequency(freqency);
        setting.set_is_start(true);
        setting.set_init_volume(0x0F);
        setting.set_is_envelope_increment(false);
        setting.set_envelope_step_time(decay.envelope);
        UpdateSound();

        adsr_mode = DECAY_MODE;

        return;
    }

    if (adsr_mode == DECAY_MODE) {
        if ( key_on_tick < ATTACK_DATA_TABLE[attack].attack_time + decay.time){
            return;
        }

        /* Sustain 動作を指示 */
        setting.set_frequency(freqency);
        setting.set_is_start(true);
        setting.set_init_volume(sustain);
        setting.set_is_envelope_increment(false);
        setting.set_envelope_step_time(0);
        UpdateSound();

        adsr_mode = SUSTAIN_MODE;

        return;
    }
}

void SquareSoundControl::onVibTimer()
{
    static uint8_t tick = 0;
    static const int8_t table[4] = {0, -1, 0, 1};
    static uint8_t depth = 1;

    /* vib_rate に合わせた呼出し間隔の生成 */
    {
        static uint16_t vib_rate_tick = 0;
        static bool is_delay = false;
        vib_rate_tick++;

        if (is_reset_vib){
            tick = 0;
            vib_rate_tick = 0;
            is_delay = true;
            is_reset_vib = false;
        }

        /* 開始遅延処理中なら終了 */
        if (is_delay){
            /* 開始遅延終了判定 */
            if (vib_rate_tick >= vib_delay){
                is_delay = false;
                depth = 1;
            }
            return;
        }

        /* vib_rate 分の待ち時間を生成 */
        if (vib_rate_tick < vib_rate) {
            return;
        }

        vib_rate_tick = 0;
    }


    /* ビブラート */
    if (table[tick] == 1){
        setting.set_frequency(freqency + depth);
    }

    else if (table[tick] == -1){
        setting.set_frequency(freqency - depth);
        depth = depth < vib_depth ? depth + 1 : vib_depth;
    }
    
    else {
        setting.set_frequency(freqency);
    }

    setting.set_is_start(false);
    UpdateSound();

    tick = (tick + 1) & 0x03;
}

void SquareSoundControl::UpdateSound()
{
    Wire.beginTransmission(0x30);

    Wire.write(REGISTER::SOUND1_ADDR);
    Wire.write(setting.BYTE, setting.BYTE_SIZE);

    Wire.endTransmission();
}

const uint16_t SquareSoundControl::FREQ_TABLE[0xFF] = {
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 22, 137, 244, 345, 441, 531, 616, 697, 772, 844, 912, 975, 1036, 1092, 1146, 1197, 1245, 1290, 1332, 1372, 1410, 1446, 1480, 1512, 1542, 1570, 1597, 1622, 1646, 1669, 1690, 1710, 1729, 1747, 1764, 1780, 1795, 1809, 1823, 1835, 1847, 1858, 1869, 1879, 1889, 1898, 1906, 1914, 1921, 1929, 1935, 1942, 1948, 1953, 1959, 1964, 1968, 1973, 1977, 1981, 1985, 1988, 1992, 1995, 1998, 2001, 2003, 2006, 2008, 2010, 2012, 2014, 2016, 2018, 2020, 2021, 2023, 2024, 2026, 2027, 2028, 2029, 2030, 2031, 2032, 2033, 2034, 2035, 2035, 2036, 2037, 2037, 2038
};

}