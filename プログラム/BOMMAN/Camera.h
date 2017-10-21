#pragma once
#include "graphics.h"

class Camera
{
public:
	D3DXVECTOR3 pos;		//カメラの位置

private:
	D3DXVECTOR3 target;		//焦点位置
	D3DXMATRIX  view;		//ビュー行列
	D3DXMATRIX	proj;		//射影行列
	D3DVIEWPORT9 vp;		//ビューポート

public:
	Camera(void);
	~Camera(void);
	void setPos(D3DXVECTOR3 v){pos = v;}
	void setTarget(D3DXVECTOR3 v){target = v;}
	D3DXVECTOR3	GetPos(){return pos;}
	D3DXMATRIX billboard();								//常にこっちを向くポリゴン
	void update();	
	
	D3DXMATRIX GetView(){return view;}
	D3DXMATRIX GetProj(){return proj;}

	void changeViewport(int x, int y, int w, int h);	//ビューポートを変更
};

