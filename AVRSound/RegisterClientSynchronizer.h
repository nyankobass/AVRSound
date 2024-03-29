/** =========================================================================
 * @file    RegisterClientSynchronizer.h
 * @brief   レジスタの同期処理を行う。
 * @author  nyankobass
 * @date    2019/08/07
 * 
 * @note    同期処理と言えば聞こえはいいが、
 *          その実Master側からの一方的な書き込みが行われるのみである。
 *          今のところ Master 側から Read することもできない。
 * ========================================================================== */
#ifndef __REGISTER_CLIENT_SYNCHRONIZER__
#define __REGISTER_CLIENT_SYNCHRONIZER__

#include "Register.h"

namespace AVRSound {

namespace RegisterClientSynchronizer
{
    /*! =============================================
     * @brief レジスタの同期処理の初期化を行う
     * @param[in] setting           レジスタ
     * @param[in] addr              APUのアドレスを設定する
     * @param[in] _callback_func    レジスタ値に変更があった時呼び出されるコールバック関数
     *! ============================================= */
    void Begin(volatile REGISTER& _setting, uint8_t addr, void(*_callback_func)(uint8_t, uint8_t));  
}

}

#endif