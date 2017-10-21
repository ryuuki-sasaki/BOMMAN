//=============================================================================
//  �Q�[���̏�ԊǗ�
//�@�쐬���F2015/8/22
//�@�X�V���F2016/10/30
//	����ҁF���X�ؗ��M
//=============================================================================
#include "gameStatus.h"
#include "modeChange.h"
#include "option.h"
#include "input.h"
#include "Wall.h"
#include "NormalCpu.h"
#include "PsyCpu.h"
#include "SpeedCpu.h"
#include "VsPlayer.h"
#include "vsCpuPlayer.h"
#include "image.h"

#define MINIT 60	//�Q�[������
#define	PICSIZE 64	//�e�N�X�`���̑傫��
#define CHARANUM 3	//�L�����N�^�[��
#define ADDSIZEX 480 / 2
#define ADDSIZEY 360
#define NOTTIMECOUNT 100	//�J�E���g�����Ȃ�

//CPU�L�����N�^�[�̎��
enum CHARA_TYPE
{
	PYECPU,
	NORMALCPU,
	SPEEDCPU
};

//=============================================================================
// �R���X�g���N�^
//=============================================================================
gameStatus::gameStatus(void)
{	
	//�C���[�W
	numImg = new Image;	
	numImg2 = new Image;
	numImg3 = new Image;
	menuImg = new Image;
	cursorImg = new Image;
	s_cursorImg = new Image;
	resultImg = new Image;
	stringImg = new Image;

	//�e�N�X�`���ǂݍ���
	if (!texMenu.initialize(graphics,"pictures\\menu.png"))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing menu texture"));
    if (!menuImg->initialize(graphics,0,0,0,&texMenu))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing menu"));

	if (!texCursor.initialize(graphics,"pictures\\cursor.png"))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing cursor texture"));
    if (!cursorImg->initialize(graphics,0,0,0,&texCursor))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing cursor"));

	if (!texs_Cursor.initialize(graphics,"pictures\\s_cursor.png"))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing cursor texture"));
    if (!s_cursorImg->initialize(graphics,0,0,0,&texs_Cursor))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing cursor"));

	if (!texResult.initialize(graphics,"pictures\\result.png"))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing result texture"));
    if (!resultImg->initialize(graphics,0,0,0,&texResult))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing result"));

	if (!texString.initialize(graphics,"pictures\\resultString.png"))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing string texture"));
    if (!stringImg->initialize(graphics,400,200,0,&texString))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing string"));

	//�e�N�X�`���ǂݍ���
	if (!texNum.initialize(graphics,"pictures\\number.png"))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing number texture"));
    if (!numImg->initialize(graphics,PICSIZE/2,PICSIZE,0,&texNum))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing number"));
	if (!numImg2->initialize(graphics,PICSIZE/2,PICSIZE,0,&texNum))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing number"));
	if (!numImg3->initialize(graphics,PICSIZE/2,PICSIZE,0,&texNum))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing number"));

	initialize();
}

//=============================================================================
// ������
// �����F �Ȃ�
// �߂�l�F�Ȃ�
//=============================================================================
void gameStatus::initialize()
{
	gameSt = INGAME;
	timeCount = 0;
	//�J�[�\���ʒu�Z�b�g
	cursor = RESUME;
	cursorImg->setX(100+ADDSIZEX/2);
	cursorImg->setY(600);
	s_cursorImg->setX(320+ADDSIZEX);
	s_cursorImg->setY(216+ADDSIZEY/3);

	//���Ԍo�ߗp���̈ʒu�Z�b�g
	numImg->setY(20);
	numImg2->setY(20);
	numImg3->setY(20);
	numImg ->setX(580);			
	numImg2->setX(644);
	numImg3->setX(676);

	//���j���[�ʒu�Z�b�g
	menuImg->setX(300+ADDSIZEX);
	menuImg->setY(140+ADDSIZEY/3);

	//���U���g�����p�摜�ʒu�Z�b�g
	stringImg->setY(100+ADDSIZEY/3);

	set1 = 2;
	set2 = 5;
	set3 = 9;
	timeCount = 1;

	wallUnit = nullptr;
	player1 = nullptr;
	player2 = nullptr;
	cpuUnit[NORMALCPU] = nullptr;
	cpuUnit[PYECPU] = nullptr;
	cpuUnit[SPEEDCPU] = nullptr;
}

//=============================================================================
// �f�X�g���N�^
//=============================================================================
gameStatus::~gameStatus(void)
{
	SAFE_DELETE(menuImg);
	SAFE_DELETE(cursorImg);
	SAFE_DELETE(s_cursorImg);
	SAFE_DELETE(resultImg);
	SAFE_DELETE(stringImg);
	SAFE_DELETE(numImg);
	SAFE_DELETE(numImg2);
	SAFE_DELETE(numImg3);
}

//=============================================================================
//�V�[���̃��j�b�g���擾
//�����F�V�[���̃��j�b�g
//�߂�l�F�Ȃ�
//=============================================================================
//ScceneBase����Ăяo���ꂽUnitBase��collisions����^�[�Q�b�g�擾
//typeid...typeid(�^���܂��͌^�^�̕ϐ����j...���^�ō��ꂽ��
void gameStatus::collisions(UnitBase* target)
{
	if(typeid(*target) == typeid(Wall))
		wallUnit = (Wall*)target;
	
	if(typeid(*target) == typeid(vsCpuPlayer))
		vsCpuPlayerUnit = (vsCpuPlayer*)target;

	if(typeid(*target) == typeid(NormalCpu))
		cpuUnit[NORMALCPU] = (NormalCpu*)target;

	if(typeid(*target) == typeid(PsyCpu))
		cpuUnit[PYECPU] = (PsyCpu*)target;

	if(typeid(*target) == typeid(SpeedCpu))
		cpuUnit[SPEEDCPU] = (SpeedCpu*)target;

	
	if(modeChange::GetInstance() -> getMode() == VS2P
		&& typeid(*target) == typeid(VsPlayer)) {
			VsPlayer* VsPlayerUnit = (VsPlayer*)target; 
			if(VsPlayerUnit->player == 1)
				player1 = VsPlayerUnit;
			else
				player2 = VsPlayerUnit;
	}

}

//=============================================================================
// �`��
// �����F1�t���[���̑��x�A�J�����ԍ�
// �ߒl�F�Ȃ�
//=============================================================================
void gameStatus::render(float frameTime, int cNum)
{
	gameTimeCount();
	numImg ->draw();
	numImg2->draw();
	numImg3->draw();

	//�ꎞ��~���̕`��
	if(gameSt == PAUSE)
	{
		menuImg->draw();
		s_cursorImg->draw();
	}

	//���U���g���̕`��
	if(gameSt == END)
	{
		//���U���g�\��
		resultRender(cNum);

		resultImg->draw();		//�w�i
		stringImg->draw();		//����
		cursorImg->draw();		//�J�[�\��
	}
}

//=============================================================================
// ���U���g�摜�`��
// �����F�J�����ԍ�
// �ߒl�F�Ȃ�
//=============================================================================
void gameStatus::resultRender(int cNum)
{
	if(modeChange::GetInstance() -> getMode() == VS2P)
	{
		if(cNum == 1/*1P*/)	
		{
			stringImg->setCurrentFrame(gameRs == WIN1P ? 0 : 1);
			stringImg->setX(0+ADDSIZEX);
		} else if(cNum == 2/*2P*/) {
			stringImg->setCurrentFrame(gameRs == WIN2P ? 0 : 1);
			stringImg->setX(400+ADDSIZEX);
		}
	} else {
		stringImg->setCurrentFrame(gameRs == WINVSCPU ? 0 : 1);
		stringImg->setX(200+ADDSIZEX);
	}
}
//=============================================================================
// update����
// �����F�Ȃ�
// �ߒl�F�Ȃ�
//=============================================================================
void gameStatus::update()
{
	//�Q�[�����Ȃ�
	if(gameSt == INGAME)
	{
		//�L�����N�^�[�̎��S����
		loseDecision();
	}

	//SPACE�L�[�ňꎞ��~
	if(input->wasKeyPush(VK_SPACE) || input->wasGamePadPush(1, GAMEPAD_X) || input->wasGamePadPush(2, GAMEPAD_X))
	{
		//�ꎞ��~���Ȃ�����E�Q�[�����Ȃ�ꎞ��~
		if(gameSt == PAUSE)
		{
			gameSt = INGAME;
		}
		else if(gameSt == INGAME)
		{
			gameSt = PAUSE;

			//�J�[�\���ʒu���Z�b�g
			cursor = RESUME;
			s_cursorImg->setY(216+ADDSIZEY/3);
		}
	}

	//�Q�[���̏��
	switch(gameSt)
	{
	case INGAME:
		break;
	case PAUSE:
		Pause();
		break;
	case END:
		End();
		break;
	}
}

//=============================================================================
// �ꎞ��~���̏���
// �����F�Ȃ�
// �ߒl�F�Ȃ�
//=============================================================================
void gameStatus::Pause()
{
	//�ꎞ��~���G���^�[�L�[�ŏI��
	if(input->wasKeyPush(VK_RETURN) || input->wasGamePadPush(1, GAMEPAD_B))
	{
		switch(cursor)
		{
		case RESUME:
			gameSt = INGAME;
			break;
		case RETRY:
			gameScene = SC_INIT;
			break;
		case EXIT:
			modeChange::GetInstance() -> setMode(0);
			option::GetInstance() -> setMode(0);
			gameScene = SC_TITLE;
			break;
		}
	}

	//�J�[�\���ړ�
	if(input->wasKeyPush(VK_UP) || input->wasGamePadPush(1, GAMEPAD_DPAD_UP))
	{
		switch(cursor)
		{
		case RESUME:
			cursor = EXIT;
			s_cursorImg->setY(286+ADDSIZEY/3);
			break;
		case RETRY:
			cursor = RESUME;
			s_cursorImg->setY(216+ADDSIZEY/3);
			break;
		case EXIT:
			cursor = RETRY;
			s_cursorImg->setY(251+ADDSIZEY/3);
			break;
		}
	}

	if(input->wasKeyPush(VK_DOWN) || input->wasGamePadPush(1, GAMEPAD_DPAD_DOWN))
	{
		switch(cursor)
		{
		case RESUME:
			cursor = RETRY;
			s_cursorImg->setY(251+ADDSIZEY/3);
			break;
		case RETRY:
			cursor = EXIT;
			s_cursorImg->setY(286+ADDSIZEY/3);
			break;
		case EXIT:
			cursor = RESUME;
			s_cursorImg->setY(216+ADDSIZEY/3);
			break;
		}
	}
}

//=============================================================================
// �Q�[���I�����̏���
// �����F�Ȃ�
// �ߒl�F�Ȃ�
//=============================================================================
void gameStatus::End()
{	
	//�ꎞ��~���G���^�[�L�[�ŏI��
	if(input->wasKeyPush(VK_RETURN) || input->wasGamePadPush(1, GAMEPAD_B))
	{
		switch(cursor)
		{
		case RESUME:
		case RETRY:
			gameScene = SC_INIT;
			break;
		case EXIT:
			modeChange::GetInstance() -> setMode(0);
			option::GetInstance() -> setMode(0);
			gameScene = SC_TITLE;
			break;
		}
	}

	//�J�[�\���ړ�
	if(input->wasKeyPush(VK_RIGHT) || input->wasKeyPush(VK_LEFT)
		 || input->wasGamePadPush(1, GAMEPAD_DPAD_LEFT) || input->wasGamePadPush(1, GAMEPAD_DPAD_RIGHT))
	{
		switch(cursor)
		{
		case RESUME:
		case RETRY:
			cursor = EXIT;
			cursorImg->setX(680);
			break;
		case EXIT:
			cursor = RETRY;
			cursorImg->setX(100+ADDSIZEX/2);
			break;
		}
	}
}

//=============================================================================
// �Q�[���I�����̏���
// �����F�Ȃ�
// �ߒl�F�Ȃ�
//=============================================================================
void gameStatus::gameTimeCount()
{
	if(gameSt == PAUSE) {
		return;
	}

	if(timeCount == 0)
		return;

	if(timeCount % (MINIT*MINIT) == 0)
	{
		set1--;
		if(set1 < 0 && set1 != NOTTIMECOUNT) {
			set1 = 2;
		}
	} 
	
	if(timeCount % (MINIT*10) == 0) {
		set2--;
		if(set2 < 0 && set2 != NOTTIMECOUNT) {
			set2 = 5;
		}
	} 

	if(timeCount % MINIT == 0) {
		set3--;
		if(set3 < 0 && set3 != NOTTIMECOUNT) {
			set3 = 9;
		}
	} 

	if(timeCount >= (3 * MINIT * MINIT))
	{
		wallUnit->setIsTimeUp(true);
		timeCount = -1;
		set1 = NOTTIMECOUNT;
		set2 = NOTTIMECOUNT;
		set3 = NOTTIMECOUNT;
	}
	
	numImg ->setCurrentFrame(set1);
	numImg2->setCurrentFrame(set2);
	numImg3->setCurrentFrame(set3);
	
	timeCount++;
}

//=============================================================================
// �L�����N�^�[�̎��S����
// �����F�Ȃ�
// �ߒl�F�Ȃ�
//=============================================================================
void gameStatus::loseDecision()
{
	//2P�ΐ�Ȃ�
	if(modeChange::GetInstance() -> getMode() == VS2P)
	{
		//�|�C���^�ɒ��g�������Ă����
 		if(player1 != nullptr && player2 != nullptr)
		{
 			if(player1->isLose || player2->isLose) {
				//���S����
				gameRs = player1->isLose == true ? WIN2P : WIN1P;	//���s������
				gameSt = END;										//�Q�[���̏�Ԃ��I����
			} 
		}
	}
	
	//CPU�ΐ�Ȃ�
	if(modeChange::GetInstance() -> getMode() == VSCPU)
	{	
		if(vsCpuPlayerUnit != nullptr && cpuUnit[PYECPU] != nullptr 
			&& cpuUnit[SPEEDCPU] != nullptr && cpuUnit[NORMALCPU] != nullptr)
		{
			//���ׂĂ�CPU�L�����N�^�[�����S������
			if(cpuUnit[PYECPU]->isLose && cpuUnit[SPEEDCPU]->isLose && cpuUnit[NORMALCPU]->isLose)
			{
				//���S����
				gameRs = WINVSCPU;	//���s������
				gameSt = END;		//�Q�[���̏�Ԃ��I����
				return;
			}

			//���ׂĂ�vsCPU�L�����N�^�[�����S������
			if(vsCpuPlayerUnit -> isLose)
			{
				//���S����
				gameRs = LOSEVSCPU;	//���s������
				gameSt = END;		//�Q�[���̏�Ԃ��I����
				return;
			}
		}
	}
}