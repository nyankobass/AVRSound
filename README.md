# AVRSound

# Overview
ATmega328(Arduino Uno) と任意の Arduino の2台にて、8bit シンセを実現する。

![](doc/BlockDiagram.png)

# AVRSound
ATmeag328 をターゲットにモノフォニックな波形メモリ音源を構成する。

レジスタはGB音源互換。

# AVRSoundMaster
AVRSound を I2C を介して操作する。