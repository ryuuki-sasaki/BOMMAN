//=============================================================================
//  �ʏ�{��
//�@�X�V���F2016/10/30
//	����ҁF���X�ؗ��M
//=============================================================================
#pragma once
#include "Bom.h"
#include "constants.h"
#include <list>
using namespace std;

class vsCpuPlayer;
class VsPlayer;
class NormalCpu;
class Wall;
class Audio;
class Model;
class effect;

class normalBom : public Bom
{	
public:
	struct bomData
	{
		D3DXVECTOR3 bomPos;						//�{����z�u�����ʒu
		int			bomRenderState;				//�{���̕`��Ɋւ����Ԃ��i�[����
		int			count;						//�o�߃t���[���̃J�E���g���s���ϐ�	
		int			cost;						//CPU�̃R�X�g
		D3DXVECTOR3 throwPos;					//��������̈ʒu�x�N�g��
		bool		isThrowFlag;				//�{����u���������邩
		D3DXVECTOR3	speed;						//���x
		float		theta;						//�p�x(�p)
		int			addRange;					//�����͈͉��Z��
	};
	bomData		 putCharaNum[4];	//�L�����N�^�[�����̃{�������i�[���郊�X�g

protected:	
	Model*       model;		//���f���N���X�̃I�u�W�F�N�g		
	Audio*		 audio;		//�I�[�f�B�I
	list<int>	 explosionProcessList;	//��������������L�����N�^�[�̃��X�g
	int			 count;
	//�G�t�F�N�g
	effect	*fire;	//�Ή�
	effect	*exp;	//�Ή�
	int		bomType;					//�{���̎��
	Wall*	wallUnit;					//�E�H�[�����j�b�g�̏����i�[����ϐ�
	vsCpuPlayer*	vsCpuPlayerUnit;	//��CPU�v���C���[���j�b�g�̏����i�[����ϐ�
	NormalCpu*		cpuUnit[3];			//CPU���j�b�g�̏����i�[����ϐ�
	VsPlayer		*player1, *player2;	//�ΐl�v���C���[���j�b�g�̏����i�[����ϐ�

public:
	normalBom(void);
	void addBom(D3DXVECTOR3 pos/*�v���C���[�̍��W�x�N�g��*/, D3DXVECTOR3 move/*�ړ��x�N�g��*/,int addExpRabge/*�����͈͉��Z��*/, int charaNum /*�L�����N�^�[��*/, D3DXVECTOR3 bomThrowPos/*�������̍��W*/, bool throwFlag/*�{���𓊂��邩�H*/);	 //�{���̕`��
	int retIsBom(float x, float z);		//�{�������݂��邩��Ԃ�

protected:
	~normalBom(void);
	void initialize(void);
	void initializeMain(void);
	void update();
	void collisions(UnitBase* target);											//���j�b�g�̏����擾
	void render(float frameTime, int cameraNum/*�J�����ԍ�*/);					//�{���̕`��
	void explosionFlow();														//���������̗���
	void explosion(int processCharaNum);										//��������		
	Model* GetModel(){return model;}											//�{���̃��f������Ԃ��A�N�Z�X�֐�
	void characterInRange(int x, int z);										//�L�����N�^�[���͈͓���
	void initList();															//�e���X�g�̏�񏉊���
	void throwProcess(int putChara);											//�����鏈��
	void initRender(int putChara);												//�`���񏉊���
	void addEffect(D3DXVECTOR3 pos, int effectType, int chara);					//�G�t�F�N�g�̒ǉ�
	void renderFlow();															//�`��̗���
	float toIntMove(float move) {return (move<0) ? (float)(-1.0 * floor(fabs(move) + 0.5)) : (float)(floor(move + 0.5));}	//Move�̒����𐮐��ɂ���
	float toIntPos(float pos) {return (pos<0) ? (float)(-1.0 * floor(fabs(pos) + 0.5)) : (float)(floor(pos + 0.5));}		//�ʒu�𐮐��ɂ���
	float getAngle(D3DXVECTOR3 pos, float speed);								//���ˊp�x��Ԃ�
	void setExpRange(bool isSet, int chara);									//�����͈͂̃R�X�g��ݒ�
	void callCameraShake(D3DXVECTOR3 pos);										//�J������h�炷�������Ă�
};

