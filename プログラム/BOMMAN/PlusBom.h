//=============================================================================
//  �v���X�{��
//�@�X�V���F2016/10/30
//	����ҁF���X�ؗ��M
//=============================================================================
#pragma once
#include "normalBom.h"
#include <list>
using namespace std;

class plusBom : public normalBom
{
public:	
	struct plusBomData
	{
		D3DXVECTOR3 bomPos;						//�{���̈ʒu
		int			bomRenderState;				//�{���̕`��Ɋւ����Ԃ��i�[����
		bool		isBomRenderComp;			//�`�悪����������
		int			count;						//�o�߃t���[���̃J�E���g���s���ϐ�	
		int			cost;						//�{����CPU�R�X�g
		D3DXVECTOR3 throwPos;					//��������̈ʒu�x�N�g��
		bool		isThrowFlag;				//�{����u���������邩
		D3DXVECTOR3	speed;						//���x		
		list<plusBomData> dataList;					//�{���̃f�[�^���i�[����z��
		list<plusBomData>::iterator	dataiter;		//�C�e���[�^
		list<plusBomData>::iterator	compareitr;		//��r�p�C�e���[�^
		int			addRange;					//�����͈͉��Z��
	};

	plusBomData		 putCharaNum[4];				//�L�����N�^�[�����̃{�������i�[���郊�X�g

private:
	float theta;	//�p�x(�p)

public:
	plusBom(void);
	void addBom(D3DXVECTOR3 pos/*�v���C���[�̍��W�x�N�g��*/, D3DXVECTOR3 move/*�ړ��x�N�g��*/, int addExpRabge/*�����͈͉��Z��*/, int charaNum /*�L�����N�^�[��*/, D3DXVECTOR3 bomThrowPos/*�������̍��W*/, bool throwFlag/*�{���𓊂��邩�H*/);	 //�v���X�{���̏��ǉ�
	int retIsBom(float x, float z);														//�{�������݂��邩��Ԃ�

private:
	~plusBom(void);	
	void initialize(void);															
	void initializeMain(void);
	void render(float frameTime, int cameraNum/*�J�����ԍ�*/);							//�{���̕`��
	void explosionFlow();																//���������̗��ꐔ
	void explosion(int processCharaNum);												//��������	
	void initList();																	//�e���X�g�̏�񏉊���
	void throwProcess(int putChara);													//�����鏈��
	void initRender(int putChara);														//�`���񏉊���
	void renderFlow();																	//�`��̗���
	void setExpRange(bool isSet, int chara);											//�����͈͂̃R�X�g��ݒ�
	void addEffect(D3DXVECTOR3 pos, int effectType, int chara);							//�G�t�F�N�g�̒ǉ�
	void update();
	void collisions(UnitBase* target);													//���j�b�g�̏����擾
};

