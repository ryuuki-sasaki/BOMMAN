#pragma once
#include "graphics.h"

class Camera
{
public:
	D3DXVECTOR3 pos;		//�J�����̈ʒu

private:
	D3DXVECTOR3 target;		//�œ_�ʒu
	D3DXMATRIX  view;		//�r���[�s��
	D3DXMATRIX	proj;		//�ˉe�s��
	D3DVIEWPORT9 vp;		//�r���[�|�[�g

public:
	Camera(void);
	~Camera(void);
	void setPos(D3DXVECTOR3 v){pos = v;}
	void setTarget(D3DXVECTOR3 v){target = v;}
	D3DXVECTOR3	GetPos(){return pos;}
	D3DXMATRIX billboard();								//��ɂ������������|���S��
	void update();	
	
	D3DXMATRIX GetView(){return view;}
	D3DXMATRIX GetProj(){return proj;}

	void changeViewport(int x, int y, int w, int h);	//�r���[�|�[�g��ύX
};

