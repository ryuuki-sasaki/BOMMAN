// Programming 2D Games
// Copyright (c) 2011 by: 
// Charles Kelly
// Chapter 5 game.h v1.0

#ifndef _GAME_H                 //���̃t�@�C���������̉ӏ��ɃC���N���[�h�����ꍇ��
#define _GAME_H                 //���d��`��h��
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <Mmsystem.h>
#include "graphics.h"
#include "input.h"
#include "constants.h"
#include "gameError.h"

class Game
{
protected:
   //�����o�[�ϐ�
	HWND     hwnd;				//�E�B���h�E�n���h��
	HRESULT	 hr;				//�����̏������������̌��ʂ�����
	LARGE_INTEGER timeStart;	//�p�t�H�[�}���X�J�E���^�̊J�n�l
	LARGE_INTEGER timeEnd;		//			�V			�@�I���l
	LARGE_INTEGER timerFreq;	//���g��
	float frameTime;			//�Ō�̃t���[���ɗv�������� 
	float fps;					//�t���[�����[�g�i1�b������̃t���[�����j
	DWORD sleepTime;			//�t���[���ԂŃX���[�v���鎞��ms
	bool paused;				//�Q�[�����ꎞ��~����Ă���ꍇ�Atrue
	bool initialized;

public:
	//�R���X�g���N�^
	Game();
	//�f�X�g���N�^
	virtual~Game();
	//�����o�[�֐�

	LRESULT messageHandler(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lparam);	//�_�C���N�g�w��Windows�𖳎�����̂���{�����ǖ����ł��Ȃ��Ƃ��̏���
	//�Q�[����������
	//hwnd�̓E�B���h�E�ւ̃n���h��
	virtual void initialize(HWND hwnd);		
	//WinMain���̃��C���̃��b�Z�[�W���[�v��ran���J��Ԃ��i1�b�Ԃɂ�鏈���������ɓ���j
	virtual void run(HWND);
	virtual void releaseAll();		//�{�̂��J�����鏈��
	virtual void resetAll();		//�t���X�N���[���ɐ؂�ւ���Ê��Ƀ��Z�b�g����Ƃ��i�H�j
	virtual void deleteAll();		//�{�̍폜
	virtual void renderGame();		//��ʂ�`��

	//���������O���t�B�b�N�f�o�C�X�������i�Q�[����ʂ������������̏����j
	virtual void handleLostGraphicsDevice();

    // Set display mode (fullscreen, window or toggle)
    void setDisplayMode(graphicsNS::DISPLAY_MODE mode = graphicsNS::TOGGLE);

    //Graphics�ւ̃|�C���^��߂��i�A�N�Z�X�֐��j
	Graphics* getGraphics() {return graphics;}
	//Input�ւ̃|�C���^��߂��i�A�N�Z�X�֐��j
	Input*    getInput() {return input;}
	//�Q�[�����I��
	void exitGame() {PostMessage(hwnd,WM_DESTROY,0,0);}
	//�������z�֐��̐錾
	//�����̊֐��́AGame���p������֐����ŋL�q����K�v������
	//�Q�[���A�C�e�����X�V
	virtual void update() = 0;	//�ړ���J�E���g�Ȃǉ�ʂɕ`�悷����̈ȊO�ŏ����������
	//AI�v�Z�����s
	virtual void ai() = 0;		//ai���K�v�ȂƂ�
	//�Q�[���A�C�e���̏���n��
	virtual void collisions() = 0;	//�Q�[���A�C�e���̏���n��
	/*
	//�O���t�B�b�N�X�������_�[
	graphics->SpriteBegin();
	//�X�v���C�g��`��
	graphics->SpriteEnd();
	*/
	//   draw non-sprites
	virtual void render() = 0;		//��ʂɊG��\��
};
#endif	_GAME_H

/*
virtual = ���z�֐��@�iGame�ɂ���Ă��̏ꏊ��ύX�ł���j�i�p����ŕύX���Ă������j
����virtual�����āA����=0�����Ə������z�֐�
�������z�֐�...�p����ŕύX���Ȃ���΂Ȃ�Ȃ�
*/