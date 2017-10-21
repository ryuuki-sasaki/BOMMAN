//=============================================================================
//  痺れボム
//　更新日：2016/10/30
//	制作者：佐々木隆貴
//=============================================================================
#include "paralyzeBom.h"
#include "VsPlayer.h"
#include "vsCpuPlayer.h"
#include "PsyCpu.h"
#include "SpeedCpu.h"
#include "NormalCpu.h"
#include "modeChange.h"
#include "Camera.h"
#include "audio.h"
#include "Wall.h"
#include "effect.h"
#include "Model.h"
#include "VsPlayer.h"

#define RENDERTIME 90	//描画時間
#define NOTWALL 0		//壁なし
#define CHARANUM 3		//キャラクター数
#define CAMERA1 1		//カメラ番号
#define SCALE 13		//大きさ
#define RANGE 5			//範囲
#define OBSTACLELCOST 100		//壁等の障害物のCPUコスト
#define NONCOST 0				//コストなし
#define ITEMCOST -3				//アイテムのコスト
#define MAXRANGE 49				//最大範囲
#define MINRANGE 1				//最低範囲
#define UPDATEANIM 1.0f			//アニメーション更新

//ボムの描画に関する状態
enum BOM_RENDER_STATE
{
	RENDER,			//描画処理
	ACTIONEXPSlON,	//爆発処理実行中
	DONAOTHING		//なんでもない
};

//ボムの種類
enum BOM_NAME
{
	NORMAL,
	PLUS,
	PARALYZE
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
paralyzeBom::paralyzeBom(void)
{	
	//モデル読み込み
	model = new Model;
	model->Load(graphics->get3Ddevice(),"Models\\pBom.fbx");

	//オーディオ
	audio = new Audio;

	//オーディオ読み込み
	audio->initialize("Sounds\\Wave Bank.xwb", "Sounds\\Sound Bank.xsb");

	initializeMain();
}

//=============================================================================
//初期化
//=============================================================================
void paralyzeBom::initializeMain(void)
{
	//エフェクト読み込み
	fire = new effect;
	fire->Load(graphics, "pictures\\hibana.png", 3, 3);

	exp = new effect;
	exp->Load(graphics, "pictures\\paralyzeEffect.png", 6, 4);

	//ボムの状態初期化
	for(int i = 0; i <= CHARANUM; i++)
	{
		putCharaNum[i].bomRenderState = DONAOTHING;
	}

	initList();

	count = 0;
	bomType = BOM_NAME::PARALYZE;

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
void paralyzeBom::initialize(void)
{
	SAFE_DELETE(fire);
	SAFE_DELETE(exp);
	initializeMain();
}

//=============================================================================
//デストラクタ
//=============================================================================
paralyzeBom::~paralyzeBom(void)
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
void paralyzeBom::collisions(UnitBase* target)
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
void paralyzeBom::update()
{	
	bomType = BOM_NAME::PARALYZE;
	//エフェクト更新
	fire->fixPosUpdate(UPDATEANIM);
	exp->fixPosUpdate(UPDATEANIM);
	//ボムの処理の進める
	renderFlow();
	explosionFlow();
}

//=============================================================================
//爆発処理の流れ
//引数：なし
//戻り値：なし
//=============================================================================
void paralyzeBom::explosionFlow()
{
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
void paralyzeBom::explosion(int processCharaNum/*処理をするキャラクター番号*/)
{	
	int blockX;
	int blockZ;

	//爆発範囲分ループ
	for(int hight = -RANGE; hight <= RANGE; hight++)
	{	
		for(int width = -RANGE; width <= RANGE; width++) 
		{
			//渡されたキャラクター番号のボムの描画処理が完了していれば
			if(putCharaNum[processCharaNum].bomRenderState == ACTIONEXPSlON) {
				//ボムの位置と判定分を加算
				blockX = (int)(putCharaNum[processCharaNum].bomPos.x+width);
				blockZ = (int)(putCharaNum[processCharaNum].bomPos.z+hight);
			}

			if(blockX < MINRANGE || blockX > MAXRANGE
				|| blockZ < MINRANGE || blockZ > MAXRANGE)
				continue;

			//各キャラクターと位置が同じなら
			//CPUの対戦
			if(modeChange::GetInstance() -> getMode() == VSCPU) {
				if(blockX == vsCpuPlayerUnit ->pos.x && blockZ == vsCpuPlayerUnit ->pos.z) {
					vsCpuPlayerUnit ->isStop = true;	//行動を停止するフラグをtrue
				}
			
				if(blockX == cpuUnit[SPEEDCPU] -> pos.x && blockZ == cpuUnit[SPEEDCPU] -> pos.z) {
					cpuUnit[SPEEDCPU]->isStop = true;
				} 
			
				if(toIntPos(blockX == cpuUnit[NORMALCPU] -> pos.x) && blockZ == toIntPos(cpuUnit[NORMALCPU] -> pos.z)) {
					cpuUnit[NORMALCPU]->isStop = true;
				} 
			
				if(toIntPos(blockX == cpuUnit[PYECPU] -> pos.x) && blockZ == toIntPos(cpuUnit[PYECPU] -> pos.z)) {
					cpuUnit[PYECPU]->isStop = true;
				}
			}

			//2P対戦
			if(modeChange::GetInstance() -> getMode() == VS2P)
			{
				if(blockX == player1 -> pos.x && blockZ == player1 -> pos.z) {
					player1->isStop = true;
				}

				if(blockX == player2 -> pos.x && blockZ == player2 -> pos.z) {
					player2->isStop = true;
				}
			}
		}
	}	
}

//=============================================================================
//ボムが存在するかを返す
//引数：ボムが存在するかを知りたい座標
//戻り値：なし
//=============================================================================
int paralyzeBom::retIsBom(float x, float z)
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
void paralyzeBom::setExpRange(bool isSet, int chara)
{
	count++;
	//爆発範囲分ループ
	for(int hight = -RANGE; hight <= RANGE; hight++)
	{	
		for(int width = -RANGE; width <= RANGE; width++) 
		{		
			int blockX = (int)(putCharaNum[chara].bomPos.x+width);
			int blockZ = (int)(putCharaNum[chara].bomPos.z+hight);

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
}