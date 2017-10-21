//=============================================================================
//  ����
//�@�쐬���F2015/8/28
//�@�X�V���F2015/8/28
//	����ҁF���X�ؗ��M
//=============================================================================

//#ifndef _INPUT_H                // Prevent multiple definitions if this 
//#define _INPUT_H                // file is included in more than one place
//#define WIN32_LEAN_AND_MEAN

#pragma once
class Input;
#include <Windows.h>
#include <Xinput.h>

#pragma comment(lib, "Xinput.lib")
//#pragma comment(lib, "Xinput9_1_0.lib")


//�R���g���[���[�L�[��`
const DWORD GAMEPAD_DPAD_UP        = 0x0001;
const DWORD GAMEPAD_DPAD_DOWN      = 0x0002;
const DWORD GAMEPAD_DPAD_LEFT      = 0x0004;
const DWORD GAMEPAD_DPAD_RIGHT     = 0x0008;
const DWORD GAMEPAD_START_BUTTON   = 0x0010;
const DWORD GAMEPAD_BACK_BUTTON    = 0x0020;
const DWORD GAMEPAD_LEFT_THUMB     = 0x0040;
const DWORD GAMEPAD_RIGHT_THUMB    = 0x0080;
const DWORD GAMEPAD_LEFT_SHOULDER  = 0x0100;
const DWORD GAMEPAD_RIGHT_SHOULDER = 0x0200;
const DWORD GAMEPAD_A              = 0x1000;
const DWORD GAMEPAD_B              = 0x2000;
const DWORD GAMEPAD_X              = 0x4000;
const DWORD GAMEPAD_Y              = 0x8000;

const int MAX_CON = 4;	//�R���g���[���ő�ڑ���

enum padButton
{
	DU,DD,DL,DR,
	START,BACK,
	TL,TR,
	LB,RB,
	A,B,X,Y,
	BUTTON_SIZE
};
static DWORD bt[BUTTON_SIZE] = {GAMEPAD_DPAD_UP, GAMEPAD_DPAD_DOWN, GAMEPAD_DPAD_LEFT, GAMEPAD_DPAD_RIGHT,
									GAMEPAD_START_BUTTON, GAMEPAD_BACK_BUTTON,
									GAMEPAD_LEFT_THUMB, GAMEPAD_RIGHT_THUMB,
									GAMEPAD_LEFT_SHOULDER, GAMEPAD_RIGHT_SHOULDER,
									GAMEPAD_A, GAMEPAD_B, GAMEPAD_X, GAMEPAD_Y};

class Input
{	
	//�L�[�{�[�h�̏��
	BOOL key[256];

	//�L�[�{�[�h�������ꂽ��
	BOOL keyWas[256];

	//�R���g���[���P�䕪�̃f�[�^������ϐ�
	XINPUT_STATE controllerState[MAX_CON];
	bool connected[MAX_CON];

	padButton PadButton;		//�{�^�����p
	bool wasPushFlg[MAX_CON][BUTTON_SIZE];		//�{�^���������ꂽ��

public:
	//�R���X�g���N�^
	Input(void);

	//�f�X�g���N�^
	~Input(void);

	//������
	void initalize();

	//�L�[���������Ƃ�
	void keyDown(WPARAM wp);

	//�L�[�𗣂����Ƃ�
	void keyUp(WPARAM wp);

	//�����̃L�[��������Ă邩
	BOOL isKeyPush(int id);

	//�����̃L�[�������ꂽ��
	BOOL wasKeyPush(int id);

	//�R���g���[�����ڑ�����Ă��邩�`�F�b�N����
	void checkControllers();

	//�R���g���[���̏�Ԃ��擾����
	void readControllers();

	//�R���g���[���̃{�^����������Ă��邩
	BOOL isGamePadPush(int num, DWORD button);

	//�R���g���[���̃{�^���������ꂽ��
	BOOL wasGamePadPush(int num, DWORD button);

	//���X�e�B�b�NX�����o
	float getLeftStickX(int num);

	//���X�e�B�b�NY�����o
	float getLeftStickY(int num);

	//�E�X�e�B�b�NX�����o
	float getRightStickX(int num);

	//�E�X�e�B�b�NY�����o
	float getRightStickY(int num);

	//���g���K�[���o
	float getLeftTrigger(int num);

	//�E�g���K�[���o
	float getRightTrigger(int num);

	//�U��
	void vibration(int num, int leftPow, int rightPow);
};


//#endif

