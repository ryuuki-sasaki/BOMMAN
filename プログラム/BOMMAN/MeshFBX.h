//������������������������������������������������������������������������������
// file : MeshFBX.h
// brief : .fbx�t�@�C������w�b�_�t�@�C��
//������������������������������������������������������������������������������

//������������������������������������������������������������������������������
// �C���N���[�h�K�[�h�̊J�n�錾
//������������������������������������������������������������������������������
#ifndef _MESHFBX_H_
#define _MESHFBX_H_

#define FBXSDK_NEW_API

//������������������������������������������������������������������������������
// �K�v�ȃw�b�_�t�@�C���̃C���N���[�h
//������������������������������������������������������������������������������
#include <d3d9.h>
#include <fbxsdk.h>

#include <windows.h>
#include <mmsystem.h>
#include <d3dx9.h>
#include <list>

//������������������������������������������������������������������������������
// MeshFBX�N���X�FFBX�ǂݍ��ݐ���S�ʂ��Ǘ�
//������������������������������������������������������������������������������
class   MeshFBX
{
	// FBX�֘A�f�[�^
    char            fbxFileName[256];   // FBX�t�@�C����
    FbxManager *    m_pSdkManager;      // FBX��������
    FbxImporter *   m_pImporter;        // FBX�p�C���|�[�g����
    FbxScene *      m_pMyScene;         // FBX�p�V�[������
public:
    FbxMesh *       m_pFbxMesh;         // FBX�p�|���S�����b�V������

protected:
	LPDIRECT3DDEVICE9 m_pDevice;

    // �{�[���\���́i�֐ߏ��j
    struct  BONE
    {
        D3DXMATRIX  mBindPose;      // �����|�[�Y���̃{�[���ϊ��s��
        D3DXMATRIX  mNewPose;       // �A�j���[�V�����ŕω������Ƃ��̃{�[���ϊ��s��
        D3DXMATRIX  mDiffPose;      // mBindPose �ɑ΂��� mNowPose �̕ω���
    };

    // �E�F�C�g�\���́i�{�[���ƒ��_�̊֘A�t���j
    struct WEIGHT
    {
        D3DXVECTOR3 vPosOrigin;     // ���X�̒��_���W
        D3DXVECTOR3 vNormalOrigin;  // ���X�̖@���x�N�g��
        int *       boneIndex;      // �֘A����{�[����ID
        float *     boneWeight;     // �{�[���̏d��
    };

    // �{�[��������
    FbxSkin *       m_pSkinInfo;    // �X�L�����b�V�����i�X�L�����b�V���A�j���[�V�����̃f�[�^�{�́j
    FbxCluster **   m_ppCluster;    // �N���X�^���i�֐߂��ƂɊ֘A�t����ꂽ���_���j
    int             m_numBone;      // FBX�Ɋ܂܂�Ă���֐߂̐�
    BONE *          m_boneArray;    // �e�֐߂̏��
    WEIGHT *        m_weightArray;  // �E�F�C�g���i���_�̑΂���e�֐߂̉e���x�����j


private:
    // �A�j���[�V����������
    FbxNode *       m_pFbxNode;         // FBX�p�A�j���[�V�����m�[�h����
    char            fbxNodeName[256];   // FBX�m�[�h��

    // �A�j���[�V�����Đ����
    FbxTime::EMode  m_FrameRate;        // �A�j���[�V�����̃t���[�����[�g
    FbxTime         m_Period;           // �A�j���[�V�����̂P�t���[������
    FbxTime         m_StartTime;        // �A�j���[�V�����J�n����
    FbxTime         m_EndTime;          // �A�j���[�V�����I������
    DWORD           m_StartFrame;       // �A�j���[�V�����J�n�t���[����
    DWORD           m_EndFrame;         // �A�j���[�V�����I���t���[����

protected:
    // �K�w���b�V�����
    DWORD           m_dwNumChild;       // �K�w���b�V���I�u�W�F�N�g�̐�
    MeshFBX **      m_ppChild;          // �K�w���b�V���I�u�W�F�N�g

    // ���W�ϊ��s��
    D3DXMATRIX      mLocalMatrix;       // �e���ʂ̃��[�J�����W�ϊ��s��

public:
    // ���_�o�b�t�@�\����
    struct VERTEX
    {
        D3DXVECTOR3 vPos;    // ���_���W
        D3DXVECTOR3 vNormal; // �@���x�N�g��
        D3DXVECTOR2 vTex;    // �e�N�X�`�����W
    };

    // DirectX�|���S�����b�V���֘A
    VERTEX *            m_pVertices;        // ���_�f�[�^
	int **				m_pIndex;
	IDirect3DVertexBuffer9 *      m_pVertexBuffer;    // ���_�o�b�t�@
	IDirect3DIndexBuffer9 **     m_ppIndexBuffer;    // �C���f�b�N�X�o�b�t�@
    DWORD               m_dwNumVert;        // ���b�V���Ɋ܂܂�钸�_�̐�
    DWORD               m_dwNumFace;        // ���b�V���Ɋ܂܂��ʂ̐�
    DWORD               m_dwNumUV;          // ���b�V���Ɋ܂܂��e�N�X�`��UV���W�̐�

    // �}�e���A���o�b�t�@�\����
    struct  MATERIAL
    {
        char                        materialName[256];  // �}�e���A����
        DWORD                       dwNumFace;          // �}�e���A���̃|���S����
        char                        textureName[256];   // �e�N�X�`���t�@�C����
		LPDIRECT3DTEXTURE9 			pTexture;           // �e�N�X�`���f�[�^
        D3DXVECTOR4                 vAmbient;           // �����ւ̔��ˋ��x
        D3DXVECTOR4                 vDiffuse;           // �g�U���ˌ��ւ̔��ˋ��x
        D3DXVECTOR4                 vSpecular;          // ���ʔ��ˌ��ւ̔��ˋ��x
    };

    // DirectX�}�e���A���֘A
    MATERIAL *      m_pMaterial;        // �}�e���A���f�[�^
    DWORD           m_dwNumMaterial;    // �}�e���A���o�b�t�@�̐�

private:
    // FBX�t�@�C���̓ǂݍ���
    BOOL    LoadFBX();              // FBX�t�@�C���ǂݍ���
    BOOL    ExtractMesh( FbxNode * pNode ); // FBX����|���S�����b�V���𒊏o
	BOOL    ExtractAnim();                  // FBX����A�j���[�V�������𒊏o
	BOOL    ExtractSkinInfo();              // FBX����X�L�����b�V�����𒊏o����


    // �|���S�����̒��o�ƒ��_�o�b�t�@�̐���
    BOOL    ConvertMesh();          // ���o�����|���S�����b�V����DirectX�p�ɕϊ�
    BOOL    CreateVertexBuffer();   // ���_�o�b�t�@���C���f�b�N�X�o�b�t�@�𐶐�

    // �}�e���A�����̒��o�ƃ}�e���A���o�b�t�@���C���f�b�N�X�o�b�t�@�̐���
    BOOL    ConvertMaterial();      // ���o�����}�e���A����DirectX�p�ɕϊ�
    BOOL    CreateIndexBuffer();    // �}�e���A���o�b�t�@���C���f�b�N�X�o�b�t�@�𐶐�


public:
    MeshFBX();      // �R���X�g���N�^
    ~MeshFBX();     // �f�X�g���N�^

	BOOL    Load_( char * filename );    // �t�@�C���̓ǂݍ���
	BOOL    Update( float frame );              // �A�j���[�V�����̍X�V

    // �A�N�Z�X�֐�
	IDirect3DVertexBuffer9 *  GetVertexBuffer()   { return m_pVertexBuffer; } // ���_�o�b�t�@�̎擾
    DWORD					GetNumMaterials()   { return m_dwNumMaterial; } // �}�e���A���f�[�^�̑����擾
    MATERIAL *				GetMaterialBuffer() { return m_pMaterial; }     // �}�e���A���o�b�t�@�̎擾
	IDirect3DIndexBuffer9 ** GetIndexBuffer()    { return m_ppIndexBuffer; } // �C���f�b�N�X�o�b�t�@�̎擾
    DWORD					GetNumChild()       { return m_dwNumChild; }    // �K�w���b�V���f�[�^�̑����擾
    MeshFBX **				GetChild()          { return m_ppChild; }       // �K�w���b�V���I�u�W�F�N�g�̎擾
    D3DXMATRIX				GetLocalMatrix()    { return mLocalMatrix; }    // ���[�J�����W�ϊ��s��̎擾
	DWORD					GetNumVertexNum(){ return m_dwNumVert; }
	VERTEX *				GetVertex(){ return	m_pVertices; }
	int **					GetIndex(){ return m_pIndex; }
	VOID					SetDevice(LPDIRECT3DDEVICE9 pDevice){ m_pDevice = pDevice; }
};

//������������������������������������������������������������������������������
// �C���N���[�h�K�[�h�̏I���錾
//������������������������������������������������������������������������������
#endif  _MESHFBX_H_