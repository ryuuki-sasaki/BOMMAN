//=============================================================================
//  グラウンド
//　更新日：2015/8/31
//	制作者：佐々木隆貴
//=============================================================================
#include "Ground.h"
#include "Camera.h"
#include "modeChange.h"
#include "Poly.h"
#include "Model.h"

LPD3DXEFFECT	 pEffect;	//シェーダを入れるための変数

//=============================================================================
//コンストラクタ
//=============================================================================
Ground::Ground(void)
{
	board = new Model;
	table = new Model;
	tableFrame = new Model;
	floor = new Model;
	backGround = new Model;
	//モデル読み込み
	board->Load(graphics->get3Ddevice(),"Models\\board.fbx");
	table->Load(graphics->get3Ddevice(),"Models\\table.fbx");
	tableFrame->Load(graphics->get3Ddevice(),"Models\\table waku.fbx");
	floor->Load(graphics->get3Ddevice(),"Models\\floor.fbx");
	backGround->Load(graphics->get3Ddevice(),"Models\\backGround.fbx");
	//位置
	pos = D3DXVECTOR3(25,-100,25);

	//テクスチャ読み込み
	texture.initialize(graphics,"pictures\\backGround.png");
	image.initialize(graphics,1000,1000,0,&texture);
	//imageヘッダのアクセス関数にtrueを渡す
	image.SetIsBackGround(true);

	//エフェクト読み込み...エフェクトファイルを読み込んで変数に入れる
	LPD3DXBUFFER err = 0;

	//最後の引数にエラー情報が入る
	//FAILED...失敗したら
	if(FAILED (D3DXCreateEffectFromFile(
		graphics -> get3Ddevice(),
		"Shader.fx",
		NULL,NULL,
		D3DXSHADER_DEBUG,
		NULL,
		&pEffect,
		&err) ) )
	{
		//エラーのときに出すメッセージ
		char mes[1024];
		strcpy_s( mes, (char*)err -> GetBufferPointer() );

		//メッセージボックスを出す (MB...メッセージボックス）
		MessageBox( NULL, mes, "シェーダーエラー", MB_OK );
	}
}

//=============================================================================
//デストラクタ
//=============================================================================
Ground::~Ground(void)
{
	SAFE_DELETE(board);
	SAFE_DELETE(table);
	SAFE_DELETE(floor);
	SAFE_DELETE(backGround);
	SAFE_DELETE(tableFrame);
}

//=============================================================================
//描画
//引数　フレーム、カメラ番号
//戻り値　なし
//=============================================================================
void Ground::render(float frameTime, int cameraNum)
{
	//行列
	D3DXMATRIX mWorld, mFroorTrans, mBackTrans;
	D3DXMatrixIdentity(&mWorld);
	//移動行列
	D3DXMatrixTranslation(&mFroorTrans,pos.x, pos.y, pos.z);
	D3DXMatrixTranslation(&mBackTrans,pos.x, 0, pos.z);
	//モデル描画
	board->Draw(&mWorld);
	tableFrame->Draw(&mWorld);
	backGround->Draw(&mBackTrans);
	floor->Draw(&mFroorTrans);
	
	//テクスチャ描画
	image.draw();

	D3DXMATRIX mat;
	//移動行列
	D3DXMatrixTranslation(&mat,0, -0.5f, 0);

	if(cameraNum == 1 || modeChange::GetInstance() -> getMode() == VSCPU/*対戦モードがCPU対戦*/) {
		//ワールド、ビュー、射影行列
		//Test.fxのグローバル変数に値を渡す
		//行列をシェーダに渡す...SetMatrix
		pEffect -> SetMatrix("g_matWVP", 
			&( mat* cameraA -> GetView() * cameraA -> GetProj() ) );

		//カメラの位置
		pEffect->SetVector("g_vecEye", (D3DXVECTOR4*)&cameraA->GetPos());
	} else {
		pEffect -> SetMatrix("g_matWVP", 
			&( mat* cameraB -> GetView() * cameraB -> GetProj() ) );

		pEffect->SetVector("g_vecEye", (D3DXVECTOR4*)&cameraB->GetPos());
	}


	//ワールドの逆行列
	D3DXMATRIX matInvW;
	D3DXMatrixInverse(&matInvW, 0, &mat); //matの逆行列がmatInvWに入る
	D3DXMatrixTranspose(&matInvW, &matInvW); //行と列をひっくり返す
	pEffect->SetMatrix("g_matInvW",&matInvW);

	//ライトの方向（物体から見て)
	D3DXVECTOR4 light(1, 1, 1, 1);	//右上手前から
	D3DXVec4Normalize(&light, &light); //正規化

	pEffect->SetVector("g_vecLightDir",&light);

	//描画開始
	pEffect -> Begin(NULL, 0);
	//パス開始
	pEffect -> BeginPass(0);
	//描画
	table->Draw(&mat);
	//パス終了
	pEffect -> EndPass();
	//描画終了
	pEffect -> End();
}