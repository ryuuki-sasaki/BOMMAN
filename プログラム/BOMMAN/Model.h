//───────────────────────────────────────
// file : Model.h
// brief : ３Ｄオブジェクト制御ヘッダファイル
//───────────────────────────────────────

//───────────────────────────────────────
// インクルードガードの開始宣言
//───────────────────────────────────────
#ifndef _MODEL_H_
#define _MODEL_H_

//───────────────────────────────────────
// 必要なヘッダファイルのインクルード
//───────────────────────────────────────
#include "MeshFBX.h"        // MeshFBX クラスの参照


//──────────────────────
//必要なライブラリファイルのロード
//──────────────────────
#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")
#pragma comment(lib,"dxguid.lib")

#if defined(DEBUG) | defined(_DEBUG)
#pragma comment(lib,"fbxsdk-2013.3-mtd.lib")
#else
#pragma comment(lib,"fbxsdk-2013.3-mt.lib")
#endif


//───────────────────────────────────────
// Modelクラス：モデルデータの制御を管理
//───────────────────────────────────────
class   Model : public MeshFBX
{
	char	m_filename[MAX_PATH];	// モデルファイル名
    float   m_Frame;				// アニメーションのフレーム数

	//private関数
    BOOL	InitPolygon();				// ポリゴンの初期化
    BOOL	DrawSub( MeshFBX * pMesh );	// 階層ごとのポリゴンの描画

public:
    Model();    // コンストラクタ
    ~Model();   // デストラクタ
	BOOL	Load(LPDIRECT3DDEVICE9 pDev, char* n);	//ファイル読み込み
	BOOL	Update( float animSpeed );				//アニメーションのフレーム更新
	BOOL	Draw(D3DXMATRIX* matWorld = NULL);				//描画(= NULLは引数なしのときデフォルトでNULL)

	
	BOOL	HitRay  (
					D3DXVECTOR3* pRayPos,
					D3DXVECTOR3* pRayDir,
					float* pDist);
	
	BOOL	HitRay  (
					D3DXVECTOR3* pRayPos,
					D3DXVECTOR3* pRayDir,
					float* pDist,
					D3DXVECTOR3* pNormal);

	//BOOL	HitRay (
	//			    D3DXVECTOR3* pRayPos,
	//				D3DXVECTOR3* pRayDir,
	//				D3DXVECTOR3* pNormal);

private:
	BOOL	HitRay  (MeshFBX* pMesh,
					D3DXVECTOR3* pRayPos,
					D3DXVECTOR3* pRayDir,
					float* pDist,
					D3DXVECTOR3* pNormal);
};

//───────────────────────────────────────
// インクルードガードの終了宣言
//───────────────────────────────────────
#endif  _MODEL_H_
 