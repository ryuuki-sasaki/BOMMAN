//=============================================================================
//  痺れボム
//　更新日：2016/10/30
//	制作者：佐々木隆貴
//=============================================================================
#pragma once
#include "normalBom.h"
#include <list>
using namespace std;

class paralyzeBom : public normalBom
{
public:
	paralyzeBom(void);
	int retIsBom(float x, float z);				//ボムが存在するかを返す

private:
	~paralyzeBom(void);
	void explosion(int processCharaNum);		//爆発判定
	void explosionFlow();						//爆発処理の流れ数
	void setExpRange(bool isSet, int chara);	//爆発範囲のコストを設定
	void update();
	void initialize(void);				
	void initializeMain(void);
	void collisions(UnitBase* target);	//ユニットの情報を取得
};


