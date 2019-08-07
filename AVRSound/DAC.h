#ifndef __DAC__
#define __DAC__

#include <inttypes.h>

namespace AVRSound {
class DAC {
public:
    enum LR{
        LR_MIN = 0,
        L_CH = LR_MIN,
        R_CH,
        LR_MAX
    };

    void Initialize();

    void Output(uint8_t value, LR lr = LR::L_CH);

    void OutputL(uint8_t value);
    void OutputR(uint8_t value);

};
}

#endif