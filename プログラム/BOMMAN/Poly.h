#pragma once
#include "constants.h"
#include "textureManager.h"

class Poly
{
	struct Vertex
	{
		D3DXVECTOR3	vPos;	//�ʒu
		DWORD		dColor;	//�F
		D3DXVECTOR2 vTex;   //�e�N�X�`�����W
	};

	//�O�p�`�̒��_������ϐ�
	Vertex vPoint[4];
	Vertex vPoint1[4];
	Vertex vPoint2[4];
	Vertex vPoint3[4];
	Vertex vPoint4[4];
	//�摜��`�悷�邽�߂�Graphics�^�̃|�C���^
	Graphics    *graphics;
	TextureManager *texManager;

public:
	Poly(void);
	~Poly(void);
	void initialize(Graphics *g, TextureManager *tm); //�|���S����p�ӂ���
	void boxInitialize(Graphics *g,TextureManager *tm);
	void draw(D3DXMATRIX* mWorld);
	void draw(D3DXMATRIX* mWorld, D3DXMATRIX* mTex);
	void boxDraw(D3DXMATRIX* mWorld);
};

