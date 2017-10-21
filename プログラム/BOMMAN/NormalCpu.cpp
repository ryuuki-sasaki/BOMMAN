//=============================================================================
//  NormalCpu
//　更新日：2016/10/30
//	制作者：佐々木隆貴
//=============================================================================
#include "NormalCpu.h"
#include "PsyCpu.h"
#include "SpeedCpu.h"
#include "vsCpuPlayer.h"
#include "input.h"
#include "Camera.h"
#include "normalBom.h"
#include "paralyzeBom.h"
#include "plusBom.h"
#include "Model.h"
#include "Wall.h"
#include "readFile.h"

#define COSTPLUS 1			//コスト
#define OBSTACLELCOST 100	//障害物のコスト
#define ITEMCOST -3			//アイテムのコスト
#define VIEWRANGE 5			//キャラクターが見える範囲
#define MAXRANGE 49			//最大範囲
#define MINRANGE 1			//最低範囲
#define ITEMVIEWRANGE 25	//アイテムに関して見える範囲
#define DIVIDEFRAME 10		//フレームごとの処理を分割する頻度
#define MAXLISTSIZE 500		//リストサイズの上限
#define TWOTILE 2			//タイル2つ分
#define STOPTIME 150		//処理を停止する時間
#define CAMERA1	1			//カメラ番号
#define NOTCALCRANGE 3		//計算しない範囲
#define NOTWALL 0			//壁が存在しない
#define ISWALL 1			//壁が存在
#define CHARANUM 3			//キャラクター数
#define CHECKRANGE 10		//キャラクターの周囲チェックの範囲
#define PREVTARGETRANGE 5	//前のターゲット位置の周囲
#define INITNUM 100			//初期化時に代入する値

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

//進行方向
enum MOVE_DIR
{
	ADVANCE,
	RIGHT,
	LEFT,
	BACKWARD
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
NormalCpu::NormalCpu(void)
{	
	model = new Model;
	//モデル読み込み
	model->Load(graphics->get3Ddevice(),"Models\\chara_b2.fbx");

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
NormalCpu::~NormalCpu(void)
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
void NormalCpu::initialize()
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
	wallTargetMul = 0.5;	
	charaTargetMul = 1.2f;
	addExpRange = 0;
	moveDir = 0;	
	prevMoveDir = 0;
	itemType = NONE;
	processBomType = INITNUM;
	charaNum = INITNUM;
	frameCount = 0;
	bomPutAngle = 0.0f;
	loopNum = 0;
	toTargetCost = 0;
	prevWallCount = 0; 
	prevDirIndex = INITNUM;

	//位置
	pos = D3DXVECTOR3(49,-0.5f,1);
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

	wallUnit = nullptr;
	vsCpuPlayerUnit = nullptr;
	cpuUnit[NORMALCPU] = nullptr;
	cpuUnit[PYECPU] = nullptr;
	cpuUnit[SPEEDCPU] = nullptr;
}

//=============================================================================
//シーンのユニットを取得
//引数：シーンのユニット
//戻り値：なし
//=============================================================================
//ScceneBaseから呼び出されたUnitBaseのcollisionsからターゲット取得
//typeid...typeid(型名または型型の変数名）...何型で作られたか
void NormalCpu::collisions(UnitBase* target)
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
//  リストを初期化
//　引数：なし
//	戻り値：なし
//=============================================================================
void NormalCpu::initList()
{
	if(!plusBomNumList.empty()) {
		plusBomNumList.clear();
	}

	if(!paralyzeBomNumList.empty()) {
		paralyzeBomNumList.clear();
	}

	if(!bomThrowNumList.empty()) {
		bomThrowNumList.clear();
	}	

	if(!openList.empty()) {
		openList.clear();
	}

	if(!closeList.empty()) {
		closeList.clear();
	}

	if(!parentList.empty()) {
		parentList.clear();
	}	

	if(!prevPosList.empty()) {
		prevPosList.clear();
	}	
}

//=============================================================================
//  描画
//　引数：フレーム
//	戻り値：なし
//=============================================================================
void NormalCpu::render(float frameTime, int cameraNum)
{
	//死亡した場合処理をしない
	if(isLose)
		return;
	
	//行列
	D3DXMATRIX mat,mTrans,mRotat;

	//何もしない行列（単位行列）
	D3DXMatrixIdentity(&mat);

	//移動行列
	D3DXMatrixTranslation(&mTrans,pos.x, pos.y, pos.z);
	//回転行列
	D3DXMatrixRotationY(&mRotat, D3DXToRadian(angle));
	//行列の掛け算
	mat = mRotat * mTrans;

	if(cameraNum == 1) {
		//ワールド、ビュー、射影行列
		//Test.fxのグローバル変数に値を渡す
		//行列をシェーダに渡す...SetMatrix
		pEffect -> SetMatrix("g_matWVP", 
			&( mat* cameraA -> GetView() * cameraA -> GetProj() ) );

		//カメラの位置
		pEffect->SetVector("g_vecEye", (D3DXVECTOR4*)&cameraA->GetPos());
	} 

	//ワールドの逆行列
	D3DXMATRIX matInvW;
	D3DXMatrixInverse(&matInvW, 0, &mat); //matの逆行列がmatInvWに入る
	D3DXMatrixTranspose(&matInvW, &matInvW); //行と列をひっくり返す
	pEffect->SetMatrix("g_matInvW",&matInvW);

	//ライトの方向（物体から見て)
	D3DXVECTOR4 light(-1, 1, 0, 1);	//右上手前から
	D3DXVec4Normalize(&light, &light); //正規化

	pEffect->SetVector("g_vecLightDir",&light);

	//描画開始
	pEffect -> Begin(NULL, 0);
	//パス開始
	pEffect -> BeginPass(0);
	//描画
	model->Draw(&mat);
	//パス終了
	pEffect -> EndPass();
	//描画終了
	pEffect -> End();

	effectRender(frameTime, cameraNum);
}

//=============================================================================
//  更新
//　引数：なし
//	戻り値：なし
//=============================================================================
void NormalCpu::update()
{
	//死亡した場合処理をしない
	if(isLose)
	{
		isControl = AI_STATE::NOTAI;
		return;
	}
	
	charaNum = CHARACTER::NORMAL;

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

//=============================================================================
//  描画中のボムがあるか
//　引数：なし
//	戻り値：なし
//=============================================================================
void NormalCpu::notRenderBom()
{
	//各ボムで、描画中のものがあれば各クラスのrender呼び出し
	if(bomUnit[NORMALBOM] -> putCharaNum[charaNum].bomRenderState != RENDER/*通常ボム*/
		&& bomUnit[PARALYZEBOM] -> putCharaNum[charaNum].bomRenderState != RENDER/*痺れボム*/) 
	{
		//何も描画処理をしていない
		processBomType = NONE;
	}
}

//=============================================================================
//  各ボムに対する処理
//　引数：なし
//	戻り値：なし
//=============================================================================
void NormalCpu::bomProcess()
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
					//ボムを置いた場所を保持
					putBomPos = D3DXVECTOR3(toIntPos(pos.x)+bomPutDir.x, pos.y, toIntPos(pos.z)+bomPutDir.z);
					bomUnit[NORMALBOM] -> addBom(putBomPos/*プレイヤーの位置ベクトル*/, bomPutDir/*移動ベクトル*/,addExpRange/*爆発範囲加算分*/, charaNum/*キャラクター名*/, bomThrowPos/*投げた先の位置ベクトル*/, throwFlag/*ボムを置くか投げるか*/);
					//使用したボムの種類を格納
					processBomType = ITEM::DEFAULTITEM;
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
					//ボムを置いた場所を保持
					putBomPos = D3DXVECTOR3(toIntPos(pos.x)+bomPutDir.x, pos.y, toIntPos(pos.z)+bomPutDir.z);
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
					//ボムを置いた場所を保持
					putBomPos = D3DXVECTOR3(toIntPos(pos.x)+bomPutDir.x, pos.y, toIntPos(pos.z)+bomPutDir.z);
					bomUnit[PARALYZEBOM] -> addBom(putBomPos/*プレイヤーの位置ベクトル*/, bomPutDir/*移動ベクトル*/, addExpRange/*爆発範囲加算分*/, charaNum/*キャラクター名*/, bomThrowPos/*投げた先の位置ベクトル*/, throwFlag/*ボムを置くか投げるか*/);
					processBomType = ITEM::PARALYZEBOMITEM;
					paralyzeitr = paralyzeBomNumList.erase(paralyzeitr);
				}	
				throwFlag = false;
			} 
			isItem = false;
		}
		break;
	}
}

//=============================================================================
//  AI初期化処理
//　引数：なし
//	戻り値：なし
//=============================================================================
void NormalCpu::initAstar()
{
	//スタート位置格納
	startPos = D3DXVECTOR3(pos.x, 0, pos.z);

	if(situationCheck()) {
		//ゴール位置格納
		goalPos = retTarget(pos);
	} else {
		return;
	}

	//オープンリストに開始タイルを追加	
	{
		pushData.tilePos = D3DXVECTOR3(pos.x,0,pos.z);
		pushData.cost = 0;
		pushData.heuristic = 0;	
		pushData.score = pushData.cost + pushData.heuristic;
		pushData.parentTile = NULL;
	}
	openList.push_back(pushData);

	openListitr = openList.begin();

	//AIの状態を計算処理へ
	isControl = AI_STATE::CALC;
}

//=============================================================================
//  キャラクターの周囲をチェック
//　引数：現在位置
//	戻り値：動くべきか
//=============================================================================
bool NormalCpu::situationCheck() 
{
	for(int i = -CHECKRANGE; i <= CHECKRANGE; i++) {
		for(int j = -CHECKRANGE; j <= CHECKRANGE; j++) {
		float x = pos.x+i, z = pos.z+j;
		D3DXVECTOR3 conpare(toIntPos(x),0,toIntPos(z));

			//現在位置が爆発範囲外でかつ爆弾が探査範囲内に存在すれば
			if(!wallUnit -> retIsExpRange(toIntPos(pos.x), toIntPos(pos.z))
				&& (bomUnit[NORMALBOM] -> retIsBom(conpare.x, conpare.z) == OBSTACLELCOST/*比較するタイルが障害物(ボム)でない？*/ 
					|| bomUnit[PLUSBOM] -> retIsBom(conpare.x, conpare.z) == OBSTACLELCOST/*比較するタイルが障害物(ボム)でない？*/ 
							|| bomUnit[PARALYZEBOM]  -> retIsBom(conpare.x, conpare.z) == OBSTACLELCOST/*比較するタイルが障害物(ボム)でない？*/))
			{
				//停止して爆発するのを待つ
				return false;
			}
		}
	}
	return true;
}

//=============================================================================
//  AI
//　引数：なし
//	戻り値：なし
//=============================================================================
void NormalCpu::ai()
{
	//死亡した場合処理をしない
	if(isLose)
		return;

	//cpuが初期化処理を求めているとき
	if(isControl == AI_STATE::INIT)
	{
		//初期化
		initAstar();
	}	
	
	//cpuが計算処理を求めているとき
	if(isControl == AI_STATE::CALC)
	{	
		//経路が決まる前にオープンリストが空かチェック
		if(openList.empty() || isOpenListEmpty || isExitExpMove) {
			if(!isOpenListEmpty) {
				isOpenListEmpty = true;
				//各リストを初期化
				openList.clear();	
				closeList.clear();
			}
			exitExpRange();
			return;
		}

		//フレームの経過をカウント
		frameCount = 0;

		//オープンリストが空でない
		while(!(openList.empty()))
		{	
			frameCount++;

			//フレームのカウントが処理を分割したい数になったら
			if(frameCount == DIVIDEFRAME)
			{
				//いったん計算処理から抜ける
				break;
			}

			//クローズリストのサイズが希望最大数以上になったら
			if(closeList.size() >= MAXLISTSIZE)
			{
				//各リスト初期化
				openList.clear();	
				closeList.clear();
				//AIの状態を初期化へ
				isControl = AI_STATE::INIT;
				break;
			}

			//リストの中の最も安価なタイルを探すためのイテレータ
			list<aStar>::iterator	minChackitr;
			openListitr = openList.begin();
			minChackitr = openList.begin();
			minChackitr++;

			//オープンリストを全てチェック
			while(minChackitr != openList.end())
			{
				if(minChackitr->score == openListitr->score/*現在判明しているタイルと今比較しているタイルのスコアが等しければ*/) {
					if(minChackitr->heuristic <= openListitr->heuristic/*ヒューリスティックの小さいほう優先*/)
						openListitr = minChackitr; 
				} else if(minChackitr->score < openListitr->score/*現在判明しているタイルより今比較しているタイルのスコアが小さければ*/) {
						openListitr = minChackitr;
				}
				minChackitr++;
			}
		
			//目的地の2マス手前なら
			if((goalPos.x == openListitr -> tilePos.x && goalPos.z + TWOTILE == openListitr -> tilePos.z)
				|| (goalPos.x + TWOTILE == openListitr -> tilePos.x && goalPos.z == openListitr -> tilePos.z)
					|| (goalPos.x == openListitr -> tilePos.x && goalPos.z - TWOTILE == openListitr -> tilePos.z) 
						|| (goalPos.x - TWOTILE == openListitr -> tilePos.x && goalPos.z == openListitr -> tilePos.z)
							|| (goalPos.x + TWOTILE == openListitr -> tilePos.x && goalPos.z + TWOTILE == openListitr -> tilePos.z)
								|| (goalPos.x - TWOTILE == openListitr -> tilePos.x && goalPos.z - TWOTILE == openListitr -> tilePos.z)
									|| (goalPos.x + TWOTILE == openListitr -> tilePos.x && goalPos.z - TWOTILE == openListitr -> tilePos.z) 
										|| (goalPos.x - TWOTILE == openListitr -> tilePos.x && goalPos.z + TWOTILE == openListitr -> tilePos.z))
			{
				//経路の完成
				{
					pushData.tilePos = openListitr -> tilePos;														
					pushData.cost = openListitr -> cost;
					pushData.heuristic = openListitr -> heuristic;	
					pushData.score = openListitr -> score;
					pushData.parentTile = openListitr -> parentTile;
				}
				closeList.push_back(pushData);	
				closeListitr = closeList.end();
				closeListitr--;
				//AIの状態を移動へ
				isControl = AI_STATE::MOVE;
				//親ポインタをたどって移動経路作成
				getPath();
				return;
			} else {
				//現在のタイルをクローズリストに移す
				{
					pushData.tilePos = openListitr -> tilePos;														
					pushData.cost = openListitr -> cost;
					pushData.heuristic = openListitr -> heuristic;	
					pushData.score = openListitr -> score;
					pushData.parentTile = openListitr -> parentTile;
				}
				closeList.push_back(pushData);

				//クローズリストに移したものをオープンリストから削除
				openListitr = openList.erase(openListitr);	
			}

			//比較したいタイルの中心となるタイル
			closeListitr = closeList.end();
			closeListitr--;

			static int tileCount;
			tileCount = 0;
			//現在のタイルに隣接する4方向の各タイルを調べる
			for(int i = 0; i <= 3; i++)
			{	
				int dx[] = {0, -1, 0, 1};
				int dz[] = {1, 0, -1, 0};
				//隣接する位置代入
				int tileX = (int)(closeListitr -> tilePos.x + dx[i]);
				int tileZ = (int)(closeListitr -> tilePos.z + dz[i]);
				D3DXVECTOR3 conpareTile((float)tileX, 0, (float)tileZ);	
				
				//比較用イテレータ
				list<aStar>::iterator conpareitr;
				//すでに比較タイルがリストに入っているかを判定する変数
				bool isAgreementopenList = false;		//オープンリスト
				bool isAgreementcloseList = false;		//クローズリスト

				//オープンリストが空でなければ
				if(!(openList.empty()))
				{
					conpareitr = openList.end();
					//リストの先頭でなければ
					while(conpareitr != openList.begin())
					{
						conpareitr--;
						//比較したいタイルと同じ位置のタイルがリストに存在すれば
						if(conpareitr -> tilePos == conpareTile)
						{
							isAgreementopenList = true;
							break;
						}
					}
				}

				//クローズリストが空でなければ
				if(!(closeList.empty()))
				{
					conpareitr = closeList.end();
					while(conpareitr != closeList.begin())
					{
						conpareitr--;
						//比較したいタイルと同じ位置のタイルがリストに存在すれば
						if(conpareitr -> tilePos == conpareTile) 
						{
							isAgreementcloseList = true;
							break;
						}    
					}
				}
				
				if(!isAgreementopenList/*オープンリストに合致するものが含まれていない*/ )
				{
					if(!isAgreementcloseList /*クローズリストに合致するものが含まれていない*/ )
					{
						if(inExpRange(conpareTile))
						{
							if(isObstacel(conpareTile))
							{

								//オープンリストに移してコストを計算する
								pushData.tilePos = conpareTile;			
								pushData.cost = closeListitr-> cost + COSTPLUS + wallUnit  -> retCost(conpareTile.x, conpareTile.z);
								pushData.heuristic = abs((int)(goalPos.x - conpareTile.x)) + abs((int)(goalPos.z - conpareTile.z));
								pushData.score = pushData.cost + pushData.heuristic;
								pushData.parentTile = &*closeListitr;
								openList.push_back(pushData);
							}
						}
					}
				}
			}
		}
	}
}

//=============================================================================
//現在爆発範囲内かを判定してそのタイルをコスト候補に含めるか返す
//引数：比較用の位置
//戻り値：コスト候補に含めるか
//=============================================================================
bool NormalCpu::inExpRange(D3DXVECTOR3 conpare)
{
	//爆発範囲内でかつ現在位置の爆発範囲の番号と比較位置の爆発範囲の番号が等しい値か
	if(wallUnit  -> retIsExpRange(toIntPos(pos.x), toIntPos(pos.z))
		&& wallUnit  -> retExpRangeNum(toIntPos(pos.x), toIntPos(pos.z)) == wallUnit  -> retExpRangeNum(conpare.x, conpare.z))
	{
		return true;
	} else if(!wallUnit  -> retIsExpRange(conpare.x, conpare.z)/*比較位置が爆発範囲内でない？*/) {
		return true;
	}
	return false;
}

//=============================================================================
//比較タイルが障害物か
//引数：比較用の位置
//戻り値：障害物かを返す
//=============================================================================
bool NormalCpu::isObstacel(D3DXVECTOR3 conpare) 
{
	conpare = D3DXVECTOR3(conpare.x, 0.0f, conpare.z);

	if(bomUnit[NORMALBOM] -> retIsBom(conpare.x, conpare.z) != OBSTACLELCOST/*比較するタイルが障害物(ボム)でない？*/) 
		if(bomUnit[PLUSBOM] -> retIsBom(conpare.x, conpare.z) != OBSTACLELCOST/*比較するタイルが障害物(ボム)でない？*/) 
			if(bomUnit[PARALYZEBOM] -> retIsBom(conpare.x, conpare.z) != OBSTACLELCOST/*比較するタイルが障害物(ボム)でない？*/) 
				if(wallUnit -> retWallArr(conpare.x, conpare.z) == NOTWALL/*比較するタイルが障害物(壁)でない？*/) 
					if(conpare != D3DXVECTOR3(toIntPos(vsCpuPlayerUnit->pos.x), 0, toIntPos(vsCpuPlayerUnit ->pos.z/*比較するタイルがプレイヤーの位置と等しい？*/))) 
						if(conpare != D3DXVECTOR3(toIntPos(cpuUnit[SPEEDCPU]->pos.x), 0, toIntPos(cpuUnit[SPEEDCPU]->pos.z/*比較するタイルがSpeedCpuの位置と等しい？*/))) 
							if(conpare != D3DXVECTOR3(toIntPos(cpuUnit[PYECPU]->pos.x), 0, toIntPos(cpuUnit[PYECPU]->pos.z/*比較するタイルがPsyCpuの位置と等しい？*/))) 
								if(conpare != D3DXVECTOR3(toIntPos(cpuUnit[NORMALCPU]->pos.x), 0, toIntPos(cpuUnit[NORMALCPU]->pos.z/*比較するタイルがNormalCpuの位置と等しい？*/))) 
	{
		return true;
	}

	return false;
	
}

//=============================================================================
//  ゴールへのパスを取得する
//　引数：なし
//	戻り値：なし
//=============================================================================
void NormalCpu::getPath() 
{
	list<aStar>::iterator closeListitr;
	//ゴールから逆算して経路をたどるからend()
	closeListitr = closeList.end();
	closeListitr--;

	//親リストに格納する情報を入れる変数
	aStar* push;
	//親ポインタのアドレス格納
	push = &*closeListitr;

	//親ポインタをたどりきる（計算時に最初、タイルの位置情報にNULLを代入している）
	while(push->parentTile != NULL)
	{	
		//親リストに親ポインタの情報格納
		parentList.push_back(push->tilePos);
		//親ポインタのアドレス格納
		push = *&push->parentTile;
	}	
}

//=============================================================================
//  移動
//　引数：なし
//	戻り値：なし
//=============================================================================
void NormalCpu::control()
{
	float addAngle = 0;
	//cpuが移動処理を求めているとき
	if(isControl == AI_STATE::MOVE)
	{
		//親タイルを格納するリストが空でなければ
		if(!parentList.empty())
		{	
			//キャラクターごとの移動速度を設定
			if(charaNum == CHARACTER::SPEED)
				moveSpeed = 0.20f;	//SpeedCpuのとき
			else 
				moveSpeed = 0.10f;	//その他のとき

			list<D3DXVECTOR3>::iterator parentListitr;
			parentListitr = parentList.end();
			parentListitr--;

			//他のキャラクターユニットと衝突したか
			if(collObstacel(*parentListitr))
				return;
		
			if(parentListitr -> x == pos.x && parentListitr -> z > pos.z/*今の位置の前方*/) {
				moveDir = MOVE_DIR::ADVANCE;
				pos.z = truncationSecondDecimalPlace(pos.z + (moveSpeed + 0.01f));
			} else if(parentListitr -> x < pos.x && parentListitr -> z == pos.z/*今の位置の左方*/) {
				moveDir = MOVE_DIR::LEFT;
				pos.x = truncationSecondDecimalPlace(pos.x - (moveSpeed - 0.01f));
				addAngle = 270;
			} else if(parentListitr -> x > pos.x && parentListitr -> z == pos.z/*今の位置の右方*/) {
				moveDir = MOVE_DIR::RIGHT;
				pos.x = truncationSecondDecimalPlace(pos.x + (moveSpeed + 0.01f));
				addAngle = 90.0f;
			} else if(parentListitr -> x == pos.x && parentListitr -> z < pos.z/*今の位置の後方*/) {
				moveDir = MOVE_DIR::BACKWARD;
				pos.z = truncationSecondDecimalPlace(pos.z - (moveSpeed - 0.01f));
				addAngle = 180.0f;
			}

			if(( pos.x == parentListitr -> x ) && ( pos.z == parentListitr -> z ))
			{
				parentListitr = parentList.erase(parentListitr);
				if(prevMoveDir != moveDir)
					angle += addAngle;
				prevMoveDir = moveDir;
			}
		} else {	
			//各リストを初期化
			openList.clear();	
			closeList.clear();
			//AIの状態を初期化へ
			isControl = AI_STATE::INIT;
			//ボムを置く
			putBom();
		} 
	}
}

//=============================================================================
//  小数点第二以下切り捨て
//　引数：切り捨てる値
//	戻り値：切り捨てた値
//=============================================================================
float NormalCpu::truncationSecondDecimalPlace(float pos) 
{
	return floor(pos * 10) / 10;
}

//=============================================================================
//  爆発範囲から離れるための探索
//　引数：なし
//	戻り値：なし
//=============================================================================
void NormalCpu::exitExpRange() 
{
	static D3DXVECTOR3 moveDir = D3DXVECTOR3(0, 0, 0);
	static int dirIndex = 0;
	float dx[] = {0.0f, -1.0f, 0.0f, 1.0f};
	float dz[] = {1.0f, 0.0f, -1.0f, 0.0f};
		
	if(!isExitExpMove)
	{		
		//爆発範囲外か
		if(!wallUnit  -> retIsExpRange(toIntPos(pos.x), toIntPos(pos.z))) {
			isOpenListEmpty = false;
			prevPosList.clear();
			//AIの状態を初期化へ
			isControl = AI_STATE::INIT;
			return;
		};

		isExitExpMove = true;
		
		if(prevTileState != PREV_TILE_STATE::MOVE_DIR)
		{	
			moveDir = D3DXVECTOR3(0, 0, 0);
			dirIndex = 0;

			for(int i = 0; i < 4; i++) {
				D3DXVECTOR3 conpareTile(toIntPos(pos.x) + dx[i], pos.y, toIntPos(pos.z) + dz[i]);
				//目の前がフィールド外なら
				if(conpareTile.x < MINRANGE || conpareTile.x > MAXRANGE
					|| conpareTile.z < MINRANGE || conpareTile.z > MAXRANGE || isObstacel(conpareTile))
				{
					continue;
				}

				//比較位置に障害物がなく、かつ進む条件が整った場所があるか
				if(equalPrevPosState(conpareTile)) {
					dirIndex = i;
					moveDir = conpareTile;
					exitExpProcess(moveDir, dirIndex);
					break;
				}
			}	
		}

		if(prevTileState == PREV_TILE_STATE::MOVE_DIR) {
			prevTileState = PREV_TILE_STATE::NOT_PRAV_TILE;
			dirIndex = prevDirIndex;
			moveDir = D3DXVECTOR3(toIntPos(pos.x) + dx[dirIndex], pos.y, toIntPos(pos.z) + dz[dirIndex]);
			exitExpProcess(moveDir, dirIndex);
		}
	}

	if(isExitExpMove)
		exitExpProcess(moveDir, dirIndex);	
}

//=============================================================================
//  爆発範囲から離れるための処理
//　引数：方向
//	戻り値：なし
//=============================================================================
void NormalCpu::exitExpProcess(D3DXVECTOR3 dir, int dirIndex) 
{
	float addAngle = 0.0f;
	float dx[] = {0.0f, -1.0f, 0.0f, 1.0f};
	float dz[] = {1.0f, 0.0f, -1.0f, 0.0f};

	//キャラクターごとの移動速度を設定
	if(charaNum == CHARACTER::SPEED)
		moveSpeed = 0.20f;	//SpeedCpuのとき
	else 
		moveSpeed = 0.10f;	//その他のとき

	if(collObstacel(dir))
		return;

	if(dir.x == pos.x && dir.z > pos.z/*今の位置の前方*/) {
		moveDir = MOVE_DIR::ADVANCE;
		pos.z = truncationSecondDecimalPlace(pos.z + (moveSpeed + 0.01f));
	} else if(dir.x < pos.x && dir.z == pos.z/*今の位置の左方*/) {
		moveDir = MOVE_DIR::LEFT;
		pos.x = truncationSecondDecimalPlace(pos.x - (moveSpeed - 0.01f));
		addAngle = 270;
	} else if(dir.x > pos.x && dir.z == pos.z/*今の位置の右方*/) {
		moveDir = MOVE_DIR::RIGHT;
		pos.x = truncationSecondDecimalPlace(pos.x + (moveSpeed + 0.01f));
		addAngle = 90.0f;
	} else if(dir.x == pos.x && dir.z < pos.z/*今の位置の後方*/) {
		moveDir = MOVE_DIR::BACKWARD;
		pos.z = truncationSecondDecimalPlace(pos.z - (moveSpeed - 0.01f));
		addAngle = 180.0f;
	}

	if(( pos.x == dir.x ) && ( pos.z == dir.z ))
	{
		prevTileInfo push;
		{
			push.prevTilePos = D3DXVECTOR3(toIntPos(pos.x) - dx[dirIndex], pos.y, toIntPos(pos.z) - dz[dirIndex]);
			if(prevTileState == PREV_TILE_STATE::NOT_PRAV_TILE) {
				push.prevTileDir = dirIndex;
			} else if(prevTileState == PREV_TILE_STATE::TO_PRAV_TILE_MOVE) {
				push.prevTileDir = dirIndex;
				prevTileState = PREV_TILE_STATE::MOVE_DIR;
			}  
		}
		prevPosList.push_back(push);
		prevTile = D3DXVECTOR3(toIntPos(pos.x) - dx[dirIndex], pos.y, toIntPos(pos.z) - dz[dirIndex]);
		if(moveDir != prevMoveDir)
			angle += addAngle;
		isExitExpMove = false;
	}
}

//=============================================================================
//  比較位置が前回通った道と等しい場合の処理
//　引数：比較位置
//	戻り値：比較タイルに進むか
//=============================================================================
bool NormalCpu::equalPrevPosState(D3DXVECTOR3 conpare) 
{	
	float dx[] = {0.0f, -1.0f, 0.0f, 1.0f};
	float dz[] = {1.0f, 0.0f, -1.0f, 0.0f};
	vector<prevTileInfo>::iterator prevTileInfoitr;	//イテレータ作成
	prevTileInfoitr = prevPosList.begin();
	list<int> prevDirList;				//前回通ったタイルから前回選択した方向を格納する
	list<int>::iterator prevDirListitr;	//イテレータ作成
	prevDirListitr = prevDirList.begin();
	int elementCount = 0;
	bool isEqual = false;

	if(prevPosList.empty())
		return true;

	while(prevTileInfoitr != prevPosList.end())
	{
		//前回通ったタイルなら
		if(prevTileInfoitr->prevTilePos == conpare) {
			//そのタイルから選択した方向格納
			prevDirList.push_back(prevTileInfoitr->prevTileDir);

		}
		prevTileInfoitr++;
	}	

	if(prevDirList.empty())
		return true;
	
	//そのタイルの4方向に前回の向きと違う向きの空きがあるか
	for(int i = 0; i < 4; i++) {
		//前回選択したことのある方向なら
		while(prevDirListitr != prevDirList.end())
		{
			if(*prevDirListitr == i)
			{
				isEqual = true;
				break;
			}
			prevDirListitr++;
		}

		if(!isEqual) {
			D3DXVECTOR3 conpareTile(toIntPos(conpare.x) + dx[i], pos.y, toIntPos(conpare.z) + dz[i]);
			//空きがある
			if(isObstacel(conpareTile)) {
				//次のタイルへ移動
				prevTileState = PREV_TILE_STATE::TO_PRAV_TILE_MOVE;
				//次のタイルに移動した後進む方向を格納
				prevDirIndex = i;
				break;
			}
		}
		isEqual = false;
	}

	prevDirList.clear();

	if(prevTileState != PREV_TILE_STATE::TO_PRAV_TILE_MOVE) {
		prevTileState = PREV_TILE_STATE::NOT_PRAV_TILE;
		return false;
	}
	
	return true;
}

//=============================================================================
//障害物に対する処理をする
//引数：比較位置、方向ベクトル
//戻り値：衝突したか
//=============================================================================
bool NormalCpu::collObstacel(D3DXVECTOR3 conpare)
{
	D3DXVECTOR3 nowPos(conpare.x, -0.5f, conpare.z);

	//同じ位置で処理をしない
	if(prevPos != nowPos)
	{
		//次進むタイルに爆弾が置かれた
		if(D3DXVECTOR3(nowPos.x, 0, nowPos.z) != D3DXVECTOR3(putBomPos.x, 0, putBomPos.z)/*目の前のボムが自分で置いたものでない*/) {
			if(bomUnit[NORMALBOM] -> retIsBom(nowPos.x, nowPos.z) == OBSTACLELCOST/*比較するタイルが障害物*/
				|| bomUnit[PLUSBOM] -> retIsBom(nowPos.x, nowPos.z) == OBSTACLELCOST/*比較するタイルが障害物*/
					|| bomUnit[PARALYZEBOM] -> retIsBom(nowPos.x, nowPos.z) == OBSTACLELCOST/*比較するタイルが障害物*/)
			{
			
				//ボムを置いた位置初期化(使わない値代入)
				putBomPos = D3DXVECTOR3(999, 999, 999);
				//位置を整数にする
				adjustmentPos();
				//AI処理を中止
				stopAi();
				prevPos = nowPos;
				return true;
			}
		}

		//次進むタイルに他のキャラクターが進んだ
		if(nowPos == vsCpuPlayerUnit ->pos
			|| nowPos == cpuUnit[SPEEDCPU]->pos
				|| nowPos == cpuUnit[PYECPU]->pos
					|| nowPos== cpuUnit[NORMALCPU]->pos)
		{
			//位置を整数にする
			adjustmentPos();
			//ボム配置
			putBom();
			//AI処理を中止
			stopAi();
			prevPos = nowPos;
			return true;
		}
	}

	//次進むタイルに壁が置かれた
	if(wallUnit -> retWallArr(nowPos.x, nowPos.z) != NOTWALL)
	{
		//位置を整数にする
		adjustmentPos();
		//AI処理を中止
		stopAi();
		prevPos = nowPos;
		return true;
	}

	return false;
}

//=============================================================================
//ボムを配置
//引数：なし
//戻り値：なし
//=============================================================================
void NormalCpu::putBom()
{	
	D3DXVECTOR3 bomPutDir = pos + calcMove(angle);

	//現在処理しているボムがなければ
	if(processBomType == NONE) {
		if(!bomThrowNumList.empty()/*スローボムがあれば*/) {
			//前後左右の延長線上にターゲットがいるか
			if(isThrowTarget()) {
				throwFlag = true;
				bomThrowNumList.pop_back();
			}
		}
			
		if((isObstacel(bomPutDir) && isPutBom(bomPutDir)
			&& !(bomPutDir.x < MINRANGE) && !(bomPutDir.x > MAXRANGE)
				&& !(bomPutDir.z < MINRANGE) && !(bomPutDir.z > MAXRANGE)) || throwFlag){
			if(!paralyzeBomNumList.empty()/*痺れボムがあれば*/ && charaSearch) { 
				if(!throwFlag)
					bomPutAngle = angle;
				//どのボムを選んだかを代入
				itemType = ITEM::PARALYZEBOMITEM;
				//アイテムボムを使用
				isItem = true;
				return;
			} else if(bomUnit[NORMALBOM] -> putCharaNum[charaNum].bomRenderState == DONAOTHING/*通常ボムが処理中でなければ*/) {
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

//=============================================================================
//現在の位置からボムを置いた場合、逃げ道があるか判定
//引数：ボムを置く予定位置
//戻り値：ボムを置くか
//=============================================================================
bool NormalCpu::isPutBom(D3DXVECTOR3 bomPutPos)
{
	D3DXVECTOR3 direction[4] = {D3DXVECTOR3(0.0f,0.0f,1.0),D3DXVECTOR3(1.0f,0.0f,0.0),D3DXVECTOR3(0.0f,0.0f,-1.0),D3DXVECTOR3(-1.0f,0.0f,0.0)};	//4方向を格納するリスト
	list<D3DXVECTOR3> conparePosList;
	list<D3DXVECTOR3> checkPosList;
	list<D3DXVECTOR3>::iterator conparePosListitr;
	list<D3DXVECTOR3>::iterator checkPosListitr;
	conparePosList.push_back(pos);
	conparePosListitr = conparePosList.begin();
	checkPosListitr = checkPosList.begin();
	int count = 0;
	int expRange = addExpRange + 2;

	while(!conparePosList.empty())
	{
		for(int i = 0; i < 4; i++)
		{
				bool isChecked = false;
				checkPosListitr = checkPosList.begin();
				D3DXVECTOR3 conparePos = *conparePosListitr+direction[i];
				while(checkPosList.end() != checkPosListitr)
				{
					if(*checkPosListitr == conparePos)
					{
						isChecked = true;
						break;
					}
					checkPosListitr++;
				}

				//以前調べたタイル以外の3方向タイルの中で、空きがあるか
				if(isObstacel(conparePos) && bomPutPos != conparePos && !isChecked 
						&& !(conparePos.x < MINRANGE) && !(conparePos.x > MAXRANGE) && !(conparePos.z < MINRANGE) && !(conparePos.z > MAXRANGE))
				{
					conparePosList.push_back(conparePos);
					checkPosList.push_back(*conparePosListitr);
					//点間距離が、仮想的な爆発範囲外ならtrueを返す
					if(sqrt((pos.x - conparePos.x)*2 + (pos.z - conparePos.z)*2) > expRange)
						return true;

					//点間距離が、15タイルより離れていたらfalseを返す(爆弾が置かれてから爆発するまで移動できる最大距離)
					if(sqrt((pos.x - conparePos.x)*2 + (pos.z - conparePos.z)*2) > 15)
						return false;
				}
		}
		conparePosListitr = conparePosList.erase(conparePosListitr);
	}	
	return false;
}

//=============================================================================
//投げる先を探索
//引数：なし
//戻り値：投げる先があるかどうか
//=============================================================================
bool NormalCpu::isThrowTarget()
{
	D3DXVECTOR3 conparePos(pos.x, 0, pos.z);
	D3DXVECTOR3 conparePosList[4] = {conparePos, conparePos, conparePos, conparePos};	//現在位置からの各方向の加算
	int directionX[4] = {0,1,0,-1};									//4方向のx成分を格納するリスト
	int directionZ[4] = {1,0,-1,0};									//4方向のz成分を格納するリスト
	D3DXVECTOR3 begin(0, 0, 0);
	D3DXVECTOR3 sumDirection[4] = {begin, begin, begin, begin};		//各方向の投げる距離の加算
	loopNum = 0;

	//ループ回数を設定(現在位置からフィールド外までのもっとも短い距離)
	(MAXRANGE - fabs(pos.x) < MAXRANGE - fabs(pos.z)) ? loopNum = (int)(MAXRANGE - fabs(pos.x)) : loopNum = (int)(MAXRANGE - fabs(pos.z));

	for(int count = 1; count <= loopNum; count++) {
		//上下左右各方向分ループ
		for(int i = 0; i < 4; i++) {
			//各方向の1タイル分加算
			sumDirection[i] += D3DXVECTOR3((float)directionX[i], 0, (float)directionZ[i]);
			conparePosList[i] += D3DXVECTOR3((float)directionX[i], 0, (float)directionZ[i]);

			{
				//比較位置がキャラクターと等しければ
				if(conparePosList[i] == D3DXVECTOR3(toIntPos(vsCpuPlayerUnit ->pos.x), 0, toIntPos(vsCpuPlayerUnit ->pos.z/*比較するタイルがプレイヤーの位置と等しい？*/))) {
					//障害物のない場所を探索
					if(searchPutPos(i, sumDirection, conparePosList)) {
						//どの方向に投げるかを定める
						bomPutAngle = angle + (i * 90.0f);
						return true;
					}
					return false;
				}
				if(conparePosList[i] == D3DXVECTOR3(toIntPos(cpuUnit[SPEEDCPU]->pos.x), 0, toIntPos(cpuUnit[SPEEDCPU]->pos.z/*比較するタイルがSpeedCpuの位置と等しい？*/))) {
					if(searchPutPos(i, sumDirection, conparePosList)) {
						bomPutAngle = angle + (i * 90.0f);
						return true;
					}
					return false;
				}
				if(conparePosList[i] == D3DXVECTOR3(toIntPos(cpuUnit[PYECPU]->pos.x), 0, toIntPos(cpuUnit[PYECPU]->pos.z/*比較するタイルがPsyCpuの位置と等しい？*/))) {
					if(searchPutPos(i, sumDirection, conparePosList)) {
						bomPutAngle = angle + (i * 90.0f);
						return true;
					}
					return false;
				}
				if(conparePosList[i] == D3DXVECTOR3(toIntPos(cpuUnit[NORMALCPU]->pos.x), 0, toIntPos(cpuUnit[NORMALCPU]->pos.z/*比較するタイルがNormalCpuの位置と等しい？*/))) {
					if(searchPutPos(i, sumDirection, conparePosList)) {
						bomPutAngle = angle + (i * 90.0f);
						return true;
					}
					return false;
				}
			}
		}
	}

	return false;
}

//=============================================================================
//ボムを置ける位置を逆算して探索
//引数：各方向の投げる距離の加算、現在位置からの各方向の加算
//戻り値：なし
//=============================================================================
bool NormalCpu::searchPutPos(int dir, D3DXVECTOR3 sumDirection[], D3DXVECTOR3 conparePosList[])
{
	int count = 0;
	int directionX[4] = {0,1,0,-1};									//4方向のx成分を格納するリスト
	int directionZ[4] = {1,0,-1,0};									//4方向のz成分を格納するリスト
	//少なくともターゲットの一つ前に飛ばしたい
	D3DXVECTOR3 direction((float)directionX[dir], 0, (float)directionZ[dir]);
	sumDirection[dir] -= direction;
	conparePosList[dir] -= direction;
	while(count <= loopNum) {
		//何も障害物のないところまでさかのぼる
		sumDirection[dir] -= direction;
		conparePosList[dir] -= direction;

		//現在の比較場所に障害物がなければ
		if(isObstacel(conparePosList[dir]))
		{
			//投げる先設定
			bomThrowPos = sumDirection[dir];
			return true;
		}
	}

	return false;
}

//=============================================================================
//アイテム取得
//引数：なし
//戻り値：なし
//=============================================================================
void NormalCpu::getItem()
{
	//アイテムと現在の位置が等しければ
	if(pos.x == wallUnit -> retItem(pos.x,pos.z).x && pos.z == wallUnit -> retItem(pos.x,pos.z).z )
	{
		//アイテム各リストに保存
		switch (wallUnit -> retItemNumber(pos.x, pos.z))
		{
			case ITEM::PLUSBOMITEM:
				plusBomNumList.push_back(PLUSBOMITEM);
				break;

			case ITEM::LARGEEXPLOSIONITEM:
				plusExpRange();
				break;

			case ITEM::PARALYZEBOMITEM:
				paralyzeBomNumList.push_back(PARALYZEBOMITEM);
				break;

			case ITEM::THROWBOMITEM:
				bomThrowNumList.push_back(THROWBOMITEM);
				break;
		}
	}
}

//=============================================================================
//ゴール位置を返す
//引数：現在位置
//戻り値：ゴール位置
//=============================================================================
D3DXVECTOR3 NormalCpu::retTarget(D3DXVECTOR3 pos)
{
	int range = 0;
	int index = 0;
	target = D3DXVECTOR3(pos.x,0,pos.z);
	prevWallCount = 0;
	toTargetCost = 999;
	isTarget = false;
	charaSearch = false;
	int posX = (int)pos.x, posZ = (int)pos.z;

	while (1) {
			int searchX = searchTargetRangeXTable[index];
			int searchZ = searchTargetRangeZTable[index];
			if(0 <= searchX)
				searchX += range;
			else 
				searchX -= range;

			if(0 <= searchZ)
				searchZ += range;
			else 
				searchZ -= range;

			int x = posX + searchX;
			int z = posZ + searchZ;

			//フィールド外は調査しない
			if(x < MINRANGE || x > MAXRANGE
				|| z < MINRANGE || z > MAXRANGE)  
			{
				index++;
				//一定範囲内にターゲットを見つけられなかった
				if(index >= 111 && !isTarget)
				{
					index = 0;
					//範囲拡大
					range += 5;
				}
				
				if(range >= MAXRANGE) 
					return retTargetOfCharacterPos();

				continue;
			}

			//ターゲット探索
			searchTarget((float)x, (float)z, index);

		//一定範囲内にターゲットを見つけた
		if(isTarget) {
			prevTargetPos = pos;
			return target;	
		}

		index++;
	}
}

//=============================================================================
//ゴール位置としてキャラクターの位置を返す
//引数：比較位置、距離コスト
//戻り値：なし
//=============================================================================
D3DXVECTOR3 NormalCpu::retTargetOfCharacterPos()
{	
	if(!vsCpuPlayerUnit->isLose)
		return D3DXVECTOR3(vsCpuPlayerUnit->pos.x, 0, vsCpuPlayerUnit->pos.z);
	
	if(!cpuUnit[SPEEDCPU]->isLose)
		return D3DXVECTOR3(cpuUnit[SPEEDCPU]->pos.x, 0, cpuUnit[SPEEDCPU]->pos.z);

	if(!cpuUnit[PYECPU]->isLose)
		return D3DXVECTOR3(cpuUnit[PYECPU]->pos.x, 0, cpuUnit[PYECPU]->pos.z);

	if(!cpuUnit[NORMALCPU]->isLose)
		return D3DXVECTOR3(cpuUnit[NORMALCPU]->pos.x, 0, cpuUnit[NORMALCPU]->pos.z);

	return D3DXVECTOR3(vsCpuPlayerUnit->pos.x, 0, vsCpuPlayerUnit->pos.z);
}

//=============================================================================
//ゴール位置を探索
//引数：比較位置、距離コスト
//戻り値：なし
//=============================================================================
void NormalCpu::searchTarget(float posX, float posZ, int cost)
{
	D3DXVECTOR3 serchPos(posX,0,posZ);
	//爆発範囲内と以前のターゲット周辺は除外
	if(wallUnit->retIsExpRange(posX, posZ)
		|| (prevTargetPos.x+PREVTARGETRANGE >= serchPos.x && prevTargetPos.x-PREVTARGETRANGE <= serchPos.x 
			&& prevTargetPos.z+PREVTARGETRANGE >= serchPos.z && prevTargetPos.z-PREVTARGETRANGE <= serchPos.z)) {
		return;
	}
		
	//壁を見つけたら
	if(wallUnit -> retWallArr(posX, posZ) == ISWALL) {	
		int nowCost = (int)(targetDistCostTable[cost] * wallTargetMul + 0.5f);
		if(nowCost <= toTargetCost) {
			toTargetCost = nowCost;
			isTarget = true;
			searchWallTarget(posX, posZ);
		}
	}

	//現在処理中のボムが存在しないまたはプラスボムアイテムを持っている
	if(processBomType == NONE || !plusBomNumList.empty()) {
		//各キャラクターを見つけたら
		if(posX == toIntPos(vsCpuPlayerUnit ->pos.x) && posZ == toIntPos(vsCpuPlayerUnit ->pos.z)) {
			//キャラクターをターゲットにしたか(パラライズボムを置くのはキャラクターに対してのみだから確認用)
			charaSearch = true;
			int nowCost = (int)(targetDistCostTable[cost] * charaTargetMul + 0.5f);
			if(nowCost <= toTargetCost) {
				toTargetCost = nowCost;
				isTarget = true;
				target = D3DXVECTOR3(posX,0,posZ);
			}
		} 

		NormalCpu* charaList[] = {cpuUnit[SPEEDCPU],
							cpuUnit[PYECPU],
							cpuUnit[NORMALCPU]};

		for(int i = 0; i < CHARANUM; i++) {
			if(posX == toIntPos(charaList[i] -> pos.x) && posZ == toIntPos(charaList[i] -> pos.z)) {
				charaSearch = true;
				int nowCost = (int)(targetDistCostTable[cost] * charaTargetMul + 0.5f);
				if(nowCost <= toTargetCost) {
					toTargetCost = nowCost;
					isTarget = true;
					target = D3DXVECTOR3(posX,0,posZ);
				}
			}
		}
	}
}

//=============================================================================
//同じ距離の壁の中で周囲８マスに壁が多いものをターゲットにする
//引数：壁の位置
//戻り値：なし
//=============================================================================
void NormalCpu::searchWallTarget(float posX, float posZ)
{
	float dx[] = {0.0f, 1.0f, 1.0f, 1.0f, 0.0f, -1.0f, -1.0f, 0.0f};
	float dz[] = {1.0f, 1.0f, 0.0f, -1.0f, -1.0f, -1.0f, 0.0f, -1.0f};
	int count = 0;
	for(int i = 0; i < 8; i++) {
		if(wallUnit -> retWallArr(posX+dx[i], posZ+dz[i]) == ISWALL)	
			count++;
	}

	if(prevWallCount <= count) {
		target = D3DXVECTOR3(posX,0,posZ);
		prevWallCount = count;
	}
}

//=============================================================================
//AIの処理中断
//引数：なし
//戻り値：なし
//=============================================================================
void NormalCpu::stopAi()
{
	//ストップ処理でなければ(=目の前に障害物があれば)
	if(!isStop)
	{
		//リストを初期化
		parentList.clear();
		openList.clear();	
		closeList.clear();

		isControl = AI_STATE::INIT;
		prevTileState = PREV_TILE_STATE::NOT_PRAV_TILE;
		isExitExpMove = false;
		isOpenListEmpty = false;
	} else /*ストップ処理なら(=痺れボムに引っかかった)*/{
		//停止する時間
		stopCount++;
		//AIの状態を待機処理へ
		isControl = AI_STATE::STAY;
		//STOPTIMEの間待機
		if(stopCount > STOPTIME)
		{
			//リストを初期化
			parentList.clear();
			openList.clear();	
			closeList.clear();

			isControl = AI_STATE::INIT;
			prevTileState = PREV_TILE_STATE::NOT_PRAV_TILE;
			isExitExpMove = false;
			isOpenListEmpty = false;
			//停止時間初期化
			stopCount = 0;
			isStop = false;
		}
	} 
}

//=============================================================================
//位置を整数に整える
//引数：なし
//戻り値：なし
//=============================================================================
void NormalCpu::adjustmentPos()
{
	pos.x = toIntPos(pos.x);	
	pos.z = toIntPos(pos.z);
}

//=============================================================================
//進行方向ベクトルを求める
//引数：角度
//戻り値：なし
//=============================================================================
D3DXVECTOR3 NormalCpu::calcMove(float angle)
{
	//回転範囲を制限（moveに影響が出る）
	if(angle > 360)
		angle = fmod(angle, 360);

	//移動ベクトル（ボムをおく際の引数に必要）
	D3DXVECTOR3 move(0, 0, 1.0f);
	//回転行列
	D3DXMATRIX mRotat;
	D3DXMatrixRotationY(&mRotat, D3DXToRadian(angle));
	D3DXVec3TransformCoord(&move,&move,&mRotat);
	//進行方向ベクトルの長さを整数に変換
	return D3DXVECTOR3(toIntMove(move.x),0,toIntMove(move.z));
}

