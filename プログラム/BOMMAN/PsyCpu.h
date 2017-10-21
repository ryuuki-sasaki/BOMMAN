//=============================================================================
//  PsyCPU
//�@�X�V���F2016/10/30
//	����ҁF���X�ؗ��M
//=============================================================================
#pragma once
#include "NormalCpu.h"

class effect;

class PsyCpu :
	public NormalCpu
{
private:
	//�G�t�F�N�g
	effect	*psyPut;	//�Ή�
	bool	isRenderEffect;		//�G�t�F�N�g�`�撆
	int		renderCount;		//�G�t�F�N�g�`�掞��	
	bool	isPsyPut;			//PSY���g���ă{�����g������

public:
	PsyCpu(void);
	~PsyCpu(void);
	void initialize();					

private:
	void effectRender(float frameTime, int cameraNum);					//�G�t�F�N�g�`��
	void update();														//�X�V
	void putBom();														//�{����u��
	void characterInRange();											//�L�����N�^�[��PSY�\�͈͓͂̔��ɂ��邩
	void serchCharacterAround(D3DXVECTOR3 serchPos);					//�L�����N�^�[�̎���8�^�C����T��
	void addEffect();													//�G�t�F�N�g�ǉ�
	void effectRenderTimeCount();										//�G�t�F�N�g�`�掞�ԃJ�E���g
	void bomProcess();													//�{���ɑ΂��鏈��
	void collisions(UnitBase* target);	//���j�b�g�̏����擾
};




