//=============================================================================
//  ��
//�@�X�V���F2016/10/30
//	����ҁF���X�ؗ��M
//=============================================================================
#include "Wall.h"
#include "Camera.h"
#include "VsPlayer.h"
#include "vsCpuPlayer.h"
#include "NormalCpu.h"
#include "PsyCpu.h"
#include "SpeedCpu.h"
#include "modeChange.h"
#include "readFile.h"
#include <mmsystem.h>

#define NOTWALL 0			//�ǂȂ�
#define PUTWALL 1			//�ǗL��
#define NOTBREAKWALL 2		//�j�󂳂�Ȃ���
#define OUTRANGE 3			//�t�B�[���h�͈͊O
#define OBSTACLELCOST 100	//�Ǔ��̏�Q���̃R�X�g
#define NONCOSTE 0			//�R�X�g�Ȃ�
#define ITEMCOST -3			//�A�C�e���̃R�X�g
#define CAMERA1 1			//�J�����ԍ�
#define CAMERA2 2			
#define CHARANUM 3			//�L�����N�^�[��
#define MINRANGE 0			//�ő�͈�
#define MAXRANGE 50			//�ŏ��͈�
#define VIEWRANGE 6		    //3D���f�����`�悳���͈�
#define REARRENGETIME 3 * 60/*�b���番*/	//�ǂ��Ĕz�u���鎞��
#define FILENUM 4			//CSV�t�@�C���̐�
#define ITEMRAN 30			//�A�C�e���ԍ��擾�̂��߂̗�����
#define ITEMNUM 4			//�A�C�e���̎�ސ�
#define TIMEUPWALLNUM 4800		//�������Ԃ��߂����Ƃ��ɕ\�������

enum ITEM
{	
	DEFAULTITEM,		//�ʏ�{��
	PLUSBOMITEM,		//�v���X�{��
	PARALYZEBOMITEM,	//Ⴢ�{��
	THROWBOMITEM,		//�{���𓊂���
	LARGEEXPLOSIONITEM,	//�唚���{��
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
Wall::Wall(void)
{
	mapTableData = new readFile;
	nonBreakBlockMapData = new readFile;

	//�e�N�X�`���ǂݍ���
    //plusBom
    if (!texPlusBom.initialize(graphics,"pictures\\plusBom.png"))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing plusBom"));
		plusBomPoly.initialize(graphics, &texPlusBom);

	//largeexplosionBom
    if (!texLargeexplosionBom.initialize(graphics,"pictures\\largeexplosion Bom.png"))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing largeexplosionBom"));
		largeexplosionPoly.initialize(graphics, &texLargeexplosionBom);

	//paralyzeBom
	if (!texParalyzeBom.initialize(graphics,"pictures\\paralyze.png"))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing paralyzeBom texture"));
		paralyzeBomPoly.initialize(graphics,&texParalyzeBom);

	//bomThrow
	if (!texThrow.initialize(graphics,"pictures\\bomThrow.png"))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing bomThrow texture"));
		bomThrowPoly.initialize(graphics,&texThrow);

	//wall
	if (!texWall.initialize(graphics,"pictures\\renga.jpg"))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing wall texture"));
		wallPoly.boxInitialize(graphics,&texWall);

	//nonBreakWall
	if (!texNonBreakWall.initialize(graphics,"pictures\\nonBreakWall.png"))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing nonBreakWall texture"));
		nonBreakWallPoly.boxInitialize(graphics,&texNonBreakWall);

	if(!nonBreakBlockMapData->read("CSV\\NonBreakBlockPosition.csv"))
	{
		throw(GameError(gameErrorNS::FATAL_ERROR, "CSV���ǂݍ��߂܂���ł���"));
	}

	int index = 0;
	for(int i = 0; i <= 4800; i++)
	{
		nonBreakBlockMapTable[i] = nonBreakBlockMapData->getToComma(&index);
	}
	nonBreakBlockMapData->deleteDataArray();

	initialize();
}

//=============================================================================
//  ������
//�@�����F�Ȃ�
//	�߂�l�F�Ȃ�
//=============================================================================
void Wall::initialize()
{
	putWallCount = 0;
	setIsTimeUp(false);
	isTimeUp = false;

	vsCpuPlayerUnit = nullptr;
	player1 = nullptr;
	player2 = nullptr;
	cpuUnit[NORMALCPU] = nullptr;
	cpuUnit[PYECPU] = nullptr;
	cpuUnit[SPEEDCPU] = nullptr;

	char* file;
	int fileNum = rand() % FILENUM;

	if(fileNum == 0)
		file = "CSV\\arry1.csv";
	else if(fileNum == 1)
		file = "CSV\\arry2.csv";
	else if(fileNum == 2)
		file = "CSV\\arry3.csv";
	else if(fileNum == 3)
		file = "CSV\\arry4.csv";
	else if(fileNum == 4)
		file = "CSV\\arry5.csv";

	if(!mapTableData->read(file))
	{
		throw(GameError(gameErrorNS::FATAL_ERROR, "CSV���ǂݍ��߂܂���ł���"));
	}	
	
	int index = 0;
	//���ԂɊe�}�X�̒l���擾
	for(int x = 0; x <= 50; x++)
	{
		for(int z = 0; z <= 50; z++)
		{
			blockMapTable[x][z] = mapTableData->getToComma(&index);
		}
	}
	mapTableData->deleteDataArray();

	initList();

	randomWallArrange();
}

//=============================================================================
//  �e���X�g�̏�Ԃ�������
//�@�����F�Ȃ�
//	�߂�l�F�Ȃ�
//=============================================================================
void Wall::initList()
{
	ZeroMemory(wallData, sizeof(wallData));
	ZeroMemory(wallArr, sizeof(wallArr));
}

//=============================================================================
//�f�X�g���N�^
//=============================================================================
Wall::~Wall(void)
{
	SAFE_DELETE(mapTableData);
	SAFE_DELETE(nonBreakBlockMapData);
}

//=============================================================================
//�V�[���̃��j�b�g���擾
//�����F�V�[���̃��j�b�g
//�߂�l�F�Ȃ�
//=============================================================================
//ScceneBase����Ăяo���ꂽUnitBase��collisions����^�[�Q�b�g�擾
//typeid...typeid(�^���܂��͌^�^�̕ϐ����j...���^�ō��ꂽ��
void Wall::collisions(UnitBase* target)
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
//  �X�V
//�@�����F�Ȃ�
//	�߂�l�F�Ȃ�
//=============================================================================
void Wall::update()
{
	static DWORD count = 0;
	if(isTimeUp && count % 3 == 0)
	{
		timeUp();
	}
}

//=============================================================================
//  �`��
//�@�����F�t���[���A�J�����ԍ�
//	�߂�l�F�Ȃ�
//=============================================================================
void Wall::render(float frameTime/*�t���[��*/, int cameraNum /*�J�����ԍ�*/)
{	
	//�ǂ�z�u����͈͕����[�v
	for(int i = 1; i < WALLARRAYRANGE-1; i++)
	{
		for(int j = 1; j < WALLARRAYRANGE-1; j++)
		{
			//i�sj��1�Ȃ�i�ǂ�z�u����Ȃ�j
			if(wallArr[i][j] == PUTWALL)
			{
				float sizeX = 1.0f, sizeY = 1.0f;
				setBillboard(i, j, sizeX, sizeY, cameraNum);
				wallPoly.boxDraw(&(matScale * matBillboard * matTrans));
			} else if(wallArr[i][j] == NOTBREAKWALL) {
				float sizeX = 1.0f, sizeY = 1.0f;
				setBillboard(i, j, sizeX, sizeY, cameraNum);
				nonBreakWallPoly.boxDraw(&(matScale * matBillboard * matTrans));
			} else if(wallArr[i][j] == NOTWALL) {
				float sizeX = 1.0f, sizeY = 1.0f;
				setBillboard(i, j, sizeX, sizeY, cameraNum);
				notWallProcess(i, j, cameraNum);
			}
		}
	}
}

//=============================================================================
//  �ǂ��Ȃ��ꏊ�̏���
//�@�����F�ʒu�A�J�����ԍ�
//	�߂�l�F�Ȃ�
//=============================================================================
void Wall::notWallProcess(int x, int z, int cNum)
{
	//�ǂ��j�󂳂ꂽ��Ȃ�
	if(wallData[x][z].cpuCost == OBSTACLELCOST)
	{
		//CPU�R�X�g��������
		wallData[x][z].cpuCost = NONCOSTE;
	}

	//�j�󂳂ꂽ�ǂɃA�C�e�������݂��Ă����
	if(wallData[x][z].isItem != DEFAULTITEM)
	{
		//�A�C�e����CPU�R�X�g����
		wallData[x][z].cpuCost = ITEMCOST;
	}

	float f_x = (float)x, f_z = (float)z;

	//�A�C�e���̈ʒu�i�[
	switch (wallData[x][z].isItem/*�ǂ̎�ނ̃A�C�e����*/)
	{
		case ITEM::DEFAULTITEM:
			break;

		case ITEM::PLUSBOMITEM:
			//�|���S���Ƀe�N�X�`���\��t���ĕ\��
			plusBomPoly.draw(&(matScale * matBillboard * matTrans));
			wallData[x][z].itemPos = D3DXVECTOR3(f_x,0,f_z);
			break;

		case ITEM::LARGEEXPLOSIONITEM:
			largeexplosionPoly.draw(&(matScale * matBillboard * matTrans));
			wallData[x][z].itemPos = D3DXVECTOR3(f_x,0,f_z);
			break;

		case ITEM::PARALYZEBOMITEM:
			paralyzeBomPoly.draw(&(matScale * matBillboard * matTrans));
			wallData[x][z].itemPos = D3DXVECTOR3(f_x,0,f_z);
			break;

		case ITEM::THROWBOMITEM:
			bomThrowPoly.draw(&(matScale * matBillboard * matTrans));
			wallData[x][z].itemPos = D3DXVECTOR3(f_x,0,f_z);
			break;

		default:
			break;
	}
}

//=============================================================================
//�r���{�[�h�̐ݒ�Z�b�g
//�����F�`��ʒu��x,z���W�A�T�C�Y�A�J�����ԍ�
//�߂�l�F�Ȃ�
//=============================================================================
void Wall::setBillboard(int x, int z, float scaleX, float scaleY, int cNum)
{
	//�r���{�[�h�ݒ�
	D3DXMatrixScaling(&matScale, scaleX, scaleY, 1);	
	//2��ʂ̕����̍ۗ����ɕ\������
	if(cNum == CAMERA1)
	{
		matBillboard = cameraA->billboard();
	} else {
		matBillboard = cameraB->billboard();
	}
	float f_x = (float)x, f_z = (float)z;
	D3DXMatrixTranslation(&matTrans,f_x, 0, f_z);
}

//=============================================================================
//�ǂ̗L���������_���Ɍ��߂�
//�����F�Ȃ�
//�߂�l�F�Ȃ�
//=============================================================================
void Wall::randomWallArrange()
{
	//�ǂ�z�u����͈͕����[�v
	for(int i = 0; i < WALLARRAYRANGE; i++)
	{
		for(int j = 0; j < WALLARRAYRANGE; j++)
		{
			float f_x = (float)i, f_z = (float)j;

			if(blockMapTable[i][j] == 0) {
				wallArr[i][j]			= OUTRANGE;
				wallData[i][j].pos		= D3DXVECTOR3(f_x,0,f_z);
				wallData[i][j].isItem	= DEFAULTITEM;
				wallData[i][j].cpuCost  = OBSTACLELCOST;
				wallData[i][j].expRangeNum = 0;
				wallData[i][j].isExpRange = false;
			}
			//��
			if(blockMapTable[i][j] == 1) {
				//i�sj��ɕǂɊւ���e�����i�[
				wallArr[i][j]			= PUTWALL;
				wallData[i][j].pos		= D3DXVECTOR3(f_x,0,f_z);
				int item = rand() % ITEMRAN;
				if(item > ITEMNUM) {
					wallData[i][j].isItem = DEFAULTITEM;
				} else {
					wallData[i][j].isItem = item;	
				}
				wallData[i][j].cpuCost  = OBSTACLELCOST;
				wallData[i][j].expRangeNum = 0;
				wallData[i][j].isExpRange = false;
			}
			//�j�󂳂�Ȃ���
			if(blockMapTable[i][j] == 2) {
				wallArr[i][j]		   = NOTBREAKWALL;
				wallData[i][j].pos	   = D3DXVECTOR3(f_x,0,f_z);
				wallData[i][j].isItem  = DEFAULTITEM;
				wallData[i][j].cpuCost = OBSTACLELCOST;
				wallData[i][j].expRangeNum = 0;
				wallData[i][j].isExpRange = false;
			}
			//�����Ȃ�
			if(blockMapTable[i][j] == 3) {
				wallArr[i][j]		   = NOTWALL;
				wallData[i][j].pos	   = D3DXVECTOR3(f_x,0,f_z);
				wallData[i][j].isItem  = DEFAULTITEM;
				wallData[i][j].cpuCost = NONCOSTE;
				wallData[i][j].expRangeNum = 0;
				wallData[i][j].isExpRange = false;
			}
		}
	}
}

//=============================================================================
//�A�C�e���ԍ���Ԃ�
//�����F�v���C���[��x,z���W
//�߂�l�F�A�C�e���ԍ�
//=============================================================================
int Wall::retItemNumber(float posX, float posZ)
{
	int x = (int)posX, z = (int)posZ;
	//�����̈ʒu�ɃA�C�e��������ΕԂ�l�̕ϐ��Ɏ�ފi�[
	int itemNum = wallData[x][z].isItem;
	//��񏉊���
	wallData[x][z].isItem = DEFAULTITEM;
	wallData[x][z].cpuCost = NONCOSTE;

	return itemNum;
}

//=============================================================================
//���Ԑ�����̏���
//�����F�Ȃ�
//�߂�l�F�Ȃ�
//=============================================================================
void Wall::timeUp()
{
	if(putWallCount <= TIMEUPWALLNUM-2) {
		int x = nonBreakBlockMapTable[putWallCount];
		int y = nonBreakBlockMapTable[putWallCount+1];
		wallArr[x][y]		   = NOTBREAKWALL;
		wallData[x][y].pos	   = D3DXVECTOR3(x,0,y);
		wallData[x][y].isItem  = DEFAULTITEM;
		wallData[x][y].cpuCost = OBSTACLELCOST;
		wallData[x][y].expRangeNum = 0;
		wallData[x][y].isExpRange = false;
		collisionOnTheWall(D3DXVECTOR2(x, y));
		putWallCount += 2;
	}
}

//=============================================================================
//�L�����N�^�[���^�C���A�b�v��̕ǂɏՓ˂�����
//�����F���݂̕ǂ̈ʒu�A�O�̂̕ǂ̈ʒu
//�߂�l�F�Ȃ�
//=============================================================================
void Wall::collisionOnTheWall(D3DXVECTOR2 pos/*, D3DXVECTOR2 prevPos*/)
{
	D3DXVECTOR3 wallPos((float)pos.x, 0, (float)pos.y);

	//CPU�ΐ�
	if(modeChange::GetInstance() -> getMode() == VSCPU){
		//�eCPU�L�����N�^�[�̏��i�[
		NormalCpu* cpuChara[] = { cpuUnit[PYECPU],
					cpuUnit[SPEEDCPU],
					cpuUnit[NORMALCPU]  };
		//CPU�L���������[�v
		for(int i = 0; i < CHARANUM; i++)
		{
			//�����ʒu�ɃL�����N�^�[�������
			if(wallPos == D3DXVECTOR3(toIntPos(cpuChara[i]->pos.x), 0, toIntPos(cpuChara[i]->pos.z)))
			{
				//�L�����N�^-�̎��S�t���O��true��
				cpuChara[i]->isLose = true;
			}
		}

		//�����ʒu�Ƀv���C���[�������
		if(wallPos == D3DXVECTOR3(toIntPos(vsCpuPlayerUnit ->pos.x), 0, toIntPos(vsCpuPlayerUnit ->pos.z)))
		{
			//�v���C��-�̎��S�t���O��true��
			vsCpuPlayerUnit ->isLose = true;
		}
	}

	//2P�ΐ�
	if(modeChange::GetInstance() -> getMode() == VS2P)
	{
		//�����ʒu�Ƀv���C���[1�������
		if(wallPos == D3DXVECTOR3(player1->pos.x, 0, player1->pos.z))
		{
			//�v���C��-1�̎��S�t���O��true��
			player1->isLose = true;
		}

		//�����ʒu�Ƀv���C���[2�������
		if(wallPos == D3DXVECTOR3(player2->pos.x, 0, player2->pos.z))
		{
			//�v���C��-2�̎��S�t���O��true��
			player2->isLose = true;
		}
	}
}