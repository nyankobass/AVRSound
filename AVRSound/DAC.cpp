#include "DAC.h"

#include <Arduino.h>
#include <SPI.h>
#include <avr/io.h>


namespace AVRSound
{

/* @attention   MCP4902 向け実装 */
/*              別ICを利用する場合は実装を変更すること */
void DAC::Initialize()
{
    pinMode(SS, OUTPUT);

    SPI.begin();
    SPI.setClockDivider(SPI_CLOCK_DIV4);
}

/* @attention   MCP4902 向け実装 */
/*              別ICを利用する場合は実装を変更すること */
void DAC::OutputL(uint8_t value)
{
    uint16_t _value = (((uint16_t)value) << 3) + (1 << 10);
    uint8_t byte_h = 0b00110000 + (_value >> 8);
    uint8_t byte_l = _value;

    /* SS (10pin:PB2) を LOW に */
    PORTB &= 0b11111011;
    
    SPI.transfer(byte_h);
    SPI.transfer(byte_l);
    
    /* SS (10pin:PB2) を HIGH に */
    PORTB |= 0b00000100;
}

/* @attention   MCP4902 向け実装 */
/*              別ICを利用する場合は実装を変更すること */
void DAC::OutputR(uint8_t value)
{
    uint16_t _value = (((uint16_t)value) << 3) + (1 << 10);
    uint8_t byte_h = 0b00110000 + (_value >> 8);
    uint8_t byte_l = _value;

    /* SS (10pin:PB2) を LOW に */
    PORTB &= 0b11111011;
    
    SPI.transfer(byte_h);
    SPI.transfer(byte_l);
    
    /* SS (10pin:PB2) を HIGH に */
    PORTB |= 0b00000100;
}


void DAC::Output(uint8_t value, LR lr)
{
    if (lr == LR::L_CH){
        OutputL(value);
    }
    else {
        OutputR(value);
    }
}

}