//=============================================================================
//  �I�v�V����
//�@�X�V���F2015/8/1
//	����ҁF���X�ؗ��M
//=============================================================================
#pragma once

class option
{
private:
	int select;		

private:
	option::option(){select = 0;}

public:
	static option* GetInstance(){static option op; return &op;}		//�C���X�^���X��Ԃ�GetInstance�֐�
	void setMode(int change){select = change;}
	int getMode(){return select;}
};