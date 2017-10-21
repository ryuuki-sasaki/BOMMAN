//=============================================================================
//  PsyCPU
//　更新日：2016/10/30
//	制作者：佐々木隆貴
//=============================================================================
#include "PsyCpu.h"
#include "SpeedCpu.h"
#include "NormalCpu.h"
#include "normalBom.h"
#include "paralyzeBom.h"
#include "plusBom.h"
#include "vsCpuPlayer.h"
#include "effect.h"
#include "Model.h"
#include "readFile.h"

#define SEARCHRANGE 5		//キャラクターを調査する範囲
#define GIRTHSEARCH	8		//周囲8タイルを
#define OBSTACLELCOST 100	//障害物のコスト
#define ITEMCOST -3			//AIで使用しているアイテムのコスト
#define MAXRANGE 49			//最大範囲
#define MINRANGE 1			//最小範囲
#define VIEWRANGE 10		//キャラクターから見える範囲
#define ITEMVIEWRANGE 5		//アイテムを調査する範囲
#define NOTWALL 0			//壁が存在しない
#define UPDATEANIM 1.0f		//アニメーョン更新

const int checkX[] = {-1, 0, 1, -1, 1, -1, 0, 1};
const int checkZ[] = {1, 1, 1, 0, 0, -1, -1, -1};

//アイテム
enum ITEM
{	
	DEFAULTITEM,		//通常ボム
	PLUSBOMITEM,		//プラスボム
	PARALYZEBOMITEM,	//痺れボム
	THROWBOMITEM,		//ボムを投げる
	LARGEEXPLOSIONITEM,	//大爆発ボム
	NONE = 100		//アイテム無し
};

//ボムの描画に関する状態
enum BOM_RENDER_STATE
{
	RENDER,			//描画処理
	ACTIONEXPSlON,	//爆発処理実行中
	DONAOTHING		//なんでもない
};

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
PsyCpu::PsyCpu(void)
{	
	model = new Model;
	//モデル読み込み
	model->Load(graphics->get3Ddevice(),"Models\\chara_b2.fbx");
	
	//エフェクト読み込み
	psyPut = new effect;
	psyPut->Load(graphics, "pictures\\psyPutEffect.png", 4, 4);
		
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
//シーンのユニットを取得
//引数：シーンのユニット
//戻り値：なし
//=============================================================================
//ScceneBaseから呼び出されたUnitBaseのcollisionsからターゲット取得
//typeid...typeid(型名または型型の変数名）...何型で作られたか
void PsyCpu::collisions(UnitBase* target)
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
//デストラクタ
//=============================================================================
PsyCpu::~PsyCpu(void)
{
	SAFE_DELETE(model);
	SAFE_DELETE(psyPut);
	SAFE_DELETE(searchTargetRangeXData);
	SAFE_DELETE(searchTargetRangeZData);
	SAFE_DELETE(targetDistCostData);
}

//=============================================================================
//  初期化
//　引数：なし
//	戻り値：なし
//=============================================================================
void PsyCpu::initialize()
{
	//各情報初期化
	isItem  = false;
	charaSearch = false;
	isLose = false;
	isOpenListEmpty = false;
	isPsyPut = false;
	isExitExpMove = false;
	throwFlag = false;	
	isStop = false;
	isTarget = false;
	isOpenListEmpty = false;
	isRenderEffect = false;

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
	renderCount = 0;

	//位置
	pos = D3DXVECTOR3(1,-0.5f,49);
	prevPos = D3DXVECTOR3(0, 0, 0);
	putBomPos = D3DXVECTOR3(0, 0, 0);
	prevTargetPos = D3DXVECTOR3(0,0,0);
	bomThrowPos = D3DXVECTOR3(0,0,0);	
	startPos = D3DXVECTOR3(0, 0, 0);
	goalPos = D3DXVECTOR3(0, 0, 0);
	target = D3DXVECTOR3(0, 0, 0);
	prevTile = D3DXVECTOR3(0, 0, 0);

	//最初の向き
    angle = 180.0f;

	initList();
}

//=============================================================================
//  更新
//　引数：なし
//	戻り値：なし
//=============================================================================
void PsyCpu::update()
{
	//死亡した場合処理をしない
	if(isLose)
	{
		isControl = AI_STATE::NOTAI;
		return;
	}

	charaNum = CHARACTER::PSY;

	//AIの計算中、または移動処理中に中断命令が出たら
	if(isStop)
	{
		//位置を整数にする
		adjustmentPos();
		//AIの計算ストップ
		stopAi();
	}	

	if(isRenderEffect) {
		effectRenderTimeCount();
	} else {
		isPsyPut = false;
	}
	
	//戦車の移動
	control();
	//ボム追加
	bomProcess();
	//アイテム取得処理
	getItem();
	//描画中のボムがあるか
	notRenderBom();
	//エフェクト更新
	psyPut->fixPosUpdate(UPDATEANIM);
	
	if(isControl == AI_STATE::MOVE) {
		//キャラクターが範囲内にいるか調査
		characterInRange();
	}
}

//=============================================================================
//  各ボムに対する処理
//　引数：なし
//	戻り値：なし
//=============================================================================
void PsyCpu::bomProcess()
{
	D3DXVECTOR3 bomPutDir = D3DXVECTOR3(0, 0, 1.0f);
	//回転行列
	D3DXMATRIX mRotat;
	D3DXMatrixRotationY(&mRotat, D3DXToRadian(bomPutAngle));
	D3DXVec3TransformCoord(&bomPutDir,&bomPutDir,&mRotat);
	//ボムを置く方向ベクトルの長さを整数に変換
	bomPutDir = D3DXVECTOR3(toIntMove(bomPutDir.x),0,toIntMove(bomPutDir.z));

	//各ボムの処理
	switch(itemType/*どの種類のボムを処理するか*/)
	{
	case ITEM::DEFAULTITEM/*通常ボム*/:
		{
			if(isItem/*アイテムボムを使用するか*/)
			{
				//現在通常ボムが何の処理もしていない(=通常ボムを使っていない)
				if(bomUnit[NORMALBOM] -> putCharaNum[charaNum].bomRenderState == DONAOTHING)
				{	
					if(!isPsyPut) {
						//ボムを置いた場所を保持
						putBomPos = D3DXVECTOR3(toIntPos(pos.x)+bomPutDir.x, pos.y, toIntPos(pos.z)+bomPutDir.z);
					}
					bomUnit[NORMALBOM] -> addBom(putBomPos/*プレイヤーの位置ベクトル*/, bomPutDir/*移動ベクトル*/,addExpRange/*爆発範囲加算分*/, charaNum/*キャラクター名*/, bomThrowPos/*投げた先の位置ベクトル*/, throwFlag/*ボムを置くか投げるか*/);
					//使用したボムの種類を格納
					processBomType = DEFAULTITEM;
				}
				throwFlag = false;
			}									
			isItem = false;
		}
		break;

	case ITEM::PLUSBOMITEM/*プラスボム*/:
		{	
			if(isItem/*アイテムボムを使用するか*/)
			{
				//plusBomがあれば	
				if(!(plusBomNumList.empty()))
				{	
					plusBomitr = plusBomNumList.begin();
					if(!isPsyPut) {
						//ボムを置いた場所を保持
						putBomPos = D3DXVECTOR3(toIntPos(pos.x)+bomPutDir.x, pos.y, toIntPos(pos.z)+bomPutDir.z);
					}
					bomUnit[PLUSBOM] -> addBom(putBomPos/*プレイヤーの位置ベクトル*/, bomPutDir/*移動ベクトル*/,addExpRange/*爆発範囲加算分*/, charaNum/*キャラクター名*/, bomThrowPos/*投げた先の位置ベクトル*/, throwFlag/*ボムを置くか投げるか*/);
					plusBomitr = plusBomNumList.erase(plusBomitr);
				}
				throwFlag = false;
			}	
			isItem = false;
		}
		break;

	case ITEM::PARALYZEBOMITEM/*痺れボム*/:
		{
			if(isItem/*アイテムボムを使用するか*/)
			{
				//paralyzeBomがあれば	
				if(!(paralyzeBomNumList.empty()))
				{	
					paralyzeitr = paralyzeBomNumList.begin();
					if(!isPsyPut) {
						//ボムを置いた場所を保持
						putBomPos = D3DXVECTOR3(toIntPos(pos.x)+bomPutDir.x, pos.y, toIntPos(pos.z)+bomPutDir.z);
					}
					bomUnit[PARALYZEBOM] -> addBom(putBomPos/*プレイヤーの位置ベクトル*/, bomPutDir/*移動ベクトル*/, addExpRange/*爆発範囲加算分*/, charaNum/*キャラクター名*/, bomThrowPos/*投げた先の位置ベクトル*/, throwFlag/*ボムを置くか投げるか*/);
					processBomType = PARALYZEBOMITEM;
					paralyzeitr = paralyzeBomNumList.erase(paralyzeitr);
				}	
				throwFlag = false;
			} 
			isItem = false;
		}
		break;
	}

	//Psyを使ったかどうかのフラグをfalse
	isPsyPut = false;
}

//=============================================================================
//  エフェクト描画時間カウント
//　引数：なし
//	戻り値：なし
//=============================================================================
void PsyCpu::effectRenderTimeCount()
{
	if(renderCount <= 16) {
		renderCount++;
	} else {
		isPsyPut = true;
		//PSYでボムを置いたかのフラグをtrue
		isRenderEffect = false;
		renderCount = 0;
		//ボムを配置
		putBom();
	}
}

//=============================================================================
//  エフェクト描画
//　引数：なし
//	戻り値：なし
//=============================================================================
void PsyCpu::effectRender(float frameTime, int cameraNum/*カメラ番号*/)
{
	//エフェクト描画
	psyPut->fixPosDraw(cameraNum);
}

//=============================================================================
//  キャラクターが範囲内にいるか(いたらそのキャラクターの近辺にボムを置く
//　引数：なし
//	戻り値：なし
//=============================================================================
void PsyCpu::characterInRange()
{
	if(processBomType == NONE) {
		//見える範囲内ループ
		for(int z = -SEARCHRANGE; z <= SEARCHRANGE; z++)
		{
			//フィールド外に出たら戻る
			if(pos.z+z < MINRANGE || pos.z+z > MAXRANGE)
				continue;

			for(int x = -SEARCHRANGE; x <= SEARCHRANGE; x++)
			{
				if(pos.x+x < MINRANGE || pos.x+x > MAXRANGE)  
					continue;


				//プレイヤーを見つけた
				if(D3DXVECTOR3(pos.x+x, 0, pos.z+z) == D3DXVECTOR3(vsCpuPlayerUnit->pos.x, 0, vsCpuPlayerUnit->pos.z))
				{				
					serchCharacterAround(D3DXVECTOR3(vsCpuPlayerUnit->pos.x, 0, vsCpuPlayerUnit->pos.z));
				}

				//スピードCPUを見つけた
				if(D3DXVECTOR3(pos.x+x, 0, pos.z+z) == D3DXVECTOR3(cpuUnit[SPEEDCPU] -> pos.x, 0, cpuUnit[SPEEDCPU] -> pos.z))
				{					
					serchCharacterAround(D3DXVECTOR3(cpuUnit[SPEEDCPU] -> pos.x, 0, cpuUnit[SPEEDCPU] -> pos.z));
				}

				//タクティクスCPUを見つけた
				if(D3DXVECTOR3(pos.x+x, 0, pos.z+z) == D3DXVECTOR3(cpuUnit[NORMALCPU] -> pos.x, 0, cpuUnit[NORMALCPU] -> pos.z))
				{					
					serchCharacterAround(D3DXVECTOR3(cpuUnit[NORMALCPU] -> pos.x, 0, cpuUnit[NORMALCPU] -> pos.z));
				}

			}
		}	
	}
}

//=============================================================================
//  キャラクターの周囲8マスを探索
//　引数：なし
//	戻り値：なし
//=============================================================================
void PsyCpu::serchCharacterAround(D3DXVECTOR3 serchPos)
{
	if(!isRenderEffect) {
		//見つけたキャラクターの周囲8タイル調査
		for(int serch = 0; serch < GIRTHSEARCH; serch++)
		{
			//ボムを置いた位置を保持
			putBomPos = D3DXVECTOR3(serchPos.x + checkX[serch], pos.y, serchPos.z + checkZ[serch]);	
			//調査タイルが壁でない
			if(isObstacel(putBomPos)
				&& !(putBomPos.x < MINRANGE) && !(putBomPos.x > MAXRANGE)
					&& !(putBomPos.z < MINRANGE) && !(putBomPos.z > MAXRANGE))
			{	
				//現在処理しているボムがなければ
				if(processBomType == NONE) {
					isRenderEffect = true;
					addEffect();	
				}
				break;
			}
		}
	}
}
  
//=============================================================================
//エフェクト追加
//引数：なし
//戻り値：なし
//=============================================================================
void PsyCpu::addEffect()
{
	D3DXVECTOR3 effectPos(putBomPos.x, 0, putBomPos.z);
	psyPut->fixPosAdd(effectPos, 2);
}

//=============================================================================
//ボムを配置
//引数：なし
//戻り値：なし
//=============================================================================
void PsyCpu::putBom()
{
	D3DXVECTOR3 bomPutDir = pos + calcMove(angle);
	
	if(!isRenderEffect) {
 		//現在処理しているボムがなければ
		if(processBomType == NONE) {
			if(!isPsyPut) {
				if(!bomThrowNumList.empty()/*スローボムがあれば*/) {
					//前後左右の延長線上にターゲットがいるか
					if(isThrowTarget()) {
						throwFlag = true;
						bomThrowNumList.pop_back();
					}
				}
			}
				
		if((isObstacel(bomPutDir) && isPutBom(bomPutDir)
			&& !(bomPutDir.x < MINRANGE) && !(bomPutDir.x > MAXRANGE)
				&& !(bomPutDir.z < MINRANGE) && !(bomPutDir.z > MAXRANGE)) || throwFlag || isPsyPut){
				if(!paralyzeBomNumList.empty()/*痺れボムがあれば*/ && charaSearch) { 
					if(!throwFlag)
						bomPutAngle = angle;
					//どのボムを選んだかを代入
					itemType = ITEM::PARALYZEBOMITEM;
					//アイテムボムを使用
					isItem = true;
					return;
				} else if(bomUnit[NORMALBOM] -> putCharaNum[PSY].bomRenderState == DONAOTHING/*通常ボムが処理中でなければ*/) {
					if(!throwFlag)
						bomPutAngle = angle;
					//どのボムを選んだかを代入
					itemType = ITEM::DEFAULTITEM;
					//アイテムボムを使用
					isItem = true;
					return;
				}
			}
		}
		
		//プラスボムが存在すれば
		if((!plusBomNumList.empty() && isObstacel(bomPutDir) && isPutBom(bomPutDir)
				&& !(bomPutDir.x < MINRANGE) && !(bomPutDir.x > MAXRANGE)
					&& !(bomPutDir.z < MINRANGE) && !(bomPutDir.z > MAXRANGE)) || throwFlag) {
			if(!throwFlag)
				bomPutAngle = angle;
			//どのボムを選んだかを代入
			itemType = ITEM::PLUSBOMITEM;
			//アイテムボムを使用
			isItem = true;
			return;
		}
	}
}

