//=============================================================================
//  通常ボム
//　更新日：2016/10/30
//	制作者：佐々木隆貴
//=============================================================================
#pragma once
#include "Bom.h"
#include "constants.h"
#include <list>
using namespace std;

class vsCpuPlayer;
class VsPlayer;
class NormalCpu;
class Wall;
class Audio;
class Model;
class effect;

class normalBom : public Bom
{	
public:
	struct bomData
	{
		D3DXVECTOR3 bomPos;						//ボムを配置した位置
		int			bomRenderState;				//ボムの描画に関する状態を格納する
		int			count;						//経過フレームのカウントを行う変数	
		int			cost;						//CPUのコスト
		D3DXVECTOR3 throwPos;					//投げた先の位置ベクトル
		bool		isThrowFlag;				//ボムを置くか投げるか
		D3DXVECTOR3	speed;						//速度
		float		theta;						//角度(仰角)
		int			addRange;					//爆発範囲加算分
	};
	bomData		 putCharaNum[4];	//キャラクター数分のボム情報を格納するリスト

protected:	
	Model*       model;		//モデルクラスのオブジェクト		
	Audio*		 audio;		//オーディオ
	list<int>	 explosionProcessList;	//爆発処理をするキャラクターのリスト
	int			 count;
	//エフェクト
	effect	*fire;	//火花
	effect	*exp;	//火花
	int		bomType;					//ボムの種類
	Wall*	wallUnit;					//ウォールユニットの情報を格納する変数
	vsCpuPlayer*	vsCpuPlayerUnit;	//対CPUプレイヤーユニットの情報を格納する変数
	NormalCpu*		cpuUnit[3];			//CPUユニットの情報を格納する変数
	VsPlayer		*player1, *player2;	//対人プレイヤーユニットの情報を格納する変数

public:
	normalBom(void);
	void addBom(D3DXVECTOR3 pos/*プレイヤーの座標ベクトル*/, D3DXVECTOR3 move/*移動ベクトル*/,int addExpRabge/*爆発範囲加算分*/, int charaNum /*キャラクター名*/, D3DXVECTOR3 bomThrowPos/*投げる先の座標*/, bool throwFlag/*ボムを投げるか？*/);	 //ボムの描画
	int retIsBom(float x, float z);		//ボムが存在するかを返す

protected:
	~normalBom(void);
	void initialize(void);
	void initializeMain(void);
	void update();
	void collisions(UnitBase* target);											//ユニットの情報を取得
	void render(float frameTime, int cameraNum/*カメラ番号*/);					//ボムの描画
	void explosionFlow();														//爆発処理の流れ
	void explosion(int processCharaNum);										//爆発判定		
	Model* GetModel(){return model;}											//ボムのモデル情報を返すアクセス関数
	void characterInRange(int x, int z);										//キャラクターが範囲内か
	void initList();															//各リストの情報初期化
	void throwProcess(int putChara);											//投げる処理
	void initRender(int putChara);												//描画情報初期化
	void addEffect(D3DXVECTOR3 pos, int effectType, int chara);					//エフェクトの追加
	void renderFlow();															//描画の流れ
	float toIntMove(float move) {return (move<0) ? (float)(-1.0 * floor(fabs(move) + 0.5)) : (float)(floor(move + 0.5));}	//Moveの長さを整数にする
	float toIntPos(float pos) {return (pos<0) ? (float)(-1.0 * floor(fabs(pos) + 0.5)) : (float)(floor(pos + 0.5));}		//位置を整数にする
	float getAngle(D3DXVECTOR3 pos, float speed);								//発射角度を返す
	void setExpRange(bool isSet, int chara);									//爆発範囲のコストを設定
	void callCameraShake(D3DXVECTOR3 pos);										//カメラを揺らす処理を呼ぶ
};

