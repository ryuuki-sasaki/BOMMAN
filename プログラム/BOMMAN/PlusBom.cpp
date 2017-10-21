//=============================================================================
//  プラスボム
//　更新日：2016/10/30
//	制作者：佐々木隆貴
//=============================================================================
#include "plusBom.h"
#include "modeChange.h"
#include "Camera.h"
#include "vsCpuPlayer.h"
#include "PsyCpu.h"
#include "SpeedCpu.h"
#include "NormalCpu.h"
#include "audio.h"
#include "Wall.h"
#include "effect.h"
#include "Model.h"
#include "VsPlayer.h"

#define RENDERTIME 90			//描画時間
#define PUTWALL 1				//壁有り
#define NOTWALL 0				//壁なし
#define CHARANUM 3				//キャラクター数
#define OBSTACLELCOST 100		//壁等の障害物のCPUコスト
#define NONCOST 0				//コストなし
#define ITEMCOST -3				//アイテムのコスト
#define GROUNDHEIGHT -0.5f		//地面の高さ
#define BEGIN_SPEED 0.3f		//砲弾初速
#define CAMERA1 1				//カメラ番号
#define PICSIZE 1				//テクスチャサイズ
#define UPDATEANIM 1.0f			//アニメーション更新
#define SCALE 5					//大きさ
#define RANGE 2					//範囲
#define MAXRANGE 49				//最大範囲
#define MINRANGE 1				//最低範囲
#define UPDATEANIM 1.0f			//アニメーション更新
#define ADDTIME 5				//時間追加分
const float gravity = 0.002f;

//ボムの種類
enum BOM_NAME
{
	NORMAL,
	PLUS,
	PARALYZE
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
plusBom::plusBom(void)
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
void plusBom::initializeMain(void)
{
	//エフェクト読み込み
	fire = new effect;
	fire->Load(graphics, "pictures\\hibana.png", 3, 3);

	//エフェクト読み込み
	exp = new effect;
	exp->Load(graphics, "pictures\\exp.png", 6, 4);

	initList();

	count = 0;
	bomType = BOM_NAME::PLUS;

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
void plusBom::initialize(void)
{
	SAFE_DELETE(fire);
	SAFE_DELETE(exp);
	initializeMain();
}

//=============================================================================
//デストラクタ
//=============================================================================
plusBom::~plusBom(void)
{
	SAFE_DELETE(model);
	SAFE_DELETE(fire);
	SAFE_DELETE(exp);
	SAFE_DELETE(audio);
}

//=============================================================================
//シーンのユニットを取得
//引数：シーンのユニット
//戻り値：なし
//=============================================================================
//ScceneBaseから呼び出されたUnitBaseのcollisionsからターゲット取得
//typeid...typeid(型名または型型の変数名）...何型で作られたか
void plusBom::collisions(UnitBase* target)
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
void plusBom::update()
{	
	bomType = BOM_NAME::PLUS;
	//エフェクト更新
	fire->fixPosUpdate(UPDATEANIM);
	exp->fixPosUpdate(UPDATEANIM);
	//ボムの処理の進める
	renderFlow();
	explosionFlow();
}

//=============================================================================
//各リストの状態を初期化
//引数：なし
//戻り値：なし
//=============================================================================
void plusBom::initList()
{
	if(!explosionProcessList.empty()) {
		explosionProcessList.clear();
	}

	for(int i = 0; i <= CHARANUM; i++)
	{
		if(!putCharaNum[i].dataList.empty())
		{
			putCharaNum[i].dataList.clear();
		}				
	}
}

//=============================================================================
//プラスボムの情報を取得
//引数：ボムの位置、移動分、キャラクター名、/*投げる処理なら*/投げる先、投げるかどうか
//戻り値：なし
//=============================================================================
void plusBom::addBom(D3DXVECTOR3 pos/*ボムの位置ベクトル*/, D3DXVECTOR3 move/*移動ベクトル*/,int addExpRabge/*爆発範囲加算分*/, int charaNum /*キャラクター名*/, D3DXVECTOR3 bomThrowPos/*投げる先の座標*/, bool throwFlag/*ボムを投げるか？*/)
{	
	plusBomData pushData;

	{
		pushData.bomPos					= pos;		
		pushData.isBomRenderComp		= false;																						
		pushData.count					= 1;																							
		pushData.cost					= OBSTACLELCOST;
		pushData.throwPos				= D3DXVECTOR3((pos.x-move.x)+(fabs(bomThrowPos.x)*move.x), -0.5f, (pos.z-move.z)+(fabs(bomThrowPos.z)*move.z));
		pushData.isThrowFlag			= throwFlag;
		pushData.addRange				= addExpRabge;

		if(throwFlag) {
			//投げるキャラクターと等しい位置を代入
			pushData.bomPos			= D3DXVECTOR3(pos.x-move.x, -0.5f, pos.z-move.z);
			//仰角を取得
			theta					= getAngle(bomThrowPos, BEGIN_SPEED);
			//速度を計算
			pushData.speed			= D3DXVECTOR3( BEGIN_SPEED * cosf( theta ) /** cosf( -putCharaNum[CHARANUM].theta )*/,
													BEGIN_SPEED * sinf( theta ),
													BEGIN_SPEED * cosf( theta ) /** sinf( putCharaNum[CHARANUM].theta )*/ );
			pushData.speed.x *= move.x;
			pushData.speed.z *= move.z;
		}	
	}
	//ボムデータ代入
	putCharaNum[charaNum].dataList.push_back(pushData);

	if(!throwFlag) { 
		//爆発範囲にコスト設定
		setExpRange(true, charaNum);
	}
}

//=============================================================================
//ボムを描画の流れ
//引数：なし
//戻り値：なし
//=============================================================================
void plusBom::renderFlow()
{	
	//キャラクター分ループ
	for(int i = 0; i <= CHARANUM; i++)
	{
		//もしプラスボムが存在していなければ
		if(putCharaNum[i].dataList.empty())
			continue;

		putCharaNum[i].dataiter = putCharaNum[i].dataList.begin();

		while(putCharaNum[i].dataiter != putCharaNum[i].dataList.end())
		{	
			//投げる処理ならば
			if(putCharaNum[i].dataiter -> isThrowFlag)
			{
				throwProcess(i);
			} else /*投げる処理でなければ*/ {		
				//カウント加算
				putCharaNum[i].dataiter -> count++;
				//エフェクトを描画したいタイミング
				if(putCharaNum[i].dataiter -> count % 9 == 0 
					&& putCharaNum[i].dataiter -> count < RENDERTIME)
				{
					//エフェクト追加
					addEffect(putCharaNum[i].dataiter->bomPos, FIRE, i);
				}
			}
			putCharaNum[i].dataiter++;
		}	
		
		putCharaNum[i].dataiter = putCharaNum[i].dataList.begin();	
	}
}

//=============================================================================
//ボムを描画
//引数：フレーム、カメラ番号
//戻り値：なし
//=============================================================================
void plusBom::render(float frameTime, int cameraNum)
{	
	//キャラクター分ループ
	for(int i = 0; i <= CHARANUM; i++)
	{
		//もしプラスボムが存在していなければ
		if(putCharaNum[i].dataList.empty())
			continue;

		putCharaNum[i].dataiter = putCharaNum[i].dataList.begin();

		while(putCharaNum[i].dataiter != putCharaNum[i].dataList.end())
		{	
			if(putCharaNum[i].dataiter -> count == RENDERTIME) {
				//エフェクト追加
				addEffect(putCharaNum[i].dataiter->bomPos, EXP, i);

			} else if(putCharaNum[i].dataiter -> count >= RENDERTIME+ADDTIME) {
				//カメラを揺らす
				callCameraShake(putCharaNum[i].dataiter->bomPos);
				initRender(i);
			}

			D3DXMATRIX mat;
			//移動行列
			D3DXMatrixTranslation(&mat,putCharaNum[i].dataiter -> bomPos.x, putCharaNum[i].dataiter -> bomPos.y, putCharaNum[i].dataiter -> bomPos.z);

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

			putCharaNum[i].dataiter++;
		}
	
		putCharaNum[i].dataiter = putCharaNum[i].dataList.begin();	
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
void plusBom::addEffect(D3DXVECTOR3 pos, int effectType, int chara)
{
	pos.y+=1.5f; 
	int add = putCharaNum[chara].dataiter -> addRange;

	//エフェクト追加
	switch (effectType) {	
	case EFFECT::EXP:
		if(bomType == NORMAL || bomType == PLUS) {
			pos.y += 1.0f + ((float)add * 0.5f);
			exp->fixPosAdd(pos,5.0f + add);
		} else if(bomType == PARALYZE) {
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
void plusBom::throwProcess(int putChara)
{
	putCharaNum[putChara].dataiter -> bomPos += putCharaNum[putChara].dataiter -> speed;
	putCharaNum[putChara].dataiter -> speed.y -= gravity;

	//ボムのｙ成分が地面以下か
	if(putCharaNum[putChara].dataiter -> bomPos.y < GROUNDHEIGHT)
	{	
		D3DXVECTOR3 bomPos = putCharaNum[putChara].dataiter -> bomPos;
		//位置を整数にしてy成分に地面の高さを代入
		putCharaNum[putChara].bomPos = D3DXVECTOR3(toIntPos(bomPos.x), -0.5f, toIntPos(bomPos.z));
		//投げるかどうかのフラグをfalseに
		putCharaNum[putChara].dataiter -> isThrowFlag = false;
		//爆発範囲にコスト設定
		setExpRange(true, putChara);
		//エフェクト追加
		addEffect(putCharaNum[putChara].dataiter->bomPos, FIRE, putChara);
	}
}

//=============================================================================
//表示情報初期化
//引数：キャラクター番号
//戻り値：なし
//=============================================================================
void plusBom::initRender(int putChara)
{
	//ボムの描画が終了したか？
	putCharaNum[putChara].dataiter -> isBomRenderComp = true;
	//カウント初期化
	putCharaNum[putChara].dataiter -> count = 0;
	putCharaNum[putChara].dataiter -> cost	 = 0;
}

//=============================================================================
//爆発処理の流れ
//引数：なし
//戻り値：なし
//=============================================================================
void plusBom::explosionFlow()
{
	//プラスボムの処理
	//キャラクター分ループ
	for(int i = 0; i <= CHARANUM; i++)
	{	
		//ボムが置かれていれば
		if(!(putCharaNum[i].dataList.empty()))
		{
			putCharaNum[i].dataiter = putCharaNum[i].dataList.begin();
			//描画処理が終了していれば
			if(putCharaNum[i].dataiter->isBomRenderComp)
			{
				//BGM再生
				audio->playCue("爆発音1");
				explosion(i);
				//爆発範囲にコスト設定
				setExpRange(false, i);
			} 
		}
	}
}

//=============================================================================
//ボムの爆発処理
//引数：壁の配置情報、キャラクター番号
//戻り値：なし
//=============================================================================
void plusBom::explosion(int processCharaNum/*処理をするキャラクター番号*/)
{	
	int blockX;
	int blockZ;
	int add = putCharaNum[processCharaNum].dataiter->addRange;

	//爆発範囲分ループ
	for(int hight = -(RANGE + add); hight <= (RANGE + add); hight++)
	{	
		for(int width = -(RANGE + add); width <= (RANGE + add); width++) 
		{	
			//プラスボムの処理
			//渡されたキャラクター番号のリストが空でなければ
			if(!(putCharaNum[processCharaNum].dataList.empty()))
			{
				//渡されたキャラクター番号のボムの描画処理が完了していれば
				if(putCharaNum[processCharaNum].dataiter->isBomRenderComp)
				{
					//ボムの位置と判定分を加算
					blockX = (int)(putCharaNum[processCharaNum].dataiter->bomPos.x+width);
					blockZ = (int)(putCharaNum[processCharaNum].dataiter->bomPos.z+hight);
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
	
	//処理したボムをリストから削除
	//リストが空ではない
	if(!(putCharaNum[processCharaNum].dataList.empty()))
	{
		//渡されたキャラクター番号のボムの描画処理が完了していれば
		if(putCharaNum[processCharaNum].dataiter->isBomRenderComp)
		{
			//要素削除
			putCharaNum[processCharaNum].dataiter = putCharaNum[processCharaNum].dataList.erase(putCharaNum[processCharaNum].dataiter);
		}
	}
}

//=============================================================================
//ボムが存在するかを返す
//引数：ボムが存在するかを知りたい座標
//戻り値：存在するか
//=============================================================================
int plusBom::retIsBom(float x, float z)
{
	//プラスボムの位置
	//キャラクター分ループ
	for(int i = 0; i < 4; i++)
	{
		putCharaNum[i].compareitr = putCharaNum[i].dataList.begin();
		//処理しているボム分ループ
		while(putCharaNum[i].compareitr != putCharaNum[i].dataList.end())
		{
			//求める位置にボムが存在すれば
			if(toIntPos(putCharaNum[i].compareitr->bomPos.x) == toIntPos(x) && toIntPos(putCharaNum[i].compareitr->bomPos.z) == toIntPos(z))
			{
				return putCharaNum[i].dataiter->cost;
			}
			putCharaNum[i].compareitr++;
		}
	}

	return 0;
}

//=============================================================================
//爆発範囲のコストを設定
//引数：コストを設定するかどうか、キャラクター番号
//戻り値：なし
//=============================================================================
void plusBom::setExpRange(bool isSet, int chara)
{
	count++;
	putCharaNum[chara].compareitr = putCharaNum[chara].dataList.begin();
	putCharaNum[chara].dataiter = putCharaNum[chara].dataList.begin();

	//処理しているボム分ループ
	while(putCharaNum[chara].compareitr != putCharaNum[chara].dataList.end())
	{
		int add = putCharaNum[chara].dataiter->addRange;

		//爆発範囲分ループ
		for(int hight = -(RANGE + add); hight <= (RANGE + add); hight++)
		{	
			for(int width = -(RANGE + add); width <= (RANGE + add); width++) 
			{		
				int blockX = (int)(putCharaNum[chara].compareitr -> bomPos.x+width);
				int blockZ = (int)(putCharaNum[chara].compareitr -> bomPos.z+hight);
		
				if(blockX < MINRANGE || blockX > MAXRANGE
					|| blockZ < MINRANGE || blockZ > MAXRANGE)
					continue;

				if(isSet) {
						wallUnit -> wallData[blockX][blockZ].isExpRange = true;
						wallUnit -> wallData[blockX][blockZ].expRangeNum = count;
				} else {
					if(wallUnit -> wallData[blockX][blockZ].isExpRange) {
						wallUnit -> wallData[blockX][blockZ].isExpRange = false;
					}
				}
			}
		}

		putCharaNum[chara].compareitr++;
	}
}
