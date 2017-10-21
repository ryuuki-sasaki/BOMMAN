//=============================================================================
//  �G�t�F�N�g
//�@�쐬���F2015/8/12
//�@�X�V���F2015/8/12
//	����ҁF���X�ؗ��M
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
	int				w, h;		//�c���̃R�}��
	
	struct Data
	{
		D3DXVECTOR3		pos;			//�`��ʒu
		float			aniFrame;		//���̕\���R�}
		float			scale;			//�摜����
	};
	std::list<Data> dataList;
	Data			data;

public:
	effect(void);
	~effect(void);
	void initialize();
	void Load(Graphics *g, char* fileName, int wide, int hight);	//�ǂݍ���
	void fixPosAdd(D3DXVECTOR3 pos, float scale);					//�G�t�F�N�g���P�ǉ�
	void movePosAdd(D3DXVECTOR3 pos, float scale);
	void fixPosUpdate(float speed);
	void movePosUpdate(float speed, D3DXVECTOR3 pos);
	void fixPosDraw(int cNum);
	void movePosDraw(int cNum);
};

