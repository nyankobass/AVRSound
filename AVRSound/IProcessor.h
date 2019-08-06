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
     * @brief 出力の更新を行う
     * @attention TIMER1_COMPA_vect 割り込み中に実行すること
     *! ============================================= */
    virtual void Update() = 0;

    /*! =============================================
     * @brief エンベロープの更新を行う
     * @attention TIMER2_COMPA_vect 割り込み中に実行すること
     *! ============================================= */
    virtual void EnvelopeUpdate() = 0;

};

}

#endif