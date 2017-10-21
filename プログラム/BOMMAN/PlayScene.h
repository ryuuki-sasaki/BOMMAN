//=============================================================================
//  �v���C�V�[��
//�@�쐬���F2014/10/23
//�@�X�V���F2016/10/30
//	����ҁF���X�ؗ��M
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


