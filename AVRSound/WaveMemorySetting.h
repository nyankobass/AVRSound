#ifndef __WAVE_MEMORY_SETTING__
#define __WAVE_MEMORY_SETTING__

namespace AVRSound
{

class WAVE_MEMORY_SETTING {
public:
    WAVE_MEMORY_SETTING() {
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
    /* 発音のON/OF */
    bool is_sound_on() const volatile {
        return (BYTE[0] >> 7) & 0b00000001;
    }
    void set_is_sound_on(bool value) volatile {
        BYTE[0] = (BYTE[0] & 0b01111111) + (((uint8_t)(value) << 7) & 0b00000001);
    }

    /* 発音の長さ */
    uint8_t length() const volatile { 
        return BYTE[1];
    }
    void set_length(uint8_t value) volatile{
        BYTE[1] = value;
    }

    /* 初期ボリューム */
    uint8_t volume() const volatile {
        return (BYTE[2] >> 5) & 0b00000011;
    }
    void set_volume(uint8_t value) volatile{
        BYTE[2] = (BYTE[2] & 0b10011111) + ((value << 5) & 0b01100000);
    }

    /* 周波数設定 */
    uint16_t frequency() const volatile{
        return ((uint8_t)BYTE[3]) + (((uint16_t)(BYTE[4]) << 8) & 0x0700);
    } 
    void set_frequency(uint16_t value) volatile {
        BYTE[3] = value & 0x00FF;
        BYTE[4] = (BYTE[4] & 0b11111000) + (((uint8_t)(value >> 8)) & 0b00000111);
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

class WAVE_MEMROY
{
public:
    WAVE_MEMROY(){
        for (int i = 0; i < BYTE_SIZE; i++) {
            BYTE[i] = 0;
        }
    }

    /* 実態 */
    static const uint8_t BYTE_SIZE = 16;
    uint8_t BYTE[BYTE_SIZE];

    /* 波形セーブ */
    void set_wave_memory(const uint8_t value_table[32]) volatile{
        for (uint8_t i = 0; i < 32; i++){
            const uint8_t table_index = i >> 1;
            /* 奇数の時 */
            if ((i & 0x01) == 0x01){
                BYTE[table_index] = (BYTE[table_index] & 0x0F) + ((value_table[i] << 4) & 0xF0);
            }
            else{
                BYTE[table_index] = (BYTE[table_index] & 0xF0) + (value_table[i] & 0x0F);
            }
        }
    }

    /* 波形取得 */
    uint8_t wave_memory(uint8_t index) const volatile{
        if (index >= 32){
            return 0;
        }

        uint8_t table_index = index >> 1;

        if ((index & 0x01) == 0x01){
            return (BYTE[table_index] >> 4) & 0x0F;
        }
        else{
            return BYTE[table_index] & 0x0F;
        }
    }
};

}

#endif