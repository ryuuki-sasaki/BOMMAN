//=============================================================================
//  �v���C�V�[��
//�@�쐬���F2014/10/23
//�@�X�V���F2016/10/30
//	����ҁF���X�ؗ��M
//=============================================================================
#include "PlayScene.h"
#include "modeChange.h"
#include "option.h"
#include "gameStatus.h"
#include "Camera.h"
#include "Ground.h"
#include "Wall.h"
#include "VsPlayer.h"
#include "vsCpuPlayer.h"
#include "Hurricane.h"
#include "Thunder.h"
#include "PsyCpu.h"
#include "NormalCpu.h"
#include "SpeedCpu.h"
#include "normalBom.h"
#include "plusBom.h"
#include "paralyzeBom.h"

//�v���C���[���v���C�V�[���̃��X�g�Ƀv�b�V����������
#define PUSHPLAYERNUM1 1
#define PUSHPLAYERNUM2 2

//2P�ΐ펞�̃L�����N�^�[�ԍ�
#define PLAYER1 1
#define PLAYER2 2
#define NOTPLAYER 100

//CPU�L�����N�^�[�̎��
enum CHARA_TYPE
{
	PYECPU,
	NORMALCPU,
	SPEEDCPU
};

//�{���̎��
enum BOM_TYPE
{
	NORMALBOM,
	PARALYZEBOM,
	PLUSBOM
};

//=============================================================================
// �R���X�g���N�^
//=============================================================================
PlayScene::PlayScene(void)
{	
	//�v���C�V�[���ɑ��݂����������j�b�g���i�[	
	if(modeChange::GetInstance() -> getMode() == VS2P)
	{
		setUnit();
	} else {
		vsCpuSetUnit();
	}
}

//=============================================================================
// �f�X�g���N�^
//=============================================================================
PlayScene::~PlayScene(void)
{
	initialize();
}

//=============================================================================
// 2P�ΐ펞�̃��j�b�g�i�[
// �����F�Ȃ�
// �ߒl�F�Ȃ�
//=============================================================================
void PlayScene::setUnit()
{
	unit.push_back(new Ground);
	unit.push_back(new VsPlayer(PLAYER1));
	unit.push_back(new VsPlayer(PLAYER2));
	//Hurricane��Thunder��Player�����擾����������Player�̌�ɏ���
	if(option::GetInstance() -> getMode() == OPTION::HURRICANE
		|| option::GetInstance() -> getMode() == OPTION::ALLSELECT)
	{
		unit.push_back(new Hurricane);
	}
	if(option::GetInstance() -> getMode() == OPTION::THUNDER
		|| option::GetInstance() -> getMode() == OPTION::ALLSELECT)
	{
		unit.push_back(new Thunder);
	}
	unit.push_back(new gameStatus);
	unit.push_back(new normalBom);
	unit.push_back(new plusBom);
	unit.push_back(new paralyzeBom);
	unit.push_back(new Wall);
}

//=============================================================================
// vsCpu�ΐ펞�̃��j�b�g�i�[
// �����F�Ȃ�
// �ߒl�F�Ȃ�
//=============================================================================
void PlayScene::vsCpuSetUnit()
{
	unit.push_back(new Ground);
	if(option::GetInstance() -> getMode() == OPTION::HURRICANE
		|| option::GetInstance() -> getMode() == OPTION::ALLSELECT)
	{
		unit.push_back(new Hurricane);
	}
	if(option::GetInstance() -> getMode() == OPTION::THUNDER
		|| option::GetInstance() -> getMode() == OPTION::ALLSELECT)
	{
		unit.push_back(new Thunder);
	}
	unit.push_back(new gameStatus);
	unit.push_back(new vsCpuPlayer);
	unit.push_back(new PsyCpu);
	unit.push_back(new NormalCpu);
	unit.push_back(new SpeedCpu);
	unit.push_back(new normalBom);
	unit.push_back(new plusBom);
	unit.push_back(new paralyzeBom);
	unit.push_back(new Wall);
	
}

//=============================================================================
// �V�[����̃��j�b�g��initialize()�Ăяo������
// �����F�Ȃ�
// �ߒl�F�Ȃ�
//=============================================================================
void PlayScene::initialize()
{
	for(DWORD i = 0; i < unit.size(); i++)
	{
		unit[i]->initialize();
	}
}

//=============================================================================
// �V�[����̃��j�b�g��update()�Ăяo������
// �����F�Ȃ�
// �ߒl�F�Ȃ�
//=============================================================================
void PlayScene::update()
{
	if(gameSt == INGAME)
	{
		for(DWORD i = 0; i < unit.size(); i++)
		{	
			unit[i]->update();
		}
	} else/*�|�[�Y���Ȃ�*/ {
		for(DWORD i = 0; i< unit.size(); i++)
		{
			if(typeid(*unit[i]) == typeid(gameStatus))
				unit[i]->update();
		}
	}
}

//=============================================================================
// �V�[����̃��j�b�g��collisions()�Ăяo������
// �����F�Ȃ�
// �ߒl�F�Ȃ�
//=============================================================================
void PlayScene::collisions()
{
	//collisions�ɂ͈���������A�������K�v�ȑ����n���Ȃ���΂Ȃ�Ȃ�
	//�����j�b�g�̏Փˑ�����(��d���[�v)
	for(DWORD i = 0; i < unit.size(); i++)
	{
		for(DWORD j = 0; j < unit.size(); j++)
		{	
			unit[i]->collisions(unit[j]);
		}
	}
}

//=============================================================================
// �V�[����̃��j�b�g��ai()�Ăяo������
// �����F�Ȃ�
// �ߒl�F�Ȃ�
//=============================================================================
void PlayScene::ai()
{
	if(gameSt == INGAME)
	{
		//���݂̃V�[���̂��ׂ�unit��ai
		for(DWORD i = 0; i < unit.size(); i++)
		{
			unit[i]->ai();
		}
	}
}

//=============================================================================
// �V�[����̃��j�b�g��render()�Ăяo������
// �����FframeTime �t���[���̍X�V����
// �ߒl�F�Ȃ�
//=============================================================================
void PlayScene::render(float frameTime)
{
	//���݂̃V�[���̂��ׂ�unit��render
	cameraA->update();
	for(DWORD i = 0; i < unit.size(); i++)
	{
		unit[i]->render(frameTime, PLAYER1);
	}

	cameraB->update();
	for(DWORD i = 0; i < unit.size(); i++)
	{
		unit[i]->render(frameTime, PLAYER2);
	}

	camraAll->update();
}


