//=============================================================================
//  �v���C���[(CPU�ΐ�p)
//�@�X�V���F2016/10/30
//	����ҁF���X�ؗ��M
//=============================================================================
#include "vsCpuPlayer.h"
#include "input.h"
#include "Camera.h"
#include "NormalCpu.h"
#include "PsyCpu.h"
#include "SpeedCpu.h"
#include "normalBom.h"
#include "paralyzeBom.h"
#include "plusBom.h"
#include "Wall.h"
#include "Model.h"
#include "Image.h"

const float posSelectSpeed = 1.0f;
#define PICSIZE         64  //�e�N�X�`���T�C�Y
#define VSCPUPLAYER		1	//�v���C��-�ԍ�		
#define PUTBOMPLAYER	0	//�{����u���v���C��-�ԍ�	
#define MAXRANGE		49	//�ő�͈�
#define MINRANGE		1	//�ŏ��͈�
#define	NOTWALL			0	//�ǂ����݂��Ȃ�
#define	ISWALL			1	//�ǂ�����
#define NOTBREAKWALL    2	//�j�󂳂�Ȃ���
#define STOPTIME        150	//��~����
#define OBSTACLELCOST   100	//��Q���̃R�X�g
#define COUNTFRAME		1	//�o�߃t���[����r�p
#define CAMERA1			1	//�J�����ԍ�
#define SHAKETIMERANGE	30	//�h�炷����
#define DIVFRAME		5	//�J�����̗h��ʒu�𕪊�

//�R�}���h�̎��+
enum COMMAND_TYPE
{
	NOTCOMMAND,		//�����Ȃ� 
	MOVEUP,			//�O�i
	LEFTTURN,		//���ɉ�] 
	RIGHTTURN,		//�E�ɉ�] 
	BACKTURN,		//���ɉ�]
	BOM,			//�ʏ�{�� 
	THROW,			//�{���𓊂���
};

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
vsCpuPlayer::vsCpuPlayer() : VsPlayer(0)
{
	//���f���ǂݍ���
	model = new Model;
	//�C���[�W�ǂݍ���
	bomimage			   = new Image;
	plusBomimage		   = new Image;
	arrowimage			   = new Image;
	paralyzeBomimage       = new Image;
	throwimage			   = new Image;
	leftItemNumimage	   = new Image;
	rightItemNumimage	   = new Image;

	//���f���ǂݍ���
	model->Load(graphics->get3Ddevice(),"Models\\chara_w2.fbx");

	//�e�N�X�`���ǂݍ���
	//�{���𓊂����������G�t�F�N�g
	if (!texBomThrowPosEffect.initialize(graphics,"pictures\\BomThrowPosEffect2.png"))
		throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing BomThrowPosEffect"));
		BomThrowPosEffectPoly.initialize(graphics, &texBomThrowPosEffect);

	//bom
	if (!texBom.initialize(graphics,"pictures\\bom.png"))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing bom texture"));
    if (!bomimage->initialize(graphics,PICSIZE,PICSIZE,0,&texBom))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing bom"));

    //plusBom
	if (!texPlusBom.initialize(graphics,"pictures\\plusBom.png"))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing plusBom texture"));
    if (!plusBomimage->initialize(graphics,PICSIZE,PICSIZE,0,&texPlusBom))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing plusBom"));

	//paralyzeBom
	if (!texParalyzeBom.initialize(graphics,"pictures\\paralyze.png"))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing paralyze texture"));
    if (!paralyzeBomimage->initialize(graphics,PICSIZE,PICSIZE,0,&texParalyzeBom))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing paralyzeBom"));

	//throwBom
	if (!texThrow.initialize(graphics,"pictures\\bomThrow.png"))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing bomThrow texture"));
    if (!throwimage->initialize(graphics,PICSIZE,PICSIZE,0,&texThrow))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing bomThrow"));

	//���
	if (!texArrow.initialize(graphics,"pictures\\�J�[�\��.png"))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing Arrow texture"));
    if (!arrowimage->initialize(graphics,PICSIZE,PICSIZE,0,&texArrow))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing largeexplosionBom"));

	//�A�C�e�����p
	if (!texItemNum.initialize(graphics,"pictures\\s_number.png"))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing number texture"));
    if (!leftItemNumimage->initialize(graphics,PICSIZE/2,PICSIZE/2,0,&texItemNum))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing number"));
	if (!rightItemNumimage->initialize(graphics,PICSIZE/2,PICSIZE/2,0,&texItemNum))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing number"));

	//�v���C���[�ԍ����
	player = VSCPUPLAYER;
	//�{����u���v���C���[�̔ԍ�
	putBomPlayer = PUTBOMPLAYER;

	initList();
	initialize();
}

//=============================================================================
//  ������
//�@�����F�Ȃ�
//	�߂�l�F�Ȃ�
//=============================================================================
void vsCpuPlayer::initialize()
{
	//�e��񏉊���
	iskeyState = true;
	throwFlag  = false;
	isItem	   = false;
	isStop     = false;
	isLose     = false;
	effectRenderFlag = false;
	isShake = false;
	isPushMoveButton = false;

	processBomType = NONE;
	commandSelect = NOTCOMMAND;
	itemType = DEFAULTITEM;

	stopCount = 0;
	moveControlCount = 0;
	minRange = 999.0f;	
	shakeDir = 1.0f;
	shakeTime = 0;
	addExpRange = 0;

	shakePos = D3DXVECTOR3(0,0,0);
	bomThrowPos = D3DXVECTOR3(0,0,0);
	bomThrowEffectPos = D3DXVECTOR3(0,0,0);

	//�ʒu
	pos = D3DXVECTOR3(49,-0.5f,49);
	//�ŏ��̌���
	angle = 180.0f;

	//�ړ��L�[
	KEY.MOVE_UP    = VK_UP;
	KEY.MOVE_DOWN  = VK_DOWN;
	KEY.MOVE_RIGHT = VK_RIGHT;
	KEY.MOVE_LEFT  = VK_LEFT;

	//�R�}���h�I��
	KEY.COMMAND_SELECT_RIGHT = 'E';
	KEY.COMMAND_SELECT_LEFT  = 'W';

	//�{����u��
	KEY.PUT_BOM = 'A';
	KEY.THROW = 'S';


	//�e�N�X�`�����Z�b�g
	bomimage->setX(0);
	plusBomimage->setX(PICSIZE);
	paralyzeBomimage->setX(PICSIZE*2);
	throwimage->setX(PICSIZE*3);
	arrowimage->setX(0);
	leftItemNumimage->setY(PICSIZE);
	rightItemNumimage->setY(PICSIZE);

	wallUnit = nullptr;
	bomUnit[NORMALBOM] = nullptr;
	bomUnit[PARALYZEBOM] = nullptr;
	bomUnit[PLUSBOM] = nullptr;
	cpuUnit[NORMALCPU] = nullptr;
	cpuUnit[PYECPU] = nullptr;
	cpuUnit[SPEEDCPU] = nullptr;
}

//=============================================================================
//�f�X�g���N�^
//=============================================================================
vsCpuPlayer::~vsCpuPlayer()
{
	SAFE_DELETE(model);
	SAFE_DELETE(bomimage);
	SAFE_DELETE(plusBomimage);
	SAFE_DELETE(arrowimage);
	SAFE_DELETE(paralyzeBomimage);
	SAFE_DELETE(throwimage);
	SAFE_DELETE(leftItemNumimage);
	SAFE_DELETE(rightItemNumimage);
}

//=============================================================================
//�V�[���̃��j�b�g���擾
//�����F�V�[���̃��j�b�g
//�߂�l�F�Ȃ�
//=============================================================================
//ScceneBase����Ăяo���ꂽUnitBase��collisions����^�[�Q�b�g�擾
//typeid...typeid(�^���܂��͌^�^�̕ϐ����j...���^�ō��ꂽ��
void vsCpuPlayer::collisions(UnitBase* target)
{
	if(typeid(*target) == typeid(Wall))
		wallUnit = (Wall*)target;

	if(typeid(*target) == typeid(normalBom))
		bomUnit[NORMALBOM] = ((normalBom*)target);
	
	if(typeid(*target) == typeid(paralyzeBom))
		bomUnit[PARALYZEBOM] = ((paralyzeBom*)target);

	if(typeid(*target) == typeid(plusBom))
		bomUnit[PLUSBOM] = ((plusBom*)target);

	if(typeid(*target) == typeid(NormalCpu))
		cpuUnit[NORMALCPU] = (NormalCpu*)target;

	if(typeid(*target) == typeid(PsyCpu))
		cpuUnit[PYECPU] = (PsyCpu*)target;

	if(typeid(*target) == typeid(SpeedCpu))
		cpuUnit[SPEEDCPU] = (SpeedCpu*)target;
}

//=============================================================================
//  �J�����R���g���[��
//�@�����F�Ȃ�
//	�߂�l�F�Ȃ�
//=============================================================================
void vsCpuPlayer::cameraCtrl()
{
	//�J�����̈ʒu(���E�̒��S���猩��)
	D3DXVECTOR3 vCamPos(0,15,-30);
	
	//�s��
	D3DXMATRIX vWorld,vCamangle;

	//��]�s��
	D3DXMatrixRotationY(&vCamangle, D3DXToRadian(angle));
	D3DXVec3TransformCoord(&vCamPos,&vCamPos,&vCamangle);
	D3DXVec3TransformCoord(&shakePos, &shakePos, &vCamangle);

	//��ԂƃJ�����̋����i�x�N�g���j��ێ�
	vCamPos += pos;
	D3DXVECTOR3 target = pos+shakePos;

	cameraA->setPos(vCamPos);
	cameraA->setTarget(target);
	cameraA->update();
}

//=============================================================================
//��Q���ɑ΂��鏈��������
//�����F�R�X�g��ݒ肷�邩�ǂ����A�L�����N�^�[�ԍ�
//�߂�l�F�Ȃ�
//=============================================================================
bool vsCpuPlayer::collObstacel(D3DXVECTOR3 pos, bool isThrow)
{
	float blockX = toIntPos(pos.x), blockZ = toIntPos(pos.z);

	if(!isThrow) {
		//�ڂ̑O�̈ʒu�i�[
		blockX += toIntMove (move.x);
		blockZ += toIntMove(move.z);
	}
	bool isHit = false;

	//�ڂ̑O���ǂ�������
	if(wallUnit -> retWallArr(blockX,blockZ) != NOTWALL) {
		//�i�߂Ȃ�����
		isHit = true;
	}

	//�ڂ̑O�����e
	if(bomUnit[NORMALBOM] -> retIsBom(blockX, blockZ) == OBSTACLELCOST/*��r����^�C������Q��*/
		|| bomUnit[PLUSBOM] -> retIsBom(blockX, blockZ) == OBSTACLELCOST/*��r����^�C������Q��*/
			|| bomUnit[PARALYZEBOM] -> retIsBom(blockX, blockZ) == OBSTACLELCOST/*��r����^�C������Q��*/)
	{
		isHit = true;
	} 

	//�ڂ̑O���L�����N�^�[
	if(D3DXVECTOR3(blockX, 0, blockZ) == D3DXVECTOR3(toIntPos(cpuUnit[SPEEDCPU]->pos.x), 0, toIntPos(cpuUnit[SPEEDCPU]->pos.z))
		|| D3DXVECTOR3(blockX, 0, blockZ ) == D3DXVECTOR3(toIntPos(cpuUnit[PYECPU]->pos.x), 0, toIntPos(cpuUnit[PYECPU]->pos.z))
			||D3DXVECTOR3(blockX, 0, blockZ) == D3DXVECTOR3(toIntPos(cpuUnit[NORMALCPU]->pos.x), 0, toIntPos(cpuUnit[NORMALCPU]->pos.z)))
	{
		isHit = true;
	} 

	return isHit;
}

//=============================================================================
//�J������h�炷���Z�b�g
//�����F�����ʒu(�߂��ق����D�悳���), �h�炷�傫��
//�����F�Ȃ�
//=============================================================================
void vsCpuPlayer::setShakeCameraInfo(D3DXVECTOR3 expPos, float shakeSize)
{ 	
	//���������ʒu�ƌ��݈ʒu�̋���
	D3DXVECTOR3 range = expPos - pos;
	//���݂̋���
	float nowRange = D3DXVec3Length(&range);
	nowRange *= shakeSize;

	//10�^�C���ȏ㗣��Ă���Ηh�炳�Ȃ�
	if(nowRange > 10.0f) 
		return;

	//���݂̋������ŏ�������菬�������
	if(nowRange < minRange) {
		isShake = true;
		shakeTime = 0;
		minRange = nowRange*(nowRange/2);  
	}
}