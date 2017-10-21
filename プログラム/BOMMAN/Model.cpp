
//=============================================================================
// file : Model.cpp
// brief : ３Ｄオブジェクト制御ソースファイル
//=============================================================================

//=============================================================================
// 必要なヘッダファイルのインクルード
//=============================================================================
#include "Model.h"          // Model クラス


//=============================================================================
// Model::Model
// brief : コンストラクタ
//=============================================================================
Model::Model()
{
	// 変数の初期化
	ZeroMemory(this, sizeof(Model));
}


//=============================================================================
// Model::~Model
// brief : デストラクタ
//=============================================================================
Model::~Model()
{

}

//=============================================================================
// Model::Load
// brief  : モデル読み込み
// param  : [in] pDev - デバイスインターフェース
// param  : [in] fname - ファイル名
// return : 正常動作なら TRUE 、異常終了なら FALSE を返す
//=============================================================================
BOOL Model::Load(LPDIRECT3DDEVICE9 pDev, char* fname)
{
	// メンバ変数へのコピー
	strcpy_s(m_filename, fname);
	m_pDevice = pDev;

	// ポリゴンの生成処理
	if (!InitPolygon())
	{
		return E_FAIL;
	}

	return S_OK;
}

//=============================================================================
// Model::InitPolygon
// brief  : ポリゴンの生成処理
// return : 正常動作なら TRUE 、異常終了なら FALSE を返す
//=============================================================================
BOOL    Model::InitPolygon()
{
	// メッシュの初期化
	if (!Load_(m_filename))
	{
		return  FALSE;
	}

	return TRUE;
}

//=============================================================================
// Model::Draw
// brief  : ポリゴンの描画処理
// param  : [in] matWorld - ワールド行列
// return : 正常動作なら TRUE 、異常終了なら FALSE を返す
//=============================================================================
BOOL    Model::Draw(D3DXMATRIX* matWorld)
{
	//引数省略時は単位行列
	if (matWorld == NULL)
	{
		D3DXMatrixIdentity(matWorld);
	}

	m_pDevice->SetTransform(D3DTS_WORLD, matWorld);
	return  DrawSub(this);
}



//=============================================================================
// Model::DrawSub
// brief  : ポリゴンの描画処理
// param  : [in] pMesh - 可動部位ごとのメッシュデータ
// return : 正常動作なら TRUE 、異常終了なら FALSE を返す
//=============================================================================
BOOL    Model::DrawSub(MeshFBX *pMesh)
{
	// アニメーションのフレーム更新設定
	pMesh->Update(m_Frame);
	
	//頂点バッファ
	if (pMesh->GetVertexBuffer())
	{
		//頂点のメモリを直でコピーする
		VERTEX *vTop;
		pMesh->GetVertexBuffer()->Lock(0, 0, (void**)&vTop, 0);
		memcpy(vTop, pMesh->GetVertex(), sizeof(VERTEX)* pMesh->GetNumVertexNum());
		pMesh->GetVertexBuffer()->Unlock();

		for (DWORD i = 0; i < pMesh->GetNumMaterials(); i++)
		{
			//頂点インデックス
			if (pMesh->GetIndexBuffer() && pMesh->GetIndexBuffer()[i])
			{

				DWORD *vIndex;
				pMesh->GetIndexBuffer()[i]->Lock(0, 0, (void**)&vIndex, 0);
				DWORD size;
				MATERIAL *m = pMesh->GetMaterialBuffer();
				size = m[i].dwNumFace;
				memcpy(vIndex, pMesh->GetIndex()[i], sizeof(int)*size * 3);
				pMesh->GetIndexBuffer()[i]->Unlock();

				m_pDevice->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1);


				//頂点インデックス指定
				m_pDevice->SetIndices(pMesh->GetIndexBuffer()[i]);

				//使用ストリーム指定と頂点バッファ設定
				m_pDevice->SetStreamSource(0, pMesh->GetVertexBuffer(), 0, sizeof(VERTEX));


				//マテリアル
				D3DMATERIAL9 material;
				ZeroMemory(&material, sizeof(material));
				material.Diffuse.r = pMesh->GetMaterialBuffer()[i].vDiffuse.x;
				material.Diffuse.g = pMesh->GetMaterialBuffer()[i].vDiffuse.y;
				material.Diffuse.b = pMesh->GetMaterialBuffer()[i].vDiffuse.z;
				material.Diffuse.a = pMesh->GetMaterialBuffer()[i].vDiffuse.w;

				material.Ambient.r = pMesh->GetMaterialBuffer()[i].vAmbient.x;
				material.Ambient.g = pMesh->GetMaterialBuffer()[i].vAmbient.y;
				material.Ambient.b = pMesh->GetMaterialBuffer()[i].vAmbient.z;
				material.Ambient.a = pMesh->GetMaterialBuffer()[i].vAmbient.w;

				material.Specular.r = pMesh->GetMaterialBuffer()[i].vSpecular.x;
				material.Specular.g = pMesh->GetMaterialBuffer()[i].vSpecular.y;
				material.Specular.b = pMesh->GetMaterialBuffer()[i].vSpecular.z;
				material.Specular.a = pMesh->GetMaterialBuffer()[i].vSpecular.w;

				m_pDevice->SetMaterial(&material);

				m_pDevice->SetTexture(0, pMesh->GetMaterialBuffer()[i].pTexture);



				//描画(面二つと０から始まる頂点と指定)
				m_pDevice->DrawIndexedPrimitive(
					D3DPT_TRIANGLELIST,
					0,	//開始地点からの相対位置
					0,	//最小インデックス番号
					pMesh->GetNumVertexNum() * 3,	//最大インデックス番号
					0,	//インデックス配列の読み取り位置	
					pMesh->GetMaterialBuffer()[i].dwNumFace	//面の数
					);
			}
		}
	}

	// 子階層オブジェクトに対しても描画処理を実行
	for (DWORD i = 0; i < pMesh->GetNumChild(); i++)
	{
		DrawSub(pMesh->GetChild()[i]);
	}


	return  TRUE;
}

//=============================================================================
// Model::Update
// brief  : ポリゴンのフレーム更新
// param  : [in] animSpeed - アニメーションの再生速度
// return : 正常動作なら TRUE 、異常終了なら FALSE を返す
//=============================================================================
BOOL    Model::Update(float animSpeed)
{
	// アニメーションを指定ステップ数だけ進める
	m_Frame += animSpeed;
	return  TRUE;
}

//=============================================================================
// Model::HitRay
// brief  : レイを飛ばす
// param  : [in] pMesh - 可動部位ごとのメッシュデータ
// param  : [in] pRayPos - レイの発射場所
// param　: [in] pRayDir - レイの発射向き
// param  : [in] pDist	 - 当たったときの距離
// return : 正常動作なら TRUE 、異常終了なら FALSE を返す
//=============================================================================
BOOL Model::HitRay(MeshFBX* pMesh,D3DXVECTOR3* pRayPos,D3DXVECTOR3* pRayDir,float* pDist,D3DXVECTOR3* pNormal)	
{
	//マテリアルの数だけループ
	for(DWORD j = 0; j < pMesh->m_dwNumMaterial; j++)
	{
		int count = 0;

		//部品ごとに関数が呼ばれ、その素材が何個の部品でできているか調べる
		//ポリゴンの数だけループ
		for(DWORD i = 0; i < pMesh->m_dwNumFace; i++)
		{
			//そのポリゴンのマテリアルを調べる
			FbxLayerElementMaterial* mtl = pMesh->m_pFbxMesh->GetLayer(0)->GetMaterials();
			int mtlId = mtl->GetIndexArray().GetAt(i);
			//マテリアルがjだったら
			if(mtlId == j )
			{
			D3DXVECTOR3 v1 = pMesh->m_pVertices[pMesh->m_pIndex[j][count+0]].vPos;	//j番目の素材でできている何番目のポリゴン
			D3DXVECTOR3 v2 = pMesh->m_pVertices[pMesh->m_pIndex[j][count+1]].vPos;
			D3DXVECTOR3 v3 = pMesh->m_pVertices[pMesh->m_pIndex[j][count+2]].vPos;

			float dist;
			BOOL hit = D3DXIntersectTri(&v1,&v2,&v3,pRayPos,pRayDir,NULL,NULL,&dist);

			if(hit)	//レイがあたったとき (pDisut..前回の距離、Dist..今回の距離）
				{
					if(dist < *pDist)
					{
						*pDist = dist;

						//法線求める
						if(pNormal != NULL)
						{
							D3DXVECTOR3 a = v2 - v1;
							D3DXVECTOR3 b = v3 - v1;

							D3DXVec3Cross(pNormal, &a, &b);			//外積を求める計算
							D3DXVec3Normalize(pNormal, pNormal);	//正規化
						}
					}
				}
				count += 3;
			}
		}
	}	
	//子メッシュも調べる(ここにくるのは親と当たらなかった場合）
	for(DWORD i = 0; i < pMesh->GetNumChild(); i++)
	{
		HitRay(pMesh->GetChild()[i], pRayPos, pRayDir, pDist,pNormal);
	}

	return FALSE;
}

//m_dwNumFace...ポリゴンの面の数
//Mayaでは親子関係がある場合がある-＞MeshFBX* pMesh...pMeshの中に今調べようとしている（親や子の）部品が入るイメージ
//=============================================================================
// Model::HitRay（最初の呼び出し用）
// brief  : レイを飛ばす
// param  : [in] pRayPos - レイの発射場所
// param　: [in] pRayDir - レイの発射向き
// param  : [in] pDist	 - 当たったときの距離
// return : 正常動作なら TRUE 、異常終了なら FALSE を返す
//=============================================================================
BOOL Model::HitRay(D3DXVECTOR3* pRayPos,D3DXVECTOR3* pRayDir,float* pDist,D3DXVECTOR3* pNormal)
{

	*pDist = 99999;		//ありえない数を入れておく
	/*for(int i = 0; i < 3; i++)
	{	
		for(int j = 0; j < 3; j++) 
		{*/
			HitRay(this, pRayPos, pRayDir, pDist,pNormal);	//引数５つのHitRay呼び出し
		/*}
	}*/
	if(*pDist < 99999)
		return TRUE;
	else
		return FALSE;
}

//=============================================================================
// Model::HitRay（最初の呼び出し用）
// brief  : レイを飛ばす
// param  : [in] pRayPos - レイの発射場所
// param　: [in] pRayDir - レイの発射向き
// param  : [in] pDist	 - 当たったときの距離
// return : 正常動作なら TRUE 、異常終了なら FALSE を返す
//=============================================================================
BOOL Model::HitRay(D3DXVECTOR3* pRayPos,D3DXVECTOR3* pRayDir,float* pDist)
{
	return HitRay(pRayPos, pRayDir, pDist, NULL);
}


//BOOL Model::HitRay(D3DXVECTOR3* pRayPos,D3DXVECTOR3* pRayDir,D3DXVECTOR3* pNormal)
//{
//	return HitRay(pRayPos, pRayDir, NULL, pNormal);
//}