#ifndef __NOISE_SOUND_CONTROL__
#define __NOISE_SOUND_CONTROL__

#include "ISoundControl.h"
#include "NoiseSetting.h"

namespace AVRSound 
{

class NoiseSoundControl : public ISoundControl
{
public:
    NoiseSoundControl();
    ~NoiseSoundControl();

    /* ISoundControl 継承 */
    void Initialize() override;
    void onKeyOn(uint8_t note_num) override;
    void onKeyOff() override;
    void onTimer() override;

private:
    void UpdateSound();

    NOISE_SETTING setting;
};

}

#endif
