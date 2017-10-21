//=============================================================================
//  SpeedCPU
//�@�X�V���F2016/10/30
//	����ҁF���X�ؗ��M
//=============================================================================
#pragma once
#include "NormalCpu.h"

class SpeedCpu :
	public NormalCpu
{
private:
	D3DXVECTOR3		 putBomPos;			//�{����u�����ꏊ

public:
	SpeedCpu(void);	
	~SpeedCpu(void);
	void initialize();					

private:					
	void update();
	void collisions(UnitBase* target);	//���j�b�g�̏����擾
};

