/** =========================================================================
 * @file    NoiseSetting.h
 * @brief   ノイズCH に関するレジスタをつかさどる。
 * @author  nyankobass
 * @date    2019/08/07
 * 
 * @note    インライン展開を期待して、実装を直書き
 * ========================================================================== */
#ifndef __NOISE_SETTING__
#define __NOISE_SETTING__

#include <inttypes.h>

namespace AVRSound
{

class NOISE_SETTING {
public:
    NOISE_SETTING() {
        for (int i = 0; i < BYTE_SIZE; i++) {
            BYTE[i] = 0;
        }
    }

    /* 実態 */
    static const uint8_t BYTE_SIZE = 5;
    uint8_t BYTE[BYTE_SIZE];

    /* ======================================== */
    /* 各種アクセサ */
    /* ゴリ押しによるビットアクセス */
    /* ビットフィールドを避けるため仕方なく */
    /* ======================================== */
    /* 音の長さ */
    uint8_t length() const volatile {
        return (BYTE[1]) & 0b00111111;
    }
    void set_length(uint8_t value) volatile {
        BYTE[1] = (BYTE[1] & 0b11000000) + ((value) & 0b00111111);
    }

    /* エンベロープ時間 */
    uint8_t envelope_step_time() const volatile{
        return BYTE[2] & 0b00000111;
    }
    void set_envelope_step_time(uint8_t value) volatile{
        BYTE[2] = (BYTE[2] & 0b11111000) + (value & 0b00000111);
    }

    /* エンベロープ 増加/減少 */
    bool is_envelope_increment() const volatile{
        return (BYTE[2] >> 3) & 0b00000001;
    }
    void set_is_envelope_increment(bool value) volatile{
        BYTE[2] = (BYTE[2] & 0b11110111) + ((((uint8_t)value) << 3) & 0b00001000);
    }

    /* 初期ボリューム */
    uint8_t init_volume() const volatile {
        return (BYTE[2] >> 4) & 0b00001111;
    }
    void set_init_volume(uint8_t value) volatile{
        BYTE[2] = (BYTE[2] & 0b00001111) + ((value << 4) & 0b11110000);
    }

    /* 周波数設定 */
    uint8_t freq_divider() const volatile{
        return BYTE[3] & 0b00000111;
    } 
    void set_freq_divider(uint8_t value) volatile {
        BYTE[3] = (BYTE[3] & 0b11111000) + (value & 0b00000111);
    }

    /* 短周期/長周期 */
    bool is_short_freq() const volatile {
        return (BYTE[3] >> 3) & 0b00000001;
    }
    void set_is_short_freq(bool value) volatile {
        BYTE[3] = (BYTE[3] & 0b11110111) + ((((uint8_t)value) << 3) & 0b00001000);
    }

    /* 周波数2 */
    uint8_t freq_bit_shift() const volatile {
        return (BYTE[3] >> 4) & 0b00001111;
    }
    void set_freq_bit_shift(uint8_t value) volatile {
        BYTE[3] = (BYTE[3] & 0b00001111) + ((value << 4) & 0b11110000);
    }

    /* 発音自動停止有効/無効 */
    bool is_enable_length() const volatile {
        return (BYTE[4] >> 6) & 0b00000001;
    }
    void set_is_enable_length(bool value) volatile {
        BYTE[4] = (BYTE[4] & 0b10111111) + ((((uint8_t)value) << 6) & 0b01000000);
    }

    /* 発音制御 */
    bool is_start() const volatile {
        return (BYTE[4] >> 7) & 0b00000001;
    }
    void set_is_start(bool value) volatile {
        BYTE[4] = (BYTE[4] & 0b01111111) + ((((uint8_t)value) << 7) & 0b10000000);
    }
};

}

#endif