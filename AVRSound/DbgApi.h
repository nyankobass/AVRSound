#ifndef __DBG__API__
#define __DBG__API__

namespace AVRSound
{
/* port2をHIGHにする  */
inline void DbgPin2High(){
    PORTD |= 0b00000100;
}

/* port2をLOWにする  */
inline void DbgPin2Low(){
    PORTD &= 0b11111011;
}

/* port3をHIGHにする  */
inline void DbgPin3High(){
    PORTD |= 0b00001000;
}

/* port3をLOWにする  */
inline void DbgPin3Low(){
    PORTD &= 0b11110111;
}

inline void DbgPin3Latch(){
    static bool is_on = true;

    if (is_on){
        DbgPin3High();
    }
    else {
        DbgPin3Low();
    }
}
}

#endif