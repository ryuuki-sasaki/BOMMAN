//������������������������������������������������������������������������������
// file : Model.h
// brief : �R�c�I�u�W�F�N�g����w�b�_�t�@�C��
//������������������������������������������������������������������������������

//������������������������������������������������������������������������������
// �C���N���[�h�K�[�h�̊J�n�錾
//������������������������������������������������������������������������������
#ifndef _MODEL_H_
#define _MODEL_H_

//������������������������������������������������������������������������������
// �K�v�ȃw�b�_�t�@�C���̃C���N���[�h
//������������������������������������������������������������������������������
#include "MeshFBX.h"        // MeshFBX �N���X�̎Q��


//��������������������������������������������
//�K�v�ȃ��C�u�����t�@�C���̃��[�h
//��������������������������������������������
#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")
#pragma comment(lib,"dxguid.lib")

#if defined(DEBUG) | defined(_DEBUG)
#pragma comment(lib,"fbxsdk-2013.3-mtd.lib")
#else
#pragma comment(lib,"fbxsdk-2013.3-mt.lib")
#endif


//������������������������������������������������������������������������������
// Model�N���X�F���f���f�[�^�̐�����Ǘ�
//������������������������������������������������������������������������������
class   Model : public MeshFBX
{
	char	m_filename[MAX_PATH];	// ���f���t�@�C����
    float   m_Frame;				// �A�j���[�V�����̃t���[����

	//private�֐�
    BOOL	InitPolygon();				// �|���S���̏�����
    BOOL	DrawSub( MeshFBX * pMesh );	// �K�w���Ƃ̃|���S���̕`��

public:
    Model();    // �R���X�g���N�^
    ~Model();   // �f�X�g���N�^
	BOOL	Load(LPDIRECT3DDEVICE9 pDev, char* n);	//�t�@�C���ǂݍ���
	BOOL	Update( float animSpeed );				//�A�j���[�V�����̃t���[���X�V
	BOOL	Draw(D3DXMATRIX* matWorld = NULL);				//�`��(= NULL�͈����Ȃ��̂Ƃ��f�t�H���g��NULL)

	
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

//������������������������������������������������������������������������������
// �C���N���[�h�K�[�h�̏I���錾
//������������������������������������������������������������������������������
#endif  _MODEL_H_
 