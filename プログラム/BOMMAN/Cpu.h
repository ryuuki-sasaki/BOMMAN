//=============================================================================
//  CPU
//　更新日：2016/10/30
//	制作者：佐々木隆貴
//=============================================================================
#pragma once
#include "unitbase.h"

class Cpu : public UnitBase
{
protected:
	virtual ~Cpu(void) = 0 {;}	
	virtual void initialize() = 0;
	virtual void effectRender(float frameTime, int cameraNum){;}	//エフェクト描画
	virtual void control(){;}		//移動
	virtual void bomProcess(){;}	//ボムに対する処理
	virtual void putBom(){;}		//ボムを置く
};


