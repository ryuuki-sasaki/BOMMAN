//=============================================================================
//  シーンベース
//　作成日：2014/10/23
//　更新日：2015/8/30
//	制作者：佐々木隆貴
//=============================================================================
#include "SceneBase.h"
#include "constants.h"
#include "Camera.h"
#include "modeChange.h"

//=============================================================================
// コンストラクタ
//=============================================================================
SceneBase::SceneBase()
{
	//画面全体を移すカメラセット
	camraAll = new Camera;
	camraAll->changeViewport(0,0,GAME_WIDTH, GAME_HEIGHT);
}

//=============================================================================
// デストラクタ（仮想関数）
//=============================================================================
SceneBase::~SceneBase(void)
{
	//ユニットの削除
	for(DWORD i = 0; i < unit.size(); i++)
	{
		SAFE_DELETE(unit[i]);
	}
	SAFE_DELETE(camraAll);
}

//=============================================================================
// シーン上のユニットのInitialize()呼び出し処理
// 引数：なし
// 戻値：なし
//=============================================================================
void SceneBase::initialize()
{
	for(DWORD i = 0; i< unit.size(); i++)
	{
		unit[i]->initialize();
	}
}

//=============================================================================
// シーン上のユニットのupdate()呼び出し処理
// 引数：なし
// 戻値：なし
//=============================================================================
void SceneBase::update()
{
	//現在のシーンのすべてunitのupdate
	for(DWORD i = 0; i < unit.size(); i++)
	{
		unit[i]->update();
	}
}

//=============================================================================
// シーン上のユニットのai()呼び出し処理
// 引数：なし
// 戻値：なし
//=============================================================================
void SceneBase::ai()
{
	//現在のシーンのすべてunitのai
	for(DWORD i = 0; i < unit.size(); i++)
	{
		unit[i]->ai();
	}
}

//=============================================================================
// シーン上のユニットのcollisions()呼び出し処理
// 引数：なし
// 戻値：なし
//=============================================================================
void SceneBase::collisions()
{
	//collisionsには引数があり、引数が必要な相手を渡さなければならない
	//→ユニットの衝突総当り(二重ループ)
	for(DWORD i = 0; i < unit.size(); i++)
	{
		for(DWORD j = 0; j < unit.size(); j++)
		{
			//iとjが同じ（同じものの衝突）
			if(i == j)	
			{
				continue;
			}
			unit[i]->collisions(unit[j]);
		}
	}
}

//=============================================================================
// シーン上のユニットのrender()呼び出し処理
// 引数：frameTime フレームの更新時間
// 戻値：なし
//=============================================================================
void SceneBase::render(float frameTime) 
{
	for(DWORD i = 0; i < unit.size(); i++)
	{
		unit[i]->render(frameTime, NULL);
	}
}

