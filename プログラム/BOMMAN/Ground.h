//=============================================================================
//  グラウンド
//　更新日：2015/8/31
//	制作者：佐々木隆貴
//=============================================================================
#pragma once
#include "unitbase.h"
#include "constants.h"
#include "textureManager.h"
#include "image.h"

class Poly;
class Model;

class Ground : public UnitBase
{
	Model* board; 
	Model* table;	
	Model* tableFrame;
	Model* floor;
	Model* backGround;
	TextureManager texture;	//テクスチャ(空）
	
	Image		   image;		//イメージ

	D3DXVECTOR3 pos;

public:
	Ground(void);
	~Ground(void);
	void render(float frameTime, int cameraNum);
};

