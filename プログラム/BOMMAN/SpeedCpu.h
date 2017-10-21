//=============================================================================
//  SpeedCPU
//　更新日：2016/10/30
//	制作者：佐々木隆貴
//=============================================================================
#pragma once
#include "NormalCpu.h"

class SpeedCpu :
	public NormalCpu
{
private:
	D3DXVECTOR3		 putBomPos;			//ボムを置いた場所

public:
	SpeedCpu(void);	
	~SpeedCpu(void);
	void initialize();					

private:					
	void update();
	void collisions(UnitBase* target);	//ユニットの情報を取得
};

