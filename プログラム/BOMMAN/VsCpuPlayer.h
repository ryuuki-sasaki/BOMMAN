//=============================================================================
//  プレイヤー(CPU対専用)
//　更新日：2016/10/30
//	制作者：佐々木隆貴
//=============================================================================
#pragma once
#include "VsPlayer.h"

class vsCpuPlayer : public VsPlayer
{
public:
	vsCpuPlayer();
	~vsCpuPlayer();
	void setShakeCameraInfo(D3DXVECTOR3 expPos, float shakeSize);	//カメラを揺らす情報セット

private:
	void cameraCtrl();									//カメラ操作
	bool collObstacel(D3DXVECTOR3 pos, bool isThrow);	//障害物への衝突判定
	void collisions(UnitBase* target);					//ユニットの情報を取得
	void initialize();									
};

