#ifndef __NOISE_PROCESSOR__
#define __NOISE_PROCESSOR__

#include "Register.h"
#include "DAC.h"

namespace AVRSound
{

class NoiseProcessor
{
public:
    /*! =============================================
     * @brief コンストラクタ
     * @param[in] sound_register レジスタの参照
     *! ============================================= */
    NoiseProcessor(DAC& dac, volatile REGISTER& sound_register);
    ~NoiseProcessor();

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

    volatile uint8_t volume_index = 0;
    volatile uint8_t output_volume_buffer = 0;

    static const uint8_t VOLUME_TABLE[16];
};

}
#endif