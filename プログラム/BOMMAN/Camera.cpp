#include "Camera.h"
#include "modeChange.h"

Camera::Camera(void)
{
}


Camera::~Camera(void)
{
}

//=============================================================================
// update����
// �����F�Ȃ�
// �ߒl�F�Ȃ�
//=============================================================================
void Camera::update()
{
	//�r���[�s��
	D3DXMatrixLookAtLH(&view, &pos, &target, &D3DXVECTOR3(0,1,0));	
	//�r���[�s��Z�b�g
	graphics->get3Ddevice()->SetTransform(D3DTS_VIEW, &view);		

	if(modeChange::GetInstance() -> getMode() == VS2P)
	{
		//�ˉe
		D3DXMatrixPerspectiveFovLH(&proj, D3DXToRadian(45), (float)GAME_WIDTH/GAME_HEIGHT/2, 0.1f, 1000.0f);
		graphics->get3Ddevice()->SetTransform(D3DTS_PROJECTION, &proj);

		//�r���[�|�[�g�Z�b�g
		graphics->get3Ddevice()->SetViewport(&vp);
	} else {
		//�ˉe
		D3DXMatrixPerspectiveFovLH(&proj, D3DXToRadian(45), (float)GAME_WIDTH/GAME_HEIGHT, 0.1f, 1000.0f);
		graphics->get3Ddevice()->SetTransform(D3DTS_PROJECTION, &proj);

		//�r���[�|�[�g�Z�b�g
		graphics->get3Ddevice()->SetViewport(&vp);
	}
}


//=============================================================================
// �J�����̊p�x��Ԃ��i�r���{�[�h�j
// �����F�Ȃ�
// �ߒl�FD3DXMATRIX �r���{�[�h�̍s��
//=============================================================================
D3DXMATRIX Camera::billboard()	//�����̂悤�Ȃ���
{
	D3DXMATRIX matrix;
	D3DXMatrixLookAtLH(&matrix, &D3DXVECTOR3(0,0,0),&(target - pos),&D3DXVECTOR3(0,1,0));

	D3DXMatrixInverse(&matrix, NULL, &matrix);
	return matrix;
}

//=============================================================================
//�r���[�|�[�g�i��ʂ̕\���͈́j��ύX����
//�����Fx,y	����ʒu
//�����Fw,h	���ƍ���
//=============================================================================
void Camera::changeViewport(int x, int y, int w, int h)
{
	vp.X = x;
	vp.Y = y;
	vp.Width = w;
	vp.Height = h;
	vp.MinZ = 0;
	vp.MaxZ = 1;
}

