//=============================================================================
//  PsyCPU
//　更新日：2016/10/30
//	制作者：佐々木隆貴
//=============================================================================
#pragma once
#include "NormalCpu.h"

class effect;

class PsyCpu :
	public NormalCpu
{
private:
	//エフェクト
	effect	*psyPut;	//火花
	bool	isRenderEffect;		//エフェクト描画中
	int		renderCount;		//エフェクト描画時間	
	bool	isPsyPut;			//PSYを使ってボムを使ったか

public:
	PsyCpu(void);
	~PsyCpu(void);
	void initialize();					

private:
	void effectRender(float frameTime, int cameraNum);					//エフェクト描画
	void update();														//更新
	void putBom();														//ボムを置く
	void characterInRange();											//キャラクターがPSY能力の範囲内にいるか
	void serchCharacterAround(D3DXVECTOR3 serchPos);					//キャラクターの周囲8タイルを探索
	void addEffect();													//エフェクト追加
	void effectRenderTimeCount();										//エフェクト描画時間カウント
	void bomProcess();													//ボムに対する処理
	void collisions(UnitBase* target);	//ユニットの情報を取得
};




