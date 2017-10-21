//=============================================================================
//  �T���_�[
//�@�쐬���F2015/6/28
//�@�X�V���F2016/10/30
//	����ҁF���X�ؗ��M
//=============================================================================
#pragma once
#include "unitbase.h"
#include "constants.h"
#include "Model.h"
#include "effect.h"

class vsCpuPlayer;
class VsPlayer;
class NormalCpu;
class Model;
class effect;

class Thunder : public UnitBase
{
private:
	D3DXVECTOR3 pos;		//�ʒu
	int probabilityRange;	//�����m��
	int count;				//�J�E���g
	int renderCount;		//�`�掞�ԃJ�E���g�ϐ�
	bool isOccurrence;		//�����������ǂ���
	//�G�t�F�N�g
	effect	*thunder;	

	vsCpuPlayer*	vsCpuPlayerUnit;	//��CPU�v���C���[���j�b�g�̏����i�[����ϐ�
	NormalCpu*		cpuUnit[3];			//CPU���j�b�g�̏����i�[����ϐ�
	VsPlayer		*player1, *player2;	//�ΐl�v���C���[���j�b�g�̏����i�[����ϐ�

public:
	Thunder(void);
	~Thunder(void);

private:
	void initialize();								//������
	void occurrenceThunder();						//�d���𔭐�������
	void collThunder();								//�d���ɓ���������
	void update();									//�X�V
	void render(float frameTime, int cameraNum);	//�`��
	void collisions(UnitBase* target);	//���j�b�g�̏����擾
	//�T���_�[�ɏՓ˂����L�����N�^�[�����m
	void collPlayerThunder(D3DXVECTOR3 conparePos);
	void collCpuThunder(D3DXVECTOR3 conparePos);
	void collvsCpuThunder(D3DXVECTOR3 conparePos);
};

