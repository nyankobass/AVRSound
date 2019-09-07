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

    /*! ===================================================
     * @brief Releaseタイムをセットする
     * @param[in] value sustain_level * n/64[s] ( n = 0 - 7 )
     * @note nの変化点 0, 16, 32, 48, 64, 80, 96, 112
     *  =================================================== */
    void onChangeRelease(uint8_t value) override;
    void onChangeAttack(uint8_t value) override;
    void onChangeDecay(uint8_t value) override;
    void onChangeSustain(uint8_t value) override;
    
    /*! ===================================================
     * @brief 波形パターンを変更する
     * @param[in] value  0 - 31  : 12.5%
     *                  32 - 63  : 25% 
     *                  64 - 95  : 50%
     *                  96 - 127 : 75% 
     *  =================================================== */
    void onChangeExpression(uint8_t value);

    /*! ===================================================
     * @brief リバーブのボリュームを変更する
     * @param[in] value 16段階のボリューム
     *  =================================================== */
    void onChangeReverbSend(uint8_t value);
   
    /*! ===================================================
     * @brief リバーブタイムを変更する
     * @param[in] value sustain_level * n/64[s] ( n = 0 - 7 )
     * @note nの変化点 0, 16, 32, 48, 64, 80, 96, 112
     *  =================================================== */
    void onChangeReverbTime(uint8_t value);
   
    /*! ===================================================
     * @brief ビブラートの速度を変更する
     * @param[in] value 250 / (value + 1)[Hz]
     *  =================================================== */
    void onChangeVibRate(uint8_t value);
    /*! ===================================================
     * @brief ビブラートの深さを変更する
     * @param[in] value 16段階の深さ
     *  =================================================== */
    void onChangeVibDepth(uint8_t value);
    /*! ===================================================
     * @brief ビブラートの開始を遅延する
     * @param[in] value 4 - 512[ms]
     *  =================================================== */
    void onChangeVibDelay(uint8_t value);

private:
    void onVibTimer();

    void UpdateSound();


    SQUARE_SETTING setting;

    static const uint16_t FREQ_TABLE[0xFF];

    /* ======================== */
    /* 基本情報 */
    /* ======================== */
    /* 再生中の周波数 */
    uint16_t freqency = 0;
    /* ビブラートのリセット信号 */
    bool is_reset_vib = false;
    /* KeyOn からの時間経過 [ms] */
    uint16_t key_on_tick = 0;
    /* keyOn中ならtrue */
    bool is_key_on = false;
    /* 現在の発音状態 */
    /* @note 発音していないならば、KEY_OFF は必ずしも成り立たない。 */
    enum ADSR{
        ADSR_MIN = 0,
        KEY_OFF,
        ATTACK_MODE,
        DECAY_MODE,
        SUSTAIN_MODE,
        RELEASE_MODE,
        REVERB_MODE
    } adsr_mode = KEY_OFF;

    /* ======================== */
    /* パラメータ群 */ 
    /* ======================== */
    /* ADSR */
    uint8_t attack = 0;          /* 0 - 15 */
    struct DecayData{
        uint8_t envelope = 0;
        uint16_t time = 0;
    } decay;
    uint8_t sustain = 0x0F;      /* 0 - 15 */
    uint16_t release = 0;        /* 0 - 7 */

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
