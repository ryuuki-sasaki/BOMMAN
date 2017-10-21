#include "Camera.h"
#include "modeChange.h"

Camera::Camera(void)
{
}


Camera::~Camera(void)
{
}

//=============================================================================
// update処理
// 引数：なし
// 戻値：なし
//=============================================================================
void Camera::update()
{
	//ビュー行列
	D3DXMatrixLookAtLH(&view, &pos, &target, &D3DXVECTOR3(0,1,0));	
	//ビュー行列セット
	graphics->get3Ddevice()->SetTransform(D3DTS_VIEW, &view);		

	if(modeChange::GetInstance() -> getMode() == VS2P)
	{
		//射影
		D3DXMatrixPerspectiveFovLH(&proj, D3DXToRadian(45), (float)GAME_WIDTH/GAME_HEIGHT/2, 0.1f, 1000.0f);
		graphics->get3Ddevice()->SetTransform(D3DTS_PROJECTION, &proj);

		//ビューポートセット
		graphics->get3Ddevice()->SetViewport(&vp);
	} else {
		//射影
		D3DXMatrixPerspectiveFovLH(&proj, D3DXToRadian(45), (float)GAME_WIDTH/GAME_HEIGHT, 0.1f, 1000.0f);
		graphics->get3Ddevice()->SetTransform(D3DTS_PROJECTION, &proj);

		//ビューポートセット
		graphics->get3Ddevice()->SetViewport(&vp);
	}
}


//=============================================================================
// カメラの角度を返す（ビルボード）
// 引数：なし
// 戻値：D3DXMATRIX ビルボードの行列
//=============================================================================
D3DXMATRIX Camera::billboard()	//公式のようなもの
{
	D3DXMATRIX matrix;
	D3DXMatrixLookAtLH(&matrix, &D3DXVECTOR3(0,0,0),&(target - pos),&D3DXVECTOR3(0,1,0));

	D3DXMatrixInverse(&matrix, NULL, &matrix);
	return matrix;
}

//=============================================================================
//ビューポート（画面の表示範囲）を変更する
//引数：x,y	左上位置
//引数：w,h	幅と高さ
//=============================================================================
void Camera::changeViewport(int x, int y, int w, int h)
{
	vp.X = x;
	vp.Y = y;
	vp.Width = w;
	vp.Height = h;
	vp.MinZ = 0;
	vp.MaxZ = 1;
}

