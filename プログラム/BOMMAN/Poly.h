#pragma once
#include "constants.h"
#include "textureManager.h"

class Poly
{
	struct Vertex
	{
		D3DXVECTOR3	vPos;	//位置
		DWORD		dColor;	//色
		D3DXVECTOR2 vTex;   //テクスチャ座標
	};

	//三角形の頂点を入れる変数
	Vertex vPoint[4];
	Vertex vPoint1[4];
	Vertex vPoint2[4];
	Vertex vPoint3[4];
	Vertex vPoint4[4];
	//画像を描画するためのGraphics型のポインタ
	Graphics    *graphics;
	TextureManager *texManager;

public:
	Poly(void);
	~Poly(void);
	void initialize(Graphics *g, TextureManager *tm); //ポリゴンを用意する
	void boxInitialize(Graphics *g,TextureManager *tm);
	void draw(D3DXMATRIX* mWorld);
	void draw(D3DXMATRIX* mWorld, D3DXMATRIX* mTex);
	void boxDraw(D3DXMATRIX* mWorld);
};

