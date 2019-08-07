#ifndef __WAVE_SOUND_CONTROL__
#define __WAVE_SOUND_CONTROL__

#include "ISoundControl.h"
#include "WaveMemorySetting.h"

namespace AVRSound 
{

class WaveMemoryControl : public ISoundControl
{
public:
    WaveMemoryControl();
    ~WaveMemoryControl();

    /* ISoundControl 継承 */
    void Initialize() override;
    void onKeyOn(uint8_t note_num) override;
    void onKeyOff() override;
    void onTimer() override;

private:
    void UpdateSound();

    WAVE_MEMORY_SETTING setting;

    static const uint16_t FREQ_TABLE[0xFF];
};

}

#endif
