//=============================================================================
//  �O���E���h
//�@�X�V���F2015/8/31
//	����ҁF���X�ؗ��M
//=============================================================================
#include "Ground.h"
#include "Camera.h"
#include "modeChange.h"
#include "Poly.h"
#include "Model.h"

LPD3DXEFFECT	 pEffect;	//�V�F�[�_�����邽�߂̕ϐ�

//=============================================================================
//�R���X�g���N�^
//=============================================================================
Ground::Ground(void)
{
	board = new Model;
	table = new Model;
	tableFrame = new Model;
	floor = new Model;
	backGround = new Model;
	//���f���ǂݍ���
	board->Load(graphics->get3Ddevice(),"Models\\board.fbx");
	table->Load(graphics->get3Ddevice(),"Models\\table.fbx");
	tableFrame->Load(graphics->get3Ddevice(),"Models\\table waku.fbx");
	floor->Load(graphics->get3Ddevice(),"Models\\floor.fbx");
	backGround->Load(graphics->get3Ddevice(),"Models\\backGround.fbx");
	//�ʒu
	pos = D3DXVECTOR3(25,-100,25);

	//�e�N�X�`���ǂݍ���
	texture.initialize(graphics,"pictures\\backGround.png");
	image.initialize(graphics,1000,1000,0,&texture);
	//image�w�b�_�̃A�N�Z�X�֐���true��n��
	image.SetIsBackGround(true);

	//�G�t�F�N�g�ǂݍ���...�G�t�F�N�g�t�@�C����ǂݍ���ŕϐ��ɓ����
	LPD3DXBUFFER err = 0;

	//�Ō�̈����ɃG���[��񂪓���
	//FAILED...���s������
	if(FAILED (D3DXCreateEffectFromFile(
		graphics -> get3Ddevice(),
		"Shader.fx",
		NULL,NULL,
		D3DXSHADER_DEBUG,
		NULL,
		&pEffect,
		&err) ) )
	{
		//�G���[�̂Ƃ��ɏo�����b�Z�[�W
		char mes[1024];
		strcpy_s( mes, (char*)err -> GetBufferPointer() );

		//���b�Z�[�W�{�b�N�X���o�� (MB...���b�Z�[�W�{�b�N�X�j
		MessageBox( NULL, mes, "�V�F�[�_�[�G���[", MB_OK );
	}
}

//=============================================================================
//�f�X�g���N�^
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
//�`��
//�����@�t���[���A�J�����ԍ�
//�߂�l�@�Ȃ�
//=============================================================================
void Ground::render(float frameTime, int cameraNum)
{
	//�s��
	D3DXMATRIX mWorld, mFroorTrans, mBackTrans;
	D3DXMatrixIdentity(&mWorld);
	//�ړ��s��
	D3DXMatrixTranslation(&mFroorTrans,pos.x, pos.y, pos.z);
	D3DXMatrixTranslation(&mBackTrans,pos.x, 0, pos.z);
	//���f���`��
	board->Draw(&mWorld);
	tableFrame->Draw(&mWorld);
	backGround->Draw(&mBackTrans);
	floor->Draw(&mFroorTrans);
	
	//�e�N�X�`���`��
	image.draw();

	D3DXMATRIX mat;
	//�ړ��s��
	D3DXMatrixTranslation(&mat,0, -0.5f, 0);

	if(cameraNum == 1 || modeChange::GetInstance() -> getMode() == VSCPU/*�ΐ탂�[�h��CPU�ΐ�*/) {
		//���[���h�A�r���[�A�ˉe�s��
		//Test.fx�̃O���[�o���ϐ��ɒl��n��
		//�s����V�F�[�_�ɓn��...SetMatrix
		pEffect -> SetMatrix("g_matWVP", 
			&( mat* cameraA -> GetView() * cameraA -> GetProj() ) );

		//�J�����̈ʒu
		pEffect->SetVector("g_vecEye", (D3DXVECTOR4*)&cameraA->GetPos());
	} else {
		pEffect -> SetMatrix("g_matWVP", 
			&( mat* cameraB -> GetView() * cameraB -> GetProj() ) );

		pEffect->SetVector("g_vecEye", (D3DXVECTOR4*)&cameraB->GetPos());
	}


	//���[���h�̋t�s��
	D3DXMATRIX matInvW;
	D3DXMatrixInverse(&matInvW, 0, &mat); //mat�̋t�s��matInvW�ɓ���
	D3DXMatrixTranspose(&matInvW, &matInvW); //�s�Ɨ���Ђ�����Ԃ�
	pEffect->SetMatrix("g_matInvW",&matInvW);

	//���C�g�̕����i���̂��猩��)
	D3DXVECTOR4 light(1, 1, 1, 1);	//�E���O����
	D3DXVec4Normalize(&light, &light); //���K��

	pEffect->SetVector("g_vecLightDir",&light);

	//�`��J�n
	pEffect -> Begin(NULL, 0);
	//�p�X�J�n
	pEffect -> BeginPass(0);
	//�`��
	table->Draw(&mat);
	//�p�X�I��
	pEffect -> EndPass();
	//�`��I��
	pEffect -> End();
}