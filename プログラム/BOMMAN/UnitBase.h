#pragma once
#include "graphics.h"

class UnitBase
{
public:
	virtual ~UnitBase(){}		//継承される可能のあるクラスには仮想デストラクタをつける
	virtual void initialize(){}
	virtual void update() {}	
	virtual void collisions(UnitBase* target) {}
	virtual void collisions(UnitBase* target, int playerNum) {}
	virtual void ai(){}
	//virtual void render(float frameTime){}	
	virtual void render(float frameTime, int cameraNum){}
};