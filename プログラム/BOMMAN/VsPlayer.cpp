//=============================================================================
//  プレイヤー(2P対専用)
//　更新日：2016/10/30
//	制作者：佐々木隆貴
//=============================================================================
#include "VsPlayer.h"
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
#include "modeChange.h"

const float posSelectSpeed = 1.0f;	
#define PICSIZE         64  //テクスチャサイズ
#define PLAYER1			1	//プレイヤ-番号		
#define MAXRANGE		49	//最大範囲
#define MINRANGE		1	//最小範囲
#define	NOTWALL			0	//壁が存在しない
#define	ISWALL			1	//壁が存在
#define NOTBREAKWALL    2	//破壊されない壁
#define STOPTIME        150	//停止時間
#define OBSTACLELCOST   100	//障害物のコスト
#define CAMERA1			1	//カメラ番号
#define CAMERA2			2	
#define COUNTFRAME		1	//経過フレーム比較用
#define ADDSETFRAME		320	//テクスチャ位置の追加分
#define SHAKETIMERANGE	30	//揺らす時間
#define DIVFRAME		5	//カメラの揺れ位置を分割
#define INITNUM 100			//初期化時に代入する値
#define ITEMNUM 3			//アイテムの種類

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

//ボムの描画に関する状態
enum BOM_RENDER_STATE
{
	RENDER,			//描画処理
	ACTIONEXPSlON,	//爆発処理実行中
	DONAOTHING		//なんでもない
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
VsPlayer::VsPlayer(int playerNum)
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
	player = playerNum;
	//ボムを置くプレイヤーの番号
	putBomPlayer = player;
	
	initList();
	initialize();
}

//=============================================================================
//  初期化
//　引数：なし
//	戻り値：なし
//=============================================================================
void VsPlayer::initialize()
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

	//コンストラクタで渡されたプレイヤー番号によって切り変え(SeneBeseから渡される)
	if(player == PLAYER1)
	{
		//位置
		pos = D3DXVECTOR3(49,-0.5f,49);
		//最初の向き
		angle = 180.0f;
	} else {
		pos = D3DXVECTOR3(1,-0.5f,1);
		angle = 0.0f;
	}

	if(player == PLAYER1)
	{
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
	} else {
		//移動キー
		KEY.MOVE_UP    = VK_NUMPAD8;
		KEY.MOVE_DOWN  = VK_NUMPAD5;
		KEY.MOVE_RIGHT = VK_NUMPAD6;
		KEY.MOVE_LEFT  = VK_NUMPAD4;

		//コマンド選択
		KEY.COMMAND_SELECT_RIGHT = VK_NUMPAD9;
		KEY.COMMAND_SELECT_LEFT  = VK_NUMPAD7;

		//ボムを置く
		KEY.PUT_BOM = VK_NUMPAD2;
		KEY.THROW = VK_NUMPAD0;
	}

	//テクスチャ情報セット
	if(player == PLAYER1)
	{
		bomimage->setX(0);
		plusBomimage->setX(PICSIZE);
		paralyzeBomimage->setX(PICSIZE*2);
		throwimage->setX(PICSIZE*3);
		arrowimage->setX(0);
		leftItemNumimage->setY(PICSIZE);
		rightItemNumimage->setY(PICSIZE);
	} else {
		bomimage->setX(GAME_WIDTH/2+ADDSETFRAME);
		plusBomimage->setX(GAME_WIDTH/2+PICSIZE+ADDSETFRAME);
		paralyzeBomimage->setX(GAME_WIDTH/2+PICSIZE*2+ADDSETFRAME);
		throwimage->setX(GAME_WIDTH/2+PICSIZE*3+ADDSETFRAME);
		arrowimage->setX(GAME_WIDTH/2+ADDSETFRAME);
		leftItemNumimage->setY(PICSIZE);
		rightItemNumimage->setY(PICSIZE);
	}

	wallUnit = nullptr;
	player1 = nullptr;
	player2 = nullptr;
	bomUnit[NORMALBOM] = nullptr;
	bomUnit[PARALYZEBOM] = nullptr;
	bomUnit[PLUSBOM] = nullptr;
	cpuUnit[NORMALCPU] = nullptr;
	cpuUnit[PYECPU] = nullptr;
	cpuUnit[SPEEDCPU] = nullptr;
}

//=============================================================================
//各リストの情報初期化
//=============================================================================
void VsPlayer::initList()
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
}

//=============================================================================
//デストラクタ
//=============================================================================
VsPlayer::~VsPlayer()
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
void VsPlayer::collisions(UnitBase* target)
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
//  描画関数
//　引数：フレーム、カメラ番号
//	戻り値：なし
//=============================================================================
void VsPlayer::render(float frameTime, int cameraNum)
{
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
	} else {
		pEffect -> SetMatrix("g_matWVP", 
			&( mat* cameraB -> GetView() * cameraB -> GetProj() ) );

		pEffect->SetVector("g_vecEye", (D3DXVECTOR4*)&cameraB->GetPos());
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

	//テクスチャ描画
	{
		graphics->spriteBegin();
		bomimage->draw();
		plusBomimage->draw();                    
		throwimage->draw();
		paralyzeBomimage->draw();  
		arrowimage->draw();
		graphics->spriteEnd();
	}

	renderNumber();
	bomProcess();

	//ボムを投げる先を示すテクスチャの描画
	if(effectRenderFlag)
	{
		if(!collObstacel(bomThrowEffectPos, throwFlag)) 
		{
			D3DXMATRIX matScale,matBillboard,matTrans;
			D3DXMatrixScaling(&matScale, 1, 6, 1);
			if(cameraNum == CAMERA1)
			{
				matBillboard = cameraA->billboard();
			} else {
				matBillboard = cameraB->billboard();
			}
			D3DXMatrixTranslation(&matTrans, bomThrowEffectPos.x, 3.0f, bomThrowEffectPos.z);
			BomThrowPosEffectPoly.draw(&(matScale * matBillboard * matTrans));
		}
	}
}

//=============================================================================
//  アイテム数描画
//　引数：なし
//	戻り値：なし
//=============================================================================
void VsPlayer::renderNumber()
{
	list<int> bomList[] = {plusBomNumList,
						paralyzeBomNumList,
						bomThrowNumList};

	for(int i = 0; i < 3/*4*/; i++) {
		int rightNum = (bomList[i].size() / 1) % 10;
		int leftNum = (bomList[i].size() / 10) % 10;
		rightItemNumimage->setCurrentFrame(rightNum);	//1の位
		leftItemNumimage->setCurrentFrame(leftNum);		//10の位

		if(player == PLAYER1 /*プレイヤー1*/) {
			rightItemNumimage->setX((float)(i+1)*PICSIZE+PICSIZE/2);			
			leftItemNumimage->setX((float)(i+1)*PICSIZE);
		} else /*プレイヤー2*/{
			rightItemNumimage->setX(((float)(i+1)*PICSIZE+PICSIZE/2)+GAME_WIDTH/2+ADDSETFRAME);			
			leftItemNumimage->setX(((float)(i+1)*PICSIZE)+GAME_WIDTH/2+ADDSETFRAME);
		}
		graphics->spriteBegin();
		rightItemNumimage->draw();
		leftItemNumimage->draw();
		graphics->spriteEnd();
	}
}

//=============================================================================
//  描画中のボムがあるか
//　引数：なし
//	戻り値：なし
//=============================================================================
void VsPlayer::notRenderBom()
{
	//各ボムで、描画中のものがあれば各クラスのrender呼び出し
	if(!bomUnit[NORMALBOM] -> putCharaNum[putBomPlayer].bomRenderState == RENDER/*通常ボム*/
		&& !bomUnit[PARALYZEBOM] -> putCharaNum[putBomPlayer].bomRenderState == RENDER/*痺れボム*/) 
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
void VsPlayer::bomProcess()
{
	//各ボムの処理
	switch(itemType/*どの種類のボムを処理するか*/)
	{
		case ITEM::DEFAULTITEM/*通常ボム*/:
			if(processBomType == NONE)
			{
				if(isItem/*アイテムボムを使用するか*/)
				{
					//現在通常ボムが何の処理もしていない(=通常ボムを使っていない)
					if(bomUnit[NORMALBOM] -> putCharaNum[putBomPlayer].bomRenderState == DONAOTHING)
					{
						//ボムを置いた場所を保持
						D3DXVECTOR3 putBomPos = D3DXVECTOR3(toIntPos(pos.x)+toIntMove(move.x), pos.y, toIntPos(pos.z)+toIntMove(move.z));
						bomUnit[NORMALBOM] -> addBom(putBomPos/*プレイヤーの位置ベクトル*/, move/*移動ベクトル*/,addExpRange/*爆発範囲加算分*/, putBomPlayer/*キャラクター名*/, bomThrowPos/*投げた先の位置ベクトル*/, throwFlag/*ボムを置くか投げるか*/);
						processBomType = ITEM::DEFAULTITEM;
					}
					//投げるかどうかをfalseにする（bom -> renderの引数になっているため、falseにしておく）
					throwFlag = false;
				}									
			}
			break;

		case ITEM::PLUSBOMITEM/*プラスボム*/:
			{	
				if(isItem/*アイテムボムを使用するか*/)
				{	
					plusBomitr = plusBomNumList.begin();
					
					//plusBomがあれば	
					if(!(plusBomNumList.empty()))
					{	
						//ボムを置いた場所を保持
						D3DXVECTOR3 putBomPos = D3DXVECTOR3(toIntPos(pos.x)+toIntMove(move.x), pos.y, toIntPos(pos.z)+toIntMove(move.z));
						bomUnit[PLUSBOM] -> addBom(putBomPos/*プレイヤーの位置ベクトル*/, move/*移動ベクトル*/,addExpRange/*爆発範囲加算分*/, putBomPlayer/*キャラクター名*/, bomThrowPos/*投げた先の位置ベクトル*/, throwFlag/*ボムを置くか投げるか*/);
						plusBomitr = plusBomNumList.erase(plusBomitr);
					}
					//投げるかどうかをfalseにする（bom -> renderの引数になっているため、falseにしておく）
					throwFlag = false;
				}
			}
			break;

		case ITEM::PARALYZEBOMITEM/*痺れボム*/:
			if(processBomType == NONE)
			{
				if(isItem/*アイテムボムを使用するか*/)
				{
					paralyzeitr = paralyzeBomNumList.begin();
					
					//paralyzeBomがあれば	
					if(!(paralyzeBomNumList.empty()))
					{	
						//ボムを置いた場所を保持
						D3DXVECTOR3 putBomPos = D3DXVECTOR3(toIntPos(pos.x)+toIntMove(move.x), pos.y, toIntPos(pos.z)+toIntMove(move.z));
						bomUnit[PARALYZEBOM] -> addBom(putBomPos/*プレイヤーの位置ベクトル*/, move/*移動ベクトル*/,addExpRange/*爆発範囲加算分*/, putBomPlayer/*キャラクター名*/, bomThrowPos/*投げた先の位置ベクトル*/, throwFlag/*ボムを置くか投げるか*/);
						paralyzeitr = paralyzeBomNumList.erase(paralyzeitr);
						processBomType = ITEM::PARALYZEBOMITEM;
					}	
					//投げるかどうかをfalseにする（bom -> renderの引数になっているため、falseにしておく）
					throwFlag = false;
				} 
			}
			break;
	}

	isItem = false;
}

//=============================================================================
//  更新
//　引数：なし
//	戻り値：なし
//=============================================================================
void VsPlayer::update()
{	
	//移動
	control();	
	//カメラの操作
	cameraCtrl();
	//範囲内か
	inRange();
	//アイテム取得
	getItem();
	//描画中のボムがあるか
	notRenderBom();
	//コマンド選択
	selectCommand();
	//カメラを揺らす
	shakeCamera();
	//投げる処理なら
	if(throwFlag)
		//ボムを投げる位置をい決める
		selectThrowPos();
}

//=============================================================================
//  コマンド選択
//　引数：なし
//	戻り値：なし
//=============================================================================
void VsPlayer::selectCommand()
{
	//どのアイテムを選択するか
	if(input -> wasKeyPush(KEY.COMMAND_SELECT_RIGHT) || input->wasGamePadPush(player, GAMEPAD_RIGHT_SHOULDER))
	{
		if(itemType < ITEMNUM)
		{
			arrowimage->setX(arrowimage->getX()+PICSIZE);
			//この変数に応じて使用アイテムが変わる
			itemType++;
		}
	}

	if(input -> wasKeyPush(KEY.COMMAND_SELECT_LEFT) || input->wasGamePadPush(player, GAMEPAD_LEFT_SHOULDER))
	{
		if(itemType > 0)
		{
			arrowimage->setX(arrowimage->getX()-PICSIZE);
			itemType--;
		}
	}
}

//=============================================================================
//  コマンド関係の処理
//　引数：なし
//	戻り値：なし
//=============================================================================
void VsPlayer::control()
{
	//移動ベクトル
	move = D3DXVECTOR3(0, 0, 1.0f);
	//回転行列
	D3DXMATRIX mRotat;
	D3DXMatrixRotationY(&mRotat, D3DXToRadian(angle));
	D3DXVec3TransformCoord(&move,&move,&mRotat);
	//moveの長さを整数に変換
	move = D3DXVECTOR3(toIntMove(move.x),0,toIntMove(move.z));


	//痺れボムに当たって行動が停止されていなければ
	if(isStop) {
		//カウントが停止時間より大きくなったら
		if(stopCount < STOPTIME/*5秒 * 30fps*/)
		{	//停止時間のカウント加算
			stopCount++;
			return;
		} else {
			//カウント初期化
			stopCount = 0;
			isStop = false;
		}
	}
	
	//ボムを投げる処理中でなければ
	if(!(throwFlag))
	{
		if(input -> isKeyPush(KEY.MOVE_UP) || input->isGamePadPush(player, GAMEPAD_DPAD_UP))
		{
			moveControlCount++;
			//1フレーム目は動かさない(遊びを入れる)
			if(moveControlCount > COUNTFRAME)
			{
				//目の前に障害物があればがあれば
				if(collObstacel(pos, throwFlag))
				{
					//前に進めない
					pos -= move * 0.5f;
				}
				//前進処理
				pos += move * 0.5f;
			}
			//移動ボタンを押したか
			isPushMoveButton = true;
			//どのコマンドを選んだかを代入
			commandSelect = COMMAND_TYPE::MOVEUP;
		}

		//反対を向く処理
		if(input -> isKeyPush(KEY.MOVE_DOWN) || input->isGamePadPush(player, GAMEPAD_DPAD_DOWN))
		{
			//キーを押して1フレーム目なら
			if(iskeyState)
			{	
				//下押したら下向く処理
				//回転範囲を制限（moveに影響が出る）
				if(angle+180 >= 360)
				{
					angle = angle - 360;
				} else {
					angle += 180;
				}

				//2フレーム目以降の処理を禁止
				iskeyState = false;
				//どのコマンドを選んだかを代入
				commandSelect = COMMAND_TYPE::BACKTURN;
			}
		}

		//90度づつ左に回転
		if(input -> isKeyPush(KEY.MOVE_LEFT) || input->isGamePadPush(player, GAMEPAD_DPAD_LEFT))
		{
			//キーを押して1フレーム目なら
			if(iskeyState)
			{	
				adjustmentPos();
				angle -= 90.0f;
				//回転範囲を制限（moveに影響が出る）
				if(angle <= -360)
				{
					angle = 0;
				}
				//2フレーム目以降の処理を禁止
				iskeyState = false;
				//どのコマンドを選んだかを代入
				commandSelect = COMMAND_TYPE::LEFTTURN;
			}
		}

		//90度づつ右に回転
		if(input -> isKeyPush(KEY.MOVE_RIGHT) || input->isGamePadPush(player, GAMEPAD_DPAD_RIGHT))
		{
			//キーを押して1フレーム目なら
			if(iskeyState)
			{	
				adjustmentPos();
				angle += 90.0f;
				//回転範囲を制限（moveに影響が出る）
				if(angle >= 360)
				{
					angle = 0;
				}
				//2フレーム目以降の処理を禁止
				iskeyState = false;
				//どのコマンドを選んだかを代入
				commandSelect = COMMAND_TYPE::RIGHTTURN; 	
			}
		}

		//ボムを置く処理
		if(input -> isKeyPush(KEY.PUT_BOM) || input->isGamePadPush(player, GAMEPAD_A))
		{	
			//目の前が壁、プレイヤー等のときボムを置けないようにする
			if(!collObstacel(pos, throwFlag))
			{
				//キーを押して1フレーム目なら
				if(iskeyState)
				{	
					adjustmentPos();
					//2フレーム目以降の処理を禁止
					iskeyState = false;
					//アイテムボムを選択したかどうかを格納する変数をtrueにする
					isItem = true;
					//どのコマンドを選んだかを代入
					commandSelect = COMMAND_TYPE::BOM;
				}
			}
		} 

		//ボムを投げる処理
		if(input -> isKeyPush(KEY.THROW) || input->isGamePadPush(player, GAMEPAD_Y))
		{	
			//キーを押して1フレーム目なら
			if(iskeyState && !throwFlag && itemType == ITEM::THROWBOMITEM)
			{	
				//2フレーム目以降の処理を禁止
				iskeyState = false;
				//どのコマンドを選んだかを代入
				commandSelect = COMMAND_TYPE::THROW;
				//bomThrowがあれば	
				if(!(bomThrowNumList.empty()))
				{	
					//ボムを投げる先を示すテクスチャ表示
					effectRenderFlag = true;	
					//投げるかどうかをfalseにする（bom -> renderの引数になっているため、falseにしておく）
					throwFlag = true;
					//投げ始める位置を代入
					bomThrowPos = D3DXVECTOR3(0,0,0);
					bomThrowEffectPos = pos;
				}	
			}
		} 
	}

	processedCommand(commandSelect);

}

//=============================================================================
//  コマンドの後処理
//　引数：選択したコマンド
//	戻り値：なし
//=============================================================================
void VsPlayer::processedCommand(int command)
{
	//選んだコマンドの後処理
	switch (commandSelect)
	{	
		case COMMAND_TYPE::MOVEUP:
			//選択したコマンドのキーを離したら
			if(!input -> isKeyPush(KEY.MOVE_UP) && !input->isGamePadPush(player, GAMEPAD_DPAD_UP))
			{
				moveControlCount = 0;
				if(isPushMoveButton)
					adjustmentPos();
				isPushMoveButton = false;
				//MOVEUPとほかのコマンドを押したとき初期化されないとずっとfalseになる
				iskeyState = true;
			}
			break;

		case COMMAND_TYPE::LEFTTURN:
			if(!input -> isKeyPush(KEY.MOVE_LEFT) && !input->isGamePadPush(player, GAMEPAD_DPAD_LEFT))
			{
				iskeyState = true;
				commandSelect = COMMAND_TYPE::NOTCOMMAND;
			}
			break;

		case COMMAND_TYPE::RIGHTTURN:
			if(!input -> isKeyPush(KEY.MOVE_RIGHT) && !input->isGamePadPush(player, GAMEPAD_DPAD_RIGHT))
			{
				iskeyState = true;
				commandSelect = COMMAND_TYPE::NOTCOMMAND;
			}
			break;

		case COMMAND_TYPE::BACKTURN:
			if(!input -> isKeyPush(KEY.MOVE_DOWN) && !input->isGamePadPush(player, GAMEPAD_DPAD_DOWN))
			{
				iskeyState = true;
				commandSelect = COMMAND_TYPE::NOTCOMMAND;
			}
			break;
				
		case COMMAND_TYPE::BOM:
			if(!input -> isKeyPush(KEY.PUT_BOM) && !input->isGamePadPush(player, GAMEPAD_A))
			{
				iskeyState = true;
				commandSelect = COMMAND_TYPE::NOTCOMMAND;
			} 
			break;

		case COMMAND_TYPE::THROW:
			if(!input -> isKeyPush(KEY.THROW) && !input->isGamePadPush(player, GAMEPAD_Y))
			{
				iskeyState = true;
				commandSelect = COMMAND_TYPE::NOTCOMMAND;
			} 
			break;
	}
}

//=============================================================================
//アイテム取得
//引数：なし
//戻り値：なし
//=============================================================================
void VsPlayer::getItem()
{
	//アイテムと現在の位置が等しければ
	if(D3DXVECTOR3(pos.x, 0, pos.z) == wallUnit -> retItem(pos.x,pos.z))
	{
		//取得したアイテムをを保存
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
//投げる先を設定
//引数：なし
//戻り値：なし
//=============================================================================
void VsPlayer::selectThrowPos()
{
	//エフェクトを前に移動
	if(input -> isKeyPush(KEY.MOVE_UP) || input->isGamePadPush(player, GAMEPAD_DPAD_UP)) {		
		bomThrowPos += move;
		bomThrowEffectPos += move;
	}

	//後ろに移動
	if(input -> isKeyPush(KEY.MOVE_DOWN) || input->isGamePadPush(player, GAMEPAD_DPAD_DOWN)) {
		bomThrowPos -= move;
		bomThrowEffectPos -= move;
	}

	//ボムを投げる
	if((input -> isKeyPush(KEY.PUT_BOM) || input->isGamePadPush(player, GAMEPAD_A))/*ボム関係の処理*/ && iskeyState/*他のコマンドが選択されていない*/ && throwFlag/*投げる処理*/ && isBomListState(itemType)/*ボムリストの状態が正しい*/) {
		//指定した先が障害物なかった
		if(!collObstacel(bomThrowEffectPos, throwFlag)) {
			//ボムを投げる先を示す非表示
			effectRenderFlag = false;
			//2フレーム目以降の処理を禁止
			iskeyState = false;
			//アイテムボムを選択したかどうかを格納する変数をtrueにする
			isItem = true;
			//どのコマンドを選んだかを代入
			commandSelect = COMMAND_TYPE::BOM;

			bomThrowitr = bomThrowNumList.begin();
			//使ったアイテム削除
			bomThrowitr = bomThrowNumList.erase(bomThrowitr);
		}
	}

	if((input -> isKeyPush(KEY.THROW) || input->isGamePadPush(player, GAMEPAD_Y)) && iskeyState) {
		//ボムの投げる位置ベクトルの初期化
		/*render処理の引数にbomThrowPosがあるため、
		投げる処理から置く処理に切り替えた際に影響のないようにする*/
		bomThrowPos = D3DXVECTOR3(0,0,0);
		bomThrowEffectPos = pos;
		//置く処理を選択した
		throwFlag = false;	
		//ボムを投げる先を示す非表示
		effectRenderFlag = false;
		iskeyState = false;
	}
}

//=============================================================================
//  カメラコントローラ
//　引数：なし
//	戻り値：なし
//=============================================================================
void VsPlayer::cameraCtrl()
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
	if(player == PLAYER1)
	{
		cameraA->setPos(vCamPos);
		cameraA->setTarget(target);
	} else {
		cameraB->setPos(vCamPos);
		cameraB->setTarget(target);
	}
}

//=============================================================================
//カメラを揺らす情報セット
//引数：爆発位置(近いほうが優先される), 揺らす大きさ
//引数：なし
//=============================================================================
void VsPlayer::setShakeCameraInfo(D3DXVECTOR3 expPos, float shakeSize)
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

//=============================================================================
//カメラを揺らす
//引数：なし
//引数：なし
//=============================================================================
void VsPlayer::shakeCamera()
{ 	
	//揺れる命令が出たら
	if(isShake) {
		// 揺らし座標の算出
		float hight = ((1.0f-(minRange*0.01f)) / (1.0f+(shakeTime+2.0f) / DIVFRAME)) * shakeDir;
	    
        shakePos = D3DXVECTOR3(0, hight, 0);

		shakeTime++;

		//一定のフレーム分動いたら逆方向に移動
		if(shakeTime % DIVFRAME == 0) {
			shakeDir *= -1.0;
		}

		//揺らす時間を過ぎたら
		if(shakeTime >= SHAKETIMERANGE){
			shakeTime = 0;
			minRange = 999.0f; 
			shakePos = D3DXVECTOR3(0, 0, 0);
			shakeDir = 1.0f;
			isShake = false;
		}
	}
}

//=============================================================================
//障害物に対する処理をする
//引数：コストを設定するかどうか、キャラクター番号
//戻り値：なし
//=============================================================================
bool VsPlayer::collObstacel(D3DXVECTOR3 pos, bool isThrow)
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
	bool isCharacter = false;
	if(player == PLAYER1) {
		//ボムを置いたプレイヤーの番号
		if(player2->pos == D3DXVECTOR3(blockX, pos.y, blockZ)) {
			isCharacter = true;
		}
	} else {
		if(player1->pos == D3DXVECTOR3(blockX, pos.y, blockZ)) {
			isCharacter = true;
		}
	}

	if(isCharacter)
	{
		isHit = true;
	} 

	return isHit;
}

//=============================================================================
//位置を整数に整える
//引数：なし
//戻り値：なし
//=============================================================================
void VsPlayer::adjustmentPos()
{
	float decimalX, decimalZ;
	decimalX = fabs(pos.x) - fabs(floor(pos.x));
	decimalZ = fabs(pos.z) - fabs(floor(pos.z));
	if(decimalX >= 0.5f || decimalZ >= 0.5f)
		pos += move * 0.5f;
}

//=============================================================================
//位置がフィールド内かを判定
//引数：なし
//戻り値：なし
//=============================================================================
void VsPlayer::inRange()
{
	if(pos.z > MAXRANGE) {
		pos.z = MAXRANGE;
	} else if(pos.x > MAXRANGE) {
		pos.x = MAXRANGE;
	} else if(pos.z < MINRANGE) {
		pos.z = MINRANGE;
	} else if(pos.x < MINRANGE) {
		pos.x = MINRANGE;
	} 

	float blockX = toIntPos(pos.x)+toIntMove(move.x), blockZ = toIntPos(pos.z)+toIntMove(move.z);

	if(throwFlag) {
		if(blockZ > pos.z) {
			if(bomThrowEffectPos.z > MAXRANGE) {
				bomThrowEffectPos.z = MAXRANGE;
			} else if(bomThrowEffectPos.z < pos.z) {
				bomThrowEffectPos.z = blockZ;
			}
		}
		
		if(blockX > pos.x) {
			if(bomThrowEffectPos.x > MAXRANGE) {
				bomThrowEffectPos.x = MAXRANGE;
			} else if(bomThrowEffectPos.x < pos.x) {
				bomThrowEffectPos.x = blockX;
			}
		}
		
		if(blockZ < pos.z) { 
			if(bomThrowEffectPos.z < MINRANGE) {
				bomThrowEffectPos.z = MINRANGE;
			} else if(bomThrowEffectPos.z > pos.z) {
				bomThrowEffectPos.z = blockZ;
			}
		}
		
		if(blockX < pos.x) {  
			if(bomThrowEffectPos.x < MINRANGE) {
				bomThrowEffectPos.x = MINRANGE;
			} else if(bomThrowEffectPos.x > pos.x) {
				bomThrowEffectPos.x = blockX;
			}
		}
	}
}

//=============================================================================
//各ボムリストが空かどうかを返す
//引数：なし
//戻り値：存在するかどうか
//=============================================================================
bool VsPlayer::isBomListState(int	itenType)
{
	switch (itenType)
	{
	case DEFAULTITEM:
		return true;
		break;
	case PLUSBOMITEM:
		if(!plusBomNumList.empty())
			return true;
		else
			return false;
		break;
	case PARALYZEBOMITEM:
		if(!paralyzeBomNumList.empty())
			return true;
		else
			return false;
		break;
	case THROWBOMITEM:
		return false;

	}

	return false;
}