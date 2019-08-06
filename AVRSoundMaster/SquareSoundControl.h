#ifndef __SQUARE_SOUND_CONTROL__
#define __SQUARE_SOUND_CONTROL__

#include "ISoundControl.h"
#include "SquareSetting.h"

namespace AVRSound 
{

class SquareSoundControl : public ISoundControl
{
public:
    SquareSoundControl();
    ~SquareSoundControl();

    /* ISoundControl 継承 */
    void Initialize() override;
    void onKeyOn(uint8_t note_num) override;
    void onKeyOff() override;
    void onTimer() override;

private:
    void UpdateSound();


    SQUARE_SETTING setting;

    static const uint16_t FREQ_TABLE[0xFF];
};

}

#endif
