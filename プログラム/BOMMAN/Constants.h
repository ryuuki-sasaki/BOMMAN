// Programming 2D Games
// Copyright (c) 2011 by: 
// Charles Kelly
// Chapter 5 constants.h v1.0


//#pragma comment(lib, "d3d9.lib")
//#pragma comment(lib, "d3dx9.lib")
//#pragma comment(lib, "winmm.lib")
//#pragma comment(lib, "Xinput.lib")
//�����J�G���[�΍�
//#pragma comment(lib, "kernel32")
#pragma comment(lib, "user32")
#pragma comment(lib, "gdi32")
//#pragma comment(lib, "winspool")
//#pragma comment(lib, "comdlg32")
#pragma comment(lib, "advapi32")
//#pragma comment(lib, "shell32")
#pragma comment(lib, "ole32")
//#pragma comment(lib, "oleaut32")
//#pragma comment(lib, "uuid")
//#pragma comment(lib, "odbc32")
//#pragma comment(lib, "odbccp32")

#ifndef _CONSTANTS_H            // Prevent multiple definitions if this 
#define _CONSTANTS_H            // file is included in more than one place
#define WIN32_LEAN_AND_MEAN		//�R���p�C���𑁂�����windows.h���O�ɏ���

#include <windows.h>

#define _CRTDBG_MAP_ALLOC

#include <stdlib.h>

#include <crtdbg.h>



//-----------------------------------------------
// �}�N��
//-----------------------------------------------
// Safely delete pointer referenced item
#define SAFE_DELETE(ptr)       { if (ptr) { delete (ptr); (ptr)=NULL; } }
// Safely release pointer referenced item
#define SAFE_RELEASE(ptr)      { if(ptr) { (ptr)->Release(); (ptr)=NULL; } }
// Safely delete pointer referenced array
#define SAFE_DELETE_ARRAY(ptr) { if(ptr) { delete [](ptr); (ptr)=NULL; } }		//���I�z���������ꍇ�J������Ƃ��͂���
// Safely call onLostDevice
#define SAFE_ON_LOST_DEVICE(ptr)    { if(ptr) { ptr->onLostDevice(); } }
// Safely call onResetDevice
#define SAFE_ON_RESET_DEVICE(ptr)   { if(ptr) { ptr->onResetDevice(); } }
#define TRANSCOLOR  SETCOLOR_ARGB(0,255,0,255)  // transparent color (magenta)

//-----------------------------------------------
//                  Constants
//-----------------------------------------------

// window
const char CLASS_NAME[] = "BOMMAN";
const char GAME_TITLE[] = "BOMMAN";
const bool FULLSCREEN = false;              // windowed or fullscreen
const UINT GAME_WIDTH =  1280;               // width of game in pixels
const UINT GAME_HEIGHT = 720;               // height of game in pixels
 
// game
const double PI = 3.14159265;					 //PI = �~����
const float FRAME_RATE  = 30.0f;                 // 1�b�Ԃɉ����ʂ��X�V���邩�i��]�j�ifps)
const float MIN_FRAME_RATE =30.0f;				 // �Œ�ł��X�V�����
const float MIN_FRAME_TIME = 1.0f/FRAME_RATE;		// ���b��1�x�X�V���邩
const float MAX_FRAME_TIME = 1.0f/MIN_FRAME_RATE;	// �x���Ă��X�V�������l

// key mappings
// In this game simple constants are used for key mappings. If variables were used
// it would be possible to save and restore key mappings from a data file.
const UCHAR ESC_KEY      = VK_ESCAPE;   // escape key
const UCHAR ALT_KEY      = VK_MENU;     // Alt key
const UCHAR ENTER_KEY    = VK_RETURN;   // Enter key

//�Q�[���V�[��
enum GameScene
{
	SC_TITLE,	//�^�C�g���V�[��
	SC_PLAY,	//�v���C�V�[��
	SC_INIT		//�������p�V�[��
};
extern	GameScene	gameScene;	//���݂̃V�[��

//�Q�[�����̏��
enum GameSt{		
	STANDBY,
	INGAME,
	PAUSE,
	END
};
extern	GameSt	gameSt;	//���݂̏��

//���s����
enum GameRs{		
	WIN1P,
	WIN2P,
	WINVSCPU,
	LOSEVSCPU
};
extern	GameRs	gameRs;	//���݂̏��s

//�ΐ탂�[�h�̎��
enum MODE
{
	VS2P,	//2P�ΐ�
	VSCPU,	//CPU�ΐ�
	OPTION	//�I�v�V����
};

enum OPTION
{
	NOSELECT,
	THUNDER,
	HURRICANE,
	ALLSELECT
};

extern GameScene gameScene;	//���݂̃V�[��
class  Graphics;			//�N���X�̃v���g�^�C�v�錾�̂悤�Ȃ��́A���Graphics�Ƃ����N���X�����ƍ��Ƃ����錾�A�|�C���^�͎g����
extern Graphics *graphics;	//Graphics�ւ̃|�C���^
class  Input;
extern Input	 *input;	//Input�ւ̃|�C���^
//const...�萔�錾

class  Camera;
extern Camera *cameraA, *cameraB;				

#endif
