//=============================================================================
//  �v���C���[
//�@�X�V���F2016/10/30
//	����ҁF���X�ؗ��M
//=============================================================================
#pragma once
#include "unitbase.h"

class Player : public UnitBase
{
protected: 
	virtual ~Player() = 0 {;}
	virtual void initialize() = 0{;}						
	virtual void cameraCtrl() = 0 {;}					//�J��������
	virtual bool collObstacel(D3DXVECTOR3 pos, bool isThrow) = 0 {return false;}	//��Q���ւ̏Փ˔���
	virtual void setShakeCameraInfo(D3DXVECTOR3 expPos, float shakeSize) = 0 {;}	//�J������h�炷���Z�b�g
};

