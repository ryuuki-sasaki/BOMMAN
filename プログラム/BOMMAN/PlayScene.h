//=============================================================================
//  プレイシーン
//　作成日：2014/10/23
//　更新日：2016/10/30
//	制作者：佐々木隆貴
//=============================================================================
#pragma once
#include "scenebase.h"	

class PlayScene : public SceneBase
{
private:
	void setUnit();
	void vsCpuSetUnit();

public:
	PlayScene(void);
	~PlayScene(void);
	void initialize();
	void update();
	void collisions();
	void ai();
	void render(float frameTime);
};


