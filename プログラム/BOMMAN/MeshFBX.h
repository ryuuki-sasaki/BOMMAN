//───────────────────────────────────────
// file : MeshFBX.h
// brief : .fbxファイル制御ヘッダファイル
//───────────────────────────────────────

//───────────────────────────────────────
// インクルードガードの開始宣言
//───────────────────────────────────────
#ifndef _MESHFBX_H_
#define _MESHFBX_H_

#define FBXSDK_NEW_API

//───────────────────────────────────────
// 必要なヘッダファイルのインクルード
//───────────────────────────────────────
#include <d3d9.h>
#include <fbxsdk.h>

#include <windows.h>
#include <mmsystem.h>
#include <d3dx9.h>
#include <list>

//───────────────────────────────────────
// MeshFBXクラス：FBX読み込み制御全般を管理
//───────────────────────────────────────
class   MeshFBX
{
	// FBX関連データ
    char            fbxFileName[256];   // FBXファイル名
    FbxManager *    m_pSdkManager;      // FBX統括制御
    FbxImporter *   m_pImporter;        // FBX用インポート制御
    FbxScene *      m_pMyScene;         // FBX用シーン制御
public:
    FbxMesh *       m_pFbxMesh;         // FBX用ポリゴンメッシュ制御

protected:
	LPDIRECT3DDEVICE9 m_pDevice;

    // ボーン構造体（関節情報）
    struct  BONE
    {
        D3DXMATRIX  mBindPose;      // 初期ポーズ時のボーン変換行列
        D3DXMATRIX  mNewPose;       // アニメーションで変化したときのボーン変換行列
        D3DXMATRIX  mDiffPose;      // mBindPose に対する mNowPose の変化量
    };

    // ウェイト構造体（ボーンと頂点の関連付け）
    struct WEIGHT
    {
        D3DXVECTOR3 vPosOrigin;     // 元々の頂点座標
        D3DXVECTOR3 vNormalOrigin;  // 元々の法線ベクトル
        int *       boneIndex;      // 関連するボーンのID
        float *     boneWeight;     // ボーンの重み
    };

    // ボーン制御情報
    FbxSkin *       m_pSkinInfo;    // スキンメッシュ情報（スキンメッシュアニメーションのデータ本体）
    FbxCluster **   m_ppCluster;    // クラスタ情報（関節ごとに関連付けられた頂点情報）
    int             m_numBone;      // FBXに含まれている関節の数
    BONE *          m_boneArray;    // 各関節の情報
    WEIGHT *        m_weightArray;  // ウェイト情報（頂点の対する各関節の影響度合い）


private:
    // アニメーション制御情報
    FbxNode *       m_pFbxNode;         // FBX用アニメーションノード制御
    char            fbxNodeName[256];   // FBXノード名

    // アニメーション再生情報
    FbxTime::EMode  m_FrameRate;        // アニメーションのフレームレート
    FbxTime         m_Period;           // アニメーションの１フレーム時間
    FbxTime         m_StartTime;        // アニメーション開始時間
    FbxTime         m_EndTime;          // アニメーション終了時間
    DWORD           m_StartFrame;       // アニメーション開始フレーム数
    DWORD           m_EndFrame;         // アニメーション終了フレーム数

protected:
    // 階層メッシュ情報
    DWORD           m_dwNumChild;       // 階層メッシュオブジェクトの数
    MeshFBX **      m_ppChild;          // 階層メッシュオブジェクト

    // 座標変換行列
    D3DXMATRIX      mLocalMatrix;       // 各部位のローカル座標変換行列

public:
    // 頂点バッファ構造体
    struct VERTEX
    {
        D3DXVECTOR3 vPos;    // 頂点座標
        D3DXVECTOR3 vNormal; // 法線ベクトル
        D3DXVECTOR2 vTex;    // テクスチャ座標
    };

    // DirectXポリゴンメッシュ関連
    VERTEX *            m_pVertices;        // 頂点データ
	int **				m_pIndex;
	IDirect3DVertexBuffer9 *      m_pVertexBuffer;    // 頂点バッファ
	IDirect3DIndexBuffer9 **     m_ppIndexBuffer;    // インデックスバッファ
    DWORD               m_dwNumVert;        // メッシュに含まれる頂点の数
    DWORD               m_dwNumFace;        // メッシュに含まれる面の数
    DWORD               m_dwNumUV;          // メッシュに含まれるテクスチャUV座標の数

    // マテリアルバッファ構造体
    struct  MATERIAL
    {
        char                        materialName[256];  // マテリアル名
        DWORD                       dwNumFace;          // マテリアルのポリゴン数
        char                        textureName[256];   // テクスチャファイル名
		LPDIRECT3DTEXTURE9 			pTexture;           // テクスチャデータ
        D3DXVECTOR4                 vAmbient;           // 環境光への反射強度
        D3DXVECTOR4                 vDiffuse;           // 拡散反射光への反射強度
        D3DXVECTOR4                 vSpecular;          // 鏡面反射光への反射強度
    };

    // DirectXマテリアル関連
    MATERIAL *      m_pMaterial;        // マテリアルデータ
    DWORD           m_dwNumMaterial;    // マテリアルバッファの数

private:
    // FBXファイルの読み込み
    BOOL    LoadFBX();              // FBXファイル読み込み
    BOOL    ExtractMesh( FbxNode * pNode ); // FBXからポリゴンメッシュを抽出
	BOOL    ExtractAnim();                  // FBXからアニメーション情報を抽出
	BOOL    ExtractSkinInfo();              // FBXからスキンメッシュ情報を抽出する


    // ポリゴン情報の抽出と頂点バッファの生成
    BOOL    ConvertMesh();          // 抽出したポリゴンメッシュをDirectX用に変換
    BOOL    CreateVertexBuffer();   // 頂点バッファ＆インデックスバッファを生成

    // マテリアル情報の抽出とマテリアルバッファ＆インデックスバッファの生成
    BOOL    ConvertMaterial();      // 抽出したマテリアルをDirectX用に変換
    BOOL    CreateIndexBuffer();    // マテリアルバッファ＆インデックスバッファを生成


public:
    MeshFBX();      // コンストラクタ
    ~MeshFBX();     // デストラクタ

	BOOL    Load_( char * filename );    // ファイルの読み込み
	BOOL    Update( float frame );              // アニメーションの更新

    // アクセス関数
	IDirect3DVertexBuffer9 *  GetVertexBuffer()   { return m_pVertexBuffer; } // 頂点バッファの取得
    DWORD					GetNumMaterials()   { return m_dwNumMaterial; } // マテリアルデータの総数取得
    MATERIAL *				GetMaterialBuffer() { return m_pMaterial; }     // マテリアルバッファの取得
	IDirect3DIndexBuffer9 ** GetIndexBuffer()    { return m_ppIndexBuffer; } // インデックスバッファの取得
    DWORD					GetNumChild()       { return m_dwNumChild; }    // 階層メッシュデータの総数取得
    MeshFBX **				GetChild()          { return m_ppChild; }       // 階層メッシュオブジェクトの取得
    D3DXMATRIX				GetLocalMatrix()    { return mLocalMatrix; }    // ローカル座標変換行列の取得
	DWORD					GetNumVertexNum(){ return m_dwNumVert; }
	VERTEX *				GetVertex(){ return	m_pVertices; }
	int **					GetIndex(){ return m_pIndex; }
	VOID					SetDevice(LPDIRECT3DDEVICE9 pDevice){ m_pDevice = pDevice; }
};

//───────────────────────────────────────
// インクルードガードの終了宣言
//───────────────────────────────────────
#endif  _MESHFBX_H_