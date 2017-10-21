//=============================================================================
//  �n���P�[��
//�@�쐬���F2015/6/28
//�@�X�V���F2016/10/30
//	����ҁF���X�ؗ��M
//=============================================================================
#pragma once
#include "constants.h"
#include "unitbase.h"
#include <list>

class vsCpuPlayer;
class VsPlayer;
class NormalCpu;
class Wall;
class effect;
class Audio;

class Hurricane : public UnitBase
{
private:		
	Audio*	   audio;			//�I�[�f�B�I
	D3DXVECTOR3 pos;			//�ʒu
	D3DXVECTOR3 move;			//�ړ�����
	int dirX;					//x����
	int dirZ;					//z����
	int prevDirX;
	int prevDirZ;
	int changeDirectionNum;		//�����]����
	int timerRange;				//�����]���܂ł̎���
	int count;					//�J�E���g
	bool changeDirection;		//�����]�����邩
	bool isOccurrence;			//�����������ǂ���

	//�G�t�F�N�g
	effect*	hurricane;				
	UnitBase* character;

	Wall*	wallUnit;					//�E�H�[�����j�b�g�̏����i�[����ϐ�
	vsCpuPlayer*	vsCpuPlayerUnit;	//��CPU�v���C���[���j�b�g�̏����i�[����ϐ�
	NormalCpu*		cpuUnit[3];			//CPU���j�b�g�̏����i�[����ϐ�
	VsPlayer		*player1, *player2;	//�ΐl�v���C���[���j�b�g�̏����i�[����ϐ�

public:
	Hurricane(void);
	~Hurricane(void);

private:
	void initialize(void);			
	void occurrenceHurricane();		//�n���P�[���̔���
	void collHurricane();			//�n���P�[���̏Փ�
	void inRange();					//�t�B�[���h�͈͓���
	void update();					//�X�V
	void render(float frameTime, int cameraNum);	//�`��
	void setInfo();					//���Z�b�g
	void initInfo();				//�e��񏉊���
	void collisions(UnitBase* target);	//���j�b�g�̏����擾
	//�n���P�[���ɏՓ˂����L�����N�^�[�����m
	void collPlayerHurricane(D3DXVECTOR3 conparePos);	
	void collCpuHurricane(D3DXVECTOR3 conparePos);
	void collvsCpuHurricane(D3DXVECTOR3 conparePos);
};

