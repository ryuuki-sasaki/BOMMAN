//=============================================================================
//  音を扱うクラス
//　作成日：2015/04/16
//　更新日：2015/07/10
//	制作者：佐々木隆貴
//=============================================================================
#ifndef _AUDIO_H                 
#define _AUDIO_H                
#define WIN32_LEAN_AND_MEAN

#include <xact3.h>
#include "constants.h"

class Audio
{
    // properties
  private:
    IXACT3Engine* xactEngine;   // 音を鳴らすための機能の本体
    IXACT3WaveBank* waveBank;   // wave bankのポインタ
    IXACT3SoundBank* soundBank; // sound bankのポインタ
    XACTINDEX cueI;             // 取り込んだ音,曲の番号
    void* mapWaveBank;          // UnmapViewOfFile()へ渡すポインタ
    void* soundBankData;
    bool coInitialized;         // coInitializeが成功したらtrueが入る

  public:
    // コンストラクタ
	Audio();

    // デストラクタ
    virtual ~Audio();

    // メンバ関数

    // 初期化
    HRESULT initialize(char* waveBankName, char* soundBankName);

    // 毎フレーム読み込み
    void run();

	//音の再生
    void playCue(const char cue[]);

	//音の停止
    void stopCue(const char cue[]);
};

#endif
