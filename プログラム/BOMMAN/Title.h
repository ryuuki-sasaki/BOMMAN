//=============================================================================
//�^�C�g���������N���X
//�쐬���F2014/09/02
//�X�V���F2014/09/02
//����ҁF���X�ؗ��M
//=============================================================================
#pragma once	//��d��`�h�~�iVS�̂݁j
#include "constants.h"	
#include "textureManager.h"
#include "image.h"
#include "UnitBase.h"

class Audio;

class Title : public UnitBase
{
	Audio*	   audio;		//�I�[�f�B�I
	//�e�N�X�`��
	TextureManager texture;
	TextureManager texCursor;
	TextureManager texs_Cursor;
	TextureManager texOption;
	//�C���[�W
	Image		   image;
	Image		   imageCursor;
	Image		   images_Cursor;	
	Image		   imageOption;	

	int			   cursor;
	int			   optionCursor;
	bool		   isSet;

public:
	Title(void);						//�R���X�g���N�^�͍ŏ���1�񂵂��Ă΂�Ȃ��i�v���C���[�̏����ʒu���R���X�g���N�^�Œ�`�����ꍇ1��N���A�������̂�2��ڃv���C���悤�Ƃ���Ƃ��łɃS�[�����Ă���j
	~Title(void);						//�f�X�g���N�^
	void initialize();					//������
	void render(float frameTime, int cameraNum);		//�`��
	void update();						//�L�[�������ꂽ�玟�̃V�[����
	void setOption();					//�I�v�V�����̐ݒ�
};

