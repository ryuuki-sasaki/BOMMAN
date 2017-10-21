//───────────────────────────────────────
// file : MeshFBX.cpp
// brief : .fbxファイル制御ソースファイル
//───────────────────────────────────────

//───────────────────────────────────────
// 必要なヘッダファイルのインクルード
//───────────────────────────────────────
#include "MeshFBX.h"    // fbxファイル制御
#include "stdio.h"
#include "constants.h"   

#ifdef _DEBUG
#pragma comment(lib, "fbxsdk-2013.3-mtd.lib")
#else
#pragma comment(lib, "fbxsdk-2013.3-mt.lib")
#endif _DEBUG

#pragma warning(disable:4996)


//=============================================================================
// MeshFBX::MeshFBX
// brief  : コンストラクタ
//=============================================================================
MeshFBX::MeshFBX()
{
    // 変数の初期化
    ZeroMemory( this, sizeof( MeshFBX ) );
	D3DXMatrixIdentity( &mLocalMatrix );

}

//=============================================================================
// MeshFBX::~MeshFBX
// brief  : デストラクタ
//=============================================================================
MeshFBX::~MeshFBX()
{
    // ボーン情報の削除
    SAFE_DELETE_ARRAY( m_boneArray );
    SAFE_DELETE_ARRAY( m_ppCluster );
    DWORD       vbSize = m_dwNumVert;
    if( m_dwNumVert < m_dwNumUV )
    {
        vbSize = m_dwNumUV;
    }
    if( m_weightArray != NULL )
    {
        for( DWORD i = 0; i < vbSize; i++ )
        {
			//if(m_weightArray[i].boneIndex==NULL)	continue;
            SAFE_DELETE( m_weightArray[i].boneIndex );
            SAFE_DELETE( m_weightArray[i].boneWeight );
        }
        SAFE_DELETE_ARRAY( m_weightArray );
    }


    // 階層メッシュデータの削除
    for( DWORD i = 0; i < m_dwNumChild; i++ )
    {
        SAFE_DELETE( m_ppChild[i] );
    }
    SAFE_DELETE_ARRAY( m_ppChild );

    // マテリアルバッファの削除
    for( DWORD i = 0; i < m_dwNumMaterial; i++ )
    {
		SAFE_DELETE( m_pIndex[i] );
        SAFE_RELEASE( m_ppIndexBuffer[i] );

    }
    SAFE_DELETE_ARRAY( m_ppIndexBuffer );
	SAFE_DELETE_ARRAY( m_pIndex );
    SAFE_DELETE_ARRAY( m_pMaterial );

    // 頂点バッファの削除
    SAFE_RELEASE( m_pVertexBuffer );
    SAFE_DELETE_ARRAY( m_pVertices );
	

    // FBXSDKの各種オブジェクトを削除
    if( m_pSdkManager != NULL )
    {
        m_pSdkManager->Destroy();
        m_pSdkManager = NULL;
    }
}

//=============================================================================
// MeshFBX::LoadFBX
// brief  : fbxファイルの読み込み制御
// return : 正常動作なら TRUE 、異常終了なら FALSE を返す
//=============================================================================
BOOL    MeshFBX::LoadFBX()
{
    // FBXSDKの統括制御インタフェースの生成
    m_pSdkManager = FbxManager::Create();
    if( m_pSdkManager == NULL )
    {
        MessageBox( 0, "FBX SDKの初期化に失敗", NULL, MB_OK );
        return  FALSE;
    }

    // 生成したm_pSdkManagerを使い、fbxファイルをインポート
    m_pImporter = FbxImporter::Create( m_pSdkManager, "my importer" );
    int iFormat = -1;
    if( !m_pImporter->Initialize( (const char *)fbxFileName, iFormat ) )
    {
        MessageBox( 0, "FBX ファイル読み込み失敗", fbxFileName, MB_OK );
        return  FALSE;
    }

    // インポートしたファイルを使ってシーンを構築
    m_pMyScene = FbxScene::Create( m_pSdkManager, "my scene" );
    m_pImporter->Import( m_pMyScene );

    // fbxからポリゴンメッシュ情報を抽出する
    if( !ExtractMesh( m_pMyScene->GetRootNode() ) )
    {
        return  FALSE;
    }

    // fbxからアニメーション情報を抽出する
    if( !ExtractAnim() )
    {
        return  FALSE;
    }

    return  TRUE;
}

//=============================================================================
// MeshFBX::ExtractMesh
// brief  : fbxからポリゴンメッシュ情報を抽出する
// param  : [in] pNode - 関節部位（ノード）の情報
// return : 正常動作なら TRUE 、異常終了なら FALSE を返す
//=============================================================================
BOOL    MeshFBX::ExtractMesh( FbxNode * pNode )
{
    // ノードの代入
    m_pFbxNode = pNode;

    // 階層データからポリゴンメッシュ情報を抽出する
    FbxNodeAttribute * pAttr = pNode->GetNodeAttribute();
    if( pAttr != NULL )
    {
        // 最上位の親が「ポリゴンメッシュ」であれば、それを抽出する
        if( pAttr->GetAttributeType() == FbxNodeAttribute::eMesh )
        {
            m_pFbxMesh = pNode->GetMesh();
            // 抽出できたかどうか確認する
            if( m_pFbxMesh == NULL )
            {
                MessageBox( 0, "ポリゴン情報抽出失敗", fbxFileName, MB_OK );
                return  FALSE;
            }
            if( !CreateVertexBuffer() )
            {
                return  FALSE;
            }
            if( !CreateIndexBuffer() )
            {
                return  FALSE;
            }
        }
    }

    // 子階層のデータから「ポリゴンメッシュ」に相当するデータを検索する
    m_dwNumChild = pNode->GetChildCount();
    m_ppChild = new MeshFBX*[m_dwNumChild];
    for( DWORD i = 0; i < m_dwNumChild; i++ )
    {
        m_ppChild[i] = new MeshFBX();
		m_ppChild[i]->SetDevice(m_pDevice);
        strcpy_s( m_ppChild[i]->fbxFileName, fbxFileName );
        FbxNode *   pChild = pNode->GetChild(i);
        strcpy_s( m_ppChild[i]->fbxNodeName, pChild->GetName() );
        m_ppChild[i]->ExtractMesh( pChild );
    }

    return  TRUE;
}

//=============================================================================
// MeshFBX::ConvertMesh
// brief  : FBX用メッシュデータをDirectX用メッシュデータに変換する
// return : 正常動作なら TRUE 、異常終了なら FALSE を返す
//=============================================================================
BOOL    MeshFBX::ConvertMesh()
{
    // 事前に頂点数＆ポリゴン数を調べる
    m_dwNumVert = m_pFbxMesh->GetControlPointsCount();
    m_dwNumUV   = m_pFbxMesh->GetTextureUVCount();
    m_dwNumFace = m_pFbxMesh->GetPolygonCount();

    // 頂点バッファの一時的なメモリ確保
    DWORD       vbSize = m_dwNumVert;
    if( m_dwNumVert < m_dwNumUV )
    {
        vbSize = m_dwNumUV;
    }
    m_pVertices = new VERTEX[ vbSize ];

    // ポリゴンごとに頂点・法線・UV座標を読み込む
    for( DWORD i = 0; i < m_dwNumFace; i++ )
    {
        // 頂点インデックスのアドレス参照
        int   index[3] = { 0, 0, 0 };
        int   startIndex  = m_pFbxMesh->GetPolygonVertexIndex(i);
        int * vertexIndex = m_pFbxMesh->GetPolygonVertices();
        for( DWORD j = 0; j < 3; j ++ )
        {
            // 頂点インデックスの抽出（UV座標登録数のほうが多い場合はUVインデックスを基準にする）
            index[j] = vertexIndex[ startIndex+j ];
            if( m_dwNumVert < m_dwNumUV )
            {
                index[j] = m_pFbxMesh->GetTextureUVIndex( i, j, FbxLayerElement::eTextureDiffuse );
            }

            // 頂点座標の抽出（FBX は右手座標系なので、Xを反転する必要がある。）
            int polyIndex = m_pFbxMesh->GetPolygonVertex( i, j );
            FbxVector4 Coord = m_pFbxMesh->GetControlPointAt( polyIndex );
            m_pVertices[ index[j] ].vPos = D3DXVECTOR3( (float)-Coord[0], (float)Coord[1], (float)Coord[2] );

            // 法線ベクトルの抽出（FBX は右手座標系なので、Xを反転する必要がある。）
            FbxVector4  Normal;
            m_pFbxMesh->GetPolygonVertexNormal( i, j, Normal );
            m_pVertices[ index[j] ].vNormal = D3DXVECTOR3( (float)-Normal[0], (float)Normal[1], (float)Normal[2] );

            // テクスチャUV座標の抽出（ポリゴン頂点にUV座標が関連付けれている場合に限る）
            FbxLayerElementUV * pUV= m_pFbxMesh->GetLayer(0)->GetUVs();
            if( m_dwNumUV > 0 && pUV->GetMappingMode() == FbxLayerElement::eByPolygonVertex )
            {
                int uvIndex = m_pFbxMesh->GetTextureUVIndex( i, j, FbxLayerElement::eTextureDiffuse );
                FbxVector2  uv = pUV->GetDirectArray().GetAt( uvIndex );
                m_pVertices[ index[j] ].vTex = D3DXVECTOR2( (float)uv.mData[0], (float)(1.0 - uv.mData[1]) );
            }
        }
    }

    // テクスチャUV座標の抽出（制御点ごとにUV座標が関連付けれている場合に限る）
    FbxLayerElementUV * pUV= m_pFbxMesh->GetLayer(0)->GetUVs();
    if( m_dwNumUV > 0 && pUV->GetMappingMode() == FbxLayerElement::eByControlPoint )
    {
        for( DWORD k = 0; k < m_dwNumUV; k++ )
        {
            FbxVector2 uv = pUV->GetDirectArray().GetAt(k);
            m_pVertices[k].vTex = D3DXVECTOR2( (float)uv.mData[0], (float)(1.0 - uv.mData[1]) );
        }
    }

    return  TRUE;
}
 
//=============================================================================
// MeshFBX::CreateVertexBuffer
// brief  : FBX用メッシュデータをDirectX用メッシュデータに変換する
// return : 正常動作なら TRUE 、異常終了なら FALSE を返す
//=============================================================================
BOOL    MeshFBX::CreateVertexBuffer()
{
    // FBX用メッシュデータをDirectX用メッシュデータに変換する
    if( !ConvertMesh() )
    {
        return  FALSE;
    }

    // 頂点バッファの数を「頂点座標」「UV座標」のうち
    // いずれか大きい方を設定するようにする
    DWORD   vertexNum = m_dwNumVert;
    if( m_dwNumVert < m_dwNumUV )
    {
        vertexNum = m_dwNumUV;
    }

	if (FAILED(m_pDevice->CreateVertexBuffer(
		sizeof(VERTEX) * vertexNum, 0, D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1,
		D3DPOOL_MANAGED, &m_pVertexBuffer, NULL)))
	{
        MessageBox( 0, "頂点バッファの生成に失敗", fbxFileName, MB_OK );
        return  FALSE;
    }

    return  TRUE;
}

//=============================================================================
// MeshFBX::ConvertMaterial
// brief  : 抽出したFBX用マテリアルデータをDirectX用マテリアルデータに変換する
// return : 正常動作なら TRUE 、異常終了なら FALSE を返す
//=============================================================================
BOOL    MeshFBX::ConvertMaterial()
{
    // マテリアルバッファの生成
    FbxNode* pNode= m_pFbxMesh->GetNode();
    m_dwNumMaterial=pNode->GetMaterialCount();
    m_pMaterial = new MATERIAL[m_dwNumMaterial];

    // マテリアルの数だけインデックスバッファーを作成
	m_ppIndexBuffer = new IDirect3DIndexBuffer9*[m_dwNumMaterial];
	m_pIndex = new int*[m_dwNumMaterial];

    for( DWORD i = 0;i < m_dwNumMaterial; i++ )
    {
        ZeroMemory( &m_pMaterial[i], sizeof( m_pMaterial[i] ) );
        ZeroMemory( &m_ppIndexBuffer[i], sizeof( m_ppIndexBuffer[i] ) );

        // フォンシェーディングを想定したマテリアルバッファの抽出
        FbxSurfaceMaterial* pMaterial = pNode->GetMaterial( i );
        FbxSurfacePhong* pPhong=(FbxSurfacePhong*)pMaterial;

        // 環境光＆拡散反射光＆鏡面反射光の反射成分値を取得
        FbxDouble3  ambient = FbxDouble3( 0, 0, 0 );
        FbxDouble3  diffuse = FbxDouble3( 0, 0, 0 );
        FbxDouble3  specular = FbxDouble3( 0, 0, 0 );
        ambient  = pPhong->Ambient;
        diffuse  = pPhong->Diffuse;
        if( pMaterial->GetClassId().Is( FbxSurfacePhong::ClassId ) )
        {
            specular  = pPhong->Specular;
        }

        // 環境光＆拡散反射光＆鏡面反射光の反射成分値をマテリアルバッファにコピー
        m_pMaterial[i].vAmbient  = D3DXVECTOR4( (float)ambient[0], (float)ambient[1], (float)ambient[2], 1.0f );
        m_pMaterial[i].vDiffuse  = D3DXVECTOR4( (float)diffuse[0], (float)diffuse[1], (float)diffuse[2], 1.0f );
        m_pMaterial[i].vSpecular = D3DXVECTOR4( (float)specular[0], (float)specular[1], (float)specular[2], 1.0f );

        // テクスチャー情報の取得
        FbxProperty  lProperty = pMaterial->FindProperty( FbxSurfaceMaterial::sDiffuse );
        FbxTexture * texture = FbxCast<FbxTexture>( lProperty.GetSrcObject( FbxTexture::ClassId, 0 ) );
        if( texture != NULL )
        {
            // FBXファイルのディレクトリを抽出する
            char    dir[MAX_PATH];
            _splitpath_s( fbxFileName, NULL, 0, dir, sizeof(dir), NULL, 0, NULL, 0 );



            // テクスチャファイル名を絶対パス → 相対パスに変更する
            char    name[MAX_PATH], ext[MAX_PATH];
            _splitpath_s( texture->GetName(), 0, 0, 0, 0, name, sizeof(name), ext, sizeof(ext) );
            strcpy_s( m_pMaterial[i].textureName, dir );
            strcat_s( m_pMaterial[i].textureName, name );
            strcat_s( m_pMaterial[i].textureName, ext );

            // テクスチャを読み込む
            //if( FAILED( D3DX10CreateShaderResourceViewFromFileA( DirectX::GetDevice(), m_pMaterial[i].textureName,
            //                                                     NULL, NULL, &m_pMaterial[i].pTexture, NULL ) ) )

			//if (FAILED(D3DXCreateTextureFromFileEx(m_pDevice, m_pMaterial[i].textureName, 
			//	D3DX_DEFAULT, D3DX_DEFAULT, 0, 0, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT,
			//	D3DX_FILTER_LINEAR, D3DX_FILTER_LINEAR, 0x000000ff, NULL, NULL, m_pMaterial[i].pTexture)))

			if (FAILED( D3DXCreateTextureFromFile(m_pDevice, m_pMaterial[i].textureName, &m_pMaterial[i].pTexture)))
            {
                MessageBox( 0, "FBXテクスチャー読み込み失敗", m_pMaterial[i].textureName, MB_OK );
				return  FALSE;
            }
        }
    }

    return  TRUE;
}

//=============================================================================
// MeshFBX::CreateIndexBuffer
// brief  : 抽出したマテリアルデータを参考にインデックスバッファを生成する
// return : 正常動作なら TRUE 、異常終了なら FALSE を返す
//=============================================================================
BOOL    MeshFBX::CreateIndexBuffer()
{
    // 抽出したFBX用マテリアルデータをDirectX用マテリアルデータに変換する
    if( !ConvertMaterial() )
    {
        return  FALSE;
    }

    // マテリアルから「ポリゴン平面」の情報を抽出する
    for(DWORD i = 0;i < m_dwNumMaterial; i++)
    {
        int     count = 0;
        m_pIndex[i] = new int[ m_dwNumFace * 3 ];

        // ポリゴンを構成する三角形平面が、
        // 「頂点バッファ」内のどの頂点を利用しているかを調べる
        for( DWORD j = 0; j < m_dwNumFace; j++ )
        {
            FbxLayerElementMaterial *   mtl = m_pFbxMesh->GetLayer(0)->GetMaterials();
            int mtlId = mtl->GetIndexArray().GetAt( j );
            if( mtlId == i )
            {
                for( DWORD k = 0; k < 3; k++ )
                {
                    if( m_dwNumVert < m_dwNumUV )
                    {
                        m_pIndex[i][count+k] = m_pFbxMesh->GetTextureUVIndex( j, k, FbxLayerElement::eTextureDiffuse );
                    }
                    else
                    {
                        m_pIndex[i][count+k] = m_pFbxMesh->GetPolygonVertex( j, k );
                    }
                }
                count += 3;
            }
        }

        // インデックスバッファを生成する
        //D3D10_BUFFER_DESC   bd;
        //bd.Usage = D3D10_USAGE_DEFAULT;
        //bd.ByteWidth = sizeof(int) * count;
        //bd.BindFlags = D3D10_BIND_INDEX_BUFFER;
        //bd.CPUAccessFlags = 0;
        //bd.MiscFlags = 0;

        //D3D10_SUBRESOURCE_DATA InitData;
        //InitData.pSysMem = pIndex;
        //InitData.SysMemPitch = 0;
        //InitData.SysMemSlicePitch = 0;

		

        //if( FAILED( DirectX::GetDevice()->CreateBuffer( &bd, &InitData, &m_ppIndexBuffer[i] ) ) )
		if (FAILED(m_pDevice->CreateIndexBuffer(sizeof(int) * m_dwNumFace * 3, 0, D3DFMT_INDEX32, D3DPOOL_MANAGED, &m_ppIndexBuffer[i], NULL))) 
		{
            MessageBox( 0, "インデックスバッファの生成に失敗", fbxFileName, MB_OK );
            return FALSE;
        }




        m_pMaterial[i].dwNumFace = count / 3;
        //SAFE_DELETE_ARRAY( m_pIndex );
    }

    return  TRUE;
}

//=============================================================================
// MeshFBX::Load
// brief  : fbxファイルからモデルデータを抽出する
// param  : [in] filename - ファイル名
// return : 正常動作なら TRUE 、異常終了なら FALSE を返す
//=============================================================================
BOOL    MeshFBX::Load_( char *filename )
{
    // ファイル名のコピー
    strcpy_s( fbxFileName, filename );

    // FBXファイル読み込み
    if( !LoadFBX() )
    {
        return  FALSE;
    }

    return  TRUE;
}

//=============================================================================
// MeshFBX::ExtractAnim
// brief  : アニメーション再生情報を取得する
// return : 正常動作なら TRUE 、異常終了なら FALSE を返す
//=============================================================================
BOOL    MeshFBX::ExtractAnim()
{
    if( m_pFbxMesh != NULL )
    {
        // アニメーションのタイムモードの取得
        m_FrameRate = m_pFbxMesh->GetScene()->GlobalTimeSettings().GetTimeMode();

        // アニメーションテイク数の取得
        FbxArray<FbxString *>   takeNameArray;
        m_pFbxMesh->GetScene()->FillAnimStackNameArray( takeNameArray );
        DWORD   numTake = takeNameArray.GetCount();

        // テイク情報から「開始フレーム数」と「終了フレーム数」を取得する
        BOOL    bTimeExist = FALSE;
        for( DWORD i = 0; i < numTake; i++ )
        {
            FbxTakeInfo *   takeInfo = m_pFbxMesh->GetScene()->GetTakeInfo( *(takeNameArray[i]) );
            if( takeInfo != NULL )
            {
                m_StartTime = takeInfo->mLocalTimeSpan.GetStart();
                m_EndTime = takeInfo->mLocalTimeSpan.GetStop();
                bTimeExist = TRUE;
                break;
            }
        }
        // テイク情報が見つからなければ、全体フレーム数から参照する
        if( bTimeExist == FALSE )
        {
            FbxTimeSpan     timeSpan;
            m_pFbxMesh->GetScene()->GlobalTimeSettings().GetTimelineDefautTimeSpan(timeSpan);

            m_StartTime = timeSpan.GetStart();
            m_EndTime = timeSpan.GetStop();
        }

        // １フレームにかかる時間で割ればフレーム数になる
        FbxTime period;
        period.SetTime( 0, 0, 0, 1, 0, 0, m_FrameRate );
        m_StartFrame = (DWORD)( m_StartTime.Get() / period.Get() );
        m_EndFrame   = (DWORD)( m_EndTime.Get() / period.Get() );

        FbxArrayDelete( takeNameArray );

		// スキンメッシュアニメーションの抽出
        if( ExtractSkinInfo() )
        {
            return  TRUE;
        }
    }

    // 子階層のデータからもアニメーション情報を取得
    for( DWORD i = 0; i < m_dwNumChild; i++ )
    {
        m_ppChild[i]->ExtractAnim();
    }

    return  TRUE;
}

//=============================================================================
// MeshFBX::Update
// brief  : アニメーションを再生する
// param  : [in] frame - 現在の再生フレーム数
// return : 正常動作なら TRUE 、異常終了なら FALSE を返す
//=============================================================================
BOOL    MeshFBX::Update( float frame )
{
    if( m_pFbxMesh != NULL )
    {
        // アニメーションフレーム数をキーフレーム数の範囲内に設定する
        if( m_EndFrame - m_StartFrame != 0 )
        {
            while( frame < m_StartFrame || frame > m_EndFrame )
            {
                if( frame < m_StartFrame )
                {
                    frame += (m_EndFrame - m_StartFrame);
                }
                if( frame > m_EndFrame )
                {
                    frame -= (m_EndFrame - m_StartFrame);
                }
            }
        }
        // スキンメッシュアニメーションの更新
        if( m_pSkinInfo != NULL )
        {
            // その瞬間の自分の姿勢行列を得る
            FbxTime     time;
            time.SetTime( 0, 0, 0, (int)frame, 0, 0, m_FrameRate );

            // ボーンごとの現在の行列を取得する
            for( int i = 0; i < m_numBone; i++ )
            {
                FbxAnimEvaluator * evaluator = m_ppCluster[i]->GetLink()->GetScene()->GetEvaluator();
                FbxMatrix mCurrentOrentation = evaluator->GetNodeGlobalTransform( m_ppCluster[i]->GetLink(), time );

                // 行列コピー（Fbx形式からDirectXへの変換）
                for( DWORD x = 0; x < 4; x++ )
                {
                    for( DWORD y = 0; y < 4; y++ )
                    {
                        m_boneArray[i].mNewPose( x, y ) = (float)mCurrentOrentation.Get( x, y );
                    }
                }

                // FBX は右手座標系なので、Ｘ軸方向成分を逆転させる
                m_boneArray[i].mNewPose( 0, 0 ) *= -1.0f;
                m_boneArray[i].mNewPose( 0, 1 ) *= -1.0f;
                m_boneArray[i].mNewPose( 0, 2 ) *= -1.0f;

                // オフセット時のポーズの差分を計算する
                D3DXMatrixInverse( &m_boneArray[i].mDiffPose, NULL, &m_boneArray[i].mBindPose );
                m_boneArray[i].mDiffPose *= m_boneArray[i].mNewPose;
            }

            // 各ボーンに対応した頂点の変形制御
            DWORD       vbSize = m_dwNumVert;
            if( m_dwNumVert < m_dwNumUV )
            {
                vbSize = m_dwNumUV;
            }
            for( DWORD i = 0; i < vbSize; i++ )
            {
                // 各頂点ごとに、「影響するボーン×ウェイト値」を反映させた関節行列を作成する
                D3DXMATRIX  matrix;
                ZeroMemory( &matrix, sizeof(matrix) );
                for( int m = 0; m < m_numBone; m++ )
                {
                    if( m_weightArray[i].boneIndex[m] < 0 )
                    {
                        break;
                    }
                    matrix += m_boneArray[m_weightArray[i].boneIndex[m]].mDiffPose * m_weightArray[i].boneWeight[m];
                }
                // 作成された関節行列を使って、頂点を変形する
                D3DXVECTOR3 Pos = m_weightArray[i].vPosOrigin;
                D3DXVECTOR3 Normal = m_weightArray[i].vNormalOrigin;
                Pos.x *= -1.0f;     // FBX は右手座標系なので、Ｘ軸方向成分を逆転させる
                Normal.x *= -1.0f;  // FBX は右手座標系なので、Ｘ軸方向成分を逆転させる
                D3DXVec3TransformCoord( &m_pVertices[i].vPos, &Pos, &matrix );
                D3DXVec3TransformNormal( &m_pVertices[i].vNormal, &Normal, &matrix );
            }

            // 頂点バッファをロックして、変形させた後の頂点情報で上書きする
            VERTEX * pv;
			if (m_pVertexBuffer != NULL && SUCCEEDED(m_pVertexBuffer->Lock(0, 0, (void**)&pv, D3DLOCK_DISCARD)))
            {
                memcpy( pv, m_pVertices, sizeof(VERTEX) * vbSize );
                m_pVertexBuffer->Unlock();
            }
        }

        // 階層メッシュアニメーションの更新
        else
        {
            // その瞬間の自分の姿勢行列を得る
            FbxTime     time;
            time.SetTime( 0, 0, 0, (int)frame, 0, 0, m_FrameRate );
            FbxAnimEvaluator *evaluator = m_pFbxMesh->GetScene()->GetEvaluator();
            FbxMatrix mCurrentOrentation = evaluator->GetNodeGlobalTransform(m_pFbxNode,time);

            // 行列コピー（Fbx形式からDirectXへの変換）
            for( DWORD x = 0; x < 4; x++ )
            {
                for( DWORD y = 0; y < 4; y++ )
                {
                    mLocalMatrix( x, y ) = (float)mCurrentOrentation.Get( x, y );
                }
            }

            // FBX は右手座標系なので、Ｘ軸方向成分を逆転させる
            mLocalMatrix( 0, 0 ) *= -1.0f;
            mLocalMatrix( 0, 1 ) *= -1.0f;
            mLocalMatrix( 0, 2 ) *= -1.0f;
        }
    }

    return  TRUE;
}


//=============================================================================
// MeshFBX::ExtractSkinInfo
// brief  : fbxからスキンメッシュ情報を抽出する
// return : 正常動作なら TRUE 、異常終了または非スキンメッシュデータなら FALSE を返す
//=============================================================================
BOOL    MeshFBX::ExtractSkinInfo()
{
    // デフォーマ情報（ボーンとモデルの関連付け）の取得
    FbxDeformer *   pDeformer = m_pFbxMesh->GetDeformer(0);
    if( pDeformer == NULL )
    {
        return  FALSE;
    }

    // デフォーマ情報からスキンメッシュ情報を取得
    m_pSkinInfo = (FbxSkin *)pDeformer;

    // 頂点からポリゴンを逆引きするための情報を作成する
    struct  POLY_INDEX
    {
        int *   polyIndex;      // ポリゴンの番号
        int *   vertexIndex;    // 頂点の番号
        int     numRef;         // 頂点を共有するポリゴンの数
    };
    POLY_INDEX * polyTable = new POLY_INDEX[ m_dwNumVert ];
    for( DWORD i = 0; i < m_dwNumVert; i++ )
    {
        // 三角形ポリゴンに合わせて、頂点とポリゴンの関連情報を構築する
        // 総頂点数＝ポリゴン数×３頂点
        polyTable[i].polyIndex = new int[m_dwNumFace * 3];
        polyTable[i].vertexIndex = new int[m_dwNumFace * 3];
        polyTable[i].numRef = 0;
        ZeroMemory( polyTable[i].polyIndex, sizeof(int) * m_dwNumFace * 3 );
        ZeroMemory( polyTable[i].vertexIndex, sizeof(int) * m_dwNumFace * 3 );

        // ポリゴン間で共有する頂点を列挙する
        for( DWORD k = 0; k < m_dwNumFace; k++ )
        {
            for( int m = 0; m < 3; m++ )
            {
                if( m_pFbxMesh->GetPolygonVertex( k, m ) == i )
                {
                    polyTable[i].polyIndex[ polyTable[i].numRef ] = k;
                    polyTable[i].vertexIndex[ polyTable[i].numRef ] = m;
                    polyTable[i].numRef++;
                }
            }
        }
    }

    // ボーン情報を取得する
    m_numBone = m_pSkinInfo->GetClusterCount();
    m_ppCluster = new FbxCluster*[m_numBone];
    for( int i = 0; i < m_numBone; i++ )
    {
        m_ppCluster[i] = m_pSkinInfo->GetCluster(i);
    }

    // ボーンの数に合わせてウェイト情報を準備する
    DWORD       vbSize = m_dwNumVert;
    if( m_dwNumVert < m_dwNumUV )
    {
        vbSize = m_dwNumUV;
    }
    m_weightArray = new WEIGHT[ vbSize ];
    for( DWORD i = 0; i < vbSize; i++ )
    {
        m_weightArray[i].vPosOrigin = m_pVertices[i].vPos;
        m_weightArray[i].vNormalOrigin = m_pVertices[i].vNormal;
        m_weightArray[i].boneIndex = new int[m_numBone];
        m_weightArray[i].boneWeight = new float[m_numBone];
        for( int j = 0; j < m_numBone; j++ )
        {
            m_weightArray[i].boneIndex[j] = -1;
            m_weightArray[i].boneWeight[j] = 0.0f;
        }
    }



    // それぞれのボーンに影響を受ける頂点を調べる
    // そこから逆に、頂点ベースでボーンインデックス・重みを整頓する
    for( int i = 0;i < m_numBone; i++ )
    {
        int numIndex = m_ppCluster[i]->GetControlPointIndicesCount();   //このボーンに影響を受ける頂点数
        int * piIndex = m_ppCluster[i]->GetControlPointIndices();       //ボーン/ウェイト情報の番号
        double * pdWeight=m_ppCluster[i]->GetControlPointWeights();     //頂点ごとのウェイト情報

        //頂点側からインデックスをたどって、頂点サイドで整理する
        
		
		for( int k = 0; k < numIndex; k++ )
        {
            //通常の場合　（頂点数>=UV数　pvVBが頂点インデックスベースの場合）
            if(m_dwNumVert >= m_dwNumUV)
            {
                // 頂点に関連付けられたウェイト情報がボーン５本以上の場合は、重みの大きい順に４本に絞る
                for( int m = 0; m < 4; m++ )
                {
					if(m >= m_numBone)
						break;

                    if( pdWeight[k] > m_weightArray[ piIndex[k] ].boneWeight[m] )
                    {
                        for( int n = m_numBone-1; n > m; n-- )
                        {
                            m_weightArray[ piIndex[k] ].boneIndex[n] = m_weightArray[ piIndex[k] ].boneIndex[n-1];
                            m_weightArray[ piIndex[k] ].boneWeight[n] = m_weightArray[ piIndex[k] ].boneWeight[n-1];
                        }
                        m_weightArray[ piIndex[k] ].boneIndex[m] = i;
                        m_weightArray[ piIndex[k] ].boneWeight[m] = (float)pdWeight[k];
                        break;
                    }
                }
            }
            //UVインデックスベースの場合　（頂点数<UV数）
            else
            {
                //その頂点を含んでいるポリゴンすべてに、このボーンとウェイトを適用
                for( int p = 0; p < polyTable[ piIndex[k] ].numRef; p++ )
                {
                    //頂点→属すポリゴン→そのポリゴンのUVインデックス　と逆引き
                    int polyIndex = polyTable[ piIndex[k] ].polyIndex[p];
                    int vertIndex = polyTable[ piIndex[k] ].vertexIndex[p];
                    int uvIndex = m_pFbxMesh->GetTextureUVIndex( polyIndex, vertIndex, FbxLayerElement::eTextureDiffuse );

                    // 頂点に関連付けられたウェイト情報がボーン５本以上の場合は、重みの大きい順に４本に絞る
                    for(int m = 0; m < 4; m++ )
                    {
						if(m >= m_numBone)
							break;

                        if( pdWeight[k] > m_weightArray[uvIndex].boneWeight[m] )
                        {
                            for( int n = m_numBone-1; n > m; n-- )
                            {
                                m_weightArray[uvIndex].boneIndex[n] = m_weightArray[uvIndex].boneIndex[n-1];
                                m_weightArray[uvIndex].boneWeight[n] = m_weightArray[uvIndex].boneWeight[n-1];
                            }


							m_weightArray[uvIndex].boneIndex[m] = i;
							m_weightArray[uvIndex].boneWeight[m] = (float)pdWeight[k];
                            break;
                        }
                    }
                }	
            }
        }
    }

    //ボーンを生成
    m_boneArray = new BONE[m_numBone];
    for( int i = 0; i < m_numBone; i++ )
    {
        // ボーンのデフォルト位置を取得する
        FbxAMatrix  matrix;
        m_ppCluster[i]->GetTransformLinkMatrix( matrix );

        // 行列コピー（Fbx形式からDirectXへの変換）
        for( DWORD x = 0; x < 4; x++ )
        {
            for( DWORD y = 0; y < 4; y++ )
            {
                m_boneArray[i].mBindPose( x, y ) = (float)matrix.Get( x, y );
            }
        }

        // FBX は右手座標系なので、Ｘ軸方向成分を逆転させる
        m_boneArray[i].mBindPose( 0, 0 ) *= -1.0f;
        m_boneArray[i].mBindPose( 0, 1 ) *= -1.0f;
        m_boneArray[i].mBindPose( 0, 2 ) *= -1.0f;
    }

    // 一時的なメモリ領域を解放する
    for( DWORD i = 0; i < m_dwNumVert; i++ )
    {
        SAFE_DELETE_ARRAY( polyTable[i].polyIndex );
        SAFE_DELETE_ARRAY( polyTable[i].vertexIndex );
    }
    SAFE_DELETE_ARRAY( polyTable );

    return  TRUE;
}

