#ifndef __DAC__
#define __DAC__

#include <inttypes.h>

namespace AVRSound {
class DAC {
public:

    void Initialize();

    void Output(uint8_t value);

};
}

#endif