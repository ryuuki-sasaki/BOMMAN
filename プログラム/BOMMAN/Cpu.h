//=============================================================================
//  CPU
//�@�X�V���F2016/10/30
//	����ҁF���X�ؗ��M
//=============================================================================
#pragma once
#include "unitbase.h"

class Cpu : public UnitBase
{
protected:
	virtual ~Cpu(void) = 0 {;}	
	virtual void initialize() = 0;
	virtual void effectRender(float frameTime, int cameraNum){;}	//�G�t�F�N�g�`��
	virtual void control(){;}		//�ړ�
	virtual void bomProcess(){;}	//�{���ɑ΂��鏈��
	virtual void putBom(){;}		//�{����u��
};


