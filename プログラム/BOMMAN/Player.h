//=============================================================================
//  プレイヤー
//　更新日：2016/10/30
//	制作者：佐々木隆貴
//=============================================================================
#pragma once
#include "unitbase.h"

class Player : public UnitBase
{
protected: 
	virtual ~Player() = 0 {;}
	virtual void initialize() = 0{;}						
	virtual void cameraCtrl() = 0 {;}					//カメラ操作
	virtual bool collObstacel(D3DXVECTOR3 pos, bool isThrow) = 0 {return false;}	//障害物への衝突判定
	virtual void setShakeCameraInfo(D3DXVECTOR3 expPos, float shakeSize) = 0 {;}	//カメラを揺らす情報セット
};

