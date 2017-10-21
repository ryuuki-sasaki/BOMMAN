//=============================================================================
//  入力
//　作成日：2015/8/28
//　更新日：2015/8/28
//	制作者：佐々木隆貴
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


//コントローラーキー定義
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

const int MAX_CON = 4;	//コントローラ最大接続数

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
	//キーボードの状態
	BOOL key[256];

	//キーボードが押されたら
	BOOL keyWas[256];

	//コントローラ１台分のデータを入れる変数
	XINPUT_STATE controllerState[MAX_CON];
	bool connected[MAX_CON];

	padButton PadButton;		//ボタン名用
	bool wasPushFlg[MAX_CON][BUTTON_SIZE];		//ボタンが押されたか

public:
	//コンストラクタ
	Input(void);

	//デストラクタ
	~Input(void);

	//初期化
	void initalize();

	//キーを押したとき
	void keyDown(WPARAM wp);

	//キーを離したとき
	void keyUp(WPARAM wp);

	//引数のキーが押されてるか
	BOOL isKeyPush(int id);

	//引数のキーが押されたか
	BOOL wasKeyPush(int id);

	//コントローラが接続されているかチェックする
	void checkControllers();

	//コントローラの状態を取得する
	void readControllers();

	//コントローラのボタンが押されているか
	BOOL isGamePadPush(int num, DWORD button);

	//コントローラのボタンが押されたか
	BOOL wasGamePadPush(int num, DWORD button);

	//左スティックX軸検出
	float getLeftStickX(int num);

	//左スティックY軸検出
	float getLeftStickY(int num);

	//右スティックX軸検出
	float getRightStickX(int num);

	//右スティックY軸検出
	float getRightStickY(int num);

	//左トリガー検出
	float getLeftTrigger(int num);

	//右トリガー検出
	float getRightTrigger(int num);

	//振動
	void vibration(int num, int leftPow, int rightPow);
};


//#endif

