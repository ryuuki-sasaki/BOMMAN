//=============================================================================
//  SpeedCPU
//　更新日：2016/10/30
//	制作者：佐々木隆貴
//=============================================================================
#include "SpeedCpu.h"
#include "NormalCpu.h"
#include "PsyCpu.h"
#include "normalBom.h"
#include "paralyzeBom.h"
#include "plusBom.h"
#include "vsCpuPlayer.h"
#include "Model.h"
#include "readFile.h"

#define VIEWRANGE 10		//キャラクターから見える範囲
#define ITEMVIEWRANGE 5		//アイテムを調査する範囲
#define OBSTACLELCOST 100	//障害物のコスト
#define MAXRANGE 49			//最大範囲
#define MINRANGE 1			//最小範囲	
#define ITEMCOST -3			//AIで使用しているアイテムのコスト

//AIの状態
enum AI_STATE
{
	INIT,	//初期化
	CALC,	//計算中
	MOVE,	//移動処理
	STAY,	//停止
	NOTAI = 100	//AIを使用しない
};

//キャラクター
enum CHARACTER
{
	PLAYER,
	PSY,
	NORMAL,
	SPEED
};

//比較タイルが以前通ったタイルか調査した後の状態
enum PREV_TILE_STATE
{
	NOT_PRAV_TILE,		//通ったことのないタイル
	TO_PRAV_TILE_MOVE,	//そのタイルに進む
	MOVE_DIR			//進んだことのないタイルに進む
};

//ボムの種類
enum BOM_TYPE
{
	NORMALBOM,
	PARALYZEBOM,
	PLUSBOM
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
SpeedCpu::SpeedCpu(void)
{	
	model = new Model;
	//モデル読み込み
	model->Load(graphics->get3Ddevice(),"Models\\chara_b2.fbx");

	//csvファイル読込み
	searchTargetRangeXData = new readFile;
	searchTargetRangeZData = new readFile;
	targetDistCostData = new readFile;

	//csvファイル読込み
	if(!searchTargetRangeXData->read("CSV\\searchTargetRangeX.csv"))
	{
		throw(GameError(gameErrorNS::FATAL_ERROR, "CSVが読み込めませんでした"));
	}

	int index1 = 0;
	//順番に各マスの値を取得
	for(int i = 0; i < 112; i++)
	{
		searchTargetRangeXTable[i] = searchTargetRangeXData -> getToComma(&index1);
	}
	searchTargetRangeXData->deleteDataArray();

	if(!searchTargetRangeZData->read("CSV\\searchTargetRangeZ.csv"))
	{
		throw(GameError(gameErrorNS::FATAL_ERROR, "CSVが読み込めませんでした"));
	}

	int index2 = 0;
	//順番に各マスの値を取得
	for(int i = 0; i < 112; i++)
	{
		searchTargetRangeZTable[i] = searchTargetRangeZData -> getToComma(&index2);
	}
	searchTargetRangeZData->deleteDataArray();

	if(!targetDistCostData->read("CSV\\targetDistCost.csv"))
	{
		throw(GameError(gameErrorNS::FATAL_ERROR, "CSVが読み込めませんでした"));
	}

	int index3 = 0;
	//順番に各マスの値を取得
	for(int i = 0; i < 112; i++)
	{
		targetDistCostTable[i] = targetDistCostData -> getToComma(&index3);
	}
	targetDistCostData->deleteDataArray();

	initialize();
}

//=============================================================================
//デストラクタ
//=============================================================================
SpeedCpu::~SpeedCpu(void)
{
	SAFE_DELETE(model);
	SAFE_DELETE(searchTargetRangeXData);
	SAFE_DELETE(searchTargetRangeZData);
	SAFE_DELETE(targetDistCostData);
}

//=============================================================================
//  初期化
//　引数：なし
//	戻り値：なし
//=============================================================================
void SpeedCpu::initialize()
{
	//各情報初期化
	isItem  = false;
	charaSearch = false;
	isLose = false;
	isOpenListEmpty = false;
	isExitExpMove = false;
	throwFlag = false;	
	isStop = false;
	isTarget = false;
	isOpenListEmpty = false;

	isControl = AI_STATE::INIT;
	prevTileState = PREV_TILE_STATE::NOT_PRAV_TILE;

	moveSpeed = 0.0f;
	stopCount = 0;
	wallTargetMul = 1.0f;	
	charaTargetMul = 0.5f;
	addExpRange = 0;
	moveDir = 0;	
	prevMoveDir = 0;
	itemType = 100;
	processBomType = 100;
	charaNum = 100;
	frameCount = 0;
	bomPutAngle = 0.0f;
	loopNum = 0;
	toTargetCost = 0;
	prevWallCount = 0; 
	prevDirIndex = 100;

	//位置
	pos = D3DXVECTOR3(1,-0.5f,1);
	prevPos = D3DXVECTOR3(0, 0, 0);
	putBomPos = D3DXVECTOR3(0, 0, 0);
	prevTargetPos = D3DXVECTOR3(0,0,0);
	bomThrowPos = D3DXVECTOR3(0,0,0);	
	startPos = D3DXVECTOR3(0, 0, 0);
	goalPos = D3DXVECTOR3(0, 0, 0);
	target = D3DXVECTOR3(0, 0, 0);
	prevTile = D3DXVECTOR3(0, 0, 0);

	//最初の向き
    angle = 0.0f;

	initList();
}

//=============================================================================
//シーンのユニットを取得
//引数：シーンのユニット
//戻り値：なし
//=============================================================================
//ScceneBaseから呼び出されたUnitBaseのcollisionsからターゲット取得
//typeid...typeid(型名または型型の変数名）...何型で作られたか
void SpeedCpu::collisions(UnitBase* target)
{
	if(typeid(*target) == typeid(normalBom))
		bomUnit[NORMALBOM] = ((normalBom*)target);
	
	if(typeid(*target) == typeid(paralyzeBom))
		bomUnit[PARALYZEBOM] = ((paralyzeBom*)target);

	if(typeid(*target) == typeid(plusBom))
		bomUnit[PLUSBOM] = ((plusBom*)target);

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
}

//=============================================================================
//  更新
//　引数：なし
//	戻り値：なし
//=============================================================================
void SpeedCpu::update()
{
	//死亡した場合処理をしない
	if(isLose)
	{
		isControl = AI_STATE::NOTAI;
		return;
	}

	charaNum = CHARACTER::SPEED;

	//AIの計算中、または移動処理中に中断命令が出たら
	if(isStop)
	{
		//位置を整数にする
		adjustmentPos();
		//AIの計算ストップ
		stopAi();
	}

	//戦車の移動
	control();
	//ボム追加
	bomProcess();
	//アイテム取得処理
	getItem();
	//描画中のボムがあるか
	notRenderBom();
}




