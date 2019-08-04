#include <Arduino.h>
#include "MainProcess.h"

int main(void)
{
    /* 各種レジスタの初期化 */
	init();
	
    /* USER定義 */
	AVRSound::setup();
    
	for (;;) {
        /* USER定義 */
		AVRSound::loop();

        /* シリアルイベントの監視を行っている余裕は無い */
		// if (serialEventRun) serialEventRun();
	}

	return 0;
}