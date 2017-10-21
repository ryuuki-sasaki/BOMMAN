//=============================================================================
//  �O���E���h
//�@�X�V���F2015/8/31
//	����ҁF���X�ؗ��M
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
	TextureManager texture;	//�e�N�X�`��(��j
	
	Image		   image;		//�C���[�W

	D3DXVECTOR3 pos;

public:
	Ground(void);
	~Ground(void);
	void render(float frameTime, int cameraNum);
};

