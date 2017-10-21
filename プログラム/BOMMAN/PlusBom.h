//=============================================================================
//  プラスボム
//　更新日：2016/10/30
//	制作者：佐々木隆貴
//=============================================================================
#pragma once
#include "normalBom.h"
#include <list>
using namespace std;

class plusBom : public normalBom
{
public:	
	struct plusBomData
	{
		D3DXVECTOR3 bomPos;						//ボムの位置
		int			bomRenderState;				//ボムの描画に関する状態を格納する
		bool		isBomRenderComp;			//描画が完了したか
		int			count;						//経過フレームのカウントを行う変数	
		int			cost;						//ボムのCPUコスト
		D3DXVECTOR3 throwPos;					//投げた先の位置ベクトル
		bool		isThrowFlag;				//ボムを置くか投げるか
		D3DXVECTOR3	speed;						//速度		
		list<plusBomData> dataList;					//ボムのデータを格納する配列
		list<plusBomData>::iterator	dataiter;		//イテレータ
		list<plusBomData>::iterator	compareitr;		//比較用イテレータ
		int			addRange;					//爆発範囲加算分
	};

	plusBomData		 putCharaNum[4];				//キャラクター数分のボム情報を格納するリスト

private:
	float theta;	//角度(仰角)

public:
	plusBom(void);
	void addBom(D3DXVECTOR3 pos/*プレイヤーの座標ベクトル*/, D3DXVECTOR3 move/*移動ベクトル*/, int addExpRabge/*爆発範囲加算分*/, int charaNum /*キャラクター名*/, D3DXVECTOR3 bomThrowPos/*投げる先の座標*/, bool throwFlag/*ボムを投げるか？*/);	 //プラスボムの情報追加
	int retIsBom(float x, float z);														//ボムが存在するかを返す

private:
	~plusBom(void);	
	void initialize(void);															
	void initializeMain(void);
	void render(float frameTime, int cameraNum/*カメラ番号*/);							//ボムの描画
	void explosionFlow();																//爆発処理の流れ数
	void explosion(int processCharaNum);												//爆発判定	
	void initList();																	//各リストの情報初期化
	void throwProcess(int putChara);													//投げる処理
	void initRender(int putChara);														//描画情報初期化
	void renderFlow();																	//描画の流れ
	void setExpRange(bool isSet, int chara);											//爆発範囲のコストを設定
	void addEffect(D3DXVECTOR3 pos, int effectType, int chara);							//エフェクトの追加
	void update();
	void collisions(UnitBase* target);													//ユニットの情報を取得
};

