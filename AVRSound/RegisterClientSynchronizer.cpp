#include "RegisterClientSynchronizer.h"

#include <Wire.h>
#include "DbgApi.h"

namespace 
{
    volatile AVRSound::REGISTER* setting = nullptr;
    void(*callback_func)(uint8_t, uint8_t) = nullptr;

    void onI2CReceived(int byte_num);
}

namespace AVRSound
{
namespace RegisterClientSynchronizer
{

void Begin(volatile REGISTER& _setting, uint8_t addr, void(*_callback_func)(uint8_t, uint8_t))
{
    setting = &_setting;
    callback_func = _callback_func;

    /* I2C 初期化 */
    Wire.TwoWire::begin(addr);
    Wire.onReceive(onI2CReceived);
}
}
}

namespace
{

void onI2CReceived(int byte_num)
{
    /* 処理時間計測用に割り込み中port3をHIGHにする  */
    AVRSound::DbgPin3Latch();
    
    /* @note 32 : wire の受信バッファのサイズ */
    uint8_t byte_data[32] = {0};
    
    /* @note そもそも Wire.readBytes に sound_register の 該当する BYTE を渡せば2重コピーしなくても済む */
    /* @note 下記関数の実行時間が約40[us]かかりボトルネック。場合によっては I2C も自前で実装する可能性あり。 */
    uint8_t read_size = Wire.readBytes(byte_data, byte_num);

    if (read_size < 2){
        return;
    }

    if (setting == nullptr){
        return;
    }

    const uint8_t addr = byte_data[0];
    setting->write_byte(addr, &byte_data[1], read_size - 1);

    if (callback_func){
        callback_func(addr, read_size - 1);
    }

    AVRSound::DbgPin3Latch();

    return;
}

}