
//=============================================================================
// file : Model.cpp
// brief : �R�c�I�u�W�F�N�g����\�[�X�t�@�C��
//=============================================================================

//=============================================================================
// �K�v�ȃw�b�_�t�@�C���̃C���N���[�h
//=============================================================================
#include "Model.h"          // Model �N���X


//=============================================================================
// Model::Model
// brief : �R���X�g���N�^
//=============================================================================
Model::Model()
{
	// �ϐ��̏�����
	ZeroMemory(this, sizeof(Model));
}


//=============================================================================
// Model::~Model
// brief : �f�X�g���N�^
//=============================================================================
Model::~Model()
{

}

//=============================================================================
// Model::Load
// brief  : ���f���ǂݍ���
// param  : [in] pDev - �f�o�C�X�C���^�[�t�F�[�X
// param  : [in] fname - �t�@�C����
// return : ���퓮��Ȃ� TRUE �A�ُ�I���Ȃ� FALSE ��Ԃ�
//=============================================================================
BOOL Model::Load(LPDIRECT3DDEVICE9 pDev, char* fname)
{
	// �����o�ϐ��ւ̃R�s�[
	strcpy_s(m_filename, fname);
	m_pDevice = pDev;

	// �|���S���̐�������
	if (!InitPolygon())
	{
		return E_FAIL;
	}

	return S_OK;
}

//=============================================================================
// Model::InitPolygon
// brief  : �|���S���̐�������
// return : ���퓮��Ȃ� TRUE �A�ُ�I���Ȃ� FALSE ��Ԃ�
//=============================================================================
BOOL    Model::InitPolygon()
{
	// ���b�V���̏�����
	if (!Load_(m_filename))
	{
		return  FALSE;
	}

	return TRUE;
}

//=============================================================================
// Model::Draw
// brief  : �|���S���̕`�揈��
// param  : [in] matWorld - ���[���h�s��
// return : ���퓮��Ȃ� TRUE �A�ُ�I���Ȃ� FALSE ��Ԃ�
//=============================================================================
BOOL    Model::Draw(D3DXMATRIX* matWorld)
{
	//�����ȗ����͒P�ʍs��
	if (matWorld == NULL)
	{
		D3DXMatrixIdentity(matWorld);
	}

	m_pDevice->SetTransform(D3DTS_WORLD, matWorld);
	return  DrawSub(this);
}



//=============================================================================
// Model::DrawSub
// brief  : �|���S���̕`�揈��
// param  : [in] pMesh - �����ʂ��Ƃ̃��b�V���f�[�^
// return : ���퓮��Ȃ� TRUE �A�ُ�I���Ȃ� FALSE ��Ԃ�
//=============================================================================
BOOL    Model::DrawSub(MeshFBX *pMesh)
{
	// �A�j���[�V�����̃t���[���X�V�ݒ�
	pMesh->Update(m_Frame);
	
	//���_�o�b�t�@
	if (pMesh->GetVertexBuffer())
	{
		//���_�̃������𒼂ŃR�s�[����
		VERTEX *vTop;
		pMesh->GetVertexBuffer()->Lock(0, 0, (void**)&vTop, 0);
		memcpy(vTop, pMesh->GetVertex(), sizeof(VERTEX)* pMesh->GetNumVertexNum());
		pMesh->GetVertexBuffer()->Unlock();

		for (DWORD i = 0; i < pMesh->GetNumMaterials(); i++)
		{
			//���_�C���f�b�N�X
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


				//���_�C���f�b�N�X�w��
				m_pDevice->SetIndices(pMesh->GetIndexBuffer()[i]);

				//�g�p�X�g���[���w��ƒ��_�o�b�t�@�ݒ�
				m_pDevice->SetStreamSource(0, pMesh->GetVertexBuffer(), 0, sizeof(VERTEX));


				//�}�e���A��
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



				//�`��(�ʓ�ƂO����n�܂钸�_�Ǝw��)
				m_pDevice->DrawIndexedPrimitive(
					D3DPT_TRIANGLELIST,
					0,	//�J�n�n�_����̑��Έʒu
					0,	//�ŏ��C���f�b�N�X�ԍ�
					pMesh->GetNumVertexNum() * 3,	//�ő�C���f�b�N�X�ԍ�
					0,	//�C���f�b�N�X�z��̓ǂݎ��ʒu	
					pMesh->GetMaterialBuffer()[i].dwNumFace	//�ʂ̐�
					);
			}
		}
	}

	// �q�K�w�I�u�W�F�N�g�ɑ΂��Ă��`�揈�������s
	for (DWORD i = 0; i < pMesh->GetNumChild(); i++)
	{
		DrawSub(pMesh->GetChild()[i]);
	}


	return  TRUE;
}

//=============================================================================
// Model::Update
// brief  : �|���S���̃t���[���X�V
// param  : [in] animSpeed - �A�j���[�V�����̍Đ����x
// return : ���퓮��Ȃ� TRUE �A�ُ�I���Ȃ� FALSE ��Ԃ�
//=============================================================================
BOOL    Model::Update(float animSpeed)
{
	// �A�j���[�V�������w��X�e�b�v�������i�߂�
	m_Frame += animSpeed;
	return  TRUE;
}

//=============================================================================
// Model::HitRay
// brief  : ���C���΂�
// param  : [in] pMesh - �����ʂ��Ƃ̃��b�V���f�[�^
// param  : [in] pRayPos - ���C�̔��ˏꏊ
// param�@: [in] pRayDir - ���C�̔��ˌ���
// param  : [in] pDist	 - ���������Ƃ��̋���
// return : ���퓮��Ȃ� TRUE �A�ُ�I���Ȃ� FALSE ��Ԃ�
//=============================================================================
BOOL Model::HitRay(MeshFBX* pMesh,D3DXVECTOR3* pRayPos,D3DXVECTOR3* pRayDir,float* pDist,D3DXVECTOR3* pNormal)	
{
	//�}�e���A���̐��������[�v
	for(DWORD j = 0; j < pMesh->m_dwNumMaterial; j++)
	{
		int count = 0;

		//���i���ƂɊ֐����Ă΂�A���̑f�ނ����̕��i�łł��Ă��邩���ׂ�
		//�|���S���̐��������[�v
		for(DWORD i = 0; i < pMesh->m_dwNumFace; i++)
		{
			//���̃|���S���̃}�e���A���𒲂ׂ�
			FbxLayerElementMaterial* mtl = pMesh->m_pFbxMesh->GetLayer(0)->GetMaterials();
			int mtlId = mtl->GetIndexArray().GetAt(i);
			//�}�e���A����j��������
			if(mtlId == j )
			{
			D3DXVECTOR3 v1 = pMesh->m_pVertices[pMesh->m_pIndex[j][count+0]].vPos;	//j�Ԗڂ̑f�ނłł��Ă��鉽�Ԗڂ̃|���S��
			D3DXVECTOR3 v2 = pMesh->m_pVertices[pMesh->m_pIndex[j][count+1]].vPos;
			D3DXVECTOR3 v3 = pMesh->m_pVertices[pMesh->m_pIndex[j][count+2]].vPos;

			float dist;
			BOOL hit = D3DXIntersectTri(&v1,&v2,&v3,pRayPos,pRayDir,NULL,NULL,&dist);

			if(hit)	//���C�����������Ƃ� (pDisut..�O��̋����ADist..����̋����j
				{
					if(dist < *pDist)
					{
						*pDist = dist;

						//�@�����߂�
						if(pNormal != NULL)
						{
							D3DXVECTOR3 a = v2 - v1;
							D3DXVECTOR3 b = v3 - v1;

							D3DXVec3Cross(pNormal, &a, &b);			//�O�ς����߂�v�Z
							D3DXVec3Normalize(pNormal, pNormal);	//���K��
						}
					}
				}
				count += 3;
			}
		}
	}	
	//�q���b�V�������ׂ�(�����ɂ���̂͐e�Ɠ�����Ȃ������ꍇ�j
	for(DWORD i = 0; i < pMesh->GetNumChild(); i++)
	{
		HitRay(pMesh->GetChild()[i], pRayPos, pRayDir, pDist,pNormal);
	}

	return FALSE;
}

//m_dwNumFace...�|���S���̖ʂ̐�
//Maya�ł͐e�q�֌W������ꍇ������-��MeshFBX* pMesh...pMesh�̒��ɍ����ׂ悤�Ƃ��Ă���i�e��q�́j���i������C���[�W
//=============================================================================
// Model::HitRay�i�ŏ��̌Ăяo���p�j
// brief  : ���C���΂�
// param  : [in] pRayPos - ���C�̔��ˏꏊ
// param�@: [in] pRayDir - ���C�̔��ˌ���
// param  : [in] pDist	 - ���������Ƃ��̋���
// return : ���퓮��Ȃ� TRUE �A�ُ�I���Ȃ� FALSE ��Ԃ�
//=============================================================================
BOOL Model::HitRay(D3DXVECTOR3* pRayPos,D3DXVECTOR3* pRayDir,float* pDist,D3DXVECTOR3* pNormal)
{

	*pDist = 99999;		//���肦�Ȃ��������Ă���
	/*for(int i = 0; i < 3; i++)
	{	
		for(int j = 0; j < 3; j++) 
		{*/
			HitRay(this, pRayPos, pRayDir, pDist,pNormal);	//�����T��HitRay�Ăяo��
		/*}
	}*/
	if(*pDist < 99999)
		return TRUE;
	else
		return FALSE;
}

//=============================================================================
// Model::HitRay�i�ŏ��̌Ăяo���p�j
// brief  : ���C���΂�
// param  : [in] pRayPos - ���C�̔��ˏꏊ
// param�@: [in] pRayDir - ���C�̔��ˌ���
// param  : [in] pDist	 - ���������Ƃ��̋���
// return : ���퓮��Ȃ� TRUE �A�ُ�I���Ȃ� FALSE ��Ԃ�
//=============================================================================
BOOL Model::HitRay(D3DXVECTOR3* pRayPos,D3DXVECTOR3* pRayDir,float* pDist)
{
	return HitRay(pRayPos, pRayDir, pDist, NULL);
}


//BOOL Model::HitRay(D3DXVECTOR3* pRayPos,D3DXVECTOR3* pRayDir,D3DXVECTOR3* pNormal)
//{
//	return HitRay(pRayPos, pRayDir, NULL, pNormal);
//}