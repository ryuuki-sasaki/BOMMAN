//=============================================================================
//  ボム
//　更新日：2016/10/30
//	制作者：佐々木隆貴
//=============================================================================
#pragma once
#include "unitbase.h"

class Bom : public UnitBase 
{
public:
	virtual void addBom(D3DXVECTOR3 pos/*プレイヤーの座標ベクトル*/, D3DXVECTOR3 move/*移動ベクトル*/,int addExpRabge/*爆発範囲加算分*/, int charaNum /*キャラクター名*/, D3DXVECTOR3 bomThrowPos/*投げる先の座標*/, bool throwFlag/*ボムを投げるか？*/){;}	 //ボムの描画

protected:
	virtual ~Bom(void) = 0 {;}
	virtual void initialize(void) = 0 {;}
	virtual void initializeMain(void) = 0 {;}
	virtual void render(float frameTime, int cameraNum/*カメラ番号*/){;}	//ボムの描画
	virtual void explosionFlow(){;}											//爆発処理の流れ
	virtual void explosion(int processCharaNum){;}							//爆発判定		
	virtual void initList(){;}												//各リストの情報初期化
	virtual void throwProcess(int putChara){;}								//投げる処理
	virtual void initRender(int putChara){;}								//描画情報初期化
	virtual void addEffect(D3DXVECTOR3 pos, int effectType, int chara){;}	//エフェクトの追加
	virtual void renderFlow(){;}											//描画の流れ
	virtual void setExpRange(bool isSet, int chara){;}						//爆発範囲のコストを設定
};

