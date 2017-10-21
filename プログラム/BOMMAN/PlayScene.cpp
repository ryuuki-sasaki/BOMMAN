//=============================================================================
//  プレイシーン
//　作成日：2014/10/23
//　更新日：2016/10/30
//	制作者：佐々木隆貴
//=============================================================================
#include "PlayScene.h"
#include "modeChange.h"
#include "option.h"
#include "gameStatus.h"
#include "Camera.h"
#include "Ground.h"
#include "Wall.h"
#include "VsPlayer.h"
#include "vsCpuPlayer.h"
#include "Hurricane.h"
#include "Thunder.h"
#include "PsyCpu.h"
#include "NormalCpu.h"
#include "SpeedCpu.h"
#include "normalBom.h"
#include "plusBom.h"
#include "paralyzeBom.h"

//プレイヤーをプレイシーンのリストにプッシュした順番
#define PUSHPLAYERNUM1 1
#define PUSHPLAYERNUM2 2

//2P対戦時のキャラクター番号
#define PLAYER1 1
#define PLAYER2 2
#define NOTPLAYER 100

//CPUキャラクターの種類
enum CHARA_TYPE
{
	PYECPU,
	NORMALCPU,
	SPEEDCPU
};

//ボムの種類
enum BOM_TYPE
{
	NORMALBOM,
	PARALYZEBOM,
	PLUSBOM
};

//=============================================================================
// コンストラクタ
//=============================================================================
PlayScene::PlayScene(void)
{	
	//プレイシーンに存在させたいユニットを格納	
	if(modeChange::GetInstance() -> getMode() == VS2P)
	{
		setUnit();
	} else {
		vsCpuSetUnit();
	}
}

//=============================================================================
// デストラクタ
//=============================================================================
PlayScene::~PlayScene(void)
{
	initialize();
}

//=============================================================================
// 2P対戦時のユニット格納
// 引数：なし
// 戻値：なし
//=============================================================================
void PlayScene::setUnit()
{
	unit.push_back(new Ground);
	unit.push_back(new VsPlayer(PLAYER1));
	unit.push_back(new VsPlayer(PLAYER2));
	//HurricaneとThunderでPlayer情報を取得したいからPlayerの後に書く
	if(option::GetInstance() -> getMode() == OPTION::HURRICANE
		|| option::GetInstance() -> getMode() == OPTION::ALLSELECT)
	{
		unit.push_back(new Hurricane);
	}
	if(option::GetInstance() -> getMode() == OPTION::THUNDER
		|| option::GetInstance() -> getMode() == OPTION::ALLSELECT)
	{
		unit.push_back(new Thunder);
	}
	unit.push_back(new gameStatus);
	unit.push_back(new normalBom);
	unit.push_back(new plusBom);
	unit.push_back(new paralyzeBom);
	unit.push_back(new Wall);
}

//=============================================================================
// vsCpu対戦時のユニット格納
// 引数：なし
// 戻値：なし
//=============================================================================
void PlayScene::vsCpuSetUnit()
{
	unit.push_back(new Ground);
	if(option::GetInstance() -> getMode() == OPTION::HURRICANE
		|| option::GetInstance() -> getMode() == OPTION::ALLSELECT)
	{
		unit.push_back(new Hurricane);
	}
	if(option::GetInstance() -> getMode() == OPTION::THUNDER
		|| option::GetInstance() -> getMode() == OPTION::ALLSELECT)
	{
		unit.push_back(new Thunder);
	}
	unit.push_back(new gameStatus);
	unit.push_back(new vsCpuPlayer);
	unit.push_back(new PsyCpu);
	unit.push_back(new NormalCpu);
	unit.push_back(new SpeedCpu);
	unit.push_back(new normalBom);
	unit.push_back(new plusBom);
	unit.push_back(new paralyzeBom);
	unit.push_back(new Wall);
	
}

//=============================================================================
// シーン上のユニットのinitialize()呼び出し処理
// 引数：なし
// 戻値：なし
//=============================================================================
void PlayScene::initialize()
{
	for(DWORD i = 0; i < unit.size(); i++)
	{
		unit[i]->initialize();
	}
}

//=============================================================================
// シーン上のユニットのupdate()呼び出し処理
// 引数：なし
// 戻値：なし
//=============================================================================
void PlayScene::update()
{
	if(gameSt == INGAME)
	{
		for(DWORD i = 0; i < unit.size(); i++)
		{	
			unit[i]->update();
		}
	} else/*ポーズ中なら*/ {
		for(DWORD i = 0; i< unit.size(); i++)
		{
			if(typeid(*unit[i]) == typeid(gameStatus))
				unit[i]->update();
		}
	}
}

//=============================================================================
// シーン上のユニットのcollisions()呼び出し処理
// 引数：なし
// 戻値：なし
//=============================================================================
void PlayScene::collisions()
{
	//collisionsには引数があり、引数が必要な相手を渡さなければならない
	//→ユニットの衝突総当り(二重ループ)
	for(DWORD i = 0; i < unit.size(); i++)
	{
		for(DWORD j = 0; j < unit.size(); j++)
		{	
			unit[i]->collisions(unit[j]);
		}
	}
}

//=============================================================================
// シーン上のユニットのai()呼び出し処理
// 引数：なし
// 戻値：なし
//=============================================================================
void PlayScene::ai()
{
	if(gameSt == INGAME)
	{
		//現在のシーンのすべてunitのai
		for(DWORD i = 0; i < unit.size(); i++)
		{
			unit[i]->ai();
		}
	}
}

//=============================================================================
// シーン上のユニットのrender()呼び出し処理
// 引数：frameTime フレームの更新時間
// 戻値：なし
//=============================================================================
void PlayScene::render(float frameTime)
{
	//現在のシーンのすべてunitのrender
	cameraA->update();
	for(DWORD i = 0; i < unit.size(); i++)
	{
		unit[i]->render(frameTime, PLAYER1);
	}

	cameraB->update();
	for(DWORD i = 0; i < unit.size(); i++)
	{
		unit[i]->render(frameTime, PLAYER2);
	}

	camraAll->update();
}


