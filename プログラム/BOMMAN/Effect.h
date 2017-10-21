//=============================================================================
//  エフェクト
//　作成日：2015/8/12
//　更新日：2015/8/12
//	制作者：佐々木隆貴
//=============================================================================
#pragma once
#include "graphics.h"
#include "textureManager.h"
#include "Poly.h"
#include <list>

class effect
{
private:
	TextureManager	texture;
	Poly			poly;
	int				w, h;		//縦横のコマ数
	
	struct Data
	{
		D3DXVECTOR3		pos;			//描画位置
		float			aniFrame;		//今の表示コマ
		float			scale;			//画像ｻｲｽﾞ
	};
	std::list<Data> dataList;
	Data			data;

public:
	effect(void);
	~effect(void);
	void initialize();
	void Load(Graphics *g, char* fileName, int wide, int hight);	//読み込み
	void fixPosAdd(D3DXVECTOR3 pos, float scale);					//エフェクトを１つ追加
	void movePosAdd(D3DXVECTOR3 pos, float scale);
	void fixPosUpdate(float speed);
	void movePosUpdate(float speed, D3DXVECTOR3 pos);
	void fixPosDraw(int cNum);
	void movePosDraw(int cNum);
};

