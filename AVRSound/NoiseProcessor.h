#ifndef __NOISE_PROCESSOR__
#define __NOISE_PROCESSOR__

#include "IProcessor.h"
#include "Register.h"
#include "DAC.h"

namespace AVRSound
{

class NoiseProcessor : public IProcessor
{
public:
    /*! =============================================
     * @brief コンストラクタ
     * @param[in] sound_register レジスタの参照
     *! ============================================= */
    NoiseProcessor(DAC& dac, volatile REGISTER& sound_register);
    ~NoiseProcessor() override;

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
    volatile NOISE_SETTING* noise_register;

    volatile uint8_t volume_index = 0;
    volatile uint8_t output_volume_buffer = 0;

    static const uint8_t VOLUME_TABLE[16];
};

}
#endif