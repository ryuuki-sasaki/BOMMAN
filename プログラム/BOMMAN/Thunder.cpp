//=============================================================================
//  サンダー
//　作成日：2015/6/28
//　更新日：2016/10/30
//	制作者：佐々木隆貴
//=============================================================================
#include "Thunder.h"
#include "NormalCpu.h"
#include "PsyCpu.h"
#include "SpeedCpu.h"
#include "VsPlayer.h"
#include "vsCpuPlayer.h"
#include "modeChange.h"
#include "Camera.h"
#include "Wall.h"
#include "Model.h"
#include "effect.h"
#include <time.h>

#define MAXRANGE 49			//最大範囲
#define MINRANGE 1			//最少範囲
#define RANDUMRANGE 200		//ランダム範囲
#define RANDUMPOSRANGE 49	//ランダムな位置の範囲
#define ISBOM 100			//ボムが存在する
#define CHARANUM 3			//キャラクターの数
#define BOMRENDERTIME 150	//ボムの描画時間
#define MOVEX 0
#define MOVEZ 1
#define ONEMINTIMER 30/*fps*/ * 60/*秒から分*/ //1分間のタイマー
#define PLAYERNUM 2			//2P対戦時のキャラクター数
#define CAMERA1 1			//カメラ番号
#define RENDERTIME 2		//描画時間
#define UPDATEANIM 1.0f		//アニメーション更新

//CPUキャラクターの種類
enum CHARA_TYPE
{
	PYECPU,
	NORMALCPU,
	SPEEDCPU
};

//=============================================================================
//コンストラクタ
//=============================================================================
Thunder::Thunder(void)
{
	//エフェクト読み込み
	thunder = new effect;
	thunder->Load(graphics, "pictures\\thunder.png", 6, 3);

	initialize();
}

//=============================================================================
//  初期化
//　引数：なし
//	戻り値：なし
//=============================================================================
void Thunder::initialize()
{
	pos = D3DXVECTOR3(0,0,0);
	isOccurrence = false;
	probabilityRange = 1;
	renderCount = 0;
	count = 0;

	vsCpuPlayerUnit = nullptr;
	player1 = nullptr;
	player2 = nullptr;
	cpuUnit[NORMALCPU] = nullptr;
	cpuUnit[PYECPU] = nullptr;
	cpuUnit[SPEEDCPU] = nullptr;
}

//=============================================================================
//デストラクタ
//=============================================================================
Thunder::~Thunder(void)
{
	SAFE_DELETE(thunder);
}

//=============================================================================
//シーンのユニットを取得
//引数：シーンのユニット
//戻り値：なし
//=============================================================================
//ScceneBaseから呼び出されたUnitBaseのcollisionsからターゲット取得
//typeid...typeid(型名または型型の変数名）...何型で作られたか
void Thunder::collisions(UnitBase* target)
{
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
//更新
//引数：なし
//戻り値：なし
//=============================================================================
void Thunder::update()
{
	//電撃を発生させるか
	occurrenceThunder();
	thunder->fixPosUpdate(UPDATEANIM);
	//発生していれば
	if(isOccurrence)
	{
		pos.y+=3.0f;
		thunder->fixPosAdd(pos,3);
		isOccurrence = false;
		renderCount++;
	}
	
	if(renderCount > 0) {
		renderCount++;
		if(renderCount >= 15) {
			//電撃の衝突判定
			collThunder();
			if(renderCount >= 18)
				renderCount = 0;
		}
	}
}

//=============================================================================
//描画
//引数：フレーム情報、カメラ番号
//戻り値：なし
//=============================================================================
void Thunder::render(float frameTime, int cameraNum)
{
	thunder->fixPosDraw(cameraNum);
}

//=============================================================================
//電撃を発生
//引数：なし
//戻り値：なし
//=============================================================================
void Thunder::occurrenceThunder()
{
	//1分ごとに発生確率を上げる
	if(count <= ONEMINTIMER)
	{
		count++;
	} else {
		//発生確率がRANDUMRANGE以下なら
		if(probabilityRange < RANDUMRANGE)
		{
			//発生確率を上げる
			probabilityRange++;
		}
		count = 0;
	}

	//まだ雷が発生していなければランダムな確率で雷発生
	if((rand() % RANDUMRANGE <= probabilityRange) && !isOccurrence)
	{
		//電撃が発生したかどうかのフラグをtrueにする
		isOccurrence = true;
		//ランダムな位置を代入
		pos = D3DXVECTOR3((float)(rand() % RANDUMPOSRANGE+1), -0.5f, (float)(rand() % RANDUMPOSRANGE+1));
	}
}

//=============================================================================
//電撃に衝突したオブジェクトを判定
//引数：なし
//戻り値：なし
//=============================================================================
void Thunder::collThunder()
{	
	float dx[] = {0.0f, 1.0f, 1.0f, 1.0f, 0.0f, -1.0f, -1.0f, 0.0f, 0.0f};
	float dz[] = {1.0f, 1.0f, 0.0f, -1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f};
	//現在のタイルに隣接する各タイルを調べる
	for(int i = 0; i <= 8; i++)
	{	
		float x = pos.x + dx[i];
		float z = pos.z + dz[i];

		D3DXVECTOR3 conpare(x, -0.5f, z);

		//キャラクターに対しての衝突判定			
		if(modeChange::GetInstance() -> getMode() == VS2P/*2P対戦なら*/)
		{
			collPlayerThunder(conpare);
		} 
		
		if(modeChange::GetInstance() -> getMode() == VSCPU/*CPU対戦なら*/)
		{
			collCpuThunder(conpare);
			collvsCpuThunder(conpare);
		}
	}
}

//=============================================================================
//電撃に衝突したプレイヤーを判定
//引数：比較位置
//戻り値：なし
//=============================================================================
void Thunder::collPlayerThunder(D3DXVECTOR3 conparePos)
{
	VsPlayer* playerList[] = {player1,
							player2};

	list<int>::iterator itr;
	for(int i = 0; i < PLAYERNUM; i++)
	{
		//範囲内にプレイヤー1がいれば
		if(conparePos == playerList[i]->pos)
		{
			//死亡フラグをtrueに
			playerList[i]->isLose = true;
		}
	}
}

//=============================================================================
//電撃に衝突したCPUを判定
//引数：比較位置
//戻り値：なし
//=============================================================================
void Thunder::collCpuThunder(D3DXVECTOR3 conparePos)
{
	//各CPUキャラクターを代入
	NormalCpu* cpuChara[] = { cpuUnit[PYECPU],
						cpuUnit[SPEEDCPU],
						cpuUnit[NORMALCPU] };

	//CPUキャラクター分ループ
	for(int i = 0; i < CHARANUM; i++)
	{
		//位置が等しければ
		if(cpuChara[i]->pos == conparePos)
		{
			//死亡フラグをtrueに
			cpuChara[i]->isLose = true;
		}
	}
}

//=============================================================================
//電撃に衝突したvsCpuを判定
//引数：比較位置
//戻り値：なし
//=============================================================================
void Thunder::collvsCpuThunder(D3DXVECTOR3 conparePos)
{
	//vsCPUプレイヤーと位置が等しければ
	if(vsCpuPlayerUnit -> pos == conparePos)
	{
		//死亡フラグをtrueに
		vsCpuPlayerUnit -> isLose = true;
	}
}