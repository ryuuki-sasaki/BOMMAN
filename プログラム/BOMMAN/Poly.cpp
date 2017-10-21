#include "Poly.h"

//=============================================================================
// �R���X�g���N�^
//=============================================================================
Poly::Poly(void)
{
}

//=============================================================================
// �f�X�g���N�^
//=============================================================================
Poly::~Poly(void)
{
}

//=============================================================================
// ������
// �����F�Ȃ�
// �ߒl�F�Ȃ�
//=============================================================================
void Poly::initialize(Graphics *g,TextureManager *tm)
{
	//3���_�w��i���v���j(������1�j
	//�F�i(16�i���̏ꍇ�j0x(�����x(�A���t�@�l�j2���j,RGB(�e2���j�A�i10�i���̏ꍇ�jD3DCORLOR_ARGB�j
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
// BOX�|���S���p������
// �����F�Ȃ�
// �ߒl�F�Ȃ�
//=============================================================================
void Poly::boxInitialize(Graphics *g,TextureManager *tm)
{
	//3���_�w��i���v���j(������1�j
	//�F�i(16�i���̏ꍇ�j0x(�����x(�A���t�@�l�j2���j,RGB(�e2���j,�i10�i���̏ꍇ�jD3DCORLOR_ARGB�j
	//�O�ʂ̋�`
	Vertex v1[4]=
	{ D3DXVECTOR3(0.5, -0.5, -0.5),  D3DCOLOR_ARGB(255, 255, 255, 255), D3DXVECTOR2(0, 0),
	  D3DXVECTOR3(-0.5, -0.5, -0.5),  D3DCOLOR_ARGB(255, 255, 255, 255), D3DXVECTOR2(1, 0),
	  D3DXVECTOR3(0.5, 0.5, -0.5), D3DCOLOR_ARGB(255, 255, 255, 255), D3DXVECTOR2(0, 1),
	  D3DXVECTOR3(-0.5, 0.5, -0.5),  D3DCOLOR_ARGB(255, 255, 255, 255), D3DXVECTOR2(1, 1)
	};
	//�E���ʂ̋�`
	Vertex v2[4]=
	{ D3DXVECTOR3(0.5, -0.5, 0.5),  D3DCOLOR_ARGB(255, 255, 255, 255), D3DXVECTOR2(0, 0),
	  D3DXVECTOR3(0.5, -0.5, -0.5),	  D3DCOLOR_ARGB(255, 255, 255, 255), D3DXVECTOR2(1, 0),
	  D3DXVECTOR3(0.5, 0.5, 0.5), D3DCOLOR_ARGB(255, 255, 255, 255), D3DXVECTOR2(0, 1),
	  D3DXVECTOR3(0.5, 0.5, -0.5),  D3DCOLOR_ARGB(255, 255, 255, 255), D3DXVECTOR2(1, 1)
	};
	//��ʂ̋�`
	Vertex v3[4]=
	{ D3DXVECTOR3(0.5, 0.5, 0.5),  D3DCOLOR_ARGB(255, 255, 255, 255), D3DXVECTOR2(0, 0),
	  D3DXVECTOR3(-0.5, 0.5, 0.5),	  D3DCOLOR_ARGB(255, 255, 255, 255), D3DXVECTOR2(1, 0),
	  D3DXVECTOR3(0.5, 0.5, -0.5), D3DCOLOR_ARGB(255, 255, 255, 255), D3DXVECTOR2(0, 1),
	  D3DXVECTOR3(-0.5, 0.5, -0.5),  D3DCOLOR_ARGB(255, 255, 255, 255), D3DXVECTOR2(1, 1)
	};
	//�����ʂ̋�`
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
// �`��(�ό`���Ȃ��ꍇ�j
// �����FframeTime�@1�t���[���̑��x
// �ߒl�F�Ȃ�
//=============================================================================
void Poly::draw(D3DXMATRIX* mWorld)
{
	graphics->get3Ddevice()->SetRenderState(D3DRS_LIGHTING, FALSE);

	graphics->get3Ddevice()->SetTransform(D3DTS_WORLD, mWorld);

	//�e�N�X�`���w��
	graphics->get3Ddevice()->SetTexture(0,texManager->getTexture());
	//���_���W�A�F����ݒ�(�����XYZ�ADIFFUSE...�g�U���ˌ�)
	graphics->get3Ddevice()->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1);
	graphics->get3Ddevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, vPoint, sizeof(Vertex));

	graphics->get3Ddevice()->SetRenderState(D3DRS_LIGHTING, TRUE);

}

//=============================================================================
// �`��
// �����FframeTime�@1�t���[���̑��x mTex �e�N�X�`���ό`�s��
// �ߒl�F�Ȃ�
//=============================================================================
void Poly::draw( D3DXMATRIX* mWorld, D3DXMATRIX* mTex)
{
	//�e�N�X�`���ɍs����w��ł���悤�ɂ���
	graphics->get3Ddevice()->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);

	//���̌v�Z�����Ȃ�
	graphics->get3Ddevice()->SetRenderState( D3DRS_LIGHTING, false );

	//�e�N�X�`���w��
	graphics->get3Ddevice()->SetTexture( 0, texManager->getTexture() );

	//���W�w��
	graphics->get3Ddevice()->SetTransform(D3DTS_WORLD, mWorld );	//�s����g���֐�

	//�s��w��
	graphics->get3Ddevice()->SetTransform(D3DTS_TEXTURE0, mTex );

	//���_����o�^�iXYZ���W�ƐF�i�g�U���ˌ��j�j
	graphics->get3Ddevice()->SetFVF( D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1 );

	//�`��
	graphics->get3Ddevice()->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, vPoint, sizeof( Vertex ) );

	//���ɖ߂�
	graphics->get3Ddevice()->SetRenderState( D3DRS_LIGHTING, true );

	//�e�N�X�`���s��w������ɖ߂�
	graphics->get3Ddevice()->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);

}

//=============================================================================
//BOX�`��
// �����FframeTime�@1�t���[���̑��x
// �ߒl�F�Ȃ�
//=============================================================================
void Poly::boxDraw(D3DXMATRIX* mWorld)
{
	graphics->get3Ddevice()->SetRenderState(D3DRS_LIGHTING, FALSE);

	graphics->get3Ddevice()->SetTransform(D3DTS_WORLD, mWorld);

	//�e�N�X�`���w��
	graphics->get3Ddevice()->SetTexture(0,texManager->getTexture());
	//���_���W�A�F����ݒ�(�����XYZ�ADIFFUSE...�g�U���ˌ�)
	graphics->get3Ddevice()->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1);
	graphics->get3Ddevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, vPoint1, sizeof(Vertex));
	graphics->get3Ddevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, vPoint2, sizeof(Vertex));
	graphics->get3Ddevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, vPoint3, sizeof(Vertex));
	graphics->get3Ddevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, vPoint4, sizeof(Vertex));
	graphics->get3Ddevice()->SetRenderState(D3DRS_LIGHTING, TRUE);

}

