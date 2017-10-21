//=============================================================================
//  壁
//　更新日：2016/10/30
//	制作者：佐々木隆貴
//=============================================================================
#include "Wall.h"
#include "Camera.h"
#include "VsPlayer.h"
#include "vsCpuPlayer.h"
#include "NormalCpu.h"
#include "PsyCpu.h"
#include "SpeedCpu.h"
#include "modeChange.h"
#include "readFile.h"
#include <mmsystem.h>

#define NOTWALL 0			//壁なし
#define PUTWALL 1			//壁有り
#define NOTBREAKWALL 2		//破壊されない壁
#define OUTRANGE 3			//フィールド範囲外
#define OBSTACLELCOST 100	//壁等の障害物のコスト
#define NONCOSTE 0			//コストなし
#define ITEMCOST -3			//アイテムのコスト
#define CAMERA1 1			//カメラ番号
#define CAMERA2 2			
#define CHARANUM 3			//キャラクター数
#define MINRANGE 0			//最大範囲
#define MAXRANGE 50			//最少範囲
#define VIEWRANGE 6		    //3Dモデルが描画される範囲
#define REARRENGETIME 3 * 60/*秒から分*/	//壁を再配置する時間
#define FILENUM 4			//CSVファイルの数
#define ITEMRAN 30			//アイテム番号取得のための乱数幅
#define ITEMNUM 4			//アイテムの種類数
#define TIMEUPWALLNUM 4800		//制限時間を過ぎたときに表示する壁

enum ITEM
{	
	DEFAULTITEM,		//通常ボム
	PLUSBOMITEM,		//プラスボム
	PARALYZEBOMITEM,	//痺れボム
	THROWBOMITEM,		//ボムを投げる
	LARGEEXPLOSIONITEM,	//大爆発ボム
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
Wall::Wall(void)
{
	mapTableData = new readFile;
	nonBreakBlockMapData = new readFile;

	//テクスチャ読み込み
    //plusBom
    if (!texPlusBom.initialize(graphics,"pictures\\plusBom.png"))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing plusBom"));
		plusBomPoly.initialize(graphics, &texPlusBom);

	//largeexplosionBom
    if (!texLargeexplosionBom.initialize(graphics,"pictures\\largeexplosion Bom.png"))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing largeexplosionBom"));
		largeexplosionPoly.initialize(graphics, &texLargeexplosionBom);

	//paralyzeBom
	if (!texParalyzeBom.initialize(graphics,"pictures\\paralyze.png"))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing paralyzeBom texture"));
		paralyzeBomPoly.initialize(graphics,&texParalyzeBom);

	//bomThrow
	if (!texThrow.initialize(graphics,"pictures\\bomThrow.png"))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing bomThrow texture"));
		bomThrowPoly.initialize(graphics,&texThrow);

	//wall
	if (!texWall.initialize(graphics,"pictures\\renga.jpg"))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing wall texture"));
		wallPoly.boxInitialize(graphics,&texWall);

	//nonBreakWall
	if (!texNonBreakWall.initialize(graphics,"pictures\\nonBreakWall.png"))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing nonBreakWall texture"));
		nonBreakWallPoly.boxInitialize(graphics,&texNonBreakWall);

	if(!nonBreakBlockMapData->read("CSV\\NonBreakBlockPosition.csv"))
	{
		throw(GameError(gameErrorNS::FATAL_ERROR, "CSVが読み込めませんでした"));
	}

	int index = 0;
	for(int i = 0; i <= 4800; i++)
	{
		nonBreakBlockMapTable[i] = nonBreakBlockMapData->getToComma(&index);
	}
	nonBreakBlockMapData->deleteDataArray();

	initialize();
}

//=============================================================================
//  初期化
//　引数：なし
//	戻り値：なし
//=============================================================================
void Wall::initialize()
{
	putWallCount = 0;
	setIsTimeUp(false);
	isTimeUp = false;

	vsCpuPlayerUnit = nullptr;
	player1 = nullptr;
	player2 = nullptr;
	cpuUnit[NORMALCPU] = nullptr;
	cpuUnit[PYECPU] = nullptr;
	cpuUnit[SPEEDCPU] = nullptr;

	char* file;
	int fileNum = rand() % FILENUM;

	if(fileNum == 0)
		file = "CSV\\arry1.csv";
	else if(fileNum == 1)
		file = "CSV\\arry2.csv";
	else if(fileNum == 2)
		file = "CSV\\arry3.csv";
	else if(fileNum == 3)
		file = "CSV\\arry4.csv";
	else if(fileNum == 4)
		file = "CSV\\arry5.csv";

	if(!mapTableData->read(file))
	{
		throw(GameError(gameErrorNS::FATAL_ERROR, "CSVが読み込めませんでした"));
	}	
	
	int index = 0;
	//順番に各マスの値を取得
	for(int x = 0; x <= 50; x++)
	{
		for(int z = 0; z <= 50; z++)
		{
			blockMapTable[x][z] = mapTableData->getToComma(&index);
		}
	}
	mapTableData->deleteDataArray();

	initList();

	randomWallArrange();
}

//=============================================================================
//  各リストの状態を初期化
//　引数：なし
//	戻り値：なし
//=============================================================================
void Wall::initList()
{
	ZeroMemory(wallData, sizeof(wallData));
	ZeroMemory(wallArr, sizeof(wallArr));
}

//=============================================================================
//デストラクタ
//=============================================================================
Wall::~Wall(void)
{
	SAFE_DELETE(mapTableData);
	SAFE_DELETE(nonBreakBlockMapData);
}

//=============================================================================
//シーンのユニットを取得
//引数：シーンのユニット
//戻り値：なし
//=============================================================================
//ScceneBaseから呼び出されたUnitBaseのcollisionsからターゲット取得
//typeid...typeid(型名または型型の変数名）...何型で作られたか
void Wall::collisions(UnitBase* target)
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
//  更新
//　引数：なし
//	戻り値：なし
//=============================================================================
void Wall::update()
{
	static DWORD count = 0;
	if(isTimeUp && count % 3 == 0)
	{
		timeUp();
	}
}

//=============================================================================
//  描画
//　引数：フレーム、カメラ番号
//	戻り値：なし
//=============================================================================
void Wall::render(float frameTime/*フレーム*/, int cameraNum /*カメラ番号*/)
{	
	//壁を配置する範囲分ループ
	for(int i = 1; i < WALLARRAYRANGE-1; i++)
	{
		for(int j = 1; j < WALLARRAYRANGE-1; j++)
		{
			//i行j列が1なら（壁を配置するなら）
			if(wallArr[i][j] == PUTWALL)
			{
				float sizeX = 1.0f, sizeY = 1.0f;
				setBillboard(i, j, sizeX, sizeY, cameraNum);
				wallPoly.boxDraw(&(matScale * matBillboard * matTrans));
			} else if(wallArr[i][j] == NOTBREAKWALL) {
				float sizeX = 1.0f, sizeY = 1.0f;
				setBillboard(i, j, sizeX, sizeY, cameraNum);
				nonBreakWallPoly.boxDraw(&(matScale * matBillboard * matTrans));
			} else if(wallArr[i][j] == NOTWALL) {
				float sizeX = 1.0f, sizeY = 1.0f;
				setBillboard(i, j, sizeX, sizeY, cameraNum);
				notWallProcess(i, j, cameraNum);
			}
		}
	}
}

//=============================================================================
//  壁がない場所の処理
//　引数：位置、カメラ番号
//	戻り値：なし
//=============================================================================
void Wall::notWallProcess(int x, int z, int cNum)
{
	//壁が破壊された後なら
	if(wallData[x][z].cpuCost == OBSTACLELCOST)
	{
		//CPUコストを初期化
		wallData[x][z].cpuCost = NONCOSTE;
	}

	//破壊された壁にアイテムが存在していれば
	if(wallData[x][z].isItem != DEFAULTITEM)
	{
		//アイテムのCPUコストを代入
		wallData[x][z].cpuCost = ITEMCOST;
	}

	float f_x = (float)x, f_z = (float)z;

	//アイテムの位置格納
	switch (wallData[x][z].isItem/*どの種類のアイテムか*/)
	{
		case ITEM::DEFAULTITEM:
			break;

		case ITEM::PLUSBOMITEM:
			//ポリゴンにテクスチャ貼り付けて表示
			plusBomPoly.draw(&(matScale * matBillboard * matTrans));
			wallData[x][z].itemPos = D3DXVECTOR3(f_x,0,f_z);
			break;

		case ITEM::LARGEEXPLOSIONITEM:
			largeexplosionPoly.draw(&(matScale * matBillboard * matTrans));
			wallData[x][z].itemPos = D3DXVECTOR3(f_x,0,f_z);
			break;

		case ITEM::PARALYZEBOMITEM:
			paralyzeBomPoly.draw(&(matScale * matBillboard * matTrans));
			wallData[x][z].itemPos = D3DXVECTOR3(f_x,0,f_z);
			break;

		case ITEM::THROWBOMITEM:
			bomThrowPoly.draw(&(matScale * matBillboard * matTrans));
			wallData[x][z].itemPos = D3DXVECTOR3(f_x,0,f_z);
			break;

		default:
			break;
	}
}

//=============================================================================
//ビルボードの設定セット
//引数：描画位置のx,z座標、サイズ、カメラ番号
//戻り値：なし
//=============================================================================
void Wall::setBillboard(int x, int z, float scaleX, float scaleY, int cNum)
{
	//ビルボード設定
	D3DXMatrixScaling(&matScale, scaleX, scaleY, 1);	
	//2画面の分割の際両方に表示する
	if(cNum == CAMERA1)
	{
		matBillboard = cameraA->billboard();
	} else {
		matBillboard = cameraB->billboard();
	}
	float f_x = (float)x, f_z = (float)z;
	D3DXMatrixTranslation(&matTrans,f_x, 0, f_z);
}

//=============================================================================
//壁の有無をランダムに決める
//引数：なし
//戻り値：なし
//=============================================================================
void Wall::randomWallArrange()
{
	//壁を配置する範囲分ループ
	for(int i = 0; i < WALLARRAYRANGE; i++)
	{
		for(int j = 0; j < WALLARRAYRANGE; j++)
		{
			float f_x = (float)i, f_z = (float)j;

			if(blockMapTable[i][j] == 0) {
				wallArr[i][j]			= OUTRANGE;
				wallData[i][j].pos		= D3DXVECTOR3(f_x,0,f_z);
				wallData[i][j].isItem	= DEFAULTITEM;
				wallData[i][j].cpuCost  = OBSTACLELCOST;
				wallData[i][j].expRangeNum = 0;
				wallData[i][j].isExpRange = false;
			}
			//壁
			if(blockMapTable[i][j] == 1) {
				//i行j列に壁に関する各情報を格納
				wallArr[i][j]			= PUTWALL;
				wallData[i][j].pos		= D3DXVECTOR3(f_x,0,f_z);
				int item = rand() % ITEMRAN;
				if(item > ITEMNUM) {
					wallData[i][j].isItem = DEFAULTITEM;
				} else {
					wallData[i][j].isItem = item;	
				}
				wallData[i][j].cpuCost  = OBSTACLELCOST;
				wallData[i][j].expRangeNum = 0;
				wallData[i][j].isExpRange = false;
			}
			//破壊されない壁
			if(blockMapTable[i][j] == 2) {
				wallArr[i][j]		   = NOTBREAKWALL;
				wallData[i][j].pos	   = D3DXVECTOR3(f_x,0,f_z);
				wallData[i][j].isItem  = DEFAULTITEM;
				wallData[i][j].cpuCost = OBSTACLELCOST;
				wallData[i][j].expRangeNum = 0;
				wallData[i][j].isExpRange = false;
			}
			//何もなし
			if(blockMapTable[i][j] == 3) {
				wallArr[i][j]		   = NOTWALL;
				wallData[i][j].pos	   = D3DXVECTOR3(f_x,0,f_z);
				wallData[i][j].isItem  = DEFAULTITEM;
				wallData[i][j].cpuCost = NONCOSTE;
				wallData[i][j].expRangeNum = 0;
				wallData[i][j].isExpRange = false;
			}
		}
	}
}

//=============================================================================
//アイテム番号を返す
//引数：プレイヤーのx,z座標
//戻り値：アイテム番号
//=============================================================================
int Wall::retItemNumber(float posX, float posZ)
{
	int x = (int)posX, z = (int)posZ;
	//引数の位置にアイテムがあれば返り値の変数に種類格納
	int itemNum = wallData[x][z].isItem;
	//情報初期化
	wallData[x][z].isItem = DEFAULTITEM;
	wallData[x][z].cpuCost = NONCOSTE;

	return itemNum;
}

//=============================================================================
//時間制限後の処理
//引数：なし
//戻り値：なし
//=============================================================================
void Wall::timeUp()
{
	if(putWallCount <= TIMEUPWALLNUM-2) {
		int x = nonBreakBlockMapTable[putWallCount];
		int y = nonBreakBlockMapTable[putWallCount+1];
		wallArr[x][y]		   = NOTBREAKWALL;
		wallData[x][y].pos	   = D3DXVECTOR3(x,0,y);
		wallData[x][y].isItem  = DEFAULTITEM;
		wallData[x][y].cpuCost = OBSTACLELCOST;
		wallData[x][y].expRangeNum = 0;
		wallData[x][y].isExpRange = false;
		collisionOnTheWall(D3DXVECTOR2(x, y));
		putWallCount += 2;
	}
}

//=============================================================================
//キャラクターがタイムアップ後の壁に衝突したか
//引数：現在の壁の位置、前のの壁の位置
//戻り値：なし
//=============================================================================
void Wall::collisionOnTheWall(D3DXVECTOR2 pos/*, D3DXVECTOR2 prevPos*/)
{
	D3DXVECTOR3 wallPos((float)pos.x, 0, (float)pos.y);

	//CPU対戦
	if(modeChange::GetInstance() -> getMode() == VSCPU){
		//各CPUキャラクターの情報格納
		NormalCpu* cpuChara[] = { cpuUnit[PYECPU],
					cpuUnit[SPEEDCPU],
					cpuUnit[NORMALCPU]  };
		//CPUキャラ分ループ
		for(int i = 0; i < CHARANUM; i++)
		{
			//同じ位置にキャラクターがいれば
			if(wallPos == D3DXVECTOR3(toIntPos(cpuChara[i]->pos.x), 0, toIntPos(cpuChara[i]->pos.z)))
			{
				//キャラクタ-の死亡フラグをtrueに
				cpuChara[i]->isLose = true;
			}
		}

		//同じ位置にプレイヤーがいれば
		if(wallPos == D3DXVECTOR3(toIntPos(vsCpuPlayerUnit ->pos.x), 0, toIntPos(vsCpuPlayerUnit ->pos.z)))
		{
			//プレイヤ-の死亡フラグをtrueに
			vsCpuPlayerUnit ->isLose = true;
		}
	}

	//2P対戦
	if(modeChange::GetInstance() -> getMode() == VS2P)
	{
		//同じ位置にプレイヤー1がいれば
		if(wallPos == D3DXVECTOR3(player1->pos.x, 0, player1->pos.z))
		{
			//プレイヤ-1の死亡フラグをtrueに
			player1->isLose = true;
		}

		//同じ位置にプレイヤー2がいれば
		if(wallPos == D3DXVECTOR3(player2->pos.x, 0, player2->pos.z))
		{
			//プレイヤ-2の死亡フラグをtrueに
			player2->isLose = true;
		}
	}
}