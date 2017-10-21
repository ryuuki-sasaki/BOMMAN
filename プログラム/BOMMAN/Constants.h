// Programming 2D Games
// Copyright (c) 2011 by: 
// Charles Kelly
// Chapter 5 constants.h v1.0


//#pragma comment(lib, "d3d9.lib")
//#pragma comment(lib, "d3dx9.lib")
//#pragma comment(lib, "winmm.lib")
//#pragma comment(lib, "Xinput.lib")
//リンカエラー対策
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
#define WIN32_LEAN_AND_MEAN		//コンパイルを早くするwindows.hより前に書く

#include <windows.h>

#define _CRTDBG_MAP_ALLOC

#include <stdlib.h>

#include <crtdbg.h>



//-----------------------------------------------
// マクロ
//-----------------------------------------------
// Safely delete pointer referenced item
#define SAFE_DELETE(ptr)       { if (ptr) { delete (ptr); (ptr)=NULL; } }
// Safely release pointer referenced item
#define SAFE_RELEASE(ptr)      { if(ptr) { (ptr)->Release(); (ptr)=NULL; } }
// Safely delete pointer referenced array
#define SAFE_DELETE_ARRAY(ptr) { if(ptr) { delete [](ptr); (ptr)=NULL; } }		//動的配列を作った場合開放するときはこれ
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
const double PI = 3.14159265;					 //PI = 円周率
const float FRAME_RATE  = 30.0f;                 // 1秒間に何回画面を更新するか（希望）（fps)
const float MIN_FRAME_RATE =30.0f;				 // 最低でも更新する回数
const float MIN_FRAME_TIME = 1.0f/FRAME_RATE;		// 何秒に1度更新するか
const float MAX_FRAME_TIME = 1.0f/MIN_FRAME_RATE;	// 遅くても更新したい値

// key mappings
// In this game simple constants are used for key mappings. If variables were used
// it would be possible to save and restore key mappings from a data file.
const UCHAR ESC_KEY      = VK_ESCAPE;   // escape key
const UCHAR ALT_KEY      = VK_MENU;     // Alt key
const UCHAR ENTER_KEY    = VK_RETURN;   // Enter key

//ゲームシーン
enum GameScene
{
	SC_TITLE,	//タイトルシーン
	SC_PLAY,	//プレイシーン
	SC_INIT		//初期化用シーン
};
extern	GameScene	gameScene;	//現在のシーン

//ゲーム中の状態
enum GameSt{		
	STANDBY,
	INGAME,
	PAUSE,
	END
};
extern	GameSt	gameSt;	//現在の状態

//勝敗結果
enum GameRs{		
	WIN1P,
	WIN2P,
	WINVSCPU,
	LOSEVSCPU
};
extern	GameRs	gameRs;	//現在の勝敗

//対戦モードの種類
enum MODE
{
	VS2P,	//2P対戦
	VSCPU,	//CPU対戦
	OPTION	//オプション
};

enum OPTION
{
	NOSELECT,
	THUNDER,
	HURRICANE,
	ALLSELECT
};

extern GameScene gameScene;	//現在のシーン
class  Graphics;			//クラスのプロトタイプ宣言のようなもの、後でGraphicsというクラスを作ると作るという宣言、ポインタは使える
extern Graphics *graphics;	//Graphicsへのポインタ
class  Input;
extern Input	 *input;	//Inputへのポインタ
//const...定数宣言

class  Camera;
extern Camera *cameraA, *cameraB;				

#endif
