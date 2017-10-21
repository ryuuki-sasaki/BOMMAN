//=============================================================================
//  �V�[���x�[�X
//�@�쐬���F2014/10/23
//�@�X�V���F2015/8/30
//	����ҁF���X�ؗ��M
//=============================================================================
#pragma once
#include "UnitBase.h"
#include<vector>

class SceneBase
{
protected:
	std::vector<UnitBase*> unit;
	Camera* camraAll;

public:
	SceneBase();
	virtual ~SceneBase();
	virtual void initialize();
	virtual void update();
	virtual void collisions();
	virtual void ai();
	virtual void render(float frameTime);
};

