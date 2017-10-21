//=============================================================================
//  ゲームの状態管理
//　作成日：2015/8/22
//　更新日：2016/10/30
//	制作者：佐々木隆貴
//=============================================================================
#include "gameStatus.h"
#include "modeChange.h"
#include "option.h"
#include "input.h"
#include "Wall.h"
#include "NormalCpu.h"
#include "PsyCpu.h"
#include "SpeedCpu.h"
#include "VsPlayer.h"
#include "vsCpuPlayer.h"
#include "image.h"

#define MINIT 60	//ゲーム時間
#define	PICSIZE 64	//テクスチャの大きさ
#define CHARANUM 3	//キャラクター数
#define ADDSIZEX 480 / 2
#define ADDSIZEY 360
#define NOTTIMECOUNT 100	//カウントをしない

//CPUキャラクターの種類
enum CHARA_TYPE
{
	PYECPU,
	NORMALCPU,
	SPEEDCPU
};

//=============================================================================
// コンストラクタ
//=============================================================================
gameStatus::gameStatus(void)
{	
	//イメージ
	numImg = new Image;	
	numImg2 = new Image;
	numImg3 = new Image;
	menuImg = new Image;
	cursorImg = new Image;
	s_cursorImg = new Image;
	resultImg = new Image;
	stringImg = new Image;

	//テクスチャ読み込み
	if (!texMenu.initialize(graphics,"pictures\\menu.png"))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing menu texture"));
    if (!menuImg->initialize(graphics,0,0,0,&texMenu))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing menu"));

	if (!texCursor.initialize(graphics,"pictures\\cursor.png"))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing cursor texture"));
    if (!cursorImg->initialize(graphics,0,0,0,&texCursor))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing cursor"));

	if (!texs_Cursor.initialize(graphics,"pictures\\s_cursor.png"))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing cursor texture"));
    if (!s_cursorImg->initialize(graphics,0,0,0,&texs_Cursor))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing cursor"));

	if (!texResult.initialize(graphics,"pictures\\result.png"))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing result texture"));
    if (!resultImg->initialize(graphics,0,0,0,&texResult))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing result"));

	if (!texString.initialize(graphics,"pictures\\resultString.png"))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing string texture"));
    if (!stringImg->initialize(graphics,400,200,0,&texString))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing string"));

	//テクスチャ読み込み
	if (!texNum.initialize(graphics,"pictures\\number.png"))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing number texture"));
    if (!numImg->initialize(graphics,PICSIZE/2,PICSIZE,0,&texNum))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing number"));
	if (!numImg2->initialize(graphics,PICSIZE/2,PICSIZE,0,&texNum))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing number"));
	if (!numImg3->initialize(graphics,PICSIZE/2,PICSIZE,0,&texNum))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing number"));

	initialize();
}

//=============================================================================
// 初期化
// 引数： なし
// 戻り値：なし
//=============================================================================
void gameStatus::initialize()
{
	gameSt = INGAME;
	timeCount = 0;
	//カーソル位置セット
	cursor = RESUME;
	cursorImg->setX(100+ADDSIZEX/2);
	cursorImg->setY(600);
	s_cursorImg->setX(320+ADDSIZEX);
	s_cursorImg->setY(216+ADDSIZEY/3);

	//時間経過用数の位置セット
	numImg->setY(20);
	numImg2->setY(20);
	numImg3->setY(20);
	numImg ->setX(580);			
	numImg2->setX(644);
	numImg3->setX(676);

	//メニュー位置セット
	menuImg->setX(300+ADDSIZEX);
	menuImg->setY(140+ADDSIZEY/3);

	//リザルト文字用画像位置セット
	stringImg->setY(100+ADDSIZEY/3);

	set1 = 2;
	set2 = 5;
	set3 = 9;
	timeCount = 1;

	wallUnit = nullptr;
	player1 = nullptr;
	player2 = nullptr;
	cpuUnit[NORMALCPU] = nullptr;
	cpuUnit[PYECPU] = nullptr;
	cpuUnit[SPEEDCPU] = nullptr;
}

//=============================================================================
// デストラクタ
//=============================================================================
gameStatus::~gameStatus(void)
{
	SAFE_DELETE(menuImg);
	SAFE_DELETE(cursorImg);
	SAFE_DELETE(s_cursorImg);
	SAFE_DELETE(resultImg);
	SAFE_DELETE(stringImg);
	SAFE_DELETE(numImg);
	SAFE_DELETE(numImg2);
	SAFE_DELETE(numImg3);
}

//=============================================================================
//シーンのユニットを取得
//引数：シーンのユニット
//戻り値：なし
//=============================================================================
//ScceneBaseから呼び出されたUnitBaseのcollisionsからターゲット取得
//typeid...typeid(型名または型型の変数名）...何型で作られたか
void gameStatus::collisions(UnitBase* target)
{
	if(typeid(*target) == typeid(Wall))
		wallUnit = (Wall*)target;
	
	if(typeid(*target) == typeid(vsCpuPlayer))
		vsCpuPlayerUnit = (vsCpuPlayer*)target;

	if(typeid(*target) == typeid(NormalCpu))
		cpuUnit[NORMALCPU] = (NormalCpu*)target;

	if(typeid(*target) == typeid(PsyCpu))
		cpuUnit[PYECPU] = (PsyCpu*)target;

	if(typeid(*target) == typeid(SpeedCpu))
		cpuUnit[SPEEDCPU] = (SpeedCpu*)target;

	
	if(modeChange::GetInstance() -> getMode() == VS2P
		&& typeid(*target) == typeid(VsPlayer)) {
			VsPlayer* VsPlayerUnit = (VsPlayer*)target; 
			if(VsPlayerUnit->player == 1)
				player1 = VsPlayerUnit;
			else
				player2 = VsPlayerUnit;
	}

}

//=============================================================================
// 描画
// 引数：1フレームの速度、カメラ番号
// 戻値：なし
//=============================================================================
void gameStatus::render(float frameTime, int cNum)
{
	gameTimeCount();
	numImg ->draw();
	numImg2->draw();
	numImg3->draw();

	//一時停止時の描画
	if(gameSt == PAUSE)
	{
		menuImg->draw();
		s_cursorImg->draw();
	}

	//リザルト時の描画
	if(gameSt == END)
	{
		//リザルト表示
		resultRender(cNum);

		resultImg->draw();		//背景
		stringImg->draw();		//文字
		cursorImg->draw();		//カーソル
	}
}

//=============================================================================
// リザルト画像描画
// 引数：カメラ番号
// 戻値：なし
//=============================================================================
void gameStatus::resultRender(int cNum)
{
	if(modeChange::GetInstance() -> getMode() == VS2P)
	{
		if(cNum == 1/*1P*/)	
		{
			stringImg->setCurrentFrame(gameRs == WIN1P ? 0 : 1);
			stringImg->setX(0+ADDSIZEX);
		} else if(cNum == 2/*2P*/) {
			stringImg->setCurrentFrame(gameRs == WIN2P ? 0 : 1);
			stringImg->setX(400+ADDSIZEX);
		}
	} else {
		stringImg->setCurrentFrame(gameRs == WINVSCPU ? 0 : 1);
		stringImg->setX(200+ADDSIZEX);
	}
}
//=============================================================================
// update処理
// 引数：なし
// 戻値：なし
//=============================================================================
void gameStatus::update()
{
	//ゲーム中なら
	if(gameSt == INGAME)
	{
		//キャラクターの死亡判定
		loseDecision();
	}

	//SPACEキーで一時停止
	if(input->wasKeyPush(VK_SPACE) || input->wasGamePadPush(1, GAMEPAD_X) || input->wasGamePadPush(2, GAMEPAD_X))
	{
		//一時停止中なら解除・ゲーム中なら一時停止
		if(gameSt == PAUSE)
		{
			gameSt = INGAME;
		}
		else if(gameSt == INGAME)
		{
			gameSt = PAUSE;

			//カーソル位置リセット
			cursor = RESUME;
			s_cursorImg->setY(216+ADDSIZEY/3);
		}
	}

	//ゲームの状態
	switch(gameSt)
	{
	case INGAME:
		break;
	case PAUSE:
		Pause();
		break;
	case END:
		End();
		break;
	}
}

//=============================================================================
// 一時停止時の処理
// 引数：なし
// 戻値：なし
//=============================================================================
void gameStatus::Pause()
{
	//一時停止中エンターキーで終了
	if(input->wasKeyPush(VK_RETURN) || input->wasGamePadPush(1, GAMEPAD_B))
	{
		switch(cursor)
		{
		case RESUME:
			gameSt = INGAME;
			break;
		case RETRY:
			gameScene = SC_INIT;
			break;
		case EXIT:
			modeChange::GetInstance() -> setMode(0);
			option::GetInstance() -> setMode(0);
			gameScene = SC_TITLE;
			break;
		}
	}

	//カーソル移動
	if(input->wasKeyPush(VK_UP) || input->wasGamePadPush(1, GAMEPAD_DPAD_UP))
	{
		switch(cursor)
		{
		case RESUME:
			cursor = EXIT;
			s_cursorImg->setY(286+ADDSIZEY/3);
			break;
		case RETRY:
			cursor = RESUME;
			s_cursorImg->setY(216+ADDSIZEY/3);
			break;
		case EXIT:
			cursor = RETRY;
			s_cursorImg->setY(251+ADDSIZEY/3);
			break;
		}
	}

	if(input->wasKeyPush(VK_DOWN) || input->wasGamePadPush(1, GAMEPAD_DPAD_DOWN))
	{
		switch(cursor)
		{
		case RESUME:
			cursor = RETRY;
			s_cursorImg->setY(251+ADDSIZEY/3);
			break;
		case RETRY:
			cursor = EXIT;
			s_cursorImg->setY(286+ADDSIZEY/3);
			break;
		case EXIT:
			cursor = RESUME;
			s_cursorImg->setY(216+ADDSIZEY/3);
			break;
		}
	}
}

//=============================================================================
// ゲーム終了時の処理
// 引数：なし
// 戻値：なし
//=============================================================================
void gameStatus::End()
{	
	//一時停止中エンターキーで終了
	if(input->wasKeyPush(VK_RETURN) || input->wasGamePadPush(1, GAMEPAD_B))
	{
		switch(cursor)
		{
		case RESUME:
		case RETRY:
			gameScene = SC_INIT;
			break;
		case EXIT:
			modeChange::GetInstance() -> setMode(0);
			option::GetInstance() -> setMode(0);
			gameScene = SC_TITLE;
			break;
		}
	}

	//カーソル移動
	if(input->wasKeyPush(VK_RIGHT) || input->wasKeyPush(VK_LEFT)
		 || input->wasGamePadPush(1, GAMEPAD_DPAD_LEFT) || input->wasGamePadPush(1, GAMEPAD_DPAD_RIGHT))
	{
		switch(cursor)
		{
		case RESUME:
		case RETRY:
			cursor = EXIT;
			cursorImg->setX(680);
			break;
		case EXIT:
			cursor = RETRY;
			cursorImg->setX(100+ADDSIZEX/2);
			break;
		}
	}
}

//=============================================================================
// ゲーム終了時の処理
// 引数：なし
// 戻値：なし
//=============================================================================
void gameStatus::gameTimeCount()
{
	if(gameSt == PAUSE) {
		return;
	}

	if(timeCount == 0)
		return;

	if(timeCount % (MINIT*MINIT) == 0)
	{
		set1--;
		if(set1 < 0 && set1 != NOTTIMECOUNT) {
			set1 = 2;
		}
	} 
	
	if(timeCount % (MINIT*10) == 0) {
		set2--;
		if(set2 < 0 && set2 != NOTTIMECOUNT) {
			set2 = 5;
		}
	} 

	if(timeCount % MINIT == 0) {
		set3--;
		if(set3 < 0 && set3 != NOTTIMECOUNT) {
			set3 = 9;
		}
	} 

	if(timeCount >= (3 * MINIT * MINIT))
	{
		wallUnit->setIsTimeUp(true);
		timeCount = -1;
		set1 = NOTTIMECOUNT;
		set2 = NOTTIMECOUNT;
		set3 = NOTTIMECOUNT;
	}
	
	numImg ->setCurrentFrame(set1);
	numImg2->setCurrentFrame(set2);
	numImg3->setCurrentFrame(set3);
	
	timeCount++;
}

//=============================================================================
// キャラクターの死亡判定
// 引数：なし
// 戻値：なし
//=============================================================================
void gameStatus::loseDecision()
{
	//2P対戦なら
	if(modeChange::GetInstance() -> getMode() == VS2P)
	{
		//ポインタに中身が入っていれば
 		if(player1 != nullptr && player2 != nullptr)
		{
 			if(player1->isLose || player2->isLose) {
				//死亡処理
				gameRs = player1->isLose == true ? WIN2P : WIN1P;	//勝敗を入れる
				gameSt = END;										//ゲームの状態を終了へ
			} 
		}
	}
	
	//CPU対戦なら
	if(modeChange::GetInstance() -> getMode() == VSCPU)
	{	
		if(vsCpuPlayerUnit != nullptr && cpuUnit[PYECPU] != nullptr 
			&& cpuUnit[SPEEDCPU] != nullptr && cpuUnit[NORMALCPU] != nullptr)
		{
			//すべてのCPUキャラクターが死亡したら
			if(cpuUnit[PYECPU]->isLose && cpuUnit[SPEEDCPU]->isLose && cpuUnit[NORMALCPU]->isLose)
			{
				//死亡処理
				gameRs = WINVSCPU;	//勝敗を入れる
				gameSt = END;		//ゲームの状態を終了へ
				return;
			}

			//すべてのvsCPUキャラクターが死亡したら
			if(vsCpuPlayerUnit -> isLose)
			{
				//死亡処理
				gameRs = LOSEVSCPU;	//勝敗を入れる
				gameSt = END;		//ゲームの状態を終了へ
				return;
			}
		}
	}
}