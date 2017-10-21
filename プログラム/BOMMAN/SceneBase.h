//=============================================================================
//  シーンベース
//　作成日：2014/10/23
//　更新日：2015/8/30
//	制作者：佐々木隆貴
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

