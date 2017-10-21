//=============================================================================
//  NormalCpu
//�@�X�V���F2016/10/30
//	����ҁF���X�ؗ��M
//=============================================================================
#include "NormalCpu.h"
#include "PsyCpu.h"
#include "SpeedCpu.h"
#include "vsCpuPlayer.h"
#include "input.h"
#include "Camera.h"
#include "normalBom.h"
#include "paralyzeBom.h"
#include "plusBom.h"
#include "Model.h"
#include "Wall.h"
#include "readFile.h"

#define COSTPLUS 1			//�R�X�g
#define OBSTACLELCOST 100	//��Q���̃R�X�g
#define ITEMCOST -3			//�A�C�e���̃R�X�g
#define VIEWRANGE 5			//�L�����N�^�[��������͈�
#define MAXRANGE 49			//�ő�͈�
#define MINRANGE 1			//�Œ�͈�
#define ITEMVIEWRANGE 25	//�A�C�e���Ɋւ��Č�����͈�
#define DIVIDEFRAME 10		//�t���[�����Ƃ̏����𕪊�����p�x
#define MAXLISTSIZE 500		//���X�g�T�C�Y�̏��
#define TWOTILE 2			//�^�C��2��
#define STOPTIME 150		//�������~���鎞��
#define CAMERA1	1			//�J�����ԍ�
#define NOTCALCRANGE 3		//�v�Z���Ȃ��͈�
#define NOTWALL 0			//�ǂ����݂��Ȃ�
#define ISWALL 1			//�ǂ�����
#define CHARANUM 3			//�L�����N�^�[��
#define CHECKRANGE 10		//�L�����N�^�[�̎��̓`�F�b�N�͈̔�
#define PREVTARGETRANGE 5	//�O�̃^�[�Q�b�g�ʒu�̎���
#define INITNUM 100			//���������ɑ������l

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

//�i�s����
enum MOVE_DIR
{
	ADVANCE,
	RIGHT,
	LEFT,
	BACKWARD
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
NormalCpu::NormalCpu(void)
{	
	model = new Model;
	//���f���ǂݍ���
	model->Load(graphics->get3Ddevice(),"Models\\chara_b2.fbx");

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
NormalCpu::~NormalCpu(void)
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
void NormalCpu::initialize()
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
	wallTargetMul = 0.5;	
	charaTargetMul = 1.2f;
	addExpRange = 0;
	moveDir = 0;	
	prevMoveDir = 0;
	itemType = NONE;
	processBomType = INITNUM;
	charaNum = INITNUM;
	frameCount = 0;
	bomPutAngle = 0.0f;
	loopNum = 0;
	toTargetCost = 0;
	prevWallCount = 0; 
	prevDirIndex = INITNUM;

	//�ʒu
	pos = D3DXVECTOR3(49,-0.5f,1);
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

	wallUnit = nullptr;
	vsCpuPlayerUnit = nullptr;
	cpuUnit[NORMALCPU] = nullptr;
	cpuUnit[PYECPU] = nullptr;
	cpuUnit[SPEEDCPU] = nullptr;
}

//=============================================================================
//�V�[���̃��j�b�g���擾
//�����F�V�[���̃��j�b�g
//�߂�l�F�Ȃ�
//=============================================================================
//ScceneBase����Ăяo���ꂽUnitBase��collisions����^�[�Q�b�g�擾
//typeid...typeid(�^���܂��͌^�^�̕ϐ����j...���^�ō��ꂽ��
void NormalCpu::collisions(UnitBase* target)
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
//  ���X�g��������
//�@�����F�Ȃ�
//	�߂�l�F�Ȃ�
//=============================================================================
void NormalCpu::initList()
{
	if(!plusBomNumList.empty()) {
		plusBomNumList.clear();
	}

	if(!paralyzeBomNumList.empty()) {
		paralyzeBomNumList.clear();
	}

	if(!bomThrowNumList.empty()) {
		bomThrowNumList.clear();
	}	

	if(!openList.empty()) {
		openList.clear();
	}

	if(!closeList.empty()) {
		closeList.clear();
	}

	if(!parentList.empty()) {
		parentList.clear();
	}	

	if(!prevPosList.empty()) {
		prevPosList.clear();
	}	
}

//=============================================================================
//  �`��
//�@�����F�t���[��
//	�߂�l�F�Ȃ�
//=============================================================================
void NormalCpu::render(float frameTime, int cameraNum)
{
	//���S�����ꍇ���������Ȃ�
	if(isLose)
		return;
	
	//�s��
	D3DXMATRIX mat,mTrans,mRotat;

	//�������Ȃ��s��i�P�ʍs��j
	D3DXMatrixIdentity(&mat);

	//�ړ��s��
	D3DXMatrixTranslation(&mTrans,pos.x, pos.y, pos.z);
	//��]�s��
	D3DXMatrixRotationY(&mRotat, D3DXToRadian(angle));
	//�s��̊|���Z
	mat = mRotat * mTrans;

	if(cameraNum == 1) {
		//���[���h�A�r���[�A�ˉe�s��
		//Test.fx�̃O���[�o���ϐ��ɒl��n��
		//�s����V�F�[�_�ɓn��...SetMatrix
		pEffect -> SetMatrix("g_matWVP", 
			&( mat* cameraA -> GetView() * cameraA -> GetProj() ) );

		//�J�����̈ʒu
		pEffect->SetVector("g_vecEye", (D3DXVECTOR4*)&cameraA->GetPos());
	} 

	//���[���h�̋t�s��
	D3DXMATRIX matInvW;
	D3DXMatrixInverse(&matInvW, 0, &mat); //mat�̋t�s��matInvW�ɓ���
	D3DXMatrixTranspose(&matInvW, &matInvW); //�s�Ɨ���Ђ�����Ԃ�
	pEffect->SetMatrix("g_matInvW",&matInvW);

	//���C�g�̕����i���̂��猩��)
	D3DXVECTOR4 light(-1, 1, 0, 1);	//�E���O����
	D3DXVec4Normalize(&light, &light); //���K��

	pEffect->SetVector("g_vecLightDir",&light);

	//�`��J�n
	pEffect -> Begin(NULL, 0);
	//�p�X�J�n
	pEffect -> BeginPass(0);
	//�`��
	model->Draw(&mat);
	//�p�X�I��
	pEffect -> EndPass();
	//�`��I��
	pEffect -> End();

	effectRender(frameTime, cameraNum);
}

//=============================================================================
//  �X�V
//�@�����F�Ȃ�
//	�߂�l�F�Ȃ�
//=============================================================================
void NormalCpu::update()
{
	//���S�����ꍇ���������Ȃ�
	if(isLose)
	{
		isControl = AI_STATE::NOTAI;
		return;
	}
	
	charaNum = CHARACTER::NORMAL;

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

//=============================================================================
//  �`�撆�̃{�������邩
//�@�����F�Ȃ�
//	�߂�l�F�Ȃ�
//=============================================================================
void NormalCpu::notRenderBom()
{
	//�e�{���ŁA�`�撆�̂��̂�����Ίe�N���X��render�Ăяo��
	if(bomUnit[NORMALBOM] -> putCharaNum[charaNum].bomRenderState != RENDER/*�ʏ�{��*/
		&& bomUnit[PARALYZEBOM] -> putCharaNum[charaNum].bomRenderState != RENDER/*Ⴢ�{��*/) 
	{
		//�����`�揈�������Ă��Ȃ�
		processBomType = NONE;
	}
}

//=============================================================================
//  �e�{���ɑ΂��鏈��
//�@�����F�Ȃ�
//	�߂�l�F�Ȃ�
//=============================================================================
void NormalCpu::bomProcess()
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
					//�{����u�����ꏊ��ێ�
					putBomPos = D3DXVECTOR3(toIntPos(pos.x)+bomPutDir.x, pos.y, toIntPos(pos.z)+bomPutDir.z);
					bomUnit[NORMALBOM] -> addBom(putBomPos/*�v���C���[�̈ʒu�x�N�g��*/, bomPutDir/*�ړ��x�N�g��*/,addExpRange/*�����͈͉��Z��*/, charaNum/*�L�����N�^�[��*/, bomThrowPos/*��������̈ʒu�x�N�g��*/, throwFlag/*�{����u���������邩*/);
					//�g�p�����{���̎�ނ��i�[
					processBomType = ITEM::DEFAULTITEM;
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
					//�{����u�����ꏊ��ێ�
					putBomPos = D3DXVECTOR3(toIntPos(pos.x)+bomPutDir.x, pos.y, toIntPos(pos.z)+bomPutDir.z);
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
					//�{����u�����ꏊ��ێ�
					putBomPos = D3DXVECTOR3(toIntPos(pos.x)+bomPutDir.x, pos.y, toIntPos(pos.z)+bomPutDir.z);
					bomUnit[PARALYZEBOM] -> addBom(putBomPos/*�v���C���[�̈ʒu�x�N�g��*/, bomPutDir/*�ړ��x�N�g��*/, addExpRange/*�����͈͉��Z��*/, charaNum/*�L�����N�^�[��*/, bomThrowPos/*��������̈ʒu�x�N�g��*/, throwFlag/*�{����u���������邩*/);
					processBomType = ITEM::PARALYZEBOMITEM;
					paralyzeitr = paralyzeBomNumList.erase(paralyzeitr);
				}	
				throwFlag = false;
			} 
			isItem = false;
		}
		break;
	}
}

//=============================================================================
//  AI����������
//�@�����F�Ȃ�
//	�߂�l�F�Ȃ�
//=============================================================================
void NormalCpu::initAstar()
{
	//�X�^�[�g�ʒu�i�[
	startPos = D3DXVECTOR3(pos.x, 0, pos.z);

	if(situationCheck()) {
		//�S�[���ʒu�i�[
		goalPos = retTarget(pos);
	} else {
		return;
	}

	//�I�[�v�����X�g�ɊJ�n�^�C����ǉ�	
	{
		pushData.tilePos = D3DXVECTOR3(pos.x,0,pos.z);
		pushData.cost = 0;
		pushData.heuristic = 0;	
		pushData.score = pushData.cost + pushData.heuristic;
		pushData.parentTile = NULL;
	}
	openList.push_back(pushData);

	openListitr = openList.begin();

	//AI�̏�Ԃ��v�Z������
	isControl = AI_STATE::CALC;
}

//=============================================================================
//  �L�����N�^�[�̎��͂��`�F�b�N
//�@�����F���݈ʒu
//	�߂�l�F�����ׂ���
//=============================================================================
bool NormalCpu::situationCheck() 
{
	for(int i = -CHECKRANGE; i <= CHECKRANGE; i++) {
		for(int j = -CHECKRANGE; j <= CHECKRANGE; j++) {
		float x = pos.x+i, z = pos.z+j;
		D3DXVECTOR3 conpare(toIntPos(x),0,toIntPos(z));

			//���݈ʒu�������͈͊O�ł����e���T���͈͓��ɑ��݂����
			if(!wallUnit -> retIsExpRange(toIntPos(pos.x), toIntPos(pos.z))
				&& (bomUnit[NORMALBOM] -> retIsBom(conpare.x, conpare.z) == OBSTACLELCOST/*��r����^�C������Q��(�{��)�łȂ��H*/ 
					|| bomUnit[PLUSBOM] -> retIsBom(conpare.x, conpare.z) == OBSTACLELCOST/*��r����^�C������Q��(�{��)�łȂ��H*/ 
							|| bomUnit[PARALYZEBOM]  -> retIsBom(conpare.x, conpare.z) == OBSTACLELCOST/*��r����^�C������Q��(�{��)�łȂ��H*/))
			{
				//��~���Ĕ�������̂�҂�
				return false;
			}
		}
	}
	return true;
}

//=============================================================================
//  AI
//�@�����F�Ȃ�
//	�߂�l�F�Ȃ�
//=============================================================================
void NormalCpu::ai()
{
	//���S�����ꍇ���������Ȃ�
	if(isLose)
		return;

	//cpu�����������������߂Ă���Ƃ�
	if(isControl == AI_STATE::INIT)
	{
		//������
		initAstar();
	}	
	
	//cpu���v�Z���������߂Ă���Ƃ�
	if(isControl == AI_STATE::CALC)
	{	
		//�o�H�����܂�O�ɃI�[�v�����X�g���󂩃`�F�b�N
		if(openList.empty() || isOpenListEmpty || isExitExpMove) {
			if(!isOpenListEmpty) {
				isOpenListEmpty = true;
				//�e���X�g��������
				openList.clear();	
				closeList.clear();
			}
			exitExpRange();
			return;
		}

		//�t���[���̌o�߂��J�E���g
		frameCount = 0;

		//�I�[�v�����X�g����łȂ�
		while(!(openList.empty()))
		{	
			frameCount++;

			//�t���[���̃J�E���g�������𕪊����������ɂȂ�����
			if(frameCount == DIVIDEFRAME)
			{
				//��������v�Z�������甲����
				break;
			}

			//�N���[�Y���X�g�̃T�C�Y����]�ő吔�ȏ�ɂȂ�����
			if(closeList.size() >= MAXLISTSIZE)
			{
				//�e���X�g������
				openList.clear();	
				closeList.clear();
				//AI�̏�Ԃ���������
				isControl = AI_STATE::INIT;
				break;
			}

			//���X�g�̒��̍ł������ȃ^�C����T�����߂̃C�e���[�^
			list<aStar>::iterator	minChackitr;
			openListitr = openList.begin();
			minChackitr = openList.begin();
			minChackitr++;

			//�I�[�v�����X�g��S�ă`�F�b�N
			while(minChackitr != openList.end())
			{
				if(minChackitr->score == openListitr->score/*���ݔ������Ă���^�C���ƍ���r���Ă���^�C���̃X�R�A�����������*/) {
					if(minChackitr->heuristic <= openListitr->heuristic/*�q���[���X�e�B�b�N�̏������ق��D��*/)
						openListitr = minChackitr; 
				} else if(minChackitr->score < openListitr->score/*���ݔ������Ă���^�C����荡��r���Ă���^�C���̃X�R�A�����������*/) {
						openListitr = minChackitr;
				}
				minChackitr++;
			}
		
			//�ړI�n��2�}�X��O�Ȃ�
			if((goalPos.x == openListitr -> tilePos.x && goalPos.z + TWOTILE == openListitr -> tilePos.z)
				|| (goalPos.x + TWOTILE == openListitr -> tilePos.x && goalPos.z == openListitr -> tilePos.z)
					|| (goalPos.x == openListitr -> tilePos.x && goalPos.z - TWOTILE == openListitr -> tilePos.z) 
						|| (goalPos.x - TWOTILE == openListitr -> tilePos.x && goalPos.z == openListitr -> tilePos.z)
							|| (goalPos.x + TWOTILE == openListitr -> tilePos.x && goalPos.z + TWOTILE == openListitr -> tilePos.z)
								|| (goalPos.x - TWOTILE == openListitr -> tilePos.x && goalPos.z - TWOTILE == openListitr -> tilePos.z)
									|| (goalPos.x + TWOTILE == openListitr -> tilePos.x && goalPos.z - TWOTILE == openListitr -> tilePos.z) 
										|| (goalPos.x - TWOTILE == openListitr -> tilePos.x && goalPos.z + TWOTILE == openListitr -> tilePos.z))
			{
				//�o�H�̊���
				{
					pushData.tilePos = openListitr -> tilePos;														
					pushData.cost = openListitr -> cost;
					pushData.heuristic = openListitr -> heuristic;	
					pushData.score = openListitr -> score;
					pushData.parentTile = openListitr -> parentTile;
				}
				closeList.push_back(pushData);	
				closeListitr = closeList.end();
				closeListitr--;
				//AI�̏�Ԃ��ړ���
				isControl = AI_STATE::MOVE;
				//�e�|�C���^�����ǂ��Ĉړ��o�H�쐬
				getPath();
				return;
			} else {
				//���݂̃^�C�����N���[�Y���X�g�Ɉڂ�
				{
					pushData.tilePos = openListitr -> tilePos;														
					pushData.cost = openListitr -> cost;
					pushData.heuristic = openListitr -> heuristic;	
					pushData.score = openListitr -> score;
					pushData.parentTile = openListitr -> parentTile;
				}
				closeList.push_back(pushData);

				//�N���[�Y���X�g�Ɉڂ������̂��I�[�v�����X�g����폜
				openListitr = openList.erase(openListitr);	
			}

			//��r�������^�C���̒��S�ƂȂ�^�C��
			closeListitr = closeList.end();
			closeListitr--;

			static int tileCount;
			tileCount = 0;
			//���݂̃^�C���ɗאڂ���4�����̊e�^�C���𒲂ׂ�
			for(int i = 0; i <= 3; i++)
			{	
				int dx[] = {0, -1, 0, 1};
				int dz[] = {1, 0, -1, 0};
				//�אڂ���ʒu���
				int tileX = (int)(closeListitr -> tilePos.x + dx[i]);
				int tileZ = (int)(closeListitr -> tilePos.z + dz[i]);
				D3DXVECTOR3 conpareTile((float)tileX, 0, (float)tileZ);	
				
				//��r�p�C�e���[�^
				list<aStar>::iterator conpareitr;
				//���łɔ�r�^�C�������X�g�ɓ����Ă��邩�𔻒肷��ϐ�
				bool isAgreementopenList = false;		//�I�[�v�����X�g
				bool isAgreementcloseList = false;		//�N���[�Y���X�g

				//�I�[�v�����X�g����łȂ����
				if(!(openList.empty()))
				{
					conpareitr = openList.end();
					//���X�g�̐擪�łȂ����
					while(conpareitr != openList.begin())
					{
						conpareitr--;
						//��r�������^�C���Ɠ����ʒu�̃^�C�������X�g�ɑ��݂����
						if(conpareitr -> tilePos == conpareTile)
						{
							isAgreementopenList = true;
							break;
						}
					}
				}

				//�N���[�Y���X�g����łȂ����
				if(!(closeList.empty()))
				{
					conpareitr = closeList.end();
					while(conpareitr != closeList.begin())
					{
						conpareitr--;
						//��r�������^�C���Ɠ����ʒu�̃^�C�������X�g�ɑ��݂����
						if(conpareitr -> tilePos == conpareTile) 
						{
							isAgreementcloseList = true;
							break;
						}    
					}
				}
				
				if(!isAgreementopenList/*�I�[�v�����X�g�ɍ��v������̂��܂܂�Ă��Ȃ�*/ )
				{
					if(!isAgreementcloseList /*�N���[�Y���X�g�ɍ��v������̂��܂܂�Ă��Ȃ�*/ )
					{
						if(inExpRange(conpareTile))
						{
							if(isObstacel(conpareTile))
							{

								//�I�[�v�����X�g�Ɉڂ��ăR�X�g���v�Z����
								pushData.tilePos = conpareTile;			
								pushData.cost = closeListitr-> cost + COSTPLUS + wallUnit  -> retCost(conpareTile.x, conpareTile.z);
								pushData.heuristic = abs((int)(goalPos.x - conpareTile.x)) + abs((int)(goalPos.z - conpareTile.z));
								pushData.score = pushData.cost + pushData.heuristic;
								pushData.parentTile = &*closeListitr;
								openList.push_back(pushData);
							}
						}
					}
				}
			}
		}
	}
}

//=============================================================================
//���ݔ����͈͓����𔻒肵�Ă��̃^�C�����R�X�g���Ɋ܂߂邩�Ԃ�
//�����F��r�p�̈ʒu
//�߂�l�F�R�X�g���Ɋ܂߂邩
//=============================================================================
bool NormalCpu::inExpRange(D3DXVECTOR3 conpare)
{
	//�����͈͓��ł����݈ʒu�̔����͈͂̔ԍ��Ɣ�r�ʒu�̔����͈͂̔ԍ����������l��
	if(wallUnit  -> retIsExpRange(toIntPos(pos.x), toIntPos(pos.z))
		&& wallUnit  -> retExpRangeNum(toIntPos(pos.x), toIntPos(pos.z)) == wallUnit  -> retExpRangeNum(conpare.x, conpare.z))
	{
		return true;
	} else if(!wallUnit  -> retIsExpRange(conpare.x, conpare.z)/*��r�ʒu�������͈͓��łȂ��H*/) {
		return true;
	}
	return false;
}

//=============================================================================
//��r�^�C������Q����
//�����F��r�p�̈ʒu
//�߂�l�F��Q������Ԃ�
//=============================================================================
bool NormalCpu::isObstacel(D3DXVECTOR3 conpare) 
{
	conpare = D3DXVECTOR3(conpare.x, 0.0f, conpare.z);

	if(bomUnit[NORMALBOM] -> retIsBom(conpare.x, conpare.z) != OBSTACLELCOST/*��r����^�C������Q��(�{��)�łȂ��H*/) 
		if(bomUnit[PLUSBOM] -> retIsBom(conpare.x, conpare.z) != OBSTACLELCOST/*��r����^�C������Q��(�{��)�łȂ��H*/) 
			if(bomUnit[PARALYZEBOM] -> retIsBom(conpare.x, conpare.z) != OBSTACLELCOST/*��r����^�C������Q��(�{��)�łȂ��H*/) 
				if(wallUnit -> retWallArr(conpare.x, conpare.z) == NOTWALL/*��r����^�C������Q��(��)�łȂ��H*/) 
					if(conpare != D3DXVECTOR3(toIntPos(vsCpuPlayerUnit->pos.x), 0, toIntPos(vsCpuPlayerUnit ->pos.z/*��r����^�C�����v���C���[�̈ʒu�Ɠ������H*/))) 
						if(conpare != D3DXVECTOR3(toIntPos(cpuUnit[SPEEDCPU]->pos.x), 0, toIntPos(cpuUnit[SPEEDCPU]->pos.z/*��r����^�C����SpeedCpu�̈ʒu�Ɠ������H*/))) 
							if(conpare != D3DXVECTOR3(toIntPos(cpuUnit[PYECPU]->pos.x), 0, toIntPos(cpuUnit[PYECPU]->pos.z/*��r����^�C����PsyCpu�̈ʒu�Ɠ������H*/))) 
								if(conpare != D3DXVECTOR3(toIntPos(cpuUnit[NORMALCPU]->pos.x), 0, toIntPos(cpuUnit[NORMALCPU]->pos.z/*��r����^�C����NormalCpu�̈ʒu�Ɠ������H*/))) 
	{
		return true;
	}

	return false;
	
}

//=============================================================================
//  �S�[���ւ̃p�X���擾����
//�@�����F�Ȃ�
//	�߂�l�F�Ȃ�
//=============================================================================
void NormalCpu::getPath() 
{
	list<aStar>::iterator closeListitr;
	//�S�[������t�Z���Čo�H�����ǂ邩��end()
	closeListitr = closeList.end();
	closeListitr--;

	//�e���X�g�Ɋi�[�����������ϐ�
	aStar* push;
	//�e�|�C���^�̃A�h���X�i�[
	push = &*closeListitr;

	//�e�|�C���^�����ǂ肫��i�v�Z���ɍŏ��A�^�C���̈ʒu����NULL�������Ă���j
	while(push->parentTile != NULL)
	{	
		//�e���X�g�ɐe�|�C���^�̏��i�[
		parentList.push_back(push->tilePos);
		//�e�|�C���^�̃A�h���X�i�[
		push = *&push->parentTile;
	}	
}

//=============================================================================
//  �ړ�
//�@�����F�Ȃ�
//	�߂�l�F�Ȃ�
//=============================================================================
void NormalCpu::control()
{
	float addAngle = 0;
	//cpu���ړ����������߂Ă���Ƃ�
	if(isControl == AI_STATE::MOVE)
	{
		//�e�^�C�����i�[���郊�X�g����łȂ����
		if(!parentList.empty())
		{	
			//�L�����N�^�[���Ƃ̈ړ����x��ݒ�
			if(charaNum == CHARACTER::SPEED)
				moveSpeed = 0.20f;	//SpeedCpu�̂Ƃ�
			else 
				moveSpeed = 0.10f;	//���̑��̂Ƃ�

			list<D3DXVECTOR3>::iterator parentListitr;
			parentListitr = parentList.end();
			parentListitr--;

			//���̃L�����N�^�[���j�b�g�ƏՓ˂�����
			if(collObstacel(*parentListitr))
				return;
		
			if(parentListitr -> x == pos.x && parentListitr -> z > pos.z/*���̈ʒu�̑O��*/) {
				moveDir = MOVE_DIR::ADVANCE;
				pos.z = truncationSecondDecimalPlace(pos.z + (moveSpeed + 0.01f));
			} else if(parentListitr -> x < pos.x && parentListitr -> z == pos.z/*���̈ʒu�̍���*/) {
				moveDir = MOVE_DIR::LEFT;
				pos.x = truncationSecondDecimalPlace(pos.x - (moveSpeed - 0.01f));
				addAngle = 270;
			} else if(parentListitr -> x > pos.x && parentListitr -> z == pos.z/*���̈ʒu�̉E��*/) {
				moveDir = MOVE_DIR::RIGHT;
				pos.x = truncationSecondDecimalPlace(pos.x + (moveSpeed + 0.01f));
				addAngle = 90.0f;
			} else if(parentListitr -> x == pos.x && parentListitr -> z < pos.z/*���̈ʒu�̌��*/) {
				moveDir = MOVE_DIR::BACKWARD;
				pos.z = truncationSecondDecimalPlace(pos.z - (moveSpeed - 0.01f));
				addAngle = 180.0f;
			}

			if(( pos.x == parentListitr -> x ) && ( pos.z == parentListitr -> z ))
			{
				parentListitr = parentList.erase(parentListitr);
				if(prevMoveDir != moveDir)
					angle += addAngle;
				prevMoveDir = moveDir;
			}
		} else {	
			//�e���X�g��������
			openList.clear();	
			closeList.clear();
			//AI�̏�Ԃ���������
			isControl = AI_STATE::INIT;
			//�{����u��
			putBom();
		} 
	}
}

//=============================================================================
//  �����_���ȉ��؂�̂�
//�@�����F�؂�̂Ă�l
//	�߂�l�F�؂�̂Ă��l
//=============================================================================
float NormalCpu::truncationSecondDecimalPlace(float pos) 
{
	return floor(pos * 10) / 10;
}

//=============================================================================
//  �����͈͂��痣��邽�߂̒T��
//�@�����F�Ȃ�
//	�߂�l�F�Ȃ�
//=============================================================================
void NormalCpu::exitExpRange() 
{
	static D3DXVECTOR3 moveDir = D3DXVECTOR3(0, 0, 0);
	static int dirIndex = 0;
	float dx[] = {0.0f, -1.0f, 0.0f, 1.0f};
	float dz[] = {1.0f, 0.0f, -1.0f, 0.0f};
		
	if(!isExitExpMove)
	{		
		//�����͈͊O��
		if(!wallUnit  -> retIsExpRange(toIntPos(pos.x), toIntPos(pos.z))) {
			isOpenListEmpty = false;
			prevPosList.clear();
			//AI�̏�Ԃ���������
			isControl = AI_STATE::INIT;
			return;
		};

		isExitExpMove = true;
		
		if(prevTileState != PREV_TILE_STATE::MOVE_DIR)
		{	
			moveDir = D3DXVECTOR3(0, 0, 0);
			dirIndex = 0;

			for(int i = 0; i < 4; i++) {
				D3DXVECTOR3 conpareTile(toIntPos(pos.x) + dx[i], pos.y, toIntPos(pos.z) + dz[i]);
				//�ڂ̑O���t�B�[���h�O�Ȃ�
				if(conpareTile.x < MINRANGE || conpareTile.x > MAXRANGE
					|| conpareTile.z < MINRANGE || conpareTile.z > MAXRANGE || isObstacel(conpareTile))
				{
					continue;
				}

				//��r�ʒu�ɏ�Q�����Ȃ��A���i�ޏ������������ꏊ�����邩
				if(equalPrevPosState(conpareTile)) {
					dirIndex = i;
					moveDir = conpareTile;
					exitExpProcess(moveDir, dirIndex);
					break;
				}
			}	
		}

		if(prevTileState == PREV_TILE_STATE::MOVE_DIR) {
			prevTileState = PREV_TILE_STATE::NOT_PRAV_TILE;
			dirIndex = prevDirIndex;
			moveDir = D3DXVECTOR3(toIntPos(pos.x) + dx[dirIndex], pos.y, toIntPos(pos.z) + dz[dirIndex]);
			exitExpProcess(moveDir, dirIndex);
		}
	}

	if(isExitExpMove)
		exitExpProcess(moveDir, dirIndex);	
}

//=============================================================================
//  �����͈͂��痣��邽�߂̏���
//�@�����F����
//	�߂�l�F�Ȃ�
//=============================================================================
void NormalCpu::exitExpProcess(D3DXVECTOR3 dir, int dirIndex) 
{
	float addAngle = 0.0f;
	float dx[] = {0.0f, -1.0f, 0.0f, 1.0f};
	float dz[] = {1.0f, 0.0f, -1.0f, 0.0f};

	//�L�����N�^�[���Ƃ̈ړ����x��ݒ�
	if(charaNum == CHARACTER::SPEED)
		moveSpeed = 0.20f;	//SpeedCpu�̂Ƃ�
	else 
		moveSpeed = 0.10f;	//���̑��̂Ƃ�

	if(collObstacel(dir))
		return;

	if(dir.x == pos.x && dir.z > pos.z/*���̈ʒu�̑O��*/) {
		moveDir = MOVE_DIR::ADVANCE;
		pos.z = truncationSecondDecimalPlace(pos.z + (moveSpeed + 0.01f));
	} else if(dir.x < pos.x && dir.z == pos.z/*���̈ʒu�̍���*/) {
		moveDir = MOVE_DIR::LEFT;
		pos.x = truncationSecondDecimalPlace(pos.x - (moveSpeed - 0.01f));
		addAngle = 270;
	} else if(dir.x > pos.x && dir.z == pos.z/*���̈ʒu�̉E��*/) {
		moveDir = MOVE_DIR::RIGHT;
		pos.x = truncationSecondDecimalPlace(pos.x + (moveSpeed + 0.01f));
		addAngle = 90.0f;
	} else if(dir.x == pos.x && dir.z < pos.z/*���̈ʒu�̌��*/) {
		moveDir = MOVE_DIR::BACKWARD;
		pos.z = truncationSecondDecimalPlace(pos.z - (moveSpeed - 0.01f));
		addAngle = 180.0f;
	}

	if(( pos.x == dir.x ) && ( pos.z == dir.z ))
	{
		prevTileInfo push;
		{
			push.prevTilePos = D3DXVECTOR3(toIntPos(pos.x) - dx[dirIndex], pos.y, toIntPos(pos.z) - dz[dirIndex]);
			if(prevTileState == PREV_TILE_STATE::NOT_PRAV_TILE) {
				push.prevTileDir = dirIndex;
			} else if(prevTileState == PREV_TILE_STATE::TO_PRAV_TILE_MOVE) {
				push.prevTileDir = dirIndex;
				prevTileState = PREV_TILE_STATE::MOVE_DIR;
			}  
		}
		prevPosList.push_back(push);
		prevTile = D3DXVECTOR3(toIntPos(pos.x) - dx[dirIndex], pos.y, toIntPos(pos.z) - dz[dirIndex]);
		if(moveDir != prevMoveDir)
			angle += addAngle;
		isExitExpMove = false;
	}
}

//=============================================================================
//  ��r�ʒu���O��ʂ������Ɠ������ꍇ�̏���
//�@�����F��r�ʒu
//	�߂�l�F��r�^�C���ɐi�ނ�
//=============================================================================
bool NormalCpu::equalPrevPosState(D3DXVECTOR3 conpare) 
{	
	float dx[] = {0.0f, -1.0f, 0.0f, 1.0f};
	float dz[] = {1.0f, 0.0f, -1.0f, 0.0f};
	vector<prevTileInfo>::iterator prevTileInfoitr;	//�C�e���[�^�쐬
	prevTileInfoitr = prevPosList.begin();
	list<int> prevDirList;				//�O��ʂ����^�C������O��I�������������i�[����
	list<int>::iterator prevDirListitr;	//�C�e���[�^�쐬
	prevDirListitr = prevDirList.begin();
	int elementCount = 0;
	bool isEqual = false;

	if(prevPosList.empty())
		return true;

	while(prevTileInfoitr != prevPosList.end())
	{
		//�O��ʂ����^�C���Ȃ�
		if(prevTileInfoitr->prevTilePos == conpare) {
			//���̃^�C������I�����������i�[
			prevDirList.push_back(prevTileInfoitr->prevTileDir);

		}
		prevTileInfoitr++;
	}	

	if(prevDirList.empty())
		return true;
	
	//���̃^�C����4�����ɑO��̌����ƈႤ�����̋󂫂����邩
	for(int i = 0; i < 4; i++) {
		//�O��I���������Ƃ̂�������Ȃ�
		while(prevDirListitr != prevDirList.end())
		{
			if(*prevDirListitr == i)
			{
				isEqual = true;
				break;
			}
			prevDirListitr++;
		}

		if(!isEqual) {
			D3DXVECTOR3 conpareTile(toIntPos(conpare.x) + dx[i], pos.y, toIntPos(conpare.z) + dz[i]);
			//�󂫂�����
			if(isObstacel(conpareTile)) {
				//���̃^�C���ֈړ�
				prevTileState = PREV_TILE_STATE::TO_PRAV_TILE_MOVE;
				//���̃^�C���Ɉړ�������i�ޕ������i�[
				prevDirIndex = i;
				break;
			}
		}
		isEqual = false;
	}

	prevDirList.clear();

	if(prevTileState != PREV_TILE_STATE::TO_PRAV_TILE_MOVE) {
		prevTileState = PREV_TILE_STATE::NOT_PRAV_TILE;
		return false;
	}
	
	return true;
}

//=============================================================================
//��Q���ɑ΂��鏈��������
//�����F��r�ʒu�A�����x�N�g��
//�߂�l�F�Փ˂�����
//=============================================================================
bool NormalCpu::collObstacel(D3DXVECTOR3 conpare)
{
	D3DXVECTOR3 nowPos(conpare.x, -0.5f, conpare.z);

	//�����ʒu�ŏ��������Ȃ�
	if(prevPos != nowPos)
	{
		//���i�ރ^�C���ɔ��e���u���ꂽ
		if(D3DXVECTOR3(nowPos.x, 0, nowPos.z) != D3DXVECTOR3(putBomPos.x, 0, putBomPos.z)/*�ڂ̑O�̃{���������Œu�������̂łȂ�*/) {
			if(bomUnit[NORMALBOM] -> retIsBom(nowPos.x, nowPos.z) == OBSTACLELCOST/*��r����^�C������Q��*/
				|| bomUnit[PLUSBOM] -> retIsBom(nowPos.x, nowPos.z) == OBSTACLELCOST/*��r����^�C������Q��*/
					|| bomUnit[PARALYZEBOM] -> retIsBom(nowPos.x, nowPos.z) == OBSTACLELCOST/*��r����^�C������Q��*/)
			{
			
				//�{����u�����ʒu������(�g��Ȃ��l���)
				putBomPos = D3DXVECTOR3(999, 999, 999);
				//�ʒu�𐮐��ɂ���
				adjustmentPos();
				//AI�����𒆎~
				stopAi();
				prevPos = nowPos;
				return true;
			}
		}

		//���i�ރ^�C���ɑ��̃L�����N�^�[���i��
		if(nowPos == vsCpuPlayerUnit ->pos
			|| nowPos == cpuUnit[SPEEDCPU]->pos
				|| nowPos == cpuUnit[PYECPU]->pos
					|| nowPos== cpuUnit[NORMALCPU]->pos)
		{
			//�ʒu�𐮐��ɂ���
			adjustmentPos();
			//�{���z�u
			putBom();
			//AI�����𒆎~
			stopAi();
			prevPos = nowPos;
			return true;
		}
	}

	//���i�ރ^�C���ɕǂ��u���ꂽ
	if(wallUnit -> retWallArr(nowPos.x, nowPos.z) != NOTWALL)
	{
		//�ʒu�𐮐��ɂ���
		adjustmentPos();
		//AI�����𒆎~
		stopAi();
		prevPos = nowPos;
		return true;
	}

	return false;
}

//=============================================================================
//�{����z�u
//�����F�Ȃ�
//�߂�l�F�Ȃ�
//=============================================================================
void NormalCpu::putBom()
{	
	D3DXVECTOR3 bomPutDir = pos + calcMove(angle);

	//���ݏ������Ă���{�����Ȃ����
	if(processBomType == NONE) {
		if(!bomThrowNumList.empty()/*�X���[�{���������*/) {
			//�O�㍶�E�̉�������Ƀ^�[�Q�b�g�����邩
			if(isThrowTarget()) {
				throwFlag = true;
				bomThrowNumList.pop_back();
			}
		}
			
		if((isObstacel(bomPutDir) && isPutBom(bomPutDir)
			&& !(bomPutDir.x < MINRANGE) && !(bomPutDir.x > MAXRANGE)
				&& !(bomPutDir.z < MINRANGE) && !(bomPutDir.z > MAXRANGE)) || throwFlag){
			if(!paralyzeBomNumList.empty()/*Ⴢ�{���������*/ && charaSearch) { 
				if(!throwFlag)
					bomPutAngle = angle;
				//�ǂ̃{����I�񂾂�����
				itemType = ITEM::PARALYZEBOMITEM;
				//�A�C�e���{�����g�p
				isItem = true;
				return;
			} else if(bomUnit[NORMALBOM] -> putCharaNum[charaNum].bomRenderState == DONAOTHING/*�ʏ�{�����������łȂ����*/) {
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

//=============================================================================
//���݂̈ʒu����{����u�����ꍇ�A�����������邩����
//�����F�{����u���\��ʒu
//�߂�l�F�{����u����
//=============================================================================
bool NormalCpu::isPutBom(D3DXVECTOR3 bomPutPos)
{
	D3DXVECTOR3 direction[4] = {D3DXVECTOR3(0.0f,0.0f,1.0),D3DXVECTOR3(1.0f,0.0f,0.0),D3DXVECTOR3(0.0f,0.0f,-1.0),D3DXVECTOR3(-1.0f,0.0f,0.0)};	//4�������i�[���郊�X�g
	list<D3DXVECTOR3> conparePosList;
	list<D3DXVECTOR3> checkPosList;
	list<D3DXVECTOR3>::iterator conparePosListitr;
	list<D3DXVECTOR3>::iterator checkPosListitr;
	conparePosList.push_back(pos);
	conparePosListitr = conparePosList.begin();
	checkPosListitr = checkPosList.begin();
	int count = 0;
	int expRange = addExpRange + 2;

	while(!conparePosList.empty())
	{
		for(int i = 0; i < 4; i++)
		{
				bool isChecked = false;
				checkPosListitr = checkPosList.begin();
				D3DXVECTOR3 conparePos = *conparePosListitr+direction[i];
				while(checkPosList.end() != checkPosListitr)
				{
					if(*checkPosListitr == conparePos)
					{
						isChecked = true;
						break;
					}
					checkPosListitr++;
				}

				//�ȑO���ׂ��^�C���ȊO��3�����^�C���̒��ŁA�󂫂����邩
				if(isObstacel(conparePos) && bomPutPos != conparePos && !isChecked 
						&& !(conparePos.x < MINRANGE) && !(conparePos.x > MAXRANGE) && !(conparePos.z < MINRANGE) && !(conparePos.z > MAXRANGE))
				{
					conparePosList.push_back(conparePos);
					checkPosList.push_back(*conparePosListitr);
					//�_�ԋ������A���z�I�Ȕ����͈͊O�Ȃ�true��Ԃ�
					if(sqrt((pos.x - conparePos.x)*2 + (pos.z - conparePos.z)*2) > expRange)
						return true;

					//�_�ԋ������A15�^�C����藣��Ă�����false��Ԃ�(���e���u����Ă��甚������܂ňړ��ł���ő勗��)
					if(sqrt((pos.x - conparePos.x)*2 + (pos.z - conparePos.z)*2) > 15)
						return false;
				}
		}
		conparePosListitr = conparePosList.erase(conparePosListitr);
	}	
	return false;
}

//=============================================================================
//��������T��
//�����F�Ȃ�
//�߂�l�F������悪���邩�ǂ���
//=============================================================================
bool NormalCpu::isThrowTarget()
{
	D3DXVECTOR3 conparePos(pos.x, 0, pos.z);
	D3DXVECTOR3 conparePosList[4] = {conparePos, conparePos, conparePos, conparePos};	//���݈ʒu����̊e�����̉��Z
	int directionX[4] = {0,1,0,-1};									//4������x�������i�[���郊�X�g
	int directionZ[4] = {1,0,-1,0};									//4������z�������i�[���郊�X�g
	D3DXVECTOR3 begin(0, 0, 0);
	D3DXVECTOR3 sumDirection[4] = {begin, begin, begin, begin};		//�e�����̓����鋗���̉��Z
	loopNum = 0;

	//���[�v�񐔂�ݒ�(���݈ʒu����t�B�[���h�O�܂ł̂����Ƃ��Z������)
	(MAXRANGE - fabs(pos.x) < MAXRANGE - fabs(pos.z)) ? loopNum = (int)(MAXRANGE - fabs(pos.x)) : loopNum = (int)(MAXRANGE - fabs(pos.z));

	for(int count = 1; count <= loopNum; count++) {
		//�㉺���E�e���������[�v
		for(int i = 0; i < 4; i++) {
			//�e������1�^�C�������Z
			sumDirection[i] += D3DXVECTOR3((float)directionX[i], 0, (float)directionZ[i]);
			conparePosList[i] += D3DXVECTOR3((float)directionX[i], 0, (float)directionZ[i]);

			{
				//��r�ʒu���L�����N�^�[�Ɠ��������
				if(conparePosList[i] == D3DXVECTOR3(toIntPos(vsCpuPlayerUnit ->pos.x), 0, toIntPos(vsCpuPlayerUnit ->pos.z/*��r����^�C�����v���C���[�̈ʒu�Ɠ������H*/))) {
					//��Q���̂Ȃ��ꏊ��T��
					if(searchPutPos(i, sumDirection, conparePosList)) {
						//�ǂ̕����ɓ����邩���߂�
						bomPutAngle = angle + (i * 90.0f);
						return true;
					}
					return false;
				}
				if(conparePosList[i] == D3DXVECTOR3(toIntPos(cpuUnit[SPEEDCPU]->pos.x), 0, toIntPos(cpuUnit[SPEEDCPU]->pos.z/*��r����^�C����SpeedCpu�̈ʒu�Ɠ������H*/))) {
					if(searchPutPos(i, sumDirection, conparePosList)) {
						bomPutAngle = angle + (i * 90.0f);
						return true;
					}
					return false;
				}
				if(conparePosList[i] == D3DXVECTOR3(toIntPos(cpuUnit[PYECPU]->pos.x), 0, toIntPos(cpuUnit[PYECPU]->pos.z/*��r����^�C����PsyCpu�̈ʒu�Ɠ������H*/))) {
					if(searchPutPos(i, sumDirection, conparePosList)) {
						bomPutAngle = angle + (i * 90.0f);
						return true;
					}
					return false;
				}
				if(conparePosList[i] == D3DXVECTOR3(toIntPos(cpuUnit[NORMALCPU]->pos.x), 0, toIntPos(cpuUnit[NORMALCPU]->pos.z/*��r����^�C����NormalCpu�̈ʒu�Ɠ������H*/))) {
					if(searchPutPos(i, sumDirection, conparePosList)) {
						bomPutAngle = angle + (i * 90.0f);
						return true;
					}
					return false;
				}
			}
		}
	}

	return false;
}

//=============================================================================
//�{����u����ʒu���t�Z���ĒT��
//�����F�e�����̓����鋗���̉��Z�A���݈ʒu����̊e�����̉��Z
//�߂�l�F�Ȃ�
//=============================================================================
bool NormalCpu::searchPutPos(int dir, D3DXVECTOR3 sumDirection[], D3DXVECTOR3 conparePosList[])
{
	int count = 0;
	int directionX[4] = {0,1,0,-1};									//4������x�������i�[���郊�X�g
	int directionZ[4] = {1,0,-1,0};									//4������z�������i�[���郊�X�g
	//���Ȃ��Ƃ��^�[�Q�b�g�̈�O�ɔ�΂�����
	D3DXVECTOR3 direction((float)directionX[dir], 0, (float)directionZ[dir]);
	sumDirection[dir] -= direction;
	conparePosList[dir] -= direction;
	while(count <= loopNum) {
		//������Q���̂Ȃ��Ƃ���܂ł����̂ڂ�
		sumDirection[dir] -= direction;
		conparePosList[dir] -= direction;

		//���݂̔�r�ꏊ�ɏ�Q�����Ȃ����
		if(isObstacel(conparePosList[dir]))
		{
			//�������ݒ�
			bomThrowPos = sumDirection[dir];
			return true;
		}
	}

	return false;
}

//=============================================================================
//�A�C�e���擾
//�����F�Ȃ�
//�߂�l�F�Ȃ�
//=============================================================================
void NormalCpu::getItem()
{
	//�A�C�e���ƌ��݂̈ʒu�����������
	if(pos.x == wallUnit -> retItem(pos.x,pos.z).x && pos.z == wallUnit -> retItem(pos.x,pos.z).z )
	{
		//�A�C�e���e���X�g�ɕۑ�
		switch (wallUnit -> retItemNumber(pos.x, pos.z))
		{
			case ITEM::PLUSBOMITEM:
				plusBomNumList.push_back(PLUSBOMITEM);
				break;

			case ITEM::LARGEEXPLOSIONITEM:
				plusExpRange();
				break;

			case ITEM::PARALYZEBOMITEM:
				paralyzeBomNumList.push_back(PARALYZEBOMITEM);
				break;

			case ITEM::THROWBOMITEM:
				bomThrowNumList.push_back(THROWBOMITEM);
				break;
		}
	}
}

//=============================================================================
//�S�[���ʒu��Ԃ�
//�����F���݈ʒu
//�߂�l�F�S�[���ʒu
//=============================================================================
D3DXVECTOR3 NormalCpu::retTarget(D3DXVECTOR3 pos)
{
	int range = 0;
	int index = 0;
	target = D3DXVECTOR3(pos.x,0,pos.z);
	prevWallCount = 0;
	toTargetCost = 999;
	isTarget = false;
	charaSearch = false;
	int posX = (int)pos.x, posZ = (int)pos.z;

	while (1) {
			int searchX = searchTargetRangeXTable[index];
			int searchZ = searchTargetRangeZTable[index];
			if(0 <= searchX)
				searchX += range;
			else 
				searchX -= range;

			if(0 <= searchZ)
				searchZ += range;
			else 
				searchZ -= range;

			int x = posX + searchX;
			int z = posZ + searchZ;

			//�t�B�[���h�O�͒������Ȃ�
			if(x < MINRANGE || x > MAXRANGE
				|| z < MINRANGE || z > MAXRANGE)  
			{
				index++;
				//���͈͓��Ƀ^�[�Q�b�g���������Ȃ�����
				if(index >= 111 && !isTarget)
				{
					index = 0;
					//�͈͊g��
					range += 5;
				}
				
				if(range >= MAXRANGE) 
					return retTargetOfCharacterPos();

				continue;
			}

			//�^�[�Q�b�g�T��
			searchTarget((float)x, (float)z, index);

		//���͈͓��Ƀ^�[�Q�b�g��������
		if(isTarget) {
			prevTargetPos = pos;
			return target;	
		}

		index++;
	}
}

//=============================================================================
//�S�[���ʒu�Ƃ��ăL�����N�^�[�̈ʒu��Ԃ�
//�����F��r�ʒu�A�����R�X�g
//�߂�l�F�Ȃ�
//=============================================================================
D3DXVECTOR3 NormalCpu::retTargetOfCharacterPos()
{	
	if(!vsCpuPlayerUnit->isLose)
		return D3DXVECTOR3(vsCpuPlayerUnit->pos.x, 0, vsCpuPlayerUnit->pos.z);
	
	if(!cpuUnit[SPEEDCPU]->isLose)
		return D3DXVECTOR3(cpuUnit[SPEEDCPU]->pos.x, 0, cpuUnit[SPEEDCPU]->pos.z);

	if(!cpuUnit[PYECPU]->isLose)
		return D3DXVECTOR3(cpuUnit[PYECPU]->pos.x, 0, cpuUnit[PYECPU]->pos.z);

	if(!cpuUnit[NORMALCPU]->isLose)
		return D3DXVECTOR3(cpuUnit[NORMALCPU]->pos.x, 0, cpuUnit[NORMALCPU]->pos.z);

	return D3DXVECTOR3(vsCpuPlayerUnit->pos.x, 0, vsCpuPlayerUnit->pos.z);
}

//=============================================================================
//�S�[���ʒu��T��
//�����F��r�ʒu�A�����R�X�g
//�߂�l�F�Ȃ�
//=============================================================================
void NormalCpu::searchTarget(float posX, float posZ, int cost)
{
	D3DXVECTOR3 serchPos(posX,0,posZ);
	//�����͈͓��ƈȑO�̃^�[�Q�b�g���ӂ͏��O
	if(wallUnit->retIsExpRange(posX, posZ)
		|| (prevTargetPos.x+PREVTARGETRANGE >= serchPos.x && prevTargetPos.x-PREVTARGETRANGE <= serchPos.x 
			&& prevTargetPos.z+PREVTARGETRANGE >= serchPos.z && prevTargetPos.z-PREVTARGETRANGE <= serchPos.z)) {
		return;
	}
		
	//�ǂ���������
	if(wallUnit -> retWallArr(posX, posZ) == ISWALL) {	
		int nowCost = (int)(targetDistCostTable[cost] * wallTargetMul + 0.5f);
		if(nowCost <= toTargetCost) {
			toTargetCost = nowCost;
			isTarget = true;
			searchWallTarget(posX, posZ);
		}
	}

	//���ݏ������̃{�������݂��Ȃ��܂��̓v���X�{���A�C�e���������Ă���
	if(processBomType == NONE || !plusBomNumList.empty()) {
		//�e�L�����N�^�[����������
		if(posX == toIntPos(vsCpuPlayerUnit ->pos.x) && posZ == toIntPos(vsCpuPlayerUnit ->pos.z)) {
			//�L�����N�^�[���^�[�Q�b�g�ɂ�����(�p�����C�Y�{����u���̂̓L�����N�^�[�ɑ΂��Ă݂̂�����m�F�p)
			charaSearch = true;
			int nowCost = (int)(targetDistCostTable[cost] * charaTargetMul + 0.5f);
			if(nowCost <= toTargetCost) {
				toTargetCost = nowCost;
				isTarget = true;
				target = D3DXVECTOR3(posX,0,posZ);
			}
		} 

		NormalCpu* charaList[] = {cpuUnit[SPEEDCPU],
							cpuUnit[PYECPU],
							cpuUnit[NORMALCPU]};

		for(int i = 0; i < CHARANUM; i++) {
			if(posX == toIntPos(charaList[i] -> pos.x) && posZ == toIntPos(charaList[i] -> pos.z)) {
				charaSearch = true;
				int nowCost = (int)(targetDistCostTable[cost] * charaTargetMul + 0.5f);
				if(nowCost <= toTargetCost) {
					toTargetCost = nowCost;
					isTarget = true;
					target = D3DXVECTOR3(posX,0,posZ);
				}
			}
		}
	}
}

//=============================================================================
//���������̕ǂ̒��Ŏ��͂W�}�X�ɕǂ��������̂��^�[�Q�b�g�ɂ���
//�����F�ǂ̈ʒu
//�߂�l�F�Ȃ�
//=============================================================================
void NormalCpu::searchWallTarget(float posX, float posZ)
{
	float dx[] = {0.0f, 1.0f, 1.0f, 1.0f, 0.0f, -1.0f, -1.0f, 0.0f};
	float dz[] = {1.0f, 1.0f, 0.0f, -1.0f, -1.0f, -1.0f, 0.0f, -1.0f};
	int count = 0;
	for(int i = 0; i < 8; i++) {
		if(wallUnit -> retWallArr(posX+dx[i], posZ+dz[i]) == ISWALL)	
			count++;
	}

	if(prevWallCount <= count) {
		target = D3DXVECTOR3(posX,0,posZ);
		prevWallCount = count;
	}
}

//=============================================================================
//AI�̏������f
//�����F�Ȃ�
//�߂�l�F�Ȃ�
//=============================================================================
void NormalCpu::stopAi()
{
	//�X�g�b�v�����łȂ����(=�ڂ̑O�ɏ�Q���������)
	if(!isStop)
	{
		//���X�g��������
		parentList.clear();
		openList.clear();	
		closeList.clear();

		isControl = AI_STATE::INIT;
		prevTileState = PREV_TILE_STATE::NOT_PRAV_TILE;
		isExitExpMove = false;
		isOpenListEmpty = false;
	} else /*�X�g�b�v�����Ȃ�(=Ⴢ�{���Ɉ�����������)*/{
		//��~���鎞��
		stopCount++;
		//AI�̏�Ԃ�ҋ@������
		isControl = AI_STATE::STAY;
		//STOPTIME�̊ԑҋ@
		if(stopCount > STOPTIME)
		{
			//���X�g��������
			parentList.clear();
			openList.clear();	
			closeList.clear();

			isControl = AI_STATE::INIT;
			prevTileState = PREV_TILE_STATE::NOT_PRAV_TILE;
			isExitExpMove = false;
			isOpenListEmpty = false;
			//��~���ԏ�����
			stopCount = 0;
			isStop = false;
		}
	} 
}

//=============================================================================
//�ʒu�𐮐��ɐ�����
//�����F�Ȃ�
//�߂�l�F�Ȃ�
//=============================================================================
void NormalCpu::adjustmentPos()
{
	pos.x = toIntPos(pos.x);	
	pos.z = toIntPos(pos.z);
}

//=============================================================================
//�i�s�����x�N�g�������߂�
//�����F�p�x
//�߂�l�F�Ȃ�
//=============================================================================
D3DXVECTOR3 NormalCpu::calcMove(float angle)
{
	//��]�͈͂𐧌��imove�ɉe�����o��j
	if(angle > 360)
		angle = fmod(angle, 360);

	//�ړ��x�N�g���i�{���������ۂ̈����ɕK�v�j
	D3DXVECTOR3 move(0, 0, 1.0f);
	//��]�s��
	D3DXMATRIX mRotat;
	D3DXMatrixRotationY(&mRotat, D3DXToRadian(angle));
	D3DXVec3TransformCoord(&move,&move,&mRotat);
	//�i�s�����x�N�g���̒����𐮐��ɕϊ�
	return D3DXVECTOR3(toIntMove(move.x),0,toIntMove(move.z));
}

