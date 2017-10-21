//=============================================================================
//  �n���P�[��
//�@�쐬���F2015/6/28
//�@�X�V���F2016/10/30
//	����ҁF���X�ؗ��M
//=============================================================================
#include "Hurricane.h"
#include "NormalCpu.h"
#include "PsyCpu.h"
#include "SpeedCpu.h"
#include "VsPlayer.h"
#include "vsCpuPlayer.h"
#include "modeChange.h"
#include "Wall.h"
#include "Camera.h"
#include "effect.h"
#include "audio.h"
#include <time.h>

#define MAXRANGE 48			//�ő�͈�
#define MINRANGE 2			//�ŏ��͈�
#define RANDUMRANGE 500		//�����_���͈�
#define RANDUMPOSRANGE 48	//�����_���Ȉʒu�͈̔�
#define RANDUMTIMERANGE 2	//�����_���Ȏ��Ԃ͈̔�
#define CHANGENUM 5			//�����]���������
#define FPS 30				//fps(1�b������̃t���[����)
#define ISBOM 100			//�{�������݂��邩
#define CHARANUM 3			//�L�����N�^�[��
#define NOTWALL 0			//�ǂ����݂��Ȃ�
#define PROBABILITY 1		//�m���̎q��
#define	MOVEX 0				//x����
#define PLAYERNUM 2			//2P�ΐ펞�̃L�����N�^�[��
#define CAMERA1 1			//�J�����ԍ�
#define PICSIZE 3			//�e�N�X�`���T�C�Y
#define UPDATEANIM 1.0f		//�A�j���[�V�����X�V
#define ITEMBOMLISTNUM 3	//�e�A�C�e���̃��X�g��

//�A�C�e��
enum ITEM
{
	DEFAULTITEM,
	PLUSBOMITEM,			//�v���X�{��
	PARALYZEITEM,     		//Ⴢ�{��
	THROWITEM				//�{���𓊂���
};

//CPU�L�����N�^�[�̎��
enum CHARA_TYPE
{
	PYECPU,
	NORMALCPU,
	SPEEDCPU
};

//=============================================================================
//�R���X�g���N�^
//=============================================================================
Hurricane::Hurricane(void)
{
	//�I�[�f�B�I
	audio = new Audio;

	//�I�[�f�B�I�ǂݍ���
	audio->initialize("Sounds\\Wave Bank.xwb", "Sounds\\Sound Bank.xsb");

	//�G�t�F�N�g�ǂݍ���
	hurricane = new effect;
	hurricane->Load(graphics, "pictures\\hurricane.png", 8, 3);

	initialize();
}

//=============================================================================
//������
//=============================================================================
void Hurricane::initialize(void)
{
	pos = D3DXVECTOR3(0,0,0);
	move = D3DXVECTOR3(0,0,0);
	dirX = 0;					
	dirZ = 0;					
	changeDirectionNum = 0;		
	timerRange = 0;				
	changeDirection = true;		
	isOccurrence= false;
	prevDirX = dirX;
	prevDirZ = dirZ;

	vsCpuPlayerUnit = nullptr;
	player1 = nullptr;
	player2 = nullptr;
	cpuUnit[NORMALCPU] = nullptr;
	cpuUnit[PYECPU] = nullptr;
	cpuUnit[SPEEDCPU] = nullptr;

	//�J�E���g������
	count = 0;
}

//=============================================================================
//�f�X�g���N�^
//=============================================================================
Hurricane::~Hurricane(void)
{
	SAFE_DELETE(audio);
	SAFE_DELETE(hurricane);
}

//=============================================================================
//�V�[���̃��j�b�g���擾
//�����F�V�[���̃��j�b�g
//�߂�l�F�Ȃ�
//=============================================================================
//ScceneBase����Ăяo���ꂽUnitBase��collisions����^�[�Q�b�g�擾
//typeid...typeid(�^���܂��͌^�^�̕ϐ����j...���^�ō��ꂽ��
void Hurricane::collisions(UnitBase* target)
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
//�X�V
//�����F�Ȃ�
//�߂�l�F�Ȃ�
//=============================================================================
void Hurricane::update()
{
	//�n���P�[���𔭐�
	occurrenceHurricane();

	//�䕗��������
	if(isOccurrence)
	{	
		//�����]�����邩
		if(changeDirection)	
			setInfo();

		//��]���Ԉȉ��Ȃ�
		if(count < timerRange)
		{
			count++;
			pos += move;
			D3DXVECTOR3 renderPos = D3DXVECTOR3(pos.x, 1.0f, pos.z);
			//�G�t�F�N�g�X�V
			hurricane->movePosUpdate(UPDATEANIM, renderPos);
			//�t�B�[���h��������		
			inRange();	
			//�n���P�[�����L�����N�^�[�ɏՓ˂��Ă��邩
			collHurricane();
		} else {
			initInfo();
		}
		//BGM�Đ�
		audio->playCue("�䕗��");
	} else {
		//BGM��~
		audio->stopCue("�䕗��");
	}
}

//=============================================================================
//�`��
//�����F�t���[�����A�J�����ԍ�
//�߂�l�F�Ȃ�
//=============================================================================
void Hurricane::render(float frameTime/*�t���[��*/, int cameraNum /*�J�����ԍ�*/)
{	
	//�䕗��������
	if(isOccurrence)
	{
		//�G�t�F�N�g�`��
		hurricane->movePosDraw(cameraNum);
	}
}

//=============================================================================
//�n���P�[���𔭐�
//�����@�@�Ȃ�
//�߂�l�@�Ȃ�
//=============================================================================
void Hurricane::occurrenceHurricane()
{
	//�������邩�������_���Ŕ���A�����������Ă��Ȃ���
	if(PROBABILITY == rand() % RANDUMRANGE&& !isOccurrence)
	{ 
		//��������
		isOccurrence = true;
		//�����_���Ȉʒu���i�[
		pos = D3DXVECTOR3((float)(rand() % RANDUMPOSRANGE+1), -0.5f, (float)(rand() % RANDUMPOSRANGE+1));
		//�G�t�F�N�g�ǉ�
		hurricane->movePosAdd(pos,PICSIZE);
		changeDirection = false;
	}
}

//=============================================================================
//�e��񏉊���
//�����F�Ȃ�
//�߂�l�F�Ȃ�
//=============================================================================
void Hurricane::initInfo()
{
	//�J�E���g������
	count = 0;
	//�����]���񐔉��Z
	changeDirectionNum++;
	//�����]������
	changeDirection = true;	

	//�����]���̉񐔂�CHANGENUM�ȏ�Ȃ�
	if(changeDirectionNum >= CHANGENUM)
	{
		//�񐔏�����
		changeDirectionNum = 0;
		//�������Ă��Ȃ�
		isOccurrence = false;
		pos = D3DXVECTOR3(0, 0, 0);
	}
}

//=============================================================================
//�e���Z�b�g
//�����F�Ȃ�
//�߂�l�F�Ȃ�
//=============================================================================
void Hurricane::setInfo()
{

	//�i�s�����i�[
	int direction[] = {0, 1, -1};
	//�i�s������+������-������
	dirX = rand() % 3;
	dirZ = rand() % 3;

	//�ȑO�̕����Ɠ����ꍇ�Ⴄ�����ɂȂ�悤�Ɍ��ߒ���
	if (dirX == prevDirX && dirZ == prevDirZ || dirX == 0 && dirZ == 0)
	{
		while (dirX != prevDirX || dirZ != prevDirZ)
		{
			dirX = rand() % 3;
			dirZ = rand() % 3;
		}
	}	

	prevDirX = dirX;
	prevDirZ = dirZ;

	//�ړ������ݒ�	
	move = D3DXVECTOR3((float)direction[dirX],0,(float)direction[dirZ]);
	//�i�s������������܂ł̎��ԁi�ő�2�b�j
	timerRange = (rand() % RANDUMTIMERANGE+1) * FPS;
	changeDirection = false;
}

//=============================================================================
//�n���P�[���ɏՓ˂����I�u�W�F�N�g�𔻒�
//�����F�Ȃ�
//�߂�l�F�Ȃ�
//=============================================================================
void Hurricane::collHurricane()
{
	float dx[] = {0.0f, 1.0f, 1.0f, 1.0f, 0.0f, -1.0f, -1.0f, 0.0f, 0.0f};
	float dz[] = {1.0f, 1.0f, 0.0f, -1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f};

	//���݂̃^�C���ɗאڂ���e�^�C���𒲂ׂ�
	for(int i = 0; i <= 8; i++)
	{	
		float x = pos.x + dx[i];
		float z = pos.z + dz[i];

		//����ʒu
		D3DXVECTOR3 conpare(x, -0.5f, z);

		//�L�����N�^�[�ɑ΂��Ă̏Փ˔���		
		if(modeChange::GetInstance() -> getMode() == VS2P/*2P�ΐ�Ȃ�*/)
		{
			collPlayerHurricane(conpare);
		}

		if(modeChange::GetInstance() -> getMode() == VSCPU/*CPU�ΐ�Ȃ�*/)
		{
			collCpuHurricane(conpare);
			collvsCpuHurricane(conpare);
		}
	}
}

//=============================================================================
//�n���P�[���ɏՓ˂����v���C���[����
//�����F��r�ʒu
//�߂�l�F�Ȃ�
//=============================================================================
void Hurricane::collPlayerHurricane(D3DXVECTOR3 conparePos)
{
	VsPlayer* playerList[] = {player1,
							player2};

	list<int>::iterator itr;
	for(int i = 0; i < PLAYERNUM; i++)
	{		
		//�͈͓��Ƀv���C���[�������
		if(conparePos == playerList[i]->pos)
		{
			list<int>* itemBomList[] = {&playerList[i]->plusBomNumList,
						&playerList[i]->paralyzeBomNumList,
						&playerList[i]->bomThrowNumList};

			for(int j = 0; j < ITEMBOMLISTNUM; j++)
			{
				//�Փ˂����L�����N�^�[�̃{�����X�g����łȂ�
				if(!itemBomList[j]->empty())
				{
					itr = itemBomList[j]->begin();
					//�{���̏����������[�v
					while(itr != itemBomList[j]->end())
					{
						//�����_���Ɉʒu������
						int x = rand() % RANDUMPOSRANGE+1;
						int z = rand() % RANDUMPOSRANGE+1;

						//���̈ʒu���u���b�N�łȂ����
						if(wallUnit  -> retWallArr(x, z) == NOTWALL)
						{
							//���̈ʒu�ɏ������Ă����A�C�e����z�u
							wallUnit  -> wallData[x][z].isItem = j+1;
							//�������Ă����A�C�e�����폜
							itr = itemBomList[j]->erase(itr);
						}
					}
				}
			}
		}
	}	
}

//=============================================================================
//�n���P�[���ɏՓ˂���CPU����
//�����F��r�ʒu
//�߂�l�F�Ȃ�
//=============================================================================
void Hurricane::collCpuHurricane(D3DXVECTOR3 conparePos)
{
	//�eCPU���p���������j�b�g���i�[
	NormalCpu* cpuChara[] = { cpuUnit[PYECPU],
						cpuUnit[SPEEDCPU],
						cpuUnit[NORMALCPU] };

	list<int>::iterator itr;
	//�L�����N�^�[�����[�v
	for(int i = 0; i < CHARANUM; i++)
	{
		//�e�L�����N�^�[�ƏՓ˂��Ă����
		if(cpuChara[i] -> pos == conparePos)
		{
			list<int>* itemBomList[] = {&cpuChara[i]->plusBomNumList,
				&cpuChara[i]->paralyzeBomNumList,
				&cpuChara[i]->bomThrowNumList};

			for(int j = 0; j < ITEMBOMLISTNUM; j++)
			{
				//�Փ˂����L�����N�^�[�̃{�����X�g����łȂ�
				if(!itemBomList[j]->empty())
				{
					itr = itemBomList[j]->begin();
					//�{���̏����������[�v
					while(itr != itemBomList[j]->end())
					{
						//�����_���Ɉʒu������
						int x = rand() % RANDUMPOSRANGE+1;
						int z = rand() % RANDUMPOSRANGE+1;

						//���̈ʒu���u���b�N�łȂ����
						if(wallUnit  -> retWallArr(x, z) == NOTWALL)
						{
							//���̈ʒu�ɏ������Ă����{����z�u
							wallUnit  -> wallData[x][z].isItem = j+1;
							//�������Ă����{���폜
							itr = itemBomList[j]->erase(itr);
						}
					}
				}
			}
		}
	}
}

//=============================================================================
//�n���P�[���ɏՓ˂���vsCPU����
//�����F��r�ʒu
//�߂�l�F�Ȃ�
//=============================================================================
void Hurricane::collvsCpuHurricane(D3DXVECTOR3 conparePos)
{
	list<int>::iterator itr;

	//�v���C���[�ƏՓ˂��Ă����
	if(vsCpuPlayerUnit  -> pos == conparePos)
	{
		list<int>* itemBomList[] = {&vsCpuPlayerUnit->plusBomNumList,
			&vsCpuPlayerUnit->paralyzeBomNumList,
			&vsCpuPlayerUnit->bomThrowNumList};

		for(int j = 0; j < ITEMBOMLISTNUM; j++)
		{
			//�Փ˂����L�����N�^�[�̃{�����X�g����łȂ�
			if(!itemBomList[j]->empty())
			{
				itr = itemBomList[j]->begin();
				//�{���̏����������[�v
				while(itr != itemBomList[j]->end())
				{
					//�����_���Ɉʒu������
					int x = rand() % RANDUMPOSRANGE+1;
					int z = rand() % RANDUMPOSRANGE+1;

					//���̈ʒu���u���b�N�łȂ����
					if(wallUnit -> retWallArr(x, z) == NOTWALL)
					{
						//���̈ʒu�ɏ������Ă����{����z�u
						wallUnit -> wallData[x][z].isItem = j+1;
						//�������Ă����{���폜
						itr = itemBomList[j]->erase(itr);
					}
				}
			}
		}
	}
}

//=============================================================================
//�ʒu���t�B�[���h�����𔻒�
//�����@�@�Ȃ�
//�߂�l�@�Ȃ�
//=============================================================================
void Hurricane::inRange()
{
	if(pos.z > MAXRANGE) {
		setInfo();
		pos.z = MAXRANGE;
	} 
	
	if(pos.x > MAXRANGE) {
		setInfo();
		pos.x = MAXRANGE;
	} 
	
	if(pos.z < MINRANGE) {
		setInfo();
		pos.z = MINRANGE;
	} 
	
	if(pos.x < MINRANGE) {
		setInfo();
		pos.x = MINRANGE;
	} 
}