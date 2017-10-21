//������������������������������������������������������������������������������
// file : MeshFBX.cpp
// brief : .fbx�t�@�C������\�[�X�t�@�C��
//������������������������������������������������������������������������������

//������������������������������������������������������������������������������
// �K�v�ȃw�b�_�t�@�C���̃C���N���[�h
//������������������������������������������������������������������������������
#include "MeshFBX.h"    // fbx�t�@�C������
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
// brief  : �R���X�g���N�^
//=============================================================================
MeshFBX::MeshFBX()
{
    // �ϐ��̏�����
    ZeroMemory( this, sizeof( MeshFBX ) );
	D3DXMatrixIdentity( &mLocalMatrix );

}

//=============================================================================
// MeshFBX::~MeshFBX
// brief  : �f�X�g���N�^
//=============================================================================
MeshFBX::~MeshFBX()
{
    // �{�[�����̍폜
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


    // �K�w���b�V���f�[�^�̍폜
    for( DWORD i = 0; i < m_dwNumChild; i++ )
    {
        SAFE_DELETE( m_ppChild[i] );
    }
    SAFE_DELETE_ARRAY( m_ppChild );

    // �}�e���A���o�b�t�@�̍폜
    for( DWORD i = 0; i < m_dwNumMaterial; i++ )
    {
		SAFE_DELETE( m_pIndex[i] );
        SAFE_RELEASE( m_ppIndexBuffer[i] );

    }
    SAFE_DELETE_ARRAY( m_ppIndexBuffer );
	SAFE_DELETE_ARRAY( m_pIndex );
    SAFE_DELETE_ARRAY( m_pMaterial );

    // ���_�o�b�t�@�̍폜
    SAFE_RELEASE( m_pVertexBuffer );
    SAFE_DELETE_ARRAY( m_pVertices );
	

    // FBXSDK�̊e��I�u�W�F�N�g���폜
    if( m_pSdkManager != NULL )
    {
        m_pSdkManager->Destroy();
        m_pSdkManager = NULL;
    }
}

//=============================================================================
// MeshFBX::LoadFBX
// brief  : fbx�t�@�C���̓ǂݍ��ݐ���
// return : ���퓮��Ȃ� TRUE �A�ُ�I���Ȃ� FALSE ��Ԃ�
//=============================================================================
BOOL    MeshFBX::LoadFBX()
{
    // FBXSDK�̓�������C���^�t�F�[�X�̐���
    m_pSdkManager = FbxManager::Create();
    if( m_pSdkManager == NULL )
    {
        MessageBox( 0, "FBX SDK�̏������Ɏ��s", NULL, MB_OK );
        return  FALSE;
    }

    // ��������m_pSdkManager���g���Afbx�t�@�C�����C���|�[�g
    m_pImporter = FbxImporter::Create( m_pSdkManager, "my importer" );
    int iFormat = -1;
    if( !m_pImporter->Initialize( (const char *)fbxFileName, iFormat ) )
    {
        MessageBox( 0, "FBX �t�@�C���ǂݍ��ݎ��s", fbxFileName, MB_OK );
        return  FALSE;
    }

    // �C���|�[�g�����t�@�C�����g���ăV�[�����\�z
    m_pMyScene = FbxScene::Create( m_pSdkManager, "my scene" );
    m_pImporter->Import( m_pMyScene );

    // fbx����|���S�����b�V�����𒊏o����
    if( !ExtractMesh( m_pMyScene->GetRootNode() ) )
    {
        return  FALSE;
    }

    // fbx����A�j���[�V�������𒊏o����
    if( !ExtractAnim() )
    {
        return  FALSE;
    }

    return  TRUE;
}

//=============================================================================
// MeshFBX::ExtractMesh
// brief  : fbx����|���S�����b�V�����𒊏o����
// param  : [in] pNode - �֐ߕ��ʁi�m�[�h�j�̏��
// return : ���퓮��Ȃ� TRUE �A�ُ�I���Ȃ� FALSE ��Ԃ�
//=============================================================================
BOOL    MeshFBX::ExtractMesh( FbxNode * pNode )
{
    // �m�[�h�̑��
    m_pFbxNode = pNode;

    // �K�w�f�[�^����|���S�����b�V�����𒊏o����
    FbxNodeAttribute * pAttr = pNode->GetNodeAttribute();
    if( pAttr != NULL )
    {
        // �ŏ�ʂ̐e���u�|���S�����b�V���v�ł���΁A����𒊏o����
        if( pAttr->GetAttributeType() == FbxNodeAttribute::eMesh )
        {
            m_pFbxMesh = pNode->GetMesh();
            // ���o�ł������ǂ����m�F����
            if( m_pFbxMesh == NULL )
            {
                MessageBox( 0, "�|���S����񒊏o���s", fbxFileName, MB_OK );
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

    // �q�K�w�̃f�[�^����u�|���S�����b�V���v�ɑ�������f�[�^����������
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
// brief  : FBX�p���b�V���f�[�^��DirectX�p���b�V���f�[�^�ɕϊ�����
// return : ���퓮��Ȃ� TRUE �A�ُ�I���Ȃ� FALSE ��Ԃ�
//=============================================================================
BOOL    MeshFBX::ConvertMesh()
{
    // ���O�ɒ��_�����|���S�����𒲂ׂ�
    m_dwNumVert = m_pFbxMesh->GetControlPointsCount();
    m_dwNumUV   = m_pFbxMesh->GetTextureUVCount();
    m_dwNumFace = m_pFbxMesh->GetPolygonCount();

    // ���_�o�b�t�@�̈ꎞ�I�ȃ������m��
    DWORD       vbSize = m_dwNumVert;
    if( m_dwNumVert < m_dwNumUV )
    {
        vbSize = m_dwNumUV;
    }
    m_pVertices = new VERTEX[ vbSize ];

    // �|���S�����Ƃɒ��_�E�@���EUV���W��ǂݍ���
    for( DWORD i = 0; i < m_dwNumFace; i++ )
    {
        // ���_�C���f�b�N�X�̃A�h���X�Q��
        int   index[3] = { 0, 0, 0 };
        int   startIndex  = m_pFbxMesh->GetPolygonVertexIndex(i);
        int * vertexIndex = m_pFbxMesh->GetPolygonVertices();
        for( DWORD j = 0; j < 3; j ++ )
        {
            // ���_�C���f�b�N�X�̒��o�iUV���W�o�^���̂ق��������ꍇ��UV�C���f�b�N�X����ɂ���j
            index[j] = vertexIndex[ startIndex+j ];
            if( m_dwNumVert < m_dwNumUV )
            {
                index[j] = m_pFbxMesh->GetTextureUVIndex( i, j, FbxLayerElement::eTextureDiffuse );
            }

            // ���_���W�̒��o�iFBX �͉E����W�n�Ȃ̂ŁAX�𔽓]����K�v������B�j
            int polyIndex = m_pFbxMesh->GetPolygonVertex( i, j );
            FbxVector4 Coord = m_pFbxMesh->GetControlPointAt( polyIndex );
            m_pVertices[ index[j] ].vPos = D3DXVECTOR3( (float)-Coord[0], (float)Coord[1], (float)Coord[2] );

            // �@���x�N�g���̒��o�iFBX �͉E����W�n�Ȃ̂ŁAX�𔽓]����K�v������B�j
            FbxVector4  Normal;
            m_pFbxMesh->GetPolygonVertexNormal( i, j, Normal );
            m_pVertices[ index[j] ].vNormal = D3DXVECTOR3( (float)-Normal[0], (float)Normal[1], (float)Normal[2] );

            // �e�N�X�`��UV���W�̒��o�i�|���S�����_��UV���W���֘A�t����Ă���ꍇ�Ɍ���j
            FbxLayerElementUV * pUV= m_pFbxMesh->GetLayer(0)->GetUVs();
            if( m_dwNumUV > 0 && pUV->GetMappingMode() == FbxLayerElement::eByPolygonVertex )
            {
                int uvIndex = m_pFbxMesh->GetTextureUVIndex( i, j, FbxLayerElement::eTextureDiffuse );
                FbxVector2  uv = pUV->GetDirectArray().GetAt( uvIndex );
                m_pVertices[ index[j] ].vTex = D3DXVECTOR2( (float)uv.mData[0], (float)(1.0 - uv.mData[1]) );
            }
        }
    }

    // �e�N�X�`��UV���W�̒��o�i����_���Ƃ�UV���W���֘A�t����Ă���ꍇ�Ɍ���j
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
// brief  : FBX�p���b�V���f�[�^��DirectX�p���b�V���f�[�^�ɕϊ�����
// return : ���퓮��Ȃ� TRUE �A�ُ�I���Ȃ� FALSE ��Ԃ�
//=============================================================================
BOOL    MeshFBX::CreateVertexBuffer()
{
    // FBX�p���b�V���f�[�^��DirectX�p���b�V���f�[�^�ɕϊ�����
    if( !ConvertMesh() )
    {
        return  FALSE;
    }

    // ���_�o�b�t�@�̐����u���_���W�v�uUV���W�v�̂���
    // �����ꂩ�傫������ݒ肷��悤�ɂ���
    DWORD   vertexNum = m_dwNumVert;
    if( m_dwNumVert < m_dwNumUV )
    {
        vertexNum = m_dwNumUV;
    }

	if (FAILED(m_pDevice->CreateVertexBuffer(
		sizeof(VERTEX) * vertexNum, 0, D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1,
		D3DPOOL_MANAGED, &m_pVertexBuffer, NULL)))
	{
        MessageBox( 0, "���_�o�b�t�@�̐����Ɏ��s", fbxFileName, MB_OK );
        return  FALSE;
    }

    return  TRUE;
}

//=============================================================================
// MeshFBX::ConvertMaterial
// brief  : ���o����FBX�p�}�e���A���f�[�^��DirectX�p�}�e���A���f�[�^�ɕϊ�����
// return : ���퓮��Ȃ� TRUE �A�ُ�I���Ȃ� FALSE ��Ԃ�
//=============================================================================
BOOL    MeshFBX::ConvertMaterial()
{
    // �}�e���A���o�b�t�@�̐���
    FbxNode* pNode= m_pFbxMesh->GetNode();
    m_dwNumMaterial=pNode->GetMaterialCount();
    m_pMaterial = new MATERIAL[m_dwNumMaterial];

    // �}�e���A���̐������C���f�b�N�X�o�b�t�@�[���쐬
	m_ppIndexBuffer = new IDirect3DIndexBuffer9*[m_dwNumMaterial];
	m_pIndex = new int*[m_dwNumMaterial];

    for( DWORD i = 0;i < m_dwNumMaterial; i++ )
    {
        ZeroMemory( &m_pMaterial[i], sizeof( m_pMaterial[i] ) );
        ZeroMemory( &m_ppIndexBuffer[i], sizeof( m_ppIndexBuffer[i] ) );

        // �t�H���V�F�[�f�B���O��z�肵���}�e���A���o�b�t�@�̒��o
        FbxSurfaceMaterial* pMaterial = pNode->GetMaterial( i );
        FbxSurfacePhong* pPhong=(FbxSurfacePhong*)pMaterial;

        // �������g�U���ˌ������ʔ��ˌ��̔��ː����l���擾
        FbxDouble3  ambient = FbxDouble3( 0, 0, 0 );
        FbxDouble3  diffuse = FbxDouble3( 0, 0, 0 );
        FbxDouble3  specular = FbxDouble3( 0, 0, 0 );
        ambient  = pPhong->Ambient;
        diffuse  = pPhong->Diffuse;
        if( pMaterial->GetClassId().Is( FbxSurfacePhong::ClassId ) )
        {
            specular  = pPhong->Specular;
        }

        // �������g�U���ˌ������ʔ��ˌ��̔��ː����l���}�e���A���o�b�t�@�ɃR�s�[
        m_pMaterial[i].vAmbient  = D3DXVECTOR4( (float)ambient[0], (float)ambient[1], (float)ambient[2], 1.0f );
        m_pMaterial[i].vDiffuse  = D3DXVECTOR4( (float)diffuse[0], (float)diffuse[1], (float)diffuse[2], 1.0f );
        m_pMaterial[i].vSpecular = D3DXVECTOR4( (float)specular[0], (float)specular[1], (float)specular[2], 1.0f );

        // �e�N�X�`���[���̎擾
        FbxProperty  lProperty = pMaterial->FindProperty( FbxSurfaceMaterial::sDiffuse );
        FbxTexture * texture = FbxCast<FbxTexture>( lProperty.GetSrcObject( FbxTexture::ClassId, 0 ) );
        if( texture != NULL )
        {
            // FBX�t�@�C���̃f�B���N�g���𒊏o����
            char    dir[MAX_PATH];
            _splitpath_s( fbxFileName, NULL, 0, dir, sizeof(dir), NULL, 0, NULL, 0 );



            // �e�N�X�`���t�@�C�������΃p�X �� ���΃p�X�ɕύX����
            char    name[MAX_PATH], ext[MAX_PATH];
            _splitpath_s( texture->GetName(), 0, 0, 0, 0, name, sizeof(name), ext, sizeof(ext) );
            strcpy_s( m_pMaterial[i].textureName, dir );
            strcat_s( m_pMaterial[i].textureName, name );
            strcat_s( m_pMaterial[i].textureName, ext );

            // �e�N�X�`����ǂݍ���
            //if( FAILED( D3DX10CreateShaderResourceViewFromFileA( DirectX::GetDevice(), m_pMaterial[i].textureName,
            //                                                     NULL, NULL, &m_pMaterial[i].pTexture, NULL ) ) )

			//if (FAILED(D3DXCreateTextureFromFileEx(m_pDevice, m_pMaterial[i].textureName, 
			//	D3DX_DEFAULT, D3DX_DEFAULT, 0, 0, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT,
			//	D3DX_FILTER_LINEAR, D3DX_FILTER_LINEAR, 0x000000ff, NULL, NULL, m_pMaterial[i].pTexture)))

			if (FAILED( D3DXCreateTextureFromFile(m_pDevice, m_pMaterial[i].textureName, &m_pMaterial[i].pTexture)))
            {
                MessageBox( 0, "FBX�e�N�X�`���[�ǂݍ��ݎ��s", m_pMaterial[i].textureName, MB_OK );
				return  FALSE;
            }
        }
    }

    return  TRUE;
}

//=============================================================================
// MeshFBX::CreateIndexBuffer
// brief  : ���o�����}�e���A���f�[�^���Q�l�ɃC���f�b�N�X�o�b�t�@�𐶐�����
// return : ���퓮��Ȃ� TRUE �A�ُ�I���Ȃ� FALSE ��Ԃ�
//=============================================================================
BOOL    MeshFBX::CreateIndexBuffer()
{
    // ���o����FBX�p�}�e���A���f�[�^��DirectX�p�}�e���A���f�[�^�ɕϊ�����
    if( !ConvertMaterial() )
    {
        return  FALSE;
    }

    // �}�e���A������u�|���S�����ʁv�̏��𒊏o����
    for(DWORD i = 0;i < m_dwNumMaterial; i++)
    {
        int     count = 0;
        m_pIndex[i] = new int[ m_dwNumFace * 3 ];

        // �|���S�����\������O�p�`���ʂ��A
        // �u���_�o�b�t�@�v���̂ǂ̒��_�𗘗p���Ă��邩�𒲂ׂ�
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

        // �C���f�b�N�X�o�b�t�@�𐶐�����
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
            MessageBox( 0, "�C���f�b�N�X�o�b�t�@�̐����Ɏ��s", fbxFileName, MB_OK );
            return FALSE;
        }




        m_pMaterial[i].dwNumFace = count / 3;
        //SAFE_DELETE_ARRAY( m_pIndex );
    }

    return  TRUE;
}

//=============================================================================
// MeshFBX::Load
// brief  : fbx�t�@�C�����烂�f���f�[�^�𒊏o����
// param  : [in] filename - �t�@�C����
// return : ���퓮��Ȃ� TRUE �A�ُ�I���Ȃ� FALSE ��Ԃ�
//=============================================================================
BOOL    MeshFBX::Load_( char *filename )
{
    // �t�@�C�����̃R�s�[
    strcpy_s( fbxFileName, filename );

    // FBX�t�@�C���ǂݍ���
    if( !LoadFBX() )
    {
        return  FALSE;
    }

    return  TRUE;
}

//=============================================================================
// MeshFBX::ExtractAnim
// brief  : �A�j���[�V�����Đ������擾����
// return : ���퓮��Ȃ� TRUE �A�ُ�I���Ȃ� FALSE ��Ԃ�
//=============================================================================
BOOL    MeshFBX::ExtractAnim()
{
    if( m_pFbxMesh != NULL )
    {
        // �A�j���[�V�����̃^�C�����[�h�̎擾
        m_FrameRate = m_pFbxMesh->GetScene()->GlobalTimeSettings().GetTimeMode();

        // �A�j���[�V�����e�C�N���̎擾
        FbxArray<FbxString *>   takeNameArray;
        m_pFbxMesh->GetScene()->FillAnimStackNameArray( takeNameArray );
        DWORD   numTake = takeNameArray.GetCount();

        // �e�C�N��񂩂�u�J�n�t���[�����v�Ɓu�I���t���[�����v���擾����
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
        // �e�C�N��񂪌�����Ȃ���΁A�S�̃t���[��������Q�Ƃ���
        if( bTimeExist == FALSE )
        {
            FbxTimeSpan     timeSpan;
            m_pFbxMesh->GetScene()->GlobalTimeSettings().GetTimelineDefautTimeSpan(timeSpan);

            m_StartTime = timeSpan.GetStart();
            m_EndTime = timeSpan.GetStop();
        }

        // �P�t���[���ɂ����鎞�ԂŊ���΃t���[�����ɂȂ�
        FbxTime period;
        period.SetTime( 0, 0, 0, 1, 0, 0, m_FrameRate );
        m_StartFrame = (DWORD)( m_StartTime.Get() / period.Get() );
        m_EndFrame   = (DWORD)( m_EndTime.Get() / period.Get() );

        FbxArrayDelete( takeNameArray );

		// �X�L�����b�V���A�j���[�V�����̒��o
        if( ExtractSkinInfo() )
        {
            return  TRUE;
        }
    }

    // �q�K�w�̃f�[�^������A�j���[�V���������擾
    for( DWORD i = 0; i < m_dwNumChild; i++ )
    {
        m_ppChild[i]->ExtractAnim();
    }

    return  TRUE;
}

//=============================================================================
// MeshFBX::Update
// brief  : �A�j���[�V�������Đ�����
// param  : [in] frame - ���݂̍Đ��t���[����
// return : ���퓮��Ȃ� TRUE �A�ُ�I���Ȃ� FALSE ��Ԃ�
//=============================================================================
BOOL    MeshFBX::Update( float frame )
{
    if( m_pFbxMesh != NULL )
    {
        // �A�j���[�V�����t���[�������L�[�t���[�����͈͓̔��ɐݒ肷��
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
        // �X�L�����b�V���A�j���[�V�����̍X�V
        if( m_pSkinInfo != NULL )
        {
            // ���̏u�Ԃ̎����̎p���s��𓾂�
            FbxTime     time;
            time.SetTime( 0, 0, 0, (int)frame, 0, 0, m_FrameRate );

            // �{�[�����Ƃ̌��݂̍s����擾����
            for( int i = 0; i < m_numBone; i++ )
            {
                FbxAnimEvaluator * evaluator = m_ppCluster[i]->GetLink()->GetScene()->GetEvaluator();
                FbxMatrix mCurrentOrentation = evaluator->GetNodeGlobalTransform( m_ppCluster[i]->GetLink(), time );

                // �s��R�s�[�iFbx�`������DirectX�ւ̕ϊ��j
                for( DWORD x = 0; x < 4; x++ )
                {
                    for( DWORD y = 0; y < 4; y++ )
                    {
                        m_boneArray[i].mNewPose( x, y ) = (float)mCurrentOrentation.Get( x, y );
                    }
                }

                // FBX �͉E����W�n�Ȃ̂ŁA�w�������������t�]������
                m_boneArray[i].mNewPose( 0, 0 ) *= -1.0f;
                m_boneArray[i].mNewPose( 0, 1 ) *= -1.0f;
                m_boneArray[i].mNewPose( 0, 2 ) *= -1.0f;

                // �I�t�Z�b�g���̃|�[�Y�̍������v�Z����
                D3DXMatrixInverse( &m_boneArray[i].mDiffPose, NULL, &m_boneArray[i].mBindPose );
                m_boneArray[i].mDiffPose *= m_boneArray[i].mNewPose;
            }

            // �e�{�[���ɑΉ��������_�̕ό`����
            DWORD       vbSize = m_dwNumVert;
            if( m_dwNumVert < m_dwNumUV )
            {
                vbSize = m_dwNumUV;
            }
            for( DWORD i = 0; i < vbSize; i++ )
            {
                // �e���_���ƂɁA�u�e������{�[���~�E�F�C�g�l�v�𔽉f�������֐ߍs����쐬����
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
                // �쐬���ꂽ�֐ߍs����g���āA���_��ό`����
                D3DXVECTOR3 Pos = m_weightArray[i].vPosOrigin;
                D3DXVECTOR3 Normal = m_weightArray[i].vNormalOrigin;
                Pos.x *= -1.0f;     // FBX �͉E����W�n�Ȃ̂ŁA�w�������������t�]������
                Normal.x *= -1.0f;  // FBX �͉E����W�n�Ȃ̂ŁA�w�������������t�]������
                D3DXVec3TransformCoord( &m_pVertices[i].vPos, &Pos, &matrix );
                D3DXVec3TransformNormal( &m_pVertices[i].vNormal, &Normal, &matrix );
            }

            // ���_�o�b�t�@�����b�N���āA�ό`��������̒��_���ŏ㏑������
            VERTEX * pv;
			if (m_pVertexBuffer != NULL && SUCCEEDED(m_pVertexBuffer->Lock(0, 0, (void**)&pv, D3DLOCK_DISCARD)))
            {
                memcpy( pv, m_pVertices, sizeof(VERTEX) * vbSize );
                m_pVertexBuffer->Unlock();
            }
        }

        // �K�w���b�V���A�j���[�V�����̍X�V
        else
        {
            // ���̏u�Ԃ̎����̎p���s��𓾂�
            FbxTime     time;
            time.SetTime( 0, 0, 0, (int)frame, 0, 0, m_FrameRate );
            FbxAnimEvaluator *evaluator = m_pFbxMesh->GetScene()->GetEvaluator();
            FbxMatrix mCurrentOrentation = evaluator->GetNodeGlobalTransform(m_pFbxNode,time);

            // �s��R�s�[�iFbx�`������DirectX�ւ̕ϊ��j
            for( DWORD x = 0; x < 4; x++ )
            {
                for( DWORD y = 0; y < 4; y++ )
                {
                    mLocalMatrix( x, y ) = (float)mCurrentOrentation.Get( x, y );
                }
            }

            // FBX �͉E����W�n�Ȃ̂ŁA�w�������������t�]������
            mLocalMatrix( 0, 0 ) *= -1.0f;
            mLocalMatrix( 0, 1 ) *= -1.0f;
            mLocalMatrix( 0, 2 ) *= -1.0f;
        }
    }

    return  TRUE;
}


//=============================================================================
// MeshFBX::ExtractSkinInfo
// brief  : fbx����X�L�����b�V�����𒊏o����
// return : ���퓮��Ȃ� TRUE �A�ُ�I���܂��͔�X�L�����b�V���f�[�^�Ȃ� FALSE ��Ԃ�
//=============================================================================
BOOL    MeshFBX::ExtractSkinInfo()
{
    // �f�t�H�[�}���i�{�[���ƃ��f���̊֘A�t���j�̎擾
    FbxDeformer *   pDeformer = m_pFbxMesh->GetDeformer(0);
    if( pDeformer == NULL )
    {
        return  FALSE;
    }

    // �f�t�H�[�}��񂩂�X�L�����b�V�������擾
    m_pSkinInfo = (FbxSkin *)pDeformer;

    // ���_����|���S�����t�������邽�߂̏����쐬����
    struct  POLY_INDEX
    {
        int *   polyIndex;      // �|���S���̔ԍ�
        int *   vertexIndex;    // ���_�̔ԍ�
        int     numRef;         // ���_�����L����|���S���̐�
    };
    POLY_INDEX * polyTable = new POLY_INDEX[ m_dwNumVert ];
    for( DWORD i = 0; i < m_dwNumVert; i++ )
    {
        // �O�p�`�|���S���ɍ��킹�āA���_�ƃ|���S���̊֘A�����\�z����
        // �����_�����|���S�����~�R���_
        polyTable[i].polyIndex = new int[m_dwNumFace * 3];
        polyTable[i].vertexIndex = new int[m_dwNumFace * 3];
        polyTable[i].numRef = 0;
        ZeroMemory( polyTable[i].polyIndex, sizeof(int) * m_dwNumFace * 3 );
        ZeroMemory( polyTable[i].vertexIndex, sizeof(int) * m_dwNumFace * 3 );

        // �|���S���Ԃŋ��L���钸�_��񋓂���
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

    // �{�[�������擾����
    m_numBone = m_pSkinInfo->GetClusterCount();
    m_ppCluster = new FbxCluster*[m_numBone];
    for( int i = 0; i < m_numBone; i++ )
    {
        m_ppCluster[i] = m_pSkinInfo->GetCluster(i);
    }

    // �{�[���̐��ɍ��킹�ăE�F�C�g������������
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



    // ���ꂼ��̃{�[���ɉe�����󂯂钸�_�𒲂ׂ�
    // ��������t�ɁA���_�x�[�X�Ń{�[���C���f�b�N�X�E�d�݂𐮓ڂ���
    for( int i = 0;i < m_numBone; i++ )
    {
        int numIndex = m_ppCluster[i]->GetControlPointIndicesCount();   //���̃{�[���ɉe�����󂯂钸�_��
        int * piIndex = m_ppCluster[i]->GetControlPointIndices();       //�{�[��/�E�F�C�g���̔ԍ�
        double * pdWeight=m_ppCluster[i]->GetControlPointWeights();     //���_���Ƃ̃E�F�C�g���

        //���_������C���f�b�N�X�����ǂ��āA���_�T�C�h�Ő�������
        
		
		for( int k = 0; k < numIndex; k++ )
        {
            //�ʏ�̏ꍇ�@�i���_��>=UV���@pvVB�����_�C���f�b�N�X�x�[�X�̏ꍇ�j
            if(m_dwNumVert >= m_dwNumUV)
            {
                // ���_�Ɋ֘A�t����ꂽ�E�F�C�g��񂪃{�[���T�{�ȏ�̏ꍇ�́A�d�݂̑傫�����ɂS�{�ɍi��
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
            //UV�C���f�b�N�X�x�[�X�̏ꍇ�@�i���_��<UV���j
            else
            {
                //���̒��_���܂�ł���|���S�����ׂĂɁA���̃{�[���ƃE�F�C�g��K�p
                for( int p = 0; p < polyTable[ piIndex[k] ].numRef; p++ )
                {
                    //���_�������|���S�������̃|���S����UV�C���f�b�N�X�@�Ƌt����
                    int polyIndex = polyTable[ piIndex[k] ].polyIndex[p];
                    int vertIndex = polyTable[ piIndex[k] ].vertexIndex[p];
                    int uvIndex = m_pFbxMesh->GetTextureUVIndex( polyIndex, vertIndex, FbxLayerElement::eTextureDiffuse );

                    // ���_�Ɋ֘A�t����ꂽ�E�F�C�g��񂪃{�[���T�{�ȏ�̏ꍇ�́A�d�݂̑傫�����ɂS�{�ɍi��
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

    //�{�[���𐶐�
    m_boneArray = new BONE[m_numBone];
    for( int i = 0; i < m_numBone; i++ )
    {
        // �{�[���̃f�t�H���g�ʒu���擾����
        FbxAMatrix  matrix;
        m_ppCluster[i]->GetTransformLinkMatrix( matrix );

        // �s��R�s�[�iFbx�`������DirectX�ւ̕ϊ��j
        for( DWORD x = 0; x < 4; x++ )
        {
            for( DWORD y = 0; y < 4; y++ )
            {
                m_boneArray[i].mBindPose( x, y ) = (float)matrix.Get( x, y );
            }
        }

        // FBX �͉E����W�n�Ȃ̂ŁA�w�������������t�]������
        m_boneArray[i].mBindPose( 0, 0 ) *= -1.0f;
        m_boneArray[i].mBindPose( 0, 1 ) *= -1.0f;
        m_boneArray[i].mBindPose( 0, 2 ) *= -1.0f;
    }

    // �ꎞ�I�ȃ������̈���������
    for( DWORD i = 0; i < m_dwNumVert; i++ )
    {
        SAFE_DELETE_ARRAY( polyTable[i].polyIndex );
        SAFE_DELETE_ARRAY( polyTable[i].vertexIndex );
    }
    SAFE_DELETE_ARRAY( polyTable );

    return  TRUE;
}

