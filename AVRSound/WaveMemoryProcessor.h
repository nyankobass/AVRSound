#ifndef __WAVE_MEMORY_PROCESSOR__
#define __WAVE_MEMORY_PROCESSOR__

#include <inttypes.h>
#include "Register.h"
#include "DAC.h"

namespace AVRSound {

/*! =============================================
 * @brief ATmega328 矩形波生成モジュール
 * @attention Timer1 / Timer2 を占有します
 *! ============================================= */
class WaveMemoryProcessor
{
public:
    /*! =============================================
     * @brief コンストラクタ
     * @param[in] sound_register レジスタの参照
     * @param[in] index 0:矩形波1 0以外:矩形波2
     *! ============================================= */
    WaveMemoryProcessor(DAC& dac, volatile REGISTER& sound_register);
    ~WaveMemoryProcessor();

    /*! =============================================
     * @brief 初期化処理を行う
     * @attention Timer1 / Timer2 の割り込み設定を行います。
     *! ============================================= */
    void Initialize();

    /*! =============================================
     * @brief 出力の更新を行う
     * @attention TIMER1_COMPA_vect 割り込み中に実行すること
     *! ============================================= */
    void Update();

    /*! =============================================
     * @brief エンベロープの更新を行う
     *! ============================================= */
    void EnvelopeUpdate();

private:
    inline void FrequencyUpdate();

private: 
    DAC& dac;
    volatile REGISTER& sound_register;

    volatile uint8_t output_volume_buffer = 0;
};
}

#endif