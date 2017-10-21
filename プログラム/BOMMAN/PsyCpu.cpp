//=============================================================================
//  PsyCPU
//�@�X�V���F2016/10/30
//	����ҁF���X�ؗ��M
//=============================================================================
#include "PsyCpu.h"
#include "SpeedCpu.h"
#include "NormalCpu.h"
#include "normalBom.h"
#include "paralyzeBom.h"
#include "plusBom.h"
#include "vsCpuPlayer.h"
#include "effect.h"
#include "Model.h"
#include "readFile.h"

#define SEARCHRANGE 5		//�L�����N�^�[�𒲍�����͈�
#define GIRTHSEARCH	8		//����8�^�C����
#define OBSTACLELCOST 100	//��Q���̃R�X�g
#define ITEMCOST -3			//AI�Ŏg�p���Ă���A�C�e���̃R�X�g
#define MAXRANGE 49			//�ő�͈�
#define MINRANGE 1			//�ŏ��͈�
#define VIEWRANGE 10		//�L�����N�^�[���猩����͈�
#define ITEMVIEWRANGE 5		//�A�C�e���𒲍�����͈�
#define NOTWALL 0			//�ǂ����݂��Ȃ�
#define UPDATEANIM 1.0f		//�A�j���[�����X�V

const int checkX[] = {-1, 0, 1, -1, 1, -1, 0, 1};
const int checkZ[] = {1, 1, 1, 0, 0, -1, -1, -1};

//�A�C�e��
enum ITEM
{	
	DEFAULTITEM,		//�ʏ�{��
	PLUSBOMITEM,		//�v���X�{��
	PARALYZEBOMITEM,	//Ⴢ�{��
	THROWBOMITEM,		//�{���𓊂���
	LARGEEXPLOSIONITEM,	//�唚���{��
	NONE = 100		//�A�C�e������
};

//�{���̕`��Ɋւ�����
enum BOM_RENDER_STATE
{
	RENDER,			//�`�揈��
	ACTIONEXPSlON,	//�����������s��
	DONAOTHING		//�Ȃ�ł��Ȃ�
};

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
PsyCpu::PsyCpu(void)
{	
	model = new Model;
	//���f���ǂݍ���
	model->Load(graphics->get3Ddevice(),"Models\\chara_b2.fbx");
	
	//�G�t�F�N�g�ǂݍ���
	psyPut = new effect;
	psyPut->Load(graphics, "pictures\\psyPutEffect.png", 4, 4);
		
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
//�V�[���̃��j�b�g���擾
//�����F�V�[���̃��j�b�g
//�߂�l�F�Ȃ�
//=============================================================================
//ScceneBase����Ăяo���ꂽUnitBase��collisions����^�[�Q�b�g�擾
//typeid...typeid(�^���܂��͌^�^�̕ϐ����j...���^�ō��ꂽ��
void PsyCpu::collisions(UnitBase* target)
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
//�f�X�g���N�^
//=============================================================================
PsyCpu::~PsyCpu(void)
{
	SAFE_DELETE(model);
	SAFE_DELETE(psyPut);
	SAFE_DELETE(searchTargetRangeXData);
	SAFE_DELETE(searchTargetRangeZData);
	SAFE_DELETE(targetDistCostData);
}

//=============================================================================
//  ������
//�@�����F�Ȃ�
//	�߂�l�F�Ȃ�
//=============================================================================
void PsyCpu::initialize()
{
	//�e��񏉊���
	isItem  = false;
	charaSearch = false;
	isLose = false;
	isOpenListEmpty = false;
	isPsyPut = false;
	isExitExpMove = false;
	throwFlag = false;	
	isStop = false;
	isTarget = false;
	isOpenListEmpty = false;
	isRenderEffect = false;

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
	renderCount = 0;

	//�ʒu
	pos = D3DXVECTOR3(1,-0.5f,49);
	prevPos = D3DXVECTOR3(0, 0, 0);
	putBomPos = D3DXVECTOR3(0, 0, 0);
	prevTargetPos = D3DXVECTOR3(0,0,0);
	bomThrowPos = D3DXVECTOR3(0,0,0);	
	startPos = D3DXVECTOR3(0, 0, 0);
	goalPos = D3DXVECTOR3(0, 0, 0);
	target = D3DXVECTOR3(0, 0, 0);
	prevTile = D3DXVECTOR3(0, 0, 0);

	//�ŏ��̌���
    angle = 180.0f;

	initList();
}

//=============================================================================
//  �X�V
//�@�����F�Ȃ�
//	�߂�l�F�Ȃ�
//=============================================================================
void PsyCpu::update()
{
	//���S�����ꍇ���������Ȃ�
	if(isLose)
	{
		isControl = AI_STATE::NOTAI;
		return;
	}

	charaNum = CHARACTER::PSY;

	//AI�̌v�Z���A�܂��͈ړ��������ɒ��f���߂��o����
	if(isStop)
	{
		//�ʒu�𐮐��ɂ���
		adjustmentPos();
		//AI�̌v�Z�X�g�b�v
		stopAi();
	}	

	if(isRenderEffect) {
		effectRenderTimeCount();
	} else {
		isPsyPut = false;
	}
	
	//��Ԃ̈ړ�
	control();
	//�{���ǉ�
	bomProcess();
	//�A�C�e���擾����
	getItem();
	//�`�撆�̃{�������邩
	notRenderBom();
	//�G�t�F�N�g�X�V
	psyPut->fixPosUpdate(UPDATEANIM);
	
	if(isControl == AI_STATE::MOVE) {
		//�L�����N�^�[���͈͓��ɂ��邩����
		characterInRange();
	}
}

//=============================================================================
//  �e�{���ɑ΂��鏈��
//�@�����F�Ȃ�
//	�߂�l�F�Ȃ�
//=============================================================================
void PsyCpu::bomProcess()
{
	D3DXVECTOR3 bomPutDir = D3DXVECTOR3(0, 0, 1.0f);
	//��]�s��
	D3DXMATRIX mRotat;
	D3DXMatrixRotationY(&mRotat, D3DXToRadian(bomPutAngle));
	D3DXVec3TransformCoord(&bomPutDir,&bomPutDir,&mRotat);
	//�{����u�������x�N�g���̒����𐮐��ɕϊ�
	bomPutDir = D3DXVECTOR3(toIntMove(bomPutDir.x),0,toIntMove(bomPutDir.z));

	//�e�{���̏���
	switch(itemType/*�ǂ̎�ނ̃{�����������邩*/)
	{
	case ITEM::DEFAULTITEM/*�ʏ�{��*/:
		{
			if(isItem/*�A�C�e���{�����g�p���邩*/)
			{
				//���ݒʏ�{�������̏��������Ă��Ȃ�(=�ʏ�{�����g���Ă��Ȃ�)
				if(bomUnit[NORMALBOM] -> putCharaNum[charaNum].bomRenderState == DONAOTHING)
				{	
					if(!isPsyPut) {
						//�{����u�����ꏊ��ێ�
						putBomPos = D3DXVECTOR3(toIntPos(pos.x)+bomPutDir.x, pos.y, toIntPos(pos.z)+bomPutDir.z);
					}
					bomUnit[NORMALBOM] -> addBom(putBomPos/*�v���C���[�̈ʒu�x�N�g��*/, bomPutDir/*�ړ��x�N�g��*/,addExpRange/*�����͈͉��Z��*/, charaNum/*�L�����N�^�[��*/, bomThrowPos/*��������̈ʒu�x�N�g��*/, throwFlag/*�{����u���������邩*/);
					//�g�p�����{���̎�ނ��i�[
					processBomType = DEFAULTITEM;
				}
				throwFlag = false;
			}									
			isItem = false;
		}
		break;

	case ITEM::PLUSBOMITEM/*�v���X�{��*/:
		{	
			if(isItem/*�A�C�e���{�����g�p���邩*/)
			{
				//plusBom�������	
				if(!(plusBomNumList.empty()))
				{	
					plusBomitr = plusBomNumList.begin();
					if(!isPsyPut) {
						//�{����u�����ꏊ��ێ�
						putBomPos = D3DXVECTOR3(toIntPos(pos.x)+bomPutDir.x, pos.y, toIntPos(pos.z)+bomPutDir.z);
					}
					bomUnit[PLUSBOM] -> addBom(putBomPos/*�v���C���[�̈ʒu�x�N�g��*/, bomPutDir/*�ړ��x�N�g��*/,addExpRange/*�����͈͉��Z��*/, charaNum/*�L�����N�^�[��*/, bomThrowPos/*��������̈ʒu�x�N�g��*/, throwFlag/*�{����u���������邩*/);
					plusBomitr = plusBomNumList.erase(plusBomitr);
				}
				throwFlag = false;
			}	
			isItem = false;
		}
		break;

	case ITEM::PARALYZEBOMITEM/*Ⴢ�{��*/:
		{
			if(isItem/*�A�C�e���{�����g�p���邩*/)
			{
				//paralyzeBom�������	
				if(!(paralyzeBomNumList.empty()))
				{	
					paralyzeitr = paralyzeBomNumList.begin();
					if(!isPsyPut) {
						//�{����u�����ꏊ��ێ�
						putBomPos = D3DXVECTOR3(toIntPos(pos.x)+bomPutDir.x, pos.y, toIntPos(pos.z)+bomPutDir.z);
					}
					bomUnit[PARALYZEBOM] -> addBom(putBomPos/*�v���C���[�̈ʒu�x�N�g��*/, bomPutDir/*�ړ��x�N�g��*/, addExpRange/*�����͈͉��Z��*/, charaNum/*�L�����N�^�[��*/, bomThrowPos/*��������̈ʒu�x�N�g��*/, throwFlag/*�{����u���������邩*/);
					processBomType = PARALYZEBOMITEM;
					paralyzeitr = paralyzeBomNumList.erase(paralyzeitr);
				}	
				throwFlag = false;
			} 
			isItem = false;
		}
		break;
	}

	//Psy���g�������ǂ����̃t���O��false
	isPsyPut = false;
}

//=============================================================================
//  �G�t�F�N�g�`�掞�ԃJ�E���g
//�@�����F�Ȃ�
//	�߂�l�F�Ȃ�
//=============================================================================
void PsyCpu::effectRenderTimeCount()
{
	if(renderCount <= 16) {
		renderCount++;
	} else {
		isPsyPut = true;
		//PSY�Ń{����u�������̃t���O��true
		isRenderEffect = false;
		renderCount = 0;
		//�{����z�u
		putBom();
	}
}

//=============================================================================
//  �G�t�F�N�g�`��
//�@�����F�Ȃ�
//	�߂�l�F�Ȃ�
//=============================================================================
void PsyCpu::effectRender(float frameTime, int cameraNum/*�J�����ԍ�*/)
{
	//�G�t�F�N�g�`��
	psyPut->fixPosDraw(cameraNum);
}

//=============================================================================
//  �L�����N�^�[���͈͓��ɂ��邩(�����炻�̃L�����N�^�[�̋ߕӂɃ{����u��
//�@�����F�Ȃ�
//	�߂�l�F�Ȃ�
//=============================================================================
void PsyCpu::characterInRange()
{
	if(processBomType == NONE) {
		//������͈͓����[�v
		for(int z = -SEARCHRANGE; z <= SEARCHRANGE; z++)
		{
			//�t�B�[���h�O�ɏo����߂�
			if(pos.z+z < MINRANGE || pos.z+z > MAXRANGE)
				continue;

			for(int x = -SEARCHRANGE; x <= SEARCHRANGE; x++)
			{
				if(pos.x+x < MINRANGE || pos.x+x > MAXRANGE)  
					continue;


				//�v���C���[��������
				if(D3DXVECTOR3(pos.x+x, 0, pos.z+z) == D3DXVECTOR3(vsCpuPlayerUnit->pos.x, 0, vsCpuPlayerUnit->pos.z))
				{				
					serchCharacterAround(D3DXVECTOR3(vsCpuPlayerUnit->pos.x, 0, vsCpuPlayerUnit->pos.z));
				}

				//�X�s�[�hCPU��������
				if(D3DXVECTOR3(pos.x+x, 0, pos.z+z) == D3DXVECTOR3(cpuUnit[SPEEDCPU] -> pos.x, 0, cpuUnit[SPEEDCPU] -> pos.z))
				{					
					serchCharacterAround(D3DXVECTOR3(cpuUnit[SPEEDCPU] -> pos.x, 0, cpuUnit[SPEEDCPU] -> pos.z));
				}

				//�^�N�e�B�N�XCPU��������
				if(D3DXVECTOR3(pos.x+x, 0, pos.z+z) == D3DXVECTOR3(cpuUnit[NORMALCPU] -> pos.x, 0, cpuUnit[NORMALCPU] -> pos.z))
				{					
					serchCharacterAround(D3DXVECTOR3(cpuUnit[NORMALCPU] -> pos.x, 0, cpuUnit[NORMALCPU] -> pos.z));
				}

			}
		}	
	}
}

//=============================================================================
//  �L�����N�^�[�̎���8�}�X��T��
//�@�����F�Ȃ�
//	�߂�l�F�Ȃ�
//=============================================================================
void PsyCpu::serchCharacterAround(D3DXVECTOR3 serchPos)
{
	if(!isRenderEffect) {
		//�������L�����N�^�[�̎���8�^�C������
		for(int serch = 0; serch < GIRTHSEARCH; serch++)
		{
			//�{����u�����ʒu��ێ�
			putBomPos = D3DXVECTOR3(serchPos.x + checkX[serch], pos.y, serchPos.z + checkZ[serch]);	
			//�����^�C�����ǂłȂ�
			if(isObstacel(putBomPos)
				&& !(putBomPos.x < MINRANGE) && !(putBomPos.x > MAXRANGE)
					&& !(putBomPos.z < MINRANGE) && !(putBomPos.z > MAXRANGE))
			{	
				//���ݏ������Ă���{�����Ȃ����
				if(processBomType == NONE) {
					isRenderEffect = true;
					addEffect();	
				}
				break;
			}
		}
	}
}
  
//=============================================================================
//�G�t�F�N�g�ǉ�
//�����F�Ȃ�
//�߂�l�F�Ȃ�
//=============================================================================
void PsyCpu::addEffect()
{
	D3DXVECTOR3 effectPos(putBomPos.x, 0, putBomPos.z);
	psyPut->fixPosAdd(effectPos, 2);
}

//=============================================================================
//�{����z�u
//�����F�Ȃ�
//�߂�l�F�Ȃ�
//=============================================================================
void PsyCpu::putBom()
{
	D3DXVECTOR3 bomPutDir = pos + calcMove(angle);
	
	if(!isRenderEffect) {
 		//���ݏ������Ă���{�����Ȃ����
		if(processBomType == NONE) {
			if(!isPsyPut) {
				if(!bomThrowNumList.empty()/*�X���[�{���������*/) {
					//�O�㍶�E�̉�������Ƀ^�[�Q�b�g�����邩
					if(isThrowTarget()) {
						throwFlag = true;
						bomThrowNumList.pop_back();
					}
				}
			}
				
		if((isObstacel(bomPutDir) && isPutBom(bomPutDir)
			&& !(bomPutDir.x < MINRANGE) && !(bomPutDir.x > MAXRANGE)
				&& !(bomPutDir.z < MINRANGE) && !(bomPutDir.z > MAXRANGE)) || throwFlag || isPsyPut){
				if(!paralyzeBomNumList.empty()/*Ⴢ�{���������*/ && charaSearch) { 
					if(!throwFlag)
						bomPutAngle = angle;
					//�ǂ̃{����I�񂾂�����
					itemType = ITEM::PARALYZEBOMITEM;
					//�A�C�e���{�����g�p
					isItem = true;
					return;
				} else if(bomUnit[NORMALBOM] -> putCharaNum[PSY].bomRenderState == DONAOTHING/*�ʏ�{�����������łȂ����*/) {
					if(!throwFlag)
						bomPutAngle = angle;
					//�ǂ̃{����I�񂾂�����
					itemType = ITEM::DEFAULTITEM;
					//�A�C�e���{�����g�p
					isItem = true;
					return;
				}
			}
		}
		
		//�v���X�{�������݂����
		if((!plusBomNumList.empty() && isObstacel(bomPutDir) && isPutBom(bomPutDir)
				&& !(bomPutDir.x < MINRANGE) && !(bomPutDir.x > MAXRANGE)
					&& !(bomPutDir.z < MINRANGE) && !(bomPutDir.z > MAXRANGE)) || throwFlag) {
			if(!throwFlag)
				bomPutAngle = angle;
			//�ǂ̃{����I�񂾂�����
			itemType = ITEM::PLUSBOMITEM;
			//�A�C�e���{�����g�p
			isItem = true;
			return;
		}
	}
}

