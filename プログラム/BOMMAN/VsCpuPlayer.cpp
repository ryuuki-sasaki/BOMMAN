//=============================================================================
//  プレイヤー(CPU対専用)
//　更新日：2016/10/30
//	制作者：佐々木隆貴
//=============================================================================
#include "vsCpuPlayer.h"
#include "input.h"
#include "Camera.h"
#include "NormalCpu.h"
#include "PsyCpu.h"
#include "SpeedCpu.h"
#include "normalBom.h"
#include "paralyzeBom.h"
#include "plusBom.h"
#include "Wall.h"
#include "Model.h"
#include "Image.h"

const float posSelectSpeed = 1.0f;
#define PICSIZE         64  //テクスチャサイズ
#define VSCPUPLAYER		1	//プレイヤ-番号		
#define PUTBOMPLAYER	0	//ボムを置くプレイヤ-番号	
#define MAXRANGE		49	//最大範囲
#define MINRANGE		1	//最小範囲
#define	NOTWALL			0	//壁が存在しない
#define	ISWALL			1	//壁が存在
#define NOTBREAKWALL    2	//破壊されない壁
#define STOPTIME        150	//停止時間
#define OBSTACLELCOST   100	//障害物のコスト
#define COUNTFRAME		1	//経過フレーム比較用
#define CAMERA1			1	//カメラ番号
#define SHAKETIMERANGE	30	//揺らす時間
#define DIVFRAME		5	//カメラの揺れ位置を分割

//コマンドの種類+
enum COMMAND_TYPE
{
	NOTCOMMAND,		//何もなし 
	MOVEUP,			//前進
	LEFTTURN,		//左に回転 
	RIGHTTURN,		//右に回転 
	BACKTURN,		//後ろに回転
	BOM,			//通常ボム 
	THROW,			//ボムを投げる
};

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
vsCpuPlayer::vsCpuPlayer() : VsPlayer(0)
{
	//モデル読み込み
	model = new Model;
	//イメージ読み込み
	bomimage			   = new Image;
	plusBomimage		   = new Image;
	arrowimage			   = new Image;
	paralyzeBomimage       = new Image;
	throwimage			   = new Image;
	leftItemNumimage	   = new Image;
	rightItemNumimage	   = new Image;

	//モデル読み込み
	model->Load(graphics->get3Ddevice(),"Models\\chara_w2.fbx");

	//テクスチャ読み込み
	//ボムを投げる先を示すエフェクト
	if (!texBomThrowPosEffect.initialize(graphics,"pictures\\BomThrowPosEffect2.png"))
		throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing BomThrowPosEffect"));
		BomThrowPosEffectPoly.initialize(graphics, &texBomThrowPosEffect);

	//bom
	if (!texBom.initialize(graphics,"pictures\\bom.png"))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing bom texture"));
    if (!bomimage->initialize(graphics,PICSIZE,PICSIZE,0,&texBom))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing bom"));

    //plusBom
	if (!texPlusBom.initialize(graphics,"pictures\\plusBom.png"))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing plusBom texture"));
    if (!plusBomimage->initialize(graphics,PICSIZE,PICSIZE,0,&texPlusBom))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing plusBom"));

	//paralyzeBom
	if (!texParalyzeBom.initialize(graphics,"pictures\\paralyze.png"))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing paralyze texture"));
    if (!paralyzeBomimage->initialize(graphics,PICSIZE,PICSIZE,0,&texParalyzeBom))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing paralyzeBom"));

	//throwBom
	if (!texThrow.initialize(graphics,"pictures\\bomThrow.png"))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing bomThrow texture"));
    if (!throwimage->initialize(graphics,PICSIZE,PICSIZE,0,&texThrow))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing bomThrow"));

	//矢印
	if (!texArrow.initialize(graphics,"pictures\\カーソル.png"))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing Arrow texture"));
    if (!arrowimage->initialize(graphics,PICSIZE,PICSIZE,0,&texArrow))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing largeexplosionBom"));

	//アイテム数用
	if (!texItemNum.initialize(graphics,"pictures\\s_number.png"))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing number texture"));
    if (!leftItemNumimage->initialize(graphics,PICSIZE/2,PICSIZE/2,0,&texItemNum))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing number"));
	if (!rightItemNumimage->initialize(graphics,PICSIZE/2,PICSIZE/2,0,&texItemNum))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing number"));

	//プレイヤー番号代入
	player = VSCPUPLAYER;
	//ボムを置くプレイヤーの番号
	putBomPlayer = PUTBOMPLAYER;

	initList();
	initialize();
}

//=============================================================================
//  初期化
//　引数：なし
//	戻り値：なし
//=============================================================================
void vsCpuPlayer::initialize()
{
	//各情報初期化
	iskeyState = true;
	throwFlag  = false;
	isItem	   = false;
	isStop     = false;
	isLose     = false;
	effectRenderFlag = false;
	isShake = false;
	isPushMoveButton = false;

	processBomType = NONE;
	commandSelect = NOTCOMMAND;
	itemType = DEFAULTITEM;

	stopCount = 0;
	moveControlCount = 0;
	minRange = 999.0f;	
	shakeDir = 1.0f;
	shakeTime = 0;
	addExpRange = 0;

	shakePos = D3DXVECTOR3(0,0,0);
	bomThrowPos = D3DXVECTOR3(0,0,0);
	bomThrowEffectPos = D3DXVECTOR3(0,0,0);

	//位置
	pos = D3DXVECTOR3(49,-0.5f,49);
	//最初の向き
	angle = 180.0f;

	//移動キー
	KEY.MOVE_UP    = VK_UP;
	KEY.MOVE_DOWN  = VK_DOWN;
	KEY.MOVE_RIGHT = VK_RIGHT;
	KEY.MOVE_LEFT  = VK_LEFT;

	//コマンド選択
	KEY.COMMAND_SELECT_RIGHT = 'E';
	KEY.COMMAND_SELECT_LEFT  = 'W';

	//ボムを置く
	KEY.PUT_BOM = 'A';
	KEY.THROW = 'S';


	//テクスチャ情報セット
	bomimage->setX(0);
	plusBomimage->setX(PICSIZE);
	paralyzeBomimage->setX(PICSIZE*2);
	throwimage->setX(PICSIZE*3);
	arrowimage->setX(0);
	leftItemNumimage->setY(PICSIZE);
	rightItemNumimage->setY(PICSIZE);

	wallUnit = nullptr;
	bomUnit[NORMALBOM] = nullptr;
	bomUnit[PARALYZEBOM] = nullptr;
	bomUnit[PLUSBOM] = nullptr;
	cpuUnit[NORMALCPU] = nullptr;
	cpuUnit[PYECPU] = nullptr;
	cpuUnit[SPEEDCPU] = nullptr;
}

//=============================================================================
//デストラクタ
//=============================================================================
vsCpuPlayer::~vsCpuPlayer()
{
	SAFE_DELETE(model);
	SAFE_DELETE(bomimage);
	SAFE_DELETE(plusBomimage);
	SAFE_DELETE(arrowimage);
	SAFE_DELETE(paralyzeBomimage);
	SAFE_DELETE(throwimage);
	SAFE_DELETE(leftItemNumimage);
	SAFE_DELETE(rightItemNumimage);
}

//=============================================================================
//シーンのユニットを取得
//引数：シーンのユニット
//戻り値：なし
//=============================================================================
//ScceneBaseから呼び出されたUnitBaseのcollisionsからターゲット取得
//typeid...typeid(型名または型型の変数名）...何型で作られたか
void vsCpuPlayer::collisions(UnitBase* target)
{
	if(typeid(*target) == typeid(Wall))
		wallUnit = (Wall*)target;

	if(typeid(*target) == typeid(normalBom))
		bomUnit[NORMALBOM] = ((normalBom*)target);
	
	if(typeid(*target) == typeid(paralyzeBom))
		bomUnit[PARALYZEBOM] = ((paralyzeBom*)target);

	if(typeid(*target) == typeid(plusBom))
		bomUnit[PLUSBOM] = ((plusBom*)target);

	if(typeid(*target) == typeid(NormalCpu))
		cpuUnit[NORMALCPU] = (NormalCpu*)target;

	if(typeid(*target) == typeid(PsyCpu))
		cpuUnit[PYECPU] = (PsyCpu*)target;

	if(typeid(*target) == typeid(SpeedCpu))
		cpuUnit[SPEEDCPU] = (SpeedCpu*)target;
}

//=============================================================================
//  カメラコントローラ
//　引数：なし
//	戻り値：なし
//=============================================================================
void vsCpuPlayer::cameraCtrl()
{
	//カメラの位置(世界の中心から見て)
	D3DXVECTOR3 vCamPos(0,15,-30);
	
	//行列
	D3DXMATRIX vWorld,vCamangle;

	//回転行列
	D3DXMatrixRotationY(&vCamangle, D3DXToRadian(angle));
	D3DXVec3TransformCoord(&vCamPos,&vCamPos,&vCamangle);
	D3DXVec3TransformCoord(&shakePos, &shakePos, &vCamangle);

	//戦車とカメラの距離（ベクトル）を保持
	vCamPos += pos;
	D3DXVECTOR3 target = pos+shakePos;

	cameraA->setPos(vCamPos);
	cameraA->setTarget(target);
	cameraA->update();
}

//=============================================================================
//障害物に対する処理をする
//引数：コストを設定するかどうか、キャラクター番号
//戻り値：なし
//=============================================================================
bool vsCpuPlayer::collObstacel(D3DXVECTOR3 pos, bool isThrow)
{
	float blockX = toIntPos(pos.x), blockZ = toIntPos(pos.z);

	if(!isThrow) {
		//目の前の位置格納
		blockX += toIntMove (move.x);
		blockZ += toIntMove(move.z);
	}
	bool isHit = false;

	//目の前が壁だったら
	if(wallUnit -> retWallArr(blockX,blockZ) != NOTWALL) {
		//進めない処理
		isHit = true;
	}

	//目の前が爆弾
	if(bomUnit[NORMALBOM] -> retIsBom(blockX, blockZ) == OBSTACLELCOST/*比較するタイルが障害物*/
		|| bomUnit[PLUSBOM] -> retIsBom(blockX, blockZ) == OBSTACLELCOST/*比較するタイルが障害物*/
			|| bomUnit[PARALYZEBOM] -> retIsBom(blockX, blockZ) == OBSTACLELCOST/*比較するタイルが障害物*/)
	{
		isHit = true;
	} 

	//目の前がキャラクター
	if(D3DXVECTOR3(blockX, 0, blockZ) == D3DXVECTOR3(toIntPos(cpuUnit[SPEEDCPU]->pos.x), 0, toIntPos(cpuUnit[SPEEDCPU]->pos.z))
		|| D3DXVECTOR3(blockX, 0, blockZ ) == D3DXVECTOR3(toIntPos(cpuUnit[PYECPU]->pos.x), 0, toIntPos(cpuUnit[PYECPU]->pos.z))
			||D3DXVECTOR3(blockX, 0, blockZ) == D3DXVECTOR3(toIntPos(cpuUnit[NORMALCPU]->pos.x), 0, toIntPos(cpuUnit[NORMALCPU]->pos.z)))
	{
		isHit = true;
	} 

	return isHit;
}

//=============================================================================
//カメラを揺らす情報セット
//引数：爆発位置(近いほうが優先される), 揺らす大きさ
//引数：なし
//=============================================================================
void vsCpuPlayer::setShakeCameraInfo(D3DXVECTOR3 expPos, float shakeSize)
{ 	
	//爆発した位置と現在位置の距離
	D3DXVECTOR3 range = expPos - pos;
	//現在の距離
	float nowRange = D3DXVec3Length(&range);
	nowRange *= shakeSize;

	//10タイル以上離れていれば揺らさない
	if(nowRange > 10.0f) 
		return;

	//現在の距離が最小距離より小さければ
	if(nowRange < minRange) {
		isShake = true;
		shakeTime = 0;
		minRange = nowRange*(nowRange/2);  
	}
}