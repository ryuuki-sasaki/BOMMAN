//=============================================================================
//  �ΐ탂�[�h
//�@�X�V���F2015/8/1
//	����ҁF���X�ؗ��M
//=============================================================================
#pragma once

class modeChange
{
private:
	int selectMode;		//�ǂ̑ΐ탂�[�h��I�񂾂�

private:
	modeChange::modeChange(){selectMode = 0;}

public:
	static modeChange* GetInstance(){static modeChange mode; return &mode;}		//�C���X�^���X��Ԃ�GetInstance�֐�
	void setMode(int change){selectMode = change;}
	int getMode(){return selectMode;}
};