//=============================================================================
//  �T���_�[
//�@�쐬���F2015/6/28
//�@�X�V���F2016/10/30
//	����ҁF���X�ؗ��M
//=============================================================================
#include "Thunder.h"
#include "NormalCpu.h"
#include "PsyCpu.h"
#include "SpeedCpu.h"
#include "VsPlayer.h"
#include "vsCpuPlayer.h"
#include "modeChange.h"
#include "Camera.h"
#include "Wall.h"
#include "Model.h"
#include "effect.h"
#include <time.h>

#define MAXRANGE 49			//�ő�͈�
#define MINRANGE 1			//�ŏ��͈�
#define RANDUMRANGE 200		//�����_���͈�
#define RANDUMPOSRANGE 49	//�����_���Ȉʒu�͈̔�
#define ISBOM 100			//�{�������݂���
#define CHARANUM 3			//�L�����N�^�[�̐�
#define BOMRENDERTIME 150	//�{���̕`�掞��
#define MOVEX 0
#define MOVEZ 1
#define ONEMINTIMER 30/*fps*/ * 60/*�b���番*/ //1���Ԃ̃^�C�}�[
#define PLAYERNUM 2			//2P�ΐ펞�̃L�����N�^�[��
#define CAMERA1 1			//�J�����ԍ�
#define RENDERTIME 2		//�`�掞��
#define UPDATEANIM 1.0f		//�A�j���[�V�����X�V

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
Thunder::Thunder(void)
{
	//�G�t�F�N�g�ǂݍ���
	thunder = new effect;
	thunder->Load(graphics, "pictures\\thunder.png", 6, 3);

	initialize();
}

//=============================================================================
//  ������
//�@�����F�Ȃ�
//	�߂�l�F�Ȃ�
//=============================================================================
void Thunder::initialize()
{
	pos = D3DXVECTOR3(0,0,0);
	isOccurrence = false;
	probabilityRange = 1;
	renderCount = 0;
	count = 0;

	vsCpuPlayerUnit = nullptr;
	player1 = nullptr;
	player2 = nullptr;
	cpuUnit[NORMALCPU] = nullptr;
	cpuUnit[PYECPU] = nullptr;
	cpuUnit[SPEEDCPU] = nullptr;
}

//=============================================================================
//�f�X�g���N�^
//=============================================================================
Thunder::~Thunder(void)
{
	SAFE_DELETE(thunder);
}

//=============================================================================
//�V�[���̃��j�b�g���擾
//�����F�V�[���̃��j�b�g
//�߂�l�F�Ȃ�
//=============================================================================
//ScceneBase����Ăяo���ꂽUnitBase��collisions����^�[�Q�b�g�擾
//typeid...typeid(�^���܂��͌^�^�̕ϐ����j...���^�ō��ꂽ��
void Thunder::collisions(UnitBase* target)
{
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
void Thunder::update()
{
	//�d���𔭐������邩
	occurrenceThunder();
	thunder->fixPosUpdate(UPDATEANIM);
	//�������Ă����
	if(isOccurrence)
	{
		pos.y+=3.0f;
		thunder->fixPosAdd(pos,3);
		isOccurrence = false;
		renderCount++;
	}
	
	if(renderCount > 0) {
		renderCount++;
		if(renderCount >= 15) {
			//�d���̏Փ˔���
			collThunder();
			if(renderCount >= 18)
				renderCount = 0;
		}
	}
}

//=============================================================================
//�`��
//�����F�t���[�����A�J�����ԍ�
//�߂�l�F�Ȃ�
//=============================================================================
void Thunder::render(float frameTime, int cameraNum)
{
	thunder->fixPosDraw(cameraNum);
}

//=============================================================================
//�d���𔭐�
//�����F�Ȃ�
//�߂�l�F�Ȃ�
//=============================================================================
void Thunder::occurrenceThunder()
{
	//1�����Ƃɔ����m�����グ��
	if(count <= ONEMINTIMER)
	{
		count++;
	} else {
		//�����m����RANDUMRANGE�ȉ��Ȃ�
		if(probabilityRange < RANDUMRANGE)
		{
			//�����m�����グ��
			probabilityRange++;
		}
		count = 0;
	}

	//�܂������������Ă��Ȃ���΃����_���Ȋm���ŗ�����
	if((rand() % RANDUMRANGE <= probabilityRange) && !isOccurrence)
	{
		//�d���������������ǂ����̃t���O��true�ɂ���
		isOccurrence = true;
		//�����_���Ȉʒu����
		pos = D3DXVECTOR3((float)(rand() % RANDUMPOSRANGE+1), -0.5f, (float)(rand() % RANDUMPOSRANGE+1));
	}
}

//=============================================================================
//�d���ɏՓ˂����I�u�W�F�N�g�𔻒�
//�����F�Ȃ�
//�߂�l�F�Ȃ�
//=============================================================================
void Thunder::collThunder()
{	
	float dx[] = {0.0f, 1.0f, 1.0f, 1.0f, 0.0f, -1.0f, -1.0f, 0.0f, 0.0f};
	float dz[] = {1.0f, 1.0f, 0.0f, -1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f};
	//���݂̃^�C���ɗאڂ���e�^�C���𒲂ׂ�
	for(int i = 0; i <= 8; i++)
	{	
		float x = pos.x + dx[i];
		float z = pos.z + dz[i];

		D3DXVECTOR3 conpare(x, -0.5f, z);

		//�L�����N�^�[�ɑ΂��Ă̏Փ˔���			
		if(modeChange::GetInstance() -> getMode() == VS2P/*2P�ΐ�Ȃ�*/)
		{
			collPlayerThunder(conpare);
		} 
		
		if(modeChange::GetInstance() -> getMode() == VSCPU/*CPU�ΐ�Ȃ�*/)
		{
			collCpuThunder(conpare);
			collvsCpuThunder(conpare);
		}
	}
}

//=============================================================================
//�d���ɏՓ˂����v���C���[�𔻒�
//�����F��r�ʒu
//�߂�l�F�Ȃ�
//=============================================================================
void Thunder::collPlayerThunder(D3DXVECTOR3 conparePos)
{
	VsPlayer* playerList[] = {player1,
							player2};

	list<int>::iterator itr;
	for(int i = 0; i < PLAYERNUM; i++)
	{
		//�͈͓��Ƀv���C���[1�������
		if(conparePos == playerList[i]->pos)
		{
			//���S�t���O��true��
			playerList[i]->isLose = true;
		}
	}
}

//=============================================================================
//�d���ɏՓ˂���CPU�𔻒�
//�����F��r�ʒu
//�߂�l�F�Ȃ�
//=============================================================================
void Thunder::collCpuThunder(D3DXVECTOR3 conparePos)
{
	//�eCPU�L�����N�^�[����
	NormalCpu* cpuChara[] = { cpuUnit[PYECPU],
						cpuUnit[SPEEDCPU],
						cpuUnit[NORMALCPU] };

	//CPU�L�����N�^�[�����[�v
	for(int i = 0; i < CHARANUM; i++)
	{
		//�ʒu�����������
		if(cpuChara[i]->pos == conparePos)
		{
			//���S�t���O��true��
			cpuChara[i]->isLose = true;
		}
	}
}

//=============================================================================
//�d���ɏՓ˂���vsCpu�𔻒�
//�����F��r�ʒu
//�߂�l�F�Ȃ�
//=============================================================================
void Thunder::collvsCpuThunder(D3DXVECTOR3 conparePos)
{
	//vsCPU�v���C���[�ƈʒu�����������
	if(vsCpuPlayerUnit -> pos == conparePos)
	{
		//���S�t���O��true��
		vsCpuPlayerUnit -> isLose = true;
	}
}