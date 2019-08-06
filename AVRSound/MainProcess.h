#ifndef __MAIN_PROCESS__
#define __MAIN_PROCESS__

namespace AVRSound{
    void setup();

    void loop();

    void onTimerEvent();

    void onTimer2Event();

    void onI2CReceived(int num_bytes);
}

#endif