#ifndef __I_SOUND_CONTROL__
#define __I_SOUND_CONTROL__

#include <inttypes.h>

namespace AVRSound 
{

class ISoundControl
{
public:
    ISoundControl(){};
    virtual ~ISoundControl(){};

    virtual void Initialize() = 0;
    virtual void onKeyOn(uint8_t note_num) = 0;
    virtual void onKeyOff() = 0;
    virtual void onTimer() = 0;
};

}

#endif