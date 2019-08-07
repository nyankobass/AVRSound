#ifndef __WAVE_MEMORY_PROCESSOR__
#define __WAVE_MEMORY_PROCESSOR__

#include "IProcessor.h"
#include "Register.h"
#include "DAC.h"

namespace AVRSound {

class WaveMemoryProcessor : public IProcessor
{
public:
    /*! =============================================
     * @brief コンストラクタ
     * @param[in] sound_register レジスタの参照
     *! ============================================= */
    WaveMemoryProcessor(DAC& dac, volatile REGISTER& sound_register);
    ~WaveMemoryProcessor();

    /* IProcessor 継承 */
    void Initialize() override;
    void onTimer1Event() override;
    void onTimer2Event() override;

private:
    inline void FrequencyUpdate();

private: 
    DAC& dac;
    volatile REGISTER& sound_register;
    volatile WAVE_MEMORY_SETTING* wave_memory_register;

    volatile uint8_t output_volume_buffer = 0;
};
}

#endif