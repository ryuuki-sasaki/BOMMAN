//=============================================================================
//  Ⴢ�{��
//�@�X�V���F2016/10/30
//	����ҁF���X�ؗ��M
//=============================================================================
#pragma once
#include "normalBom.h"
#include <list>
using namespace std;

class paralyzeBom : public normalBom
{
public:
	paralyzeBom(void);
	int retIsBom(float x, float z);				//�{�������݂��邩��Ԃ�

private:
	~paralyzeBom(void);
	void explosion(int processCharaNum);		//��������
	void explosionFlow();						//���������̗��ꐔ
	void setExpRange(bool isSet, int chara);	//�����͈͂̃R�X�g��ݒ�
	void update();
	void initialize(void);				
	void initializeMain(void);
	void collisions(UnitBase* target);	//���j�b�g�̏����擾
};


