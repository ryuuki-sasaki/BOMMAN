//=============================================================================
//�Q�[���������N���X
//�쐬���F2014/09/08
//�X�V���F2016/10/19
//����ҁF���X�ؗ��M
//=============================================================================
#pragma once
#include "textureManager.h"
#include "image.h"
#include "game.h"

class SceneBase;
class Audio;

class PlayGame : public Game
{
private:
	SceneBase* nowScene;	//���݂̃V�[��(SceneBase*)
	GameScene  prevScene;	//�O�̃V�[��  (GameScene)
	Audio*	   audio;		//�I�[�f�B�I

	//���[�h���
	TextureManager loadTex;			//���[�h�e�N�X�`��
	Image          loadImg;			//�C���[�W

	BOOL           sceneChangFlg;	//�`�F���W����true

public:
	//�R���X�g���N�^
	PlayGame(void);
	//�f�X�g���N�^
	virtual ~PlayGame(void);
	//�Q�[����������
	void initialize(HWND hwnd);
	void update();			//Game����̏������z�֐����I�[�o�[���C�h����
	void ai();				//������
	void collisions();		//������
	void render();			//������
	void releaseAll();
	void resetAll();
	void playSceneCamera();
};

