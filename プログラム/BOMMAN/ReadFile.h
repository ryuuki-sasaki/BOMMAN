//=============================================================================
//  CSV�t�@�C���ǂݍ���
//�@�쐬���F2015/9/1
//�@�X�V���F2016/10/30
//	����ҁF���X�ؗ��M
//=============================================================================
#pragma once
#include "constants.h"

class readFile
{
private:
	char* data;

public:
	readFile();
	bool read(char* fileName);	//�t�@�C���ǂݍ���
	int getToComma(int* index);	//�R���}��؂�ŏ����擾
	void deleteDataArray();		//data�z��폜
};
