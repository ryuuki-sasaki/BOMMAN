//=============================================================================
//  ハリケーン
//　作成日：2015/6/28
//　更新日：2016/10/30
//	制作者：佐々木隆貴
//=============================================================================
#include "Hurricane.h"
#include "NormalCpu.h"
#include "PsyCpu.h"
#include "SpeedCpu.h"
#include "VsPlayer.h"
#include "vsCpuPlayer.h"
#include "modeChange.h"
#include "Wall.h"
#include "Camera.h"
#include "effect.h"
#include "audio.h"
#include <time.h>

#define MAXRANGE 48			//最大範囲
#define MINRANGE 2			//最小範囲
#define RANDUMRANGE 500		//ランダム範囲
#define RANDUMPOSRANGE 48	//ランダムな位置の範囲
#define RANDUMTIMERANGE 2	//ランダムな時間の範囲
#define CHANGENUM 5			//方向転換をする回数
#define FPS 30				//fps(1秒あたりのフレーム数)
#define ISBOM 100			//ボムが存在するか
#define CHARANUM 3			//キャラクター数
#define NOTWALL 0			//壁が存在しない
#define PROBABILITY 1		//確立の子数
#define	MOVEX 0				//x方向
#define PLAYERNUM 2			//2P対戦時のキャラクター数
#define CAMERA1 1			//カメラ番号
#define PICSIZE 3			//テクスチャサイズ
#define UPDATEANIM 1.0f		//アニメーション更新
#define ITEMBOMLISTNUM 3	//各アイテムのリスト数

//アイテム
enum ITEM
{
	DEFAULTITEM,
	PLUSBOMITEM,			//プラスボム
	PARALYZEITEM,     		//痺れボム
	THROWITEM				//ボムを投げる
};

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
Hurricane::Hurricane(void)
{
	//オーディオ
	audio = new Audio;

	//オーディオ読み込み
	audio->initialize("Sounds\\Wave Bank.xwb", "Sounds\\Sound Bank.xsb");

	//エフェクト読み込み
	hurricane = new effect;
	hurricane->Load(graphics, "pictures\\hurricane.png", 8, 3);

	initialize();
}

//=============================================================================
//初期化
//=============================================================================
void Hurricane::initialize(void)
{
	pos = D3DXVECTOR3(0,0,0);
	move = D3DXVECTOR3(0,0,0);
	dirX = 0;					
	dirZ = 0;					
	changeDirectionNum = 0;		
	timerRange = 0;				
	changeDirection = true;		
	isOccurrence= false;
	prevDirX = dirX;
	prevDirZ = dirZ;

	vsCpuPlayerUnit = nullptr;
	player1 = nullptr;
	player2 = nullptr;
	cpuUnit[NORMALCPU] = nullptr;
	cpuUnit[PYECPU] = nullptr;
	cpuUnit[SPEEDCPU] = nullptr;

	//カウント初期化
	count = 0;
}

//=============================================================================
//デストラクタ
//=============================================================================
Hurricane::~Hurricane(void)
{
	SAFE_DELETE(audio);
	SAFE_DELETE(hurricane);
}

//=============================================================================
//シーンのユニットを取得
//引数：シーンのユニット
//戻り値：なし
//=============================================================================
//ScceneBaseから呼び出されたUnitBaseのcollisionsからターゲット取得
//typeid...typeid(型名または型型の変数名）...何型で作られたか
void Hurricane::collisions(UnitBase* target)
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
//更新
//引数：なし
//戻り値：なし
//=============================================================================
void Hurricane::update()
{
	//ハリケーンを発生
	occurrenceHurricane();

	//台風発生中か
	if(isOccurrence)
	{	
		//方向転換するか
		if(changeDirection)	
			setInfo();

		//希望時間以下なら
		if(count < timerRange)
		{
			count++;
			pos += move;
			D3DXVECTOR3 renderPos = D3DXVECTOR3(pos.x, 1.0f, pos.z);
			//エフェクト更新
			hurricane->movePosUpdate(UPDATEANIM, renderPos);
			//フィールド内か判定		
			inRange();	
			//ハリケーンかキャラクターに衝突しているか
			collHurricane();
		} else {
			initInfo();
		}
		//BGM再生
		audio->playCue("台風小");
	} else {
		//BGM停止
		audio->stopCue("台風小");
	}
}

//=============================================================================
//描画
//引数：フレーム情報、カメラ番号
//戻り値：なし
//=============================================================================
void Hurricane::render(float frameTime/*フレーム*/, int cameraNum /*カメラ番号*/)
{	
	//台風発生中か
	if(isOccurrence)
	{
		//エフェクト描画
		hurricane->movePosDraw(cameraNum);
	}
}

//=============================================================================
//ハリケーンを発生
//引数　　なし
//戻り値　なし
//=============================================================================
void Hurricane::occurrenceHurricane()
{
	//発生するかをランダムで判定、未だ発生していないか
	if(PROBABILITY == rand() % RANDUMRANGE&& !isOccurrence)
	{ 
		//発生した
		isOccurrence = true;
		//ランダムな位置を格納
		pos = D3DXVECTOR3((float)(rand() % RANDUMPOSRANGE+1), -0.5f, (float)(rand() % RANDUMPOSRANGE+1));
		//エフェクト追加
		hurricane->movePosAdd(pos,PICSIZE);
		changeDirection = false;
	}
}

//=============================================================================
//各情報初期化
//引数：なし
//戻り値：なし
//=============================================================================
void Hurricane::initInfo()
{
	//カウント初期化
	count = 0;
	//方向転換回数加算
	changeDirectionNum++;
	//方向転換する
	changeDirection = true;	

	//方向転換の回数がCHANGENUM以上なら
	if(changeDirectionNum >= CHANGENUM)
	{
		//回数初期化
		changeDirectionNum = 0;
		//発生していない
		isOccurrence = false;
		pos = D3DXVECTOR3(0, 0, 0);
	}
}

//=============================================================================
//各情報セット
//引数：なし
//戻り値：なし
//=============================================================================
void Hurricane::setInfo()
{

	//進行方向格納
	int direction[] = {0, 1, -1};
	//進行方向が+方向か-方向か
	dirX = rand() % 3;
	dirZ = rand() % 3;

	//以前の方向と同じ場合違う方向になるように決め直し
	if (dirX == prevDirX && dirZ == prevDirZ || dirX == 0 && dirZ == 0)
	{
		while (dirX != prevDirX || dirZ != prevDirZ)
		{
			dirX = rand() % 3;
			dirZ = rand() % 3;
		}
	}	

	prevDirX = dirX;
	prevDirZ = dirZ;

	//移動方向設定	
	move = D3DXVECTOR3((float)direction[dirX],0,(float)direction[dirZ]);
	//進行方向をかえるまでの時間（最大2秒）
	timerRange = (rand() % RANDUMTIMERANGE+1) * FPS;
	changeDirection = false;
}

//=============================================================================
//ハリケーンに衝突したオブジェクトを判定
//引数：なし
//戻り値：なし
//=============================================================================
void Hurricane::collHurricane()
{
	float dx[] = {0.0f, 1.0f, 1.0f, 1.0f, 0.0f, -1.0f, -1.0f, 0.0f, 0.0f};
	float dz[] = {1.0f, 1.0f, 0.0f, -1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f};

	//現在のタイルに隣接する各タイルを調べる
	for(int i = 0; i <= 8; i++)
	{	
		float x = pos.x + dx[i];
		float z = pos.z + dz[i];

		//判定位置
		D3DXVECTOR3 conpare(x, -0.5f, z);

		//キャラクターに対しての衝突判定		
		if(modeChange::GetInstance() -> getMode() == VS2P/*2P対戦なら*/)
		{
			collPlayerHurricane(conpare);
		}

		if(modeChange::GetInstance() -> getMode() == VSCPU/*CPU対戦なら*/)
		{
			collCpuHurricane(conpare);
			collvsCpuHurricane(conpare);
		}
	}
}

//=============================================================================
//ハリケーンに衝突したプレイヤー判定
//引数：比較位置
//戻り値：なし
//=============================================================================
void Hurricane::collPlayerHurricane(D3DXVECTOR3 conparePos)
{
	VsPlayer* playerList[] = {player1,
							player2};

	list<int>::iterator itr;
	for(int i = 0; i < PLAYERNUM; i++)
	{		
		//範囲内にプレイヤーがいれば
		if(conparePos == playerList[i]->pos)
		{
			list<int>* itemBomList[] = {&playerList[i]->plusBomNumList,
						&playerList[i]->paralyzeBomNumList,
						&playerList[i]->bomThrowNumList};

			for(int j = 0; j < ITEMBOMLISTNUM; j++)
			{
				//衝突したキャラクターのボムリストが空でない
				if(!itemBomList[j]->empty())
				{
					itr = itemBomList[j]->begin();
					//ボムの所持数分ループ
					while(itr != itemBomList[j]->end())
					{
						//ランダムに位置を決定
						int x = rand() % RANDUMPOSRANGE+1;
						int z = rand() % RANDUMPOSRANGE+1;

						//その位置がブロックでなければ
						if(wallUnit  -> retWallArr(x, z) == NOTWALL)
						{
							//その位置に所持していたアイテムを配置
							wallUnit  -> wallData[x][z].isItem = j+1;
							//所持していたアイテムを削除
							itr = itemBomList[j]->erase(itr);
						}
					}
				}
			}
		}
	}	
}

//=============================================================================
//ハリケーンに衝突したCPU判定
//引数：比較位置
//戻り値：なし
//=============================================================================
void Hurricane::collCpuHurricane(D3DXVECTOR3 conparePos)
{
	//各CPUを継承したユニットを格納
	NormalCpu* cpuChara[] = { cpuUnit[PYECPU],
						cpuUnit[SPEEDCPU],
						cpuUnit[NORMALCPU] };

	list<int>::iterator itr;
	//キャラクター分ループ
	for(int i = 0; i < CHARANUM; i++)
	{
		//各キャラクターと衝突していれば
		if(cpuChara[i] -> pos == conparePos)
		{
			list<int>* itemBomList[] = {&cpuChara[i]->plusBomNumList,
				&cpuChara[i]->paralyzeBomNumList,
				&cpuChara[i]->bomThrowNumList};

			for(int j = 0; j < ITEMBOMLISTNUM; j++)
			{
				//衝突したキャラクターのボムリストが空でない
				if(!itemBomList[j]->empty())
				{
					itr = itemBomList[j]->begin();
					//ボムの所持数分ループ
					while(itr != itemBomList[j]->end())
					{
						//ランダムに位置を決定
						int x = rand() % RANDUMPOSRANGE+1;
						int z = rand() % RANDUMPOSRANGE+1;

						//その位置がブロックでなければ
						if(wallUnit  -> retWallArr(x, z) == NOTWALL)
						{
							//その位置に所持していたボムを配置
							wallUnit  -> wallData[x][z].isItem = j+1;
							//所持していたボム削除
							itr = itemBomList[j]->erase(itr);
						}
					}
				}
			}
		}
	}
}

//=============================================================================
//ハリケーンに衝突したvsCPU判定
//引数：比較位置
//戻り値：なし
//=============================================================================
void Hurricane::collvsCpuHurricane(D3DXVECTOR3 conparePos)
{
	list<int>::iterator itr;

	//プレイヤーと衝突していれば
	if(vsCpuPlayerUnit  -> pos == conparePos)
	{
		list<int>* itemBomList[] = {&vsCpuPlayerUnit->plusBomNumList,
			&vsCpuPlayerUnit->paralyzeBomNumList,
			&vsCpuPlayerUnit->bomThrowNumList};

		for(int j = 0; j < ITEMBOMLISTNUM; j++)
		{
			//衝突したキャラクターのボムリストが空でない
			if(!itemBomList[j]->empty())
			{
				itr = itemBomList[j]->begin();
				//ボムの所持数分ループ
				while(itr != itemBomList[j]->end())
				{
					//ランダムに位置を決定
					int x = rand() % RANDUMPOSRANGE+1;
					int z = rand() % RANDUMPOSRANGE+1;

					//その位置がブロックでなければ
					if(wallUnit -> retWallArr(x, z) == NOTWALL)
					{
						//その位置に所持していたボムを配置
						wallUnit -> wallData[x][z].isItem = j+1;
						//所持していたボム削除
						itr = itemBomList[j]->erase(itr);
					}
				}
			}
		}
	}
}

//=============================================================================
//位置がフィールド内かを判定
//引数　　なし
//戻り値　なし
//=============================================================================
void Hurricane::inRange()
{
	if(pos.z > MAXRANGE) {
		setInfo();
		pos.z = MAXRANGE;
	} 
	
	if(pos.x > MAXRANGE) {
		setInfo();
		pos.x = MAXRANGE;
	} 
	
	if(pos.z < MINRANGE) {
		setInfo();
		pos.z = MINRANGE;
	} 
	
	if(pos.x < MINRANGE) {
		setInfo();
		pos.x = MINRANGE;
	} 
}