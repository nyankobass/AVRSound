#ifndef __I_PROCESSOR__
#define __I_PROCESSOR__

#include <inttypes.h>

namespace AVRSound{

class IProcessor 
{
public:
    IProcessor(){};
    virtual ~IProcessor(){};

    /*! =============================================
     * @brief 初期化処理を行う
     * @attention Timer1 / Timer2 の割り込み設定を行います。
     *! ============================================= */
    virtual void Initialize() = 0;

    /*! =============================================
     * @brief Timer1割り込み時呼出し
     * @attention TIMER1_COMPA_vect 割り込み中に実行すること
     *! ============================================= */
    virtual void onTimer1Event() = 0;

    /*! =============================================
     * @brief Timer2割り込み時呼出し
     * @attention TIMER2_COMPA_vect 割り込み中に実行すること
     *! ============================================= */
    virtual void onTimer2Event() = 0;

};

}

#endif