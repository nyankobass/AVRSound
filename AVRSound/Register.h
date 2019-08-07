/** =========================================================================
 * @file    Register.h
 * @brief   レジスタへのアクセスを行う。
 * @author  nyankobass
 * @date    2019/08/07
 * 
 * @note    インライン展開を期待して、実装を直書き
 * ========================================================================== */
#ifndef __REGISTER__
#define __REGISTER__

#include <inttypes.h>
#include "SquareSetting.h"
#include "WaveMemorySetting.h"
#include "NoiseSetting.h"

namespace AVRSound{

/* 制御用レジスタを宣言する */
struct REGISTER{
    /* 矩形波1 */
    SQUARE_SETTING SOUND1;
    
    /* 矩形波2 */
    SQUARE_SETTING SOUND2;

    /* 波形メモリ */
    WAVE_MEMORY_SETTING SOUND3;

    /* ノイズ */
    NOISE_SETTING SOUND4;


    union TOTAL_SETTING{
        uint8_t BYTE[1];
        struct {
            uint8_t is_key_on_square1       :1; // 矩形波1 が発音中
            uint8_t is_key_on_square2       :1; // 矩形波1 が発音中
            uint8_t is_key_on_wave_memory   :1; // 波形メモリが発音中
            uint8_t is_key_on_noise         :1; // ノイズが発音中

            uint8_t __unused1               :3;
            uint8_t is_output_enable        :1; // すべての出力の ON / OFF を決める

        } BIT;
    } TOTAL;
    
    WAVE_MEMORY WAVE;

    /* アドレス指定で書き込む */
    void write_byte(uint8_t addr, uint8_t byte_data[], uint8_t byte_num) volatile {
        volatile uint8_t* byte_addr = nullptr;
        uint8_t index = 0;
        uint8_t byte_size = 0;
        if (addr >= MAX_ADDR){
            return;
        }

        if (addr >= WAVE_ADDR){
            byte_addr = WAVE.BYTE;
            byte_size = WAVE_MEMORY::BYTE_SIZE;
            index = addr - WAVE_ADDR;
        }
        else if (addr >= TOTAL_ADDR){
            byte_addr = TOTAL.BYTE;
            byte_size = 1;
            index = addr - TOTAL_ADDR;
        }
        else if (addr >= SOUND4_ADDR){
            byte_addr = SOUND4.BYTE;
            byte_size = NOISE_SETTING::BYTE_SIZE;
            index = addr - SOUND4_ADDR;
        }
        else if (addr >= SOUND3_ADDR){
            byte_addr = SOUND3.BYTE;
            byte_size = WAVE_MEMORY_SETTING::BYTE_SIZE;
            index = addr - SOUND3_ADDR;
        }
        else if (addr >= SOUND2_ADDR){
            byte_addr = SOUND2.BYTE;
            byte_size = SQUARE_SETTING::BYTE_SIZE;
            index = addr - SOUND2_ADDR;
        }
        else if (addr >= SOUND1_ADDR){
            byte_addr = SOUND1.BYTE;
            byte_size = SQUARE_SETTING::BYTE_SIZE;
            index = addr - SOUND1_ADDR;
        }

        for (int i = 0; i < byte_num; i++){
            if (byte_size <= index + i){
                break;
            }

            byte_addr[index + i] = byte_data[i]; 
        }

        return;
    }

    static const uint8_t SOUND1_ADDR = 0;
    static const uint8_t SOUND2_ADDR = SOUND1_ADDR + SQUARE_SETTING::BYTE_SIZE;
    static const uint8_t SOUND3_ADDR = SOUND2_ADDR + SQUARE_SETTING::BYTE_SIZE;
    static const uint8_t SOUND4_ADDR = SOUND3_ADDR + WAVE_MEMORY_SETTING::BYTE_SIZE;
    static const uint8_t TOTAL_ADDR  = SOUND4_ADDR + NOISE_SETTING::BYTE_SIZE;
    static const uint8_t WAVE_ADDR   = TOTAL_ADDR + 1;
    static const uint8_t MAX_ADDR    = WAVE_ADDR + WAVE_MEMORY::BYTE_SIZE;
};

}

#endif