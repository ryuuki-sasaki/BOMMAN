//=============================================================================
//  SpeedCPU
//�@�X�V���F2016/10/30
//	����ҁF���X�ؗ��M
//=============================================================================
#include "SpeedCpu.h"
#include "NormalCpu.h"
#include "PsyCpu.h"
#include "normalBom.h"
#include "paralyzeBom.h"
#include "plusBom.h"
#include "vsCpuPlayer.h"
#include "Model.h"
#include "readFile.h"

#define VIEWRANGE 10		//�L�����N�^�[���猩����͈�
#define ITEMVIEWRANGE 5		//�A�C�e���𒲍�����͈�
#define OBSTACLELCOST 100	//��Q���̃R�X�g
#define MAXRANGE 49			//�ő�͈�
#define MINRANGE 1			//�ŏ��͈�	
#define ITEMCOST -3			//AI�Ŏg�p���Ă���A�C�e���̃R�X�g

//AI�̏��
enum AI_STATE
{
	INIT,	//������
	CALC,	//�v�Z��
	MOVE,	//�ړ�����
	STAY,	//��~
	NOTAI = 100	//AI���g�p���Ȃ�
};

//�L�����N�^�[
enum CHARACTER
{
	PLAYER,
	PSY,
	NORMAL,
	SPEED
};

//��r�^�C�����ȑO�ʂ����^�C��������������̏��
enum PREV_TILE_STATE
{
	NOT_PRAV_TILE,		//�ʂ������Ƃ̂Ȃ��^�C��
	TO_PRAV_TILE_MOVE,	//���̃^�C���ɐi��
	MOVE_DIR			//�i�񂾂��Ƃ̂Ȃ��^�C���ɐi��
};

//�{���̎��
enum BOM_TYPE
{
	NORMALBOM,
	PARALYZEBOM,
	PLUSBOM
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
SpeedCpu::SpeedCpu(void)
{	
	model = new Model;
	//���f���ǂݍ���
	model->Load(graphics->get3Ddevice(),"Models\\chara_b2.fbx");

	//csv�t�@�C���Ǎ���
	searchTargetRangeXData = new readFile;
	searchTargetRangeZData = new readFile;
	targetDistCostData = new readFile;

	//csv�t�@�C���Ǎ���
	if(!searchTargetRangeXData->read("CSV\\searchTargetRangeX.csv"))
	{
		throw(GameError(gameErrorNS::FATAL_ERROR, "CSV���ǂݍ��߂܂���ł���"));
	}

	int index1 = 0;
	//���ԂɊe�}�X�̒l���擾
	for(int i = 0; i < 112; i++)
	{
		searchTargetRangeXTable[i] = searchTargetRangeXData -> getToComma(&index1);
	}
	searchTargetRangeXData->deleteDataArray();

	if(!searchTargetRangeZData->read("CSV\\searchTargetRangeZ.csv"))
	{
		throw(GameError(gameErrorNS::FATAL_ERROR, "CSV���ǂݍ��߂܂���ł���"));
	}

	int index2 = 0;
	//���ԂɊe�}�X�̒l���擾
	for(int i = 0; i < 112; i++)
	{
		searchTargetRangeZTable[i] = searchTargetRangeZData -> getToComma(&index2);
	}
	searchTargetRangeZData->deleteDataArray();

	if(!targetDistCostData->read("CSV\\targetDistCost.csv"))
	{
		throw(GameError(gameErrorNS::FATAL_ERROR, "CSV���ǂݍ��߂܂���ł���"));
	}

	int index3 = 0;
	//���ԂɊe�}�X�̒l���擾
	for(int i = 0; i < 112; i++)
	{
		targetDistCostTable[i] = targetDistCostData -> getToComma(&index3);
	}
	targetDistCostData->deleteDataArray();

	initialize();
}

//=============================================================================
//�f�X�g���N�^
//=============================================================================
SpeedCpu::~SpeedCpu(void)
{
	SAFE_DELETE(model);
	SAFE_DELETE(searchTargetRangeXData);
	SAFE_DELETE(searchTargetRangeZData);
	SAFE_DELETE(targetDistCostData);
}

//=============================================================================
//  ������
//�@�����F�Ȃ�
//	�߂�l�F�Ȃ�
//=============================================================================
void SpeedCpu::initialize()
{
	//�e��񏉊���
	isItem  = false;
	charaSearch = false;
	isLose = false;
	isOpenListEmpty = false;
	isExitExpMove = false;
	throwFlag = false;	
	isStop = false;
	isTarget = false;
	isOpenListEmpty = false;

	isControl = AI_STATE::INIT;
	prevTileState = PREV_TILE_STATE::NOT_PRAV_TILE;

	moveSpeed = 0.0f;
	stopCount = 0;
	wallTargetMul = 1.0f;	
	charaTargetMul = 0.5f;
	addExpRange = 0;
	moveDir = 0;	
	prevMoveDir = 0;
	itemType = 100;
	processBomType = 100;
	charaNum = 100;
	frameCount = 0;
	bomPutAngle = 0.0f;
	loopNum = 0;
	toTargetCost = 0;
	prevWallCount = 0; 
	prevDirIndex = 100;

	//�ʒu
	pos = D3DXVECTOR3(1,-0.5f,1);
	prevPos = D3DXVECTOR3(0, 0, 0);
	putBomPos = D3DXVECTOR3(0, 0, 0);
	prevTargetPos = D3DXVECTOR3(0,0,0);
	bomThrowPos = D3DXVECTOR3(0,0,0);	
	startPos = D3DXVECTOR3(0, 0, 0);
	goalPos = D3DXVECTOR3(0, 0, 0);
	target = D3DXVECTOR3(0, 0, 0);
	prevTile = D3DXVECTOR3(0, 0, 0);

	//�ŏ��̌���
    angle = 0.0f;

	initList();
}

//=============================================================================
//�V�[���̃��j�b�g���擾
//�����F�V�[���̃��j�b�g
//�߂�l�F�Ȃ�
//=============================================================================
//ScceneBase����Ăяo���ꂽUnitBase��collisions����^�[�Q�b�g�擾
//typeid...typeid(�^���܂��͌^�^�̕ϐ����j...���^�ō��ꂽ��
void SpeedCpu::collisions(UnitBase* target)
{
	if(typeid(*target) == typeid(normalBom))
		bomUnit[NORMALBOM] = ((normalBom*)target);
	
	if(typeid(*target) == typeid(paralyzeBom))
		bomUnit[PARALYZEBOM] = ((paralyzeBom*)target);

	if(typeid(*target) == typeid(plusBom))
		bomUnit[PLUSBOM] = ((plusBom*)target);

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
}

//=============================================================================
//  �X�V
//�@�����F�Ȃ�
//	�߂�l�F�Ȃ�
//=============================================================================
void SpeedCpu::update()
{
	//���S�����ꍇ���������Ȃ�
	if(isLose)
	{
		isControl = AI_STATE::NOTAI;
		return;
	}

	charaNum = CHARACTER::SPEED;

	//AI�̌v�Z���A�܂��͈ړ��������ɒ��f���߂��o����
	if(isStop)
	{
		//�ʒu�𐮐��ɂ���
		adjustmentPos();
		//AI�̌v�Z�X�g�b�v
		stopAi();
	}

	//��Ԃ̈ړ�
	control();
	//�{���ǉ�
	bomProcess();
	//�A�C�e���擾����
	getItem();
	//�`�撆�̃{�������邩
	notRenderBom();
}




