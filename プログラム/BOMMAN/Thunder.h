//=============================================================================
//  サンダー
//　作成日：2015/6/28
//　更新日：2016/10/30
//	制作者：佐々木隆貴
//=============================================================================
#pragma once
#include "unitbase.h"
#include "constants.h"
#include "Model.h"
#include "effect.h"

class vsCpuPlayer;
class VsPlayer;
class NormalCpu;
class Model;
class effect;

class Thunder : public UnitBase
{
private:
	D3DXVECTOR3 pos;		//位置
	int probabilityRange;	//発生確率
	int count;				//カウント
	int renderCount;		//描画時間カウント変数
	bool isOccurrence;		//発生したかどうか
	//エフェクト
	effect	*thunder;	

	vsCpuPlayer*	vsCpuPlayerUnit;	//対CPUプレイヤーユニットの情報を格納する変数
	NormalCpu*		cpuUnit[3];			//CPUユニットの情報を格納する変数
	VsPlayer		*player1, *player2;	//対人プレイヤーユニットの情報を格納する変数

public:
	Thunder(void);
	~Thunder(void);

private:
	void initialize();								//初期化
	void occurrenceThunder();						//電撃を発生させる
	void collThunder();								//電撃に当たったか
	void update();									//更新
	void render(float frameTime, int cameraNum);	//描画
	void collisions(UnitBase* target);	//ユニットの情報を取得
	//サンダーに衝突したキャラクターを検知
	void collPlayerThunder(D3DXVECTOR3 conparePos);
	void collCpuThunder(D3DXVECTOR3 conparePos);
	void collvsCpuThunder(D3DXVECTOR3 conparePos);
};

