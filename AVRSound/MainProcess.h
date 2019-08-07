#ifndef __MAIN_PROCESS__
#define __MAIN_PROCESS__

#include <inttypes.h>

namespace AVRSound{
    void setup();

    void loop();

    void onTimerEvent();

    void onTimer2Event();

    void onRegisterChanged(uint8_t addr, uint8_t byte_num);
}

#endif