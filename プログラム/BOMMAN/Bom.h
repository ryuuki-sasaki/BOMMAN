//=============================================================================
//  �{��
//�@�X�V���F2016/10/30
//	����ҁF���X�ؗ��M
//=============================================================================
#pragma once
#include "unitbase.h"

class Bom : public UnitBase 
{
public:
	virtual void addBom(D3DXVECTOR3 pos/*�v���C���[�̍��W�x�N�g��*/, D3DXVECTOR3 move/*�ړ��x�N�g��*/,int addExpRabge/*�����͈͉��Z��*/, int charaNum /*�L�����N�^�[��*/, D3DXVECTOR3 bomThrowPos/*�������̍��W*/, bool throwFlag/*�{���𓊂��邩�H*/){;}	 //�{���̕`��

protected:
	virtual ~Bom(void) = 0 {;}
	virtual void initialize(void) = 0 {;}
	virtual void initializeMain(void) = 0 {;}
	virtual void render(float frameTime, int cameraNum/*�J�����ԍ�*/){;}	//�{���̕`��
	virtual void explosionFlow(){;}											//���������̗���
	virtual void explosion(int processCharaNum){;}							//��������		
	virtual void initList(){;}												//�e���X�g�̏�񏉊���
	virtual void throwProcess(int putChara){;}								//�����鏈��
	virtual void initRender(int putChara){;}								//�`���񏉊���
	virtual void addEffect(D3DXVECTOR3 pos, int effectType, int chara){;}	//�G�t�F�N�g�̒ǉ�
	virtual void renderFlow(){;}											//�`��̗���
	virtual void setExpRange(bool isSet, int chara){;}						//�����͈͂̃R�X�g��ݒ�
};

