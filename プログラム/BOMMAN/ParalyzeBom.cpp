//=============================================================================
//  Ⴢ�{��
//�@�X�V���F2016/10/30
//	����ҁF���X�ؗ��M
//=============================================================================
#include "paralyzeBom.h"
#include "VsPlayer.h"
#include "vsCpuPlayer.h"
#include "PsyCpu.h"
#include "SpeedCpu.h"
#include "NormalCpu.h"
#include "modeChange.h"
#include "Camera.h"
#include "audio.h"
#include "Wall.h"
#include "effect.h"
#include "Model.h"
#include "VsPlayer.h"

#define RENDERTIME 90	//�`�掞��
#define NOTWALL 0		//�ǂȂ�
#define CHARANUM 3		//�L�����N�^�[��
#define CAMERA1 1		//�J�����ԍ�
#define SCALE 13		//�傫��
#define RANGE 5			//�͈�
#define OBSTACLELCOST 100		//�Ǔ��̏�Q����CPU�R�X�g
#define NONCOST 0				//�R�X�g�Ȃ�
#define ITEMCOST -3				//�A�C�e���̃R�X�g
#define MAXRANGE 49				//�ő�͈�
#define MINRANGE 1				//�Œ�͈�
#define UPDATEANIM 1.0f			//�A�j���[�V�����X�V

//�{���̕`��Ɋւ�����
enum BOM_RENDER_STATE
{
	RENDER,			//�`�揈��
	ACTIONEXPSlON,	//�����������s��
	DONAOTHING		//�Ȃ�ł��Ȃ�
};

//�{���̎��
enum BOM_NAME
{
	NORMAL,
	PLUS,
	PARALYZE
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
paralyzeBom::paralyzeBom(void)
{	
	//���f���ǂݍ���
	model = new Model;
	model->Load(graphics->get3Ddevice(),"Models\\pBom.fbx");

	//�I�[�f�B�I
	audio = new Audio;

	//�I�[�f�B�I�ǂݍ���
	audio->initialize("Sounds\\Wave Bank.xwb", "Sounds\\Sound Bank.xsb");

	initializeMain();
}

//=============================================================================
//������
//=============================================================================
void paralyzeBom::initializeMain(void)
{
	//�G�t�F�N�g�ǂݍ���
	fire = new effect;
	fire->Load(graphics, "pictures\\hibana.png", 3, 3);

	exp = new effect;
	exp->Load(graphics, "pictures\\paralyzeEffect.png", 6, 4);

	//�{���̏�ԏ�����
	for(int i = 0; i <= CHARANUM; i++)
	{
		putCharaNum[i].bomRenderState = DONAOTHING;
	}

	initList();

	count = 0;
	bomType = BOM_NAME::PARALYZE;

	wallUnit = nullptr;
	player1 = nullptr;
	player2 = nullptr;
	vsCpuPlayerUnit = nullptr;
	cpuUnit[NORMALCPU] = nullptr;
	cpuUnit[PYECPU] = nullptr;
	cpuUnit[SPEEDCPU] = nullptr;
}

//=============================================================================
//������
//=============================================================================
void paralyzeBom::initialize(void)
{
	SAFE_DELETE(fire);
	SAFE_DELETE(exp);
	initializeMain();
}

//=============================================================================
//�f�X�g���N�^
//=============================================================================
paralyzeBom::~paralyzeBom(void)
{
	SAFE_DELETE(model);
	SAFE_DELETE(fire);
	SAFE_DELETE(exp);
	SAFE_DELETE(audio);
}

//=============================================================================
//�V�[���̃��j�b�g���擾
//�����F�V�[���̃��j�b�g
//�߂�l�F�Ȃ�
//=============================================================================
//ScceneBase����Ăяo���ꂽUnitBase��collisions����^�[�Q�b�g�擾
//typeid...typeid(�^���܂��͌^�^�̕ϐ����j...���^�ō��ꂽ��
void paralyzeBom::collisions(UnitBase* target)
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
//  �X�V
//�@�����F�Ȃ�
//	�߂�l�F�Ȃ�
//=============================================================================
void paralyzeBom::update()
{	
	bomType = BOM_NAME::PARALYZE;
	//�G�t�F�N�g�X�V
	fire->fixPosUpdate(UPDATEANIM);
	exp->fixPosUpdate(UPDATEANIM);
	//�{���̏����̐i�߂�
	renderFlow();
	explosionFlow();
}

//=============================================================================
//���������̗���
//�����F�Ȃ�
//�߂�l�F�Ȃ�
//=============================================================================
void paralyzeBom::explosionFlow()
{
	//�{���̏������s���Ă���L�����N�^�[��1�l�ȏ㑶��
	if(!(explosionProcessList.empty()))
	{
		list<int>::iterator	procitr;	
		procitr = explosionProcessList.begin();
		//�`�揈�����I�����Ă����
		if(putCharaNum[*procitr].bomRenderState == ACTIONEXPSlON) 
		{
			//BGM�Đ�
			audio->playCue("������1");
			explosion(*procitr);
			//�����͈͂ɃR�X�g�ݒ�
			setExpRange(false, *procitr);
			//�������Ȃ�
			putCharaNum[*procitr].bomRenderState = DONAOTHING;
			//�ʒu��������(�K�v�Ƃ��Ȃ����������Ă���)
			putCharaNum[*procitr].bomPos = D3DXVECTOR3(999,999,999);
			procitr = explosionProcessList.erase(procitr);			

		}
	}
}

//=============================================================================
//�{���̔�������
//�����F�L�����N�^�[�ԍ�
//�߂�l�F�Ȃ�
//=============================================================================
void paralyzeBom::explosion(int processCharaNum/*����������L�����N�^�[�ԍ�*/)
{	
	int blockX;
	int blockZ;

	//�����͈͕����[�v
	for(int hight = -RANGE; hight <= RANGE; hight++)
	{	
		for(int width = -RANGE; width <= RANGE; width++) 
		{
			//�n���ꂽ�L�����N�^�[�ԍ��̃{���̕`�揈�����������Ă����
			if(putCharaNum[processCharaNum].bomRenderState == ACTIONEXPSlON) {
				//�{���̈ʒu�Ɣ��蕪�����Z
				blockX = (int)(putCharaNum[processCharaNum].bomPos.x+width);
				blockZ = (int)(putCharaNum[processCharaNum].bomPos.z+hight);
			}

			if(blockX < MINRANGE || blockX > MAXRANGE
				|| blockZ < MINRANGE || blockZ > MAXRANGE)
				continue;

			//�e�L�����N�^�[�ƈʒu�������Ȃ�
			//CPU�̑ΐ�
			if(modeChange::GetInstance() -> getMode() == VSCPU) {
				if(blockX == vsCpuPlayerUnit ->pos.x && blockZ == vsCpuPlayerUnit ->pos.z) {
					vsCpuPlayerUnit ->isStop = true;	//�s�����~����t���O��true
				}
			
				if(blockX == cpuUnit[SPEEDCPU] -> pos.x && blockZ == cpuUnit[SPEEDCPU] -> pos.z) {
					cpuUnit[SPEEDCPU]->isStop = true;
				} 
			
				if(toIntPos(blockX == cpuUnit[NORMALCPU] -> pos.x) && blockZ == toIntPos(cpuUnit[NORMALCPU] -> pos.z)) {
					cpuUnit[NORMALCPU]->isStop = true;
				} 
			
				if(toIntPos(blockX == cpuUnit[PYECPU] -> pos.x) && blockZ == toIntPos(cpuUnit[PYECPU] -> pos.z)) {
					cpuUnit[PYECPU]->isStop = true;
				}
			}

			//2P�ΐ�
			if(modeChange::GetInstance() -> getMode() == VS2P)
			{
				if(blockX == player1 -> pos.x && blockZ == player1 -> pos.z) {
					player1->isStop = true;
				}

				if(blockX == player2 -> pos.x && blockZ == player2 -> pos.z) {
					player2->isStop = true;
				}
			}
		}
	}	
}

//=============================================================================
//�{�������݂��邩��Ԃ�
//�����F�{�������݂��邩��m�肽�����W
//�߂�l�F�Ȃ�
//=============================================================================
int paralyzeBom::retIsBom(float x, float z)
{
	//�ʏ�{���̈ʒu
	//�L�����N�^�[�����[�v
	for(int i = 0; i < 4; i++)
	{
		//���߂�ʒu�Ƀ{�������݂����
		if(toIntPos(putCharaNum[i].bomPos.x) == toIntPos(x) && toIntPos(putCharaNum[i].bomPos.z) == toIntPos(z))
		{
			return putCharaNum[i].cost;
		}
	}

	return 0;
}

//=============================================================================
//�����͈͂̃R�X�g��ݒ�
//�����F�R�X�g��ݒ肷�邩�ǂ����A�L�����N�^�[�ԍ�
//�߂�l�F�Ȃ�
//=============================================================================
void paralyzeBom::setExpRange(bool isSet, int chara)
{
	count++;
	//�����͈͕����[�v
	for(int hight = -RANGE; hight <= RANGE; hight++)
	{	
		for(int width = -RANGE; width <= RANGE; width++) 
		{		
			int blockX = (int)(putCharaNum[chara].bomPos.x+width);
			int blockZ = (int)(putCharaNum[chara].bomPos.z+hight);

			if(blockX < MINRANGE || blockX > MAXRANGE
				|| blockZ < MINRANGE || blockZ > MAXRANGE)
				continue;

			if(isSet) {
					wallUnit -> wallData[blockX][blockZ].isExpRange = true;
					wallUnit -> wallData[blockX][blockZ].expRangeNum = count;
			} else {
				if(wallUnit -> wallData[blockX][blockZ].isExpRange) {
					wallUnit -> wallData[blockX][blockZ].isExpRange = false;
				}
			}
		}
	}
}