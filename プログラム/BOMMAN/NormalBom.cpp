//=============================================================================
//  通常ボム
//　更新日：2016/10/30
//	制作者：佐々木隆貴
//=============================================================================
#include "normalBom.h"
#include "VsPlayer.h"
#include "vsCpuPlayer.h"
#include "NormalCpu.h"
#include "PsyCpu.h"
#include "SpeedCpu.h"
#include "modeChange.h"
#include "Camera.h"
#include "Wall.h"
#include "Model.h"
#include "audio.h"
#include "effect.h"

#define RENDERTIME 120			//描画時間
#define PUTWALL 1				//壁有り
#define NOTWALL 0				//壁なし
#define CHARANUM 3				//キャラクター数
#define OBSTACLELCOST 100		//壁等の障害物のCPUコスト
#define NONCOST 0				//コストなし
#define ITEMCOST -3				//アイテムのコスト
#define GROUNDHEIGHT -0.5f		//地面の高さ
#define BEGIN_SPEED 0.3f		//砲弾初速
#define PI 3.1415927f			//円周率
#define CAMERA1 1				//カメラ番号
#define PICSIZE 1				//テクスチャサイズ
#define UPDATEANIM 1.0f			//アニメーション更新
#define SCALE 5					//大きさ
#define RANGE 2					//範囲
#define MAXRANGE 49				//最大範囲
#define MINRANGE 1				//最低範囲
#define ADDTIME 5				//時間追加分
const float gravity = 0.002f;

//ボムの描画に関する状態
enum BOM_RENDER_STATE
{
	RENDER,			//描画処理
	ACTIONEXPSlON,	//爆発処理実行中
	DONAOTHING		//なんでもない
};

//キャラクターの名前
enum CHARA_NAME
{
	PLAYER,
	PYE,
	NORMAL,
	SPEED
};

//ボムの名前
enum BOM_NAME
{
	NORMALBOM,
	PLUSBOM,
	PARALYZEBOM
};	

//エフェクトの種類
enum EFFECT
{
	EXP,
	FIRE
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
normalBom::normalBom(void)
{
	//モデル読み込み
	model = new Model;
	model->Load(graphics->get3Ddevice(),"Models\\Bom.fbx");

	//オーディオ
	audio = new Audio;

	//オーディオ読み込み
	audio->initialize("Sounds\\Wave Bank.xwb", "Sounds\\Sound Bank.xsb");
	
	initializeMain();
}

//=============================================================================
//初期化
//=============================================================================
void normalBom::initializeMain(void)
{
	//エフェクト読み込み
	fire = new effect;
	fire->Load(graphics, "pictures\\hibana.png", 3, 3);

	//エフェクト読み込み
	exp = new effect;
	exp->Load(graphics, "pictures\\exp.png", 6, 4);

	//ボムの状態初期化
	for(int i = 0; i <= CHARANUM; i++)
	{
		putCharaNum[i].bomRenderState = DONAOTHING;
	}

	initList();

	count = 0;
	bomType = BOM_NAME::NORMALBOM;

	wallUnit = nullptr;
	player1 = nullptr;
	player2 = nullptr;
	vsCpuPlayerUnit = nullptr;
	cpuUnit[NORMALCPU] = nullptr;
	cpuUnit[PYECPU] = nullptr;
	cpuUnit[SPEEDCPU] = nullptr;
}

//=============================================================================
//初期化
//=============================================================================
void normalBom::initialize(void)
{
	SAFE_DELETE(fire);
	SAFE_DELETE(exp);
	initializeMain();
}

//=============================================================================
//デストラクタ
//=============================================================================
normalBom::~normalBom(void)
{
	SAFE_DELETE(model);
	SAFE_DELETE(fire);
	SAFE_DELETE(exp);
	SAFE_DELETE(audio);
}

//=============================================================================
//各リストの状態を初期化
//引数：なし
//戻り値：なし
//=============================================================================
void normalBom::initList()
{
	if(!explosionProcessList.empty()) {
		explosionProcessList.clear();
	}

	for(int i = 0; i <= CHARANUM; i++)
	{			
		putCharaNum[i].bomPos			= D3DXVECTOR3( 0, 0, 0 );
		putCharaNum[i].bomRenderState	= DONAOTHING;
		putCharaNum[i].count			= 0;
		putCharaNum[i].cost				= 0;
		putCharaNum[i].throwPos			= D3DXVECTOR3( 0, 0, 0 );
		putCharaNum[i].isThrowFlag		= false;
		putCharaNum[i].theta			= 0;
		putCharaNum[i].speed			= D3DXVECTOR3( 0, 0, 0 );
		putCharaNum[i].addRange			= 0;
	}
}

//=============================================================================
//シーンのユニットを取得
//引数：シーンのユニット
//戻り値：なし
//=============================================================================
//ScceneBaseから呼び出されたUnitBaseのcollisionsからターゲット取得
//typeid...typeid(型名または型型の変数名）...何型で作られたか
void normalBom::collisions(UnitBase* target)
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
//  更新
//　引数：なし
//	戻り値：なし
//=============================================================================
void normalBom::update()
{	
	bomType = BOM_NAME::NORMALBOM;
	//エフェクト更新
	fire->fixPosUpdate(UPDATEANIM);
	exp->fixPosUpdate(UPDATEANIM);
	//ボムの処理の進める
	renderFlow();
	explosionFlow();
}

//=============================================================================
//ボムを追加
//引数：ボムの位置、移動分、キャラクター名、/*投げる処理なら*/投げる先、投げるかどうか
//戻り値：なし
//=============================================================================
void normalBom::addBom(D3DXVECTOR3 pos/*ボムの位置ベクトル*/, D3DXVECTOR3 move/*移動ベクトル*/,int addExpRabge/*爆発範囲加算分*/, int charaNum /*キャラクター名*/, D3DXVECTOR3 bomThrowPos/*投げる先の座標*/, bool throwFlag/*ボムを投げるか？*/)
{	
	putCharaNum[charaNum].bomPos			= pos;
	putCharaNum[charaNum].bomRenderState	= RENDER;
	putCharaNum[charaNum].count				= 1;
	putCharaNum[charaNum].cost				= OBSTACLELCOST;
	putCharaNum[charaNum].throwPos		    = D3DXVECTOR3((pos.x-move.x)+(fabs(bomThrowPos.x)*move.x), -0.5f, (pos.z-move.z)+(fabs(bomThrowPos.z)*move.z));
	putCharaNum[charaNum].isThrowFlag		= throwFlag;
	putCharaNum[charaNum].addRange			= addExpRabge;

	if(putCharaNum[charaNum].isThrowFlag) {
		//投げるキャラクターと等しい位置を代入
		putCharaNum[charaNum].bomPos		= D3DXVECTOR3(pos.x-move.x, -0.5f, pos.z-move.z);
		//仰角を取得
		putCharaNum[charaNum].theta			= getAngle(bomThrowPos, BEGIN_SPEED);
		//速度を計算
		putCharaNum[charaNum].speed			= D3DXVECTOR3( BEGIN_SPEED * cosf( putCharaNum[charaNum].theta )/* * cosf( -putCharaNum[charaNum].theta )*/,
												BEGIN_SPEED * sinf( putCharaNum[charaNum].theta ),
												BEGIN_SPEED * cosf( putCharaNum[charaNum].theta )/* * sinf( putCharaNum[charaNum].theta ) */);

		putCharaNum[charaNum].speed.x *= move.x;
		putCharaNum[charaNum].speed.z *= move.z;
	}
	if(!putCharaNum[charaNum].isThrowFlag) { 
		//爆発範囲にコスト設定
		setExpRange(true, charaNum);
	}
}

//=============================================================================
//ボムを描画の流れ
//引数：なし
//戻り値：なし
//=============================================================================
void normalBom::renderFlow()
{
	//キャラクターの数分ループ
	for(int i = 0; i <= CHARANUM; i++)
	{
		//描画のカウントが始まっていなければ
		if(putCharaNum[i].count < 1)
			continue;

		//投げる処理ならば
		if(putCharaNum[i].isThrowFlag)
		{
			//投げる処理
			throwProcess(i);
		} else /*投げる処理でなければ*/ {
			//カウント加算
			putCharaNum[i].count++;
			//エフェクトを追加したいタイミング
			if(putCharaNum[i].count % 9 == 0 
				&& putCharaNum[i].count < RENDERTIME)
			{
				addEffect(putCharaNum[i].bomPos, FIRE, i);
			}
		}
	}
}

//=============================================================================
//ボムを描画
//引数：フレーム、カメラ番号
//戻り値：なし
//=============================================================================
void normalBom::render(float frameTime, int cameraNum)
{	
	//キャラクターの数分ループ
	for(int i = 0; i <= CHARANUM; i++)
	{
		//描画のカウントが始まっていなければ
		if(putCharaNum[i].count < 1)
			continue;

		if(putCharaNum[i].count == RENDERTIME) {
			//エフェクト追加
			addEffect(putCharaNum[i].bomPos, EXP, i);

		} else if(putCharaNum[i].count >= RENDERTIME+ADDTIME) {	
			//カメラを揺らす
			callCameraShake(putCharaNum[i].bomPos);
			//爆発処理中にする
			putCharaNum[i].bomRenderState = ACTIONEXPSlON;
			initRender(i);
		}

		D3DXMATRIX mat;
		//移動行列
		D3DXMatrixTranslation(&mat,putCharaNum[i].bomPos.x,putCharaNum[i].bomPos.y,putCharaNum[i].bomPos.z);

		if(cameraNum == 1 || modeChange::GetInstance() -> getMode() == VSCPU/*対戦モードがCPU対戦*/) {
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
	}

	//エフェクト描画
	fire->fixPosDraw(cameraNum);
	exp->fixPosDraw(cameraNum);
}

//=============================================================================
//エフェクト追加
//引数：位置, エフェクトの種類, キャラクター番号
//戻り値：なし
//=============================================================================
void normalBom::addEffect(D3DXVECTOR3 pos, int effectType, int chara)
{
	pos.y+=1.5f; 
	int add = putCharaNum[chara].addRange;

	//エフェクト追加
	switch (effectType) {	
	case EFFECT::EXP:
		if(bomType == NORMALBOM || bomType == PLUSBOM) {
			pos.y += 1.0f + ((float)add * 0.5f);
			exp->fixPosAdd(pos,5.0f + add);
		} else if(bomType == PARALYZEBOM) {
			pos.y += 4.0f;
			exp->fixPosAdd(pos,11);
		}
		break;
	case EFFECT::FIRE:	
		fire->fixPosAdd(pos,(float)((rand() % PICSIZE)+1));
		break;
	}
}

//=============================================================================
//投げる処理
//引数：キャラクター番号
//戻り値：なし
//=============================================================================
void normalBom::throwProcess(int putChara)
{
	putCharaNum[putChara].bomPos += putCharaNum[putChara].speed;
	putCharaNum[putChara].speed.y -= gravity;

	//ボムのｙ成分が地面以下か
	if(putCharaNum[putChara].bomPos.y < GROUNDHEIGHT)
	{	
		D3DXVECTOR3 bomPos = putCharaNum[putChara].bomPos;
		//位置を整数にしてy成分に地面の高さを代入
		putCharaNum[putChara].bomPos = D3DXVECTOR3(toIntPos(bomPos.x), -0.5f, toIntPos(bomPos.z));
		//投げるかどうかのフラグをfalseに
		putCharaNum[putChara].isThrowFlag = false;
		//爆発範囲にコスト設定
		setExpRange(true, putChara);
		//エフェクト追加
		addEffect(putCharaNum[putChara].bomPos, FIRE, putChara);
	}
}

//=============================================================================
//表示情報初期化
//引数：キャラクター番号
//戻り値：なし
//=============================================================================
void normalBom::initRender(int putChara)
{
	//カウント初期化
	putCharaNum[putChara].count = 0;
	putCharaNum[putChara].cost	= 0;
	//ボム処理中のキャラクターリスト
	explosionProcessList.push_back(putChara);
}

//=============================================================================
//爆発処理の流れ
//引数：なし
//戻り値：なし
//=============================================================================
void normalBom::explosionFlow()
{
	//通常ボムの処理
	//ボムの処理を行っているキャラクターが1人以上存在
	if(!(explosionProcessList.empty()))
	{
		list<int>::iterator	procitr;	
		procitr = explosionProcessList.begin();
		//描画処理が終了していれば
		if(putCharaNum[*procitr].bomRenderState == ACTIONEXPSlON) 
		{
			//BGM再生
			audio->playCue("爆発音1");
			explosion(*procitr);
			//爆発範囲にコスト設定
			setExpRange(false, *procitr);
			//何もしない
			putCharaNum[*procitr].bomRenderState = DONAOTHING;	
			//位置を初期化(必要としない数字を入れておく)
			putCharaNum[*procitr].bomPos = D3DXVECTOR3(999,999,999);
			procitr = explosionProcessList.erase(procitr);
		} 
	}
}

//=============================================================================
//ボムの爆発処理
//引数：キャラクター番号
//戻り値：なし
//=============================================================================
void normalBom::explosion(int processCharaNum/*処理をするキャラクター番号*/)
{	
	int blockX;
	int blockZ;
	int add = putCharaNum[processCharaNum].addRange;

	//爆発範囲分ループ
	for(int hight = -(RANGE + add); hight <= (RANGE + add); hight++)
	{	
		for(int width = -(RANGE + add); width <= (RANGE + add); width++) 
		{	
			//通常ボムの処理
			//渡されたキャラクター番号のボムの描画処理が完了していれば
			if(putCharaNum[processCharaNum].bomRenderState == ACTIONEXPSlON) {
				//ボムの位置と判定分を加算
				blockX = (int)(putCharaNum[processCharaNum].bomPos.x+width);
				blockZ = (int)(putCharaNum[processCharaNum].bomPos.z+hight);
			}

			if(blockX < MINRANGE || blockX > MAXRANGE
				|| blockZ < MINRANGE || blockZ > MAXRANGE)
				continue;

			//範囲内に壁が存在するか判定
			if(wallUnit -> retWallArr((float)blockX, (float)blockZ) == PUTWALL)
			{
				//あれば0を代入してウォールクラスで描画されないようにする
				wallUnit -> wallArr[blockX][blockZ] = NOTWALL;
			} else {
				//範囲内にキャラクターが存在するか判定
				characterInRange(blockX, blockZ);
			}
		}
	}	
}

//=============================================================================
//キャラクターが範囲内か判定処理
//引数：ボムの爆発範囲の位置
//戻り値：なし
//=============================================================================
void normalBom::characterInRange(int x, int z)
{
	D3DXVECTOR3 explosionRange((float)x, 0, (float)z);

	//CPU対戦
	if(modeChange::GetInstance() -> getMode() == VSCPU){
		//各CPUキャラクターの情報格納
		NormalCpu* cpuChara[] = { cpuUnit[PYECPU],
					cpuUnit[SPEEDCPU],
					cpuUnit[NORMALCPU] };
		//CPUキャラ分ループ
		for(int i = 0; i < CHARANUM; i++)
		{
			//爆発範囲内にキャラクターがいれば
			if(explosionRange == D3DXVECTOR3(toIntPos(cpuChara[i]->pos.x), 0, toIntPos(cpuChara[i]->pos.z)))
			{
				//キャラクタ-の死亡フラグをtrueに
				cpuChara[i]->isLose = true;
			}
		}

		//爆発範囲内にプレイヤーがいれば
		if(explosionRange == D3DXVECTOR3(toIntPos(vsCpuPlayerUnit ->pos.x), 0, toIntPos(vsCpuPlayerUnit ->pos.z)))
		{
			//プレイヤ-の死亡フラグをtrueに
			vsCpuPlayerUnit ->isLose = true;
		}
	}

	//2P対戦
	if(modeChange::GetInstance() -> getMode() == VS2P)
	{
		//爆発範囲内にプレイヤー1がいれば
		if(explosionRange == D3DXVECTOR3(player1->pos.x, 0, player1->pos.z))
		{
			//プレイヤ-1の死亡フラグをtrueに
			player1->isLose = true;
		}

		//爆発範囲内にプレイヤー2がいれば
		if(explosionRange == D3DXVECTOR3(player2->pos.x, 0, player2->pos.z))
		{
			//プレイヤ-2の死亡フラグをtrueに
			player2->isLose = true;
		}
	}
}

//=============================================================================
//ボムが存在するかを返す
//引数：ボムが存在するかを知りたい座標
//戻り値：なし
//=============================================================================
int normalBom::retIsBom(float x, float z)
{
	//通常ボムの位置
	//キャラクター分ループ
	for(int i = 0; i < 4; i++)
	{
		//求める位置にボムが存在すれば
		if(toIntPos(putCharaNum[i].bomPos.x) == toIntPos(x) && toIntPos(putCharaNum[i].bomPos.z) == toIntPos(z))
		{
			return putCharaNum[i].cost;
		}
	}

	return 0;
}

//=============================================================================
//爆発範囲のコストを設定
//引数：コストを設定するかどうか、キャラクター番号
//戻り値：なし
//=============================================================================
void normalBom::setExpRange(bool isSet, int chara)
{
	count++;
	int add = putCharaNum[chara].addRange;

	//爆発範囲分ループ
	for(int hight = -(RANGE + add); hight <= (RANGE + add); hight++)
	{	
		for(int width = -(RANGE + add); width <= (RANGE + add); width++) 
		{	
			int blockX = (int)(putCharaNum[chara].bomPos.x+width);
			int blockZ = (int)(putCharaNum[chara].bomPos.z+hight);

			if(blockX < MINRANGE || blockX > MAXRANGE
				|| blockZ < MINRANGE || blockZ > MAXRANGE)
				continue;

			if(isSet) {
					wallUnit  -> wallData[blockX][blockZ].isExpRange = true;
					wallUnit  -> wallData[blockX][blockZ].expRangeNum = count;
			} else {
				if(wallUnit  -> wallData[blockX][blockZ].isExpRange) {
					wallUnit -> wallData[blockX][blockZ].isExpRange = false;
				}
			}
		}
	}
}

//=============================================================================
//投げる先へ到達するための角度計算
//引数：投げた先の位置、初速
//戻り値：なし
//=============================================================================
float normalBom::getAngle(D3DXVECTOR3 pos, float speed)
{
	float distance = sqrtf(pos.x * pos.x + pos.z * pos.z);	//標的までの距離
	float sin;

	//アークサインの中身の計算
	sin = distance/*距離*/ * gravity/*重力*/ / (speed/*初速*/ * speed);

	//届かない場合の処理
	//sin関数は上限1の値しかとらない(初速変更で距離変化)
	if( sin > 1.0f ) {
		sin = 1.0f;
	}

	//角度を返す
	//円周率から引くことで、大きいほうの角度を返すため、より曲線的な放物線を描く
	return 0.5f * /*( PI -*/ asinf( sin )/* )*/;
}

//=============================================================================
//カメラを揺らす処理を呼ぶ
//引数：ボムの位置
//戻り値：なし
//=============================================================================
void normalBom::callCameraShake(D3DXVECTOR3 pos)
{
	float shakeSize = 1.0f;

	//カメラを揺らす
	//2P対戦
	if(modeChange::GetInstance() -> getMode() == VS2P)
	{
		player1->setShakeCameraInfo(pos, shakeSize);
		player2->setShakeCameraInfo(pos, shakeSize);
	} else {
		vsCpuPlayerUnit ->setShakeCameraInfo(pos, shakeSize);
	}
}