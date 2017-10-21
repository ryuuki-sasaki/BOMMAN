#include "Poly.h"

//=============================================================================
// コンストラクタ
//=============================================================================
Poly::Poly(void)
{
}

//=============================================================================
// デストラクタ
//=============================================================================
Poly::~Poly(void)
{
}

//=============================================================================
// 初期化
// 引数：なし
// 戻値：なし
//=============================================================================
void Poly::initialize(Graphics *g,TextureManager *tm)
{
	//3頂点指定（時計回り）(幅高さ1）
	//色（(16進数の場合）0x(透明度(アルファ値）2桁）,RGB(各2桁）、（10進数の場合）D3DCORLOR_ARGB）
	Vertex v[4]=
	{ D3DXVECTOR3(-0.5, 0.5, 0),  D3DCOLOR_ARGB(255, 255, 255, 255), D3DXVECTOR2(0, 0),
	  D3DXVECTOR3(0.5, 0.5, 0),	  D3DCOLOR_ARGB(255, 255, 255, 255), D3DXVECTOR2(1, 0),
	  D3DXVECTOR3(-0.5, -0.5, 0), D3DCOLOR_ARGB(255, 255, 255, 255), D3DXVECTOR2(0, 1),
	  D3DXVECTOR3(0.5, -0.5, 0),  D3DCOLOR_ARGB(255, 255, 255, 255), D3DXVECTOR2(1, 1)
	};
	memcpy(vPoint, v, sizeof(v));

	graphics = g;

	texManager = tm;
}

//=============================================================================
// BOXポリゴン用初期化
// 引数：なし
// 戻値：なし
//=============================================================================
void Poly::boxInitialize(Graphics *g,TextureManager *tm)
{
	//3頂点指定（時計回り）(幅高さ1）
	//色（(16進数の場合）0x(透明度(アルファ値）2桁）,RGB(各2桁）,（10進数の場合）D3DCORLOR_ARGB）
	//前面の矩形
	Vertex v1[4]=
	{ D3DXVECTOR3(0.5, -0.5, -0.5),  D3DCOLOR_ARGB(255, 255, 255, 255), D3DXVECTOR2(0, 0),
	  D3DXVECTOR3(-0.5, -0.5, -0.5),  D3DCOLOR_ARGB(255, 255, 255, 255), D3DXVECTOR2(1, 0),
	  D3DXVECTOR3(0.5, 0.5, -0.5), D3DCOLOR_ARGB(255, 255, 255, 255), D3DXVECTOR2(0, 1),
	  D3DXVECTOR3(-0.5, 0.5, -0.5),  D3DCOLOR_ARGB(255, 255, 255, 255), D3DXVECTOR2(1, 1)
	};
	//右側面の矩形
	Vertex v2[4]=
	{ D3DXVECTOR3(0.5, -0.5, 0.5),  D3DCOLOR_ARGB(255, 255, 255, 255), D3DXVECTOR2(0, 0),
	  D3DXVECTOR3(0.5, -0.5, -0.5),	  D3DCOLOR_ARGB(255, 255, 255, 255), D3DXVECTOR2(1, 0),
	  D3DXVECTOR3(0.5, 0.5, 0.5), D3DCOLOR_ARGB(255, 255, 255, 255), D3DXVECTOR2(0, 1),
	  D3DXVECTOR3(0.5, 0.5, -0.5),  D3DCOLOR_ARGB(255, 255, 255, 255), D3DXVECTOR2(1, 1)
	};
	//上面の矩形
	Vertex v3[4]=
	{ D3DXVECTOR3(0.5, 0.5, 0.5),  D3DCOLOR_ARGB(255, 255, 255, 255), D3DXVECTOR2(0, 0),
	  D3DXVECTOR3(-0.5, 0.5, 0.5),	  D3DCOLOR_ARGB(255, 255, 255, 255), D3DXVECTOR2(1, 0),
	  D3DXVECTOR3(0.5, 0.5, -0.5), D3DCOLOR_ARGB(255, 255, 255, 255), D3DXVECTOR2(0, 1),
	  D3DXVECTOR3(-0.5, 0.5, -0.5),  D3DCOLOR_ARGB(255, 255, 255, 255), D3DXVECTOR2(1, 1)
	};
	//左側面の矩形
	Vertex v4[4]=
	{ D3DXVECTOR3(-0.5, 0.5, 0.5),  D3DCOLOR_ARGB(255, 255, 255, 255), D3DXVECTOR2(0, 0),
	  D3DXVECTOR3(-0.5, 0.5, -0.5),	  D3DCOLOR_ARGB(255, 255, 255, 255), D3DXVECTOR2(1, 0),
	  D3DXVECTOR3(-0.5, -0.5, 0.5), D3DCOLOR_ARGB(255, 255, 255, 255), D3DXVECTOR2(0, 1),
	  D3DXVECTOR3(-0.5, -0.5, -0.5),  D3DCOLOR_ARGB(255, 255, 255, 255), D3DXVECTOR2(1, 1)
	};


	memcpy(vPoint1, v1, sizeof(v1));
	memcpy(vPoint2, v2, sizeof(v2));
	memcpy(vPoint3, v3, sizeof(v3));
	memcpy(vPoint4, v4, sizeof(v4));

	graphics = g;

	texManager = tm;
}

//=============================================================================
// 描画(変形がない場合）
// 引数：frameTime　1フレームの速度
// 戻値：なし
//=============================================================================
void Poly::draw(D3DXMATRIX* mWorld)
{
	graphics->get3Ddevice()->SetRenderState(D3DRS_LIGHTING, FALSE);

	graphics->get3Ddevice()->SetTransform(D3DTS_WORLD, mWorld);

	//テクスチャ指定
	graphics->get3Ddevice()->SetTexture(0,texManager->getTexture());
	//頂点座標、色情報を設定(今回はXYZ、DIFFUSE...拡散反射光)
	graphics->get3Ddevice()->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1);
	graphics->get3Ddevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, vPoint, sizeof(Vertex));

	graphics->get3Ddevice()->SetRenderState(D3DRS_LIGHTING, TRUE);

}

//=============================================================================
// 描画
// 引数：frameTime　1フレームの速度 mTex テクスチャ変形行列
// 戻値：なし
//=============================================================================
void Poly::draw( D3DXMATRIX* mWorld, D3DXMATRIX* mTex)
{
	//テクスチャに行列を指定できるようにする
	graphics->get3Ddevice()->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);

	//光の計算をしない
	graphics->get3Ddevice()->SetRenderState( D3DRS_LIGHTING, false );

	//テクスチャ指定
	graphics->get3Ddevice()->SetTexture( 0, texManager->getTexture() );

	//座標指定
	graphics->get3Ddevice()->SetTransform(D3DTS_WORLD, mWorld );	//行列を使う関数

	//行列指定
	graphics->get3Ddevice()->SetTransform(D3DTS_TEXTURE0, mTex );

	//頂点情報を登録（XYZ座標と色（拡散反射光））
	graphics->get3Ddevice()->SetFVF( D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1 );

	//描画
	graphics->get3Ddevice()->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, vPoint, sizeof( Vertex ) );

	//元に戻す
	graphics->get3Ddevice()->SetRenderState( D3DRS_LIGHTING, true );

	//テクスチャ行列指定を元に戻す
	graphics->get3Ddevice()->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);

}

//=============================================================================
//BOX描画
// 引数：frameTime　1フレームの速度
// 戻値：なし
//=============================================================================
void Poly::boxDraw(D3DXMATRIX* mWorld)
{
	graphics->get3Ddevice()->SetRenderState(D3DRS_LIGHTING, FALSE);

	graphics->get3Ddevice()->SetTransform(D3DTS_WORLD, mWorld);

	//テクスチャ指定
	graphics->get3Ddevice()->SetTexture(0,texManager->getTexture());
	//頂点座標、色情報を設定(今回はXYZ、DIFFUSE...拡散反射光)
	graphics->get3Ddevice()->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1);
	graphics->get3Ddevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, vPoint1, sizeof(Vertex));
	graphics->get3Ddevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, vPoint2, sizeof(Vertex));
	graphics->get3Ddevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, vPoint3, sizeof(Vertex));
	graphics->get3Ddevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, vPoint4, sizeof(Vertex));
	graphics->get3Ddevice()->SetRenderState(D3DRS_LIGHTING, TRUE);

}

