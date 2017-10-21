//=============================================================================
//  ハリケーン
//　作成日：2015/6/28
//　更新日：2016/10/30
//	制作者：佐々木隆貴
//=============================================================================
#pragma once
#include "constants.h"
#include "unitbase.h"
#include <list>

class vsCpuPlayer;
class VsPlayer;
class NormalCpu;
class Wall;
class effect;
class Audio;

class Hurricane : public UnitBase
{
private:		
	Audio*	   audio;			//オーディオ
	D3DXVECTOR3 pos;			//位置
	D3DXVECTOR3 move;			//移動方向
	int dirX;					//x方向
	int dirZ;					//z方向
	int prevDirX;
	int prevDirZ;
	int changeDirectionNum;		//方向転換回数
	int timerRange;				//方向転換までの時間
	int count;					//カウント
	bool changeDirection;		//方向転換するか
	bool isOccurrence;			//発生したかどうか

	//エフェクト
	effect*	hurricane;				
	UnitBase* character;

	Wall*	wallUnit;					//ウォールユニットの情報を格納する変数
	vsCpuPlayer*	vsCpuPlayerUnit;	//対CPUプレイヤーユニットの情報を格納する変数
	NormalCpu*		cpuUnit[3];			//CPUユニットの情報を格納する変数
	VsPlayer		*player1, *player2;	//対人プレイヤーユニットの情報を格納する変数

public:
	Hurricane(void);
	~Hurricane(void);

private:
	void initialize(void);			
	void occurrenceHurricane();		//ハリケーンの発生
	void collHurricane();			//ハリケーンの衝突
	void inRange();					//フィールド範囲内か
	void update();					//更新
	void render(float frameTime, int cameraNum);	//描画
	void setInfo();					//情報セット
	void initInfo();				//各情報初期化
	void collisions(UnitBase* target);	//ユニットの情報を取得
	//ハリケーンに衝突したキャラクターを検知
	void collPlayerHurricane(D3DXVECTOR3 conparePos);	
	void collCpuHurricane(D3DXVECTOR3 conparePos);
	void collvsCpuHurricane(D3DXVECTOR3 conparePos);
};

