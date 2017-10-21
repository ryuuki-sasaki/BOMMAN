//=============================================================================
//  �G�t�F�N�g
//�@�쐬���F2015/8/12
//�@�X�V���F2015/8/12
//	����ҁF���X�ؗ��M
//=============================================================================
#include "effect.h"
#include "Camera.h"

effect::effect(void)
{
}


effect::~effect(void)
{
}

//=============================================================================
// �ǂݍ���
// �����F�O���t�B�b�N�X�|�C���^�A�t�@�C�����A���A�����A�傫��
// �ߒl�F�Ȃ�
//=============================================================================
void effect::Load(Graphics *g, char* fileName, int wide, int hight)
{
	texture.initialize(g, fileName);
	poly.initialize(g, &texture);
	this->w = wide;
	this->h = hight;
}


//=============================================================================
// �G�t�F�N�g�ǉ�(�ʒu�Œ�)
// �����F�ʒu�A�X�P�[���A�`�掞��
// �ߒl�F�Ȃ�
//=============================================================================
void effect::fixPosAdd(D3DXVECTOR3 pos, float scale)
{
	Data d;
	d.pos = pos;
	d.aniFrame = 0;
	d.scale = scale;
	dataList.push_back(d);
}

//=============================================================================
// �G�t�F�N�g�ǉ�(�ʒu�ύX)
// �����F�ʒu�A�X�P�[���A�`�掞��
// �ߒl�F�Ȃ�
//=============================================================================
void effect::movePosAdd(D3DXVECTOR3 pos, float scale)
{
	data.pos = pos;
	data.aniFrame = 0;
	data.scale = scale;
}

//=============================================================================
// �X�V����(�ʒu�Œ�)
// �����F�A�j���[�V�����X�V���x
// �ߒl�F�Ȃ�
//=============================================================================
void effect::fixPosUpdate(float speed)
{
	std::list<Data>::iterator itr;
	itr = dataList.begin();
	while(itr != dataList.end())
	{
		itr->aniFrame += speed;
		if(itr->aniFrame >= w*h)
		{
			itr = dataList.erase(itr);
			continue;
		}

		itr++;
	}
}

//=============================================================================
// �X�V����(�ʒu�ύX)
// �����F�A�j���[�V�����X�V���x�A���݈ʒu
// �ߒl�F�Ȃ�
//=============================================================================
void effect::movePosUpdate(float speed, D3DXVECTOR3 pos)
{
	data.pos = pos;
	data.aniFrame += speed;
	if(data.aniFrame >= w*h)
	{
		data.aniFrame = 0;
	}
}

//=============================================================================
// �`��(�ʒu�Œ�)
// �����F�J�����ԍ�
// �ߒl�F�Ȃ�
//=============================================================================
void effect::fixPosDraw(int cNum)
{
	std::list<Data>::iterator itr;	//�C�e���[�^�쐬
	itr = dataList.begin();
	while(itr != dataList.end())	//�C�e���[�^���I�����w���܂�
	{
		D3DXMATRIX mTrans, mScale;
		D3DXMatrixTranslation(&mTrans, itr->pos.x, itr->pos.y, itr->pos.z);
		D3DXMatrixScaling(&mScale, itr->scale, itr->scale, itr->scale);

		//�e�N�X�`���̐؂蔲��
		D3DXMATRIX mTeXScale, mTexTrans;
		D3DXMatrixScaling(&mTeXScale, 1.0f/w, 1.0f/h, 1);
		matrixTrans2D(&mTexTrans, (float)((int)itr->aniFrame%w)/w, (float)((int)itr->aniFrame/w)/h);	//aniFrame�����ɐ؂蔲���ʒu�����߂�
		if(cNum == 1)
			poly.draw(&(mScale * cameraA->billboard() * mTrans), &(mTeXScale*mTexTrans));
		else
			poly.draw(&(mScale * cameraB->billboard() * mTrans), &(mTeXScale*mTexTrans));
		itr++;
	}
}

//=============================================================================
// �`��(�ʒu�ύX)
// �����F�J�����ԍ�
// �ߒl�F�Ȃ�
//=============================================================================
void effect::movePosDraw(int cNum)
{
	D3DXMATRIX mTrans, mScale;
	D3DXMatrixTranslation(&mTrans, data.pos.x, data.pos.y, data.pos.z);
	D3DXMatrixScaling(&mScale, data.scale, data.scale, data.scale);

	//�e�N�X�`���̐؂蔲��
	D3DXMATRIX mTeXScale, mTexTrans;
	D3DXMatrixScaling(&mTeXScale, 1.0f/w, 1.0f/h, 1);
	matrixTrans2D(&mTexTrans, (float)((int)data.aniFrame%w)/w, (float)((int)data.aniFrame/w)/h);	//aniFrame�����ɐ؂蔲���ʒu�����߂�
	if(cNum == 1)
		poly.draw(&(mScale * cameraA->billboard() * mTrans), &(mTeXScale*mTexTrans));
	else
		poly.draw(&(mScale * cameraB->billboard() * mTrans), &(mTeXScale*mTexTrans));
}