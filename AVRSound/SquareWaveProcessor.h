#ifndef __SQUARE_WAVE_DATA__
#define __SQUARE_WAVE_DATA__

#include "IProcessor.h"
#include "Register.h"
#include "DAC.h"

namespace AVRSound {

/*! =============================================
 * @brief ATmega328 矩形波生成モジュール
 * @attention Timer1 / Timer2 を占有します
 *! ============================================= */
class SquareWaveProcessor : public IProcessor
{
public:
    /*! =============================================
     * @brief コンストラクタ
     * @param[in] sound_register レジスタの参照
     * @param[in] index 0:矩形波1 0以外:矩形波2
     *! ============================================= */
    SquareWaveProcessor(DAC& dac, volatile REGISTER& sound_register, uint8_t index);
    ~SquareWaveProcessor();

    /* IProcessor 継承 */
    void Initialize() override;
    void onTimer1Event() override;
    void onTimer2Event() override;

private:
    inline void FrequencyUpdate();
    inline void EnvelopeUpdate();

private: 
    DAC& dac;
    volatile REGISTER& sound_register;
    volatile SQUARE_SETTING* square_register;

    volatile uint8_t volume_index = 0;
    volatile uint8_t output_volume_buffer = 0;

    static const uint8_t WAVE_PATTERN_MAX = 4;
    static const uint8_t WAVE_PATTERN_TABLE[WAVE_PATTERN_MAX][8];
    static const uint8_t VOLUME_TABLE[16];
};
}

#endif