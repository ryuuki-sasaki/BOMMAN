//=============================================================================
//  エフェクト
//　作成日：2015/8/12
//　更新日：2015/8/12
//	制作者：佐々木隆貴
//=============================================================================
#include "effect.h"
#include "Camera.h"

effect::effect(void)
{
}


effect::~effect(void)
{
}

//=============================================================================
// 読み込み
// 引数：グラフィックスポインタ、ファイル名、幅、高さ、大きさ
// 戻値：なし
//=============================================================================
void effect::Load(Graphics *g, char* fileName, int wide, int hight)
{
	texture.initialize(g, fileName);
	poly.initialize(g, &texture);
	this->w = wide;
	this->h = hight;
}


//=============================================================================
// エフェクト追加(位置固定)
// 引数：位置、スケール、描画時間
// 戻値：なし
//=============================================================================
void effect::fixPosAdd(D3DXVECTOR3 pos, float scale)
{
	Data d;
	d.pos = pos;
	d.aniFrame = 0;
	d.scale = scale;
	dataList.push_back(d);
}

//=============================================================================
// エフェクト追加(位置変更)
// 引数：位置、スケール、描画時間
// 戻値：なし
//=============================================================================
void effect::movePosAdd(D3DXVECTOR3 pos, float scale)
{
	data.pos = pos;
	data.aniFrame = 0;
	data.scale = scale;
}

//=============================================================================
// 更新処理(位置固定)
// 引数：アニメーション更新速度
// 戻値：なし
//=============================================================================
void effect::fixPosUpdate(float speed)
{
	std::list<Data>::iterator itr;
	itr = dataList.begin();
	while(itr != dataList.end())
	{
		itr->aniFrame += speed;
		if(itr->aniFrame >= w*h)
		{
			itr = dataList.erase(itr);
			continue;
		}

		itr++;
	}
}

//=============================================================================
// 更新処理(位置変更)
// 引数：アニメーション更新速度、現在位置
// 戻値：なし
//=============================================================================
void effect::movePosUpdate(float speed, D3DXVECTOR3 pos)
{
	data.pos = pos;
	data.aniFrame += speed;
	if(data.aniFrame >= w*h)
	{
		data.aniFrame = 0;
	}
}

//=============================================================================
// 描画(位置固定)
// 引数：カメラ番号
// 戻値：なし
//=============================================================================
void effect::fixPosDraw(int cNum)
{
	std::list<Data>::iterator itr;	//イテレータ作成
	itr = dataList.begin();
	while(itr != dataList.end())	//イテレータが終わりを指すまで
	{
		D3DXMATRIX mTrans, mScale;
		D3DXMatrixTranslation(&mTrans, itr->pos.x, itr->pos.y, itr->pos.z);
		D3DXMatrixScaling(&mScale, itr->scale, itr->scale, itr->scale);

		//テクスチャの切り抜き
		D3DXMATRIX mTeXScale, mTexTrans;
		D3DXMatrixScaling(&mTeXScale, 1.0f/w, 1.0f/h, 1);
		matrixTrans2D(&mTexTrans, (float)((int)itr->aniFrame%w)/w, (float)((int)itr->aniFrame/w)/h);	//aniFrameを元に切り抜く位置を決める
		if(cNum == 1)
			poly.draw(&(mScale * cameraA->billboard() * mTrans), &(mTeXScale*mTexTrans));
		else
			poly.draw(&(mScale * cameraB->billboard() * mTrans), &(mTeXScale*mTexTrans));
		itr++;
	}
}

//=============================================================================
// 描画(位置変更)
// 引数：カメラ番号
// 戻値：なし
//=============================================================================
void effect::movePosDraw(int cNum)
{
	D3DXMATRIX mTrans, mScale;
	D3DXMatrixTranslation(&mTrans, data.pos.x, data.pos.y, data.pos.z);
	D3DXMatrixScaling(&mScale, data.scale, data.scale, data.scale);

	//テクスチャの切り抜き
	D3DXMATRIX mTeXScale, mTexTrans;
	D3DXMatrixScaling(&mTeXScale, 1.0f/w, 1.0f/h, 1);
	matrixTrans2D(&mTexTrans, (float)((int)data.aniFrame%w)/w, (float)((int)data.aniFrame/w)/h);	//aniFrameを元に切り抜く位置を決める
	if(cNum == 1)
		poly.draw(&(mScale * cameraA->billboard() * mTrans), &(mTeXScale*mTexTrans));
	else
		poly.draw(&(mScale * cameraB->billboard() * mTrans), &(mTeXScale*mTexTrans));
}