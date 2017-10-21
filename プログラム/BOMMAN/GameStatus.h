//=============================================================================
//  �Q�[���̏�ԊǗ�
//�@�쐬���F2015/8/22
//�@�X�V���F2016/10/30
//	����ҁF���X�ؗ��M
//=============================================================================
#pragma once
#include "UnitBase.h"
#include "constants.h"
#include "textureManager.h"

class vsCpuPlayer;
class VsPlayer;
class NormalCpu;
class Wall;
class Image;

class gameStatus : public UnitBase
{
	//�J�[�\���ʒu
	enum Cursor
	{
		RESUME,
		RETRY,
		EXIT
	}cursor;

	//�N���X�쐬
	//�e�N�X�`��
	TextureManager texMenu;		//�|�[�Y���j���[�e�N�X�`��
	Image	*menuImg;			
	TextureManager texCursor;	//�J�[�\���e�N�X�`��
	Image	*cursorImg;
	TextureManager texs_Cursor;	//�X���[���J�[�\���e�N�X�`��
	Image	*s_cursorImg;
	TextureManager texResult;	//���U���g�e�N�X�`��
	Image	*resultImg;
	TextureManager texString;	//���s�����e�N�X�`��
	Image	*stringImg;
	TextureManager texNum;		//�i���o�[�e�N�X�`��
	Image	*numImg, *numImg2, *numImg3;
	DWORD timeCount;			//�Q�[���J�n����
	int set1;
	int set2;
	int set3;		
	vsCpuPlayer*	vsCpuPlayerUnit;	//��CPU�v���C���[���j�b�g�̏����i�[����ϐ�
	NormalCpu*		cpuUnit[3];			//CPU���j�b�g�̏����i�[����ϐ�
	Wall*			wallUnit;			//�E�H�[�����j�b�g�̏����i�[����ϐ�
	VsPlayer		*player1, *player2;	//�ΐl�v���C���[���j�b�g�̏����i�[����ϐ�

public:
	gameStatus(void);
	~gameStatus(void);
	void initialize();
	void render(float frameTime, int cNum);
	void update();

private:
	void resultRender(int cNum);
	void Pause();
	void End();
	void gameTimeCount();
	void loseDecision();		//���S����
	void collisions(UnitBase* target);	//���j�b�g�̏����擾	
};

