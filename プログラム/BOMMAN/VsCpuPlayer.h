//=============================================================================
//  �v���C���[(CPU�ΐ�p)
//�@�X�V���F2016/10/30
//	����ҁF���X�ؗ��M
//=============================================================================
#pragma once
#include "VsPlayer.h"

class vsCpuPlayer : public VsPlayer
{
public:
	vsCpuPlayer();
	~vsCpuPlayer();
	void setShakeCameraInfo(D3DXVECTOR3 expPos, float shakeSize);	//�J������h�炷���Z�b�g

private:
	void cameraCtrl();									//�J��������
	bool collObstacel(D3DXVECTOR3 pos, bool isThrow);	//��Q���ւ̏Փ˔���
	void collisions(UnitBase* target);					//���j�b�g�̏����擾
	void initialize();									
};

