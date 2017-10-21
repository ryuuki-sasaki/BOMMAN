//=============================================================================
//  �v���C���[(2P�ΐ�p)
//�@�X�V���F2016/10/30
//	����ҁF���X�ؗ��M
//=============================================================================
#include "VsPlayer.h"
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
#include "modeChange.h"

const float posSelectSpeed = 1.0f;	
#define PICSIZE         64  //�e�N�X�`���T�C�Y
#define PLAYER1			1	//�v���C��-�ԍ�		
#define MAXRANGE		49	//�ő�͈�
#define MINRANGE		1	//�ŏ��͈�
#define	NOTWALL			0	//�ǂ����݂��Ȃ�
#define	ISWALL			1	//�ǂ�����
#define NOTBREAKWALL    2	//�j�󂳂�Ȃ���
#define STOPTIME        150	//��~����
#define OBSTACLELCOST   100	//��Q���̃R�X�g
#define CAMERA1			1	//�J�����ԍ�
#define CAMERA2			2	
#define COUNTFRAME		1	//�o�߃t���[����r�p
#define ADDSETFRAME		320	//�e�N�X�`���ʒu�̒ǉ���
#define SHAKETIMERANGE	30	//�h�炷����
#define DIVFRAME		5	//�J�����̗h��ʒu�𕪊�
#define INITNUM 100			//���������ɑ������l
#define ITEMNUM 3			//�A�C�e���̎��

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

//�{���̕`��Ɋւ�����
enum BOM_RENDER_STATE
{
	RENDER,			//�`�揈��
	ACTIONEXPSlON,	//�����������s��
	DONAOTHING		//�Ȃ�ł��Ȃ�
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
VsPlayer::VsPlayer(int playerNum)
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
	player = playerNum;
	//�{����u���v���C���[�̔ԍ�
	putBomPlayer = player;
	
	initList();
	initialize();
}

//=============================================================================
//  ������
//�@�����F�Ȃ�
//	�߂�l�F�Ȃ�
//=============================================================================
void VsPlayer::initialize()
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

	//�R���X�g���N�^�œn���ꂽ�v���C���[�ԍ��ɂ���Đ؂�ς�(SeneBese����n�����)
	if(player == PLAYER1)
	{
		//�ʒu
		pos = D3DXVECTOR3(49,-0.5f,49);
		//�ŏ��̌���
		angle = 180.0f;
	} else {
		pos = D3DXVECTOR3(1,-0.5f,1);
		angle = 0.0f;
	}

	if(player == PLAYER1)
	{
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
	} else {
		//�ړ��L�[
		KEY.MOVE_UP    = VK_NUMPAD8;
		KEY.MOVE_DOWN  = VK_NUMPAD5;
		KEY.MOVE_RIGHT = VK_NUMPAD6;
		KEY.MOVE_LEFT  = VK_NUMPAD4;

		//�R�}���h�I��
		KEY.COMMAND_SELECT_RIGHT = VK_NUMPAD9;
		KEY.COMMAND_SELECT_LEFT  = VK_NUMPAD7;

		//�{����u��
		KEY.PUT_BOM = VK_NUMPAD2;
		KEY.THROW = VK_NUMPAD0;
	}

	//�e�N�X�`�����Z�b�g
	if(player == PLAYER1)
	{
		bomimage->setX(0);
		plusBomimage->setX(PICSIZE);
		paralyzeBomimage->setX(PICSIZE*2);
		throwimage->setX(PICSIZE*3);
		arrowimage->setX(0);
		leftItemNumimage->setY(PICSIZE);
		rightItemNumimage->setY(PICSIZE);
	} else {
		bomimage->setX(GAME_WIDTH/2+ADDSETFRAME);
		plusBomimage->setX(GAME_WIDTH/2+PICSIZE+ADDSETFRAME);
		paralyzeBomimage->setX(GAME_WIDTH/2+PICSIZE*2+ADDSETFRAME);
		throwimage->setX(GAME_WIDTH/2+PICSIZE*3+ADDSETFRAME);
		arrowimage->setX(GAME_WIDTH/2+ADDSETFRAME);
		leftItemNumimage->setY(PICSIZE);
		rightItemNumimage->setY(PICSIZE);
	}

	wallUnit = nullptr;
	player1 = nullptr;
	player2 = nullptr;
	bomUnit[NORMALBOM] = nullptr;
	bomUnit[PARALYZEBOM] = nullptr;
	bomUnit[PLUSBOM] = nullptr;
	cpuUnit[NORMALCPU] = nullptr;
	cpuUnit[PYECPU] = nullptr;
	cpuUnit[SPEEDCPU] = nullptr;
}

//=============================================================================
//�e���X�g�̏�񏉊���
//=============================================================================
void VsPlayer::initList()
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
}

//=============================================================================
//�f�X�g���N�^
//=============================================================================
VsPlayer::~VsPlayer()
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
void VsPlayer::collisions(UnitBase* target)
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
//  �`��֐�
//�@�����F�t���[���A�J�����ԍ�
//	�߂�l�F�Ȃ�
//=============================================================================
void VsPlayer::render(float frameTime, int cameraNum)
{
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
	} else {
		pEffect -> SetMatrix("g_matWVP", 
			&( mat* cameraB -> GetView() * cameraB -> GetProj() ) );

		pEffect->SetVector("g_vecEye", (D3DXVECTOR4*)&cameraB->GetPos());
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

	//�e�N�X�`���`��
	{
		graphics->spriteBegin();
		bomimage->draw();
		plusBomimage->draw();                    
		throwimage->draw();
		paralyzeBomimage->draw();  
		arrowimage->draw();
		graphics->spriteEnd();
	}

	renderNumber();
	bomProcess();

	//�{���𓊂����������e�N�X�`���̕`��
	if(effectRenderFlag)
	{
		if(!collObstacel(bomThrowEffectPos, throwFlag)) 
		{
			D3DXMATRIX matScale,matBillboard,matTrans;
			D3DXMatrixScaling(&matScale, 1, 6, 1);
			if(cameraNum == CAMERA1)
			{
				matBillboard = cameraA->billboard();
			} else {
				matBillboard = cameraB->billboard();
			}
			D3DXMatrixTranslation(&matTrans, bomThrowEffectPos.x, 3.0f, bomThrowEffectPos.z);
			BomThrowPosEffectPoly.draw(&(matScale * matBillboard * matTrans));
		}
	}
}

//=============================================================================
//  �A�C�e�����`��
//�@�����F�Ȃ�
//	�߂�l�F�Ȃ�
//=============================================================================
void VsPlayer::renderNumber()
{
	list<int> bomList[] = {plusBomNumList,
						paralyzeBomNumList,
						bomThrowNumList};

	for(int i = 0; i < 3/*4*/; i++) {
		int rightNum = (bomList[i].size() / 1) % 10;
		int leftNum = (bomList[i].size() / 10) % 10;
		rightItemNumimage->setCurrentFrame(rightNum);	//1�̈�
		leftItemNumimage->setCurrentFrame(leftNum);		//10�̈�

		if(player == PLAYER1 /*�v���C���[1*/) {
			rightItemNumimage->setX((float)(i+1)*PICSIZE+PICSIZE/2);			
			leftItemNumimage->setX((float)(i+1)*PICSIZE);
		} else /*�v���C���[2*/{
			rightItemNumimage->setX(((float)(i+1)*PICSIZE+PICSIZE/2)+GAME_WIDTH/2+ADDSETFRAME);			
			leftItemNumimage->setX(((float)(i+1)*PICSIZE)+GAME_WIDTH/2+ADDSETFRAME);
		}
		graphics->spriteBegin();
		rightItemNumimage->draw();
		leftItemNumimage->draw();
		graphics->spriteEnd();
	}
}

//=============================================================================
//  �`�撆�̃{�������邩
//�@�����F�Ȃ�
//	�߂�l�F�Ȃ�
//=============================================================================
void VsPlayer::notRenderBom()
{
	//�e�{���ŁA�`�撆�̂��̂�����Ίe�N���X��render�Ăяo��
	if(!bomUnit[NORMALBOM] -> putCharaNum[putBomPlayer].bomRenderState == RENDER/*�ʏ�{��*/
		&& !bomUnit[PARALYZEBOM] -> putCharaNum[putBomPlayer].bomRenderState == RENDER/*Ⴢ�{��*/) 
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
void VsPlayer::bomProcess()
{
	//�e�{���̏���
	switch(itemType/*�ǂ̎�ނ̃{�����������邩*/)
	{
		case ITEM::DEFAULTITEM/*�ʏ�{��*/:
			if(processBomType == NONE)
			{
				if(isItem/*�A�C�e���{�����g�p���邩*/)
				{
					//���ݒʏ�{�������̏��������Ă��Ȃ�(=�ʏ�{�����g���Ă��Ȃ�)
					if(bomUnit[NORMALBOM] -> putCharaNum[putBomPlayer].bomRenderState == DONAOTHING)
					{
						//�{����u�����ꏊ��ێ�
						D3DXVECTOR3 putBomPos = D3DXVECTOR3(toIntPos(pos.x)+toIntMove(move.x), pos.y, toIntPos(pos.z)+toIntMove(move.z));
						bomUnit[NORMALBOM] -> addBom(putBomPos/*�v���C���[�̈ʒu�x�N�g��*/, move/*�ړ��x�N�g��*/,addExpRange/*�����͈͉��Z��*/, putBomPlayer/*�L�����N�^�[��*/, bomThrowPos/*��������̈ʒu�x�N�g��*/, throwFlag/*�{����u���������邩*/);
						processBomType = ITEM::DEFAULTITEM;
					}
					//�����邩�ǂ�����false�ɂ���ibom -> render�̈����ɂȂ��Ă��邽�߁Afalse�ɂ��Ă����j
					throwFlag = false;
				}									
			}
			break;

		case ITEM::PLUSBOMITEM/*�v���X�{��*/:
			{	
				if(isItem/*�A�C�e���{�����g�p���邩*/)
				{	
					plusBomitr = plusBomNumList.begin();
					
					//plusBom�������	
					if(!(plusBomNumList.empty()))
					{	
						//�{����u�����ꏊ��ێ�
						D3DXVECTOR3 putBomPos = D3DXVECTOR3(toIntPos(pos.x)+toIntMove(move.x), pos.y, toIntPos(pos.z)+toIntMove(move.z));
						bomUnit[PLUSBOM] -> addBom(putBomPos/*�v���C���[�̈ʒu�x�N�g��*/, move/*�ړ��x�N�g��*/,addExpRange/*�����͈͉��Z��*/, putBomPlayer/*�L�����N�^�[��*/, bomThrowPos/*��������̈ʒu�x�N�g��*/, throwFlag/*�{����u���������邩*/);
						plusBomitr = plusBomNumList.erase(plusBomitr);
					}
					//�����邩�ǂ�����false�ɂ���ibom -> render�̈����ɂȂ��Ă��邽�߁Afalse�ɂ��Ă����j
					throwFlag = false;
				}
			}
			break;

		case ITEM::PARALYZEBOMITEM/*Ⴢ�{��*/:
			if(processBomType == NONE)
			{
				if(isItem/*�A�C�e���{�����g�p���邩*/)
				{
					paralyzeitr = paralyzeBomNumList.begin();
					
					//paralyzeBom�������	
					if(!(paralyzeBomNumList.empty()))
					{	
						//�{����u�����ꏊ��ێ�
						D3DXVECTOR3 putBomPos = D3DXVECTOR3(toIntPos(pos.x)+toIntMove(move.x), pos.y, toIntPos(pos.z)+toIntMove(move.z));
						bomUnit[PARALYZEBOM] -> addBom(putBomPos/*�v���C���[�̈ʒu�x�N�g��*/, move/*�ړ��x�N�g��*/,addExpRange/*�����͈͉��Z��*/, putBomPlayer/*�L�����N�^�[��*/, bomThrowPos/*��������̈ʒu�x�N�g��*/, throwFlag/*�{����u���������邩*/);
						paralyzeitr = paralyzeBomNumList.erase(paralyzeitr);
						processBomType = ITEM::PARALYZEBOMITEM;
					}	
					//�����邩�ǂ�����false�ɂ���ibom -> render�̈����ɂȂ��Ă��邽�߁Afalse�ɂ��Ă����j
					throwFlag = false;
				} 
			}
			break;
	}

	isItem = false;
}

//=============================================================================
//  �X�V
//�@�����F�Ȃ�
//	�߂�l�F�Ȃ�
//=============================================================================
void VsPlayer::update()
{	
	//�ړ�
	control();	
	//�J�����̑���
	cameraCtrl();
	//�͈͓���
	inRange();
	//�A�C�e���擾
	getItem();
	//�`�撆�̃{�������邩
	notRenderBom();
	//�R�}���h�I��
	selectCommand();
	//�J������h�炷
	shakeCamera();
	//�����鏈���Ȃ�
	if(throwFlag)
		//�{���𓊂���ʒu�������߂�
		selectThrowPos();
}

//=============================================================================
//  �R�}���h�I��
//�@�����F�Ȃ�
//	�߂�l�F�Ȃ�
//=============================================================================
void VsPlayer::selectCommand()
{
	//�ǂ̃A�C�e����I�����邩
	if(input -> wasKeyPush(KEY.COMMAND_SELECT_RIGHT) || input->wasGamePadPush(player, GAMEPAD_RIGHT_SHOULDER))
	{
		if(itemType < ITEMNUM)
		{
			arrowimage->setX(arrowimage->getX()+PICSIZE);
			//���̕ϐ��ɉ����Ďg�p�A�C�e�����ς��
			itemType++;
		}
	}

	if(input -> wasKeyPush(KEY.COMMAND_SELECT_LEFT) || input->wasGamePadPush(player, GAMEPAD_LEFT_SHOULDER))
	{
		if(itemType > 0)
		{
			arrowimage->setX(arrowimage->getX()-PICSIZE);
			itemType--;
		}
	}
}

//=============================================================================
//  �R�}���h�֌W�̏���
//�@�����F�Ȃ�
//	�߂�l�F�Ȃ�
//=============================================================================
void VsPlayer::control()
{
	//�ړ��x�N�g��
	move = D3DXVECTOR3(0, 0, 1.0f);
	//��]�s��
	D3DXMATRIX mRotat;
	D3DXMatrixRotationY(&mRotat, D3DXToRadian(angle));
	D3DXVec3TransformCoord(&move,&move,&mRotat);
	//move�̒����𐮐��ɕϊ�
	move = D3DXVECTOR3(toIntMove(move.x),0,toIntMove(move.z));


	//Ⴢ�{���ɓ������čs������~����Ă��Ȃ����
	if(isStop) {
		//�J�E���g����~���Ԃ��傫���Ȃ�����
		if(stopCount < STOPTIME/*5�b * 30fps*/)
		{	//��~���Ԃ̃J�E���g���Z
			stopCount++;
			return;
		} else {
			//�J�E���g������
			stopCount = 0;
			isStop = false;
		}
	}
	
	//�{���𓊂��鏈�����łȂ����
	if(!(throwFlag))
	{
		if(input -> isKeyPush(KEY.MOVE_UP) || input->isGamePadPush(player, GAMEPAD_DPAD_UP))
		{
			moveControlCount++;
			//1�t���[���ڂ͓������Ȃ�(�V�т�����)
			if(moveControlCount > COUNTFRAME)
			{
				//�ڂ̑O�ɏ�Q��������΂������
				if(collObstacel(pos, throwFlag))
				{
					//�O�ɐi�߂Ȃ�
					pos -= move * 0.5f;
				}
				//�O�i����
				pos += move * 0.5f;
			}
			//�ړ��{�^������������
			isPushMoveButton = true;
			//�ǂ̃R�}���h��I�񂾂�����
			commandSelect = COMMAND_TYPE::MOVEUP;
		}

		//���΂���������
		if(input -> isKeyPush(KEY.MOVE_DOWN) || input->isGamePadPush(player, GAMEPAD_DPAD_DOWN))
		{
			//�L�[��������1�t���[���ڂȂ�
			if(iskeyState)
			{	
				//���������牺��������
				//��]�͈͂𐧌��imove�ɉe�����o��j
				if(angle+180 >= 360)
				{
					angle = angle - 360;
				} else {
					angle += 180;
				}

				//2�t���[���ڈȍ~�̏������֎~
				iskeyState = false;
				//�ǂ̃R�}���h��I�񂾂�����
				commandSelect = COMMAND_TYPE::BACKTURN;
			}
		}

		//90�x�Â��ɉ�]
		if(input -> isKeyPush(KEY.MOVE_LEFT) || input->isGamePadPush(player, GAMEPAD_DPAD_LEFT))
		{
			//�L�[��������1�t���[���ڂȂ�
			if(iskeyState)
			{	
				adjustmentPos();
				angle -= 90.0f;
				//��]�͈͂𐧌��imove�ɉe�����o��j
				if(angle <= -360)
				{
					angle = 0;
				}
				//2�t���[���ڈȍ~�̏������֎~
				iskeyState = false;
				//�ǂ̃R�}���h��I�񂾂�����
				commandSelect = COMMAND_TYPE::LEFTTURN;
			}
		}

		//90�x�ÂE�ɉ�]
		if(input -> isKeyPush(KEY.MOVE_RIGHT) || input->isGamePadPush(player, GAMEPAD_DPAD_RIGHT))
		{
			//�L�[��������1�t���[���ڂȂ�
			if(iskeyState)
			{	
				adjustmentPos();
				angle += 90.0f;
				//��]�͈͂𐧌��imove�ɉe�����o��j
				if(angle >= 360)
				{
					angle = 0;
				}
				//2�t���[���ڈȍ~�̏������֎~
				iskeyState = false;
				//�ǂ̃R�}���h��I�񂾂�����
				commandSelect = COMMAND_TYPE::RIGHTTURN; 	
			}
		}

		//�{����u������
		if(input -> isKeyPush(KEY.PUT_BOM) || input->isGamePadPush(player, GAMEPAD_A))
		{	
			//�ڂ̑O���ǁA�v���C���[���̂Ƃ��{����u���Ȃ��悤�ɂ���
			if(!collObstacel(pos, throwFlag))
			{
				//�L�[��������1�t���[���ڂȂ�
				if(iskeyState)
				{	
					adjustmentPos();
					//2�t���[���ڈȍ~�̏������֎~
					iskeyState = false;
					//�A�C�e���{����I���������ǂ������i�[����ϐ���true�ɂ���
					isItem = true;
					//�ǂ̃R�}���h��I�񂾂�����
					commandSelect = COMMAND_TYPE::BOM;
				}
			}
		} 

		//�{���𓊂��鏈��
		if(input -> isKeyPush(KEY.THROW) || input->isGamePadPush(player, GAMEPAD_Y))
		{	
			//�L�[��������1�t���[���ڂȂ�
			if(iskeyState && !throwFlag && itemType == ITEM::THROWBOMITEM)
			{	
				//2�t���[���ڈȍ~�̏������֎~
				iskeyState = false;
				//�ǂ̃R�}���h��I�񂾂�����
				commandSelect = COMMAND_TYPE::THROW;
				//bomThrow�������	
				if(!(bomThrowNumList.empty()))
				{	
					//�{���𓊂����������e�N�X�`���\��
					effectRenderFlag = true;	
					//�����邩�ǂ�����false�ɂ���ibom -> render�̈����ɂȂ��Ă��邽�߁Afalse�ɂ��Ă����j
					throwFlag = true;
					//�����n�߂�ʒu����
					bomThrowPos = D3DXVECTOR3(0,0,0);
					bomThrowEffectPos = pos;
				}	
			}
		} 
	}

	processedCommand(commandSelect);

}

//=============================================================================
//  �R�}���h�̌㏈��
//�@�����F�I�������R�}���h
//	�߂�l�F�Ȃ�
//=============================================================================
void VsPlayer::processedCommand(int command)
{
	//�I�񂾃R�}���h�̌㏈��
	switch (commandSelect)
	{	
		case COMMAND_TYPE::MOVEUP:
			//�I�������R�}���h�̃L�[�𗣂�����
			if(!input -> isKeyPush(KEY.MOVE_UP) && !input->isGamePadPush(player, GAMEPAD_DPAD_UP))
			{
				moveControlCount = 0;
				if(isPushMoveButton)
					adjustmentPos();
				isPushMoveButton = false;
				//MOVEUP�Ƃق��̃R�}���h���������Ƃ�����������Ȃ��Ƃ�����false�ɂȂ�
				iskeyState = true;
			}
			break;

		case COMMAND_TYPE::LEFTTURN:
			if(!input -> isKeyPush(KEY.MOVE_LEFT) && !input->isGamePadPush(player, GAMEPAD_DPAD_LEFT))
			{
				iskeyState = true;
				commandSelect = COMMAND_TYPE::NOTCOMMAND;
			}
			break;

		case COMMAND_TYPE::RIGHTTURN:
			if(!input -> isKeyPush(KEY.MOVE_RIGHT) && !input->isGamePadPush(player, GAMEPAD_DPAD_RIGHT))
			{
				iskeyState = true;
				commandSelect = COMMAND_TYPE::NOTCOMMAND;
			}
			break;

		case COMMAND_TYPE::BACKTURN:
			if(!input -> isKeyPush(KEY.MOVE_DOWN) && !input->isGamePadPush(player, GAMEPAD_DPAD_DOWN))
			{
				iskeyState = true;
				commandSelect = COMMAND_TYPE::NOTCOMMAND;
			}
			break;
				
		case COMMAND_TYPE::BOM:
			if(!input -> isKeyPush(KEY.PUT_BOM) && !input->isGamePadPush(player, GAMEPAD_A))
			{
				iskeyState = true;
				commandSelect = COMMAND_TYPE::NOTCOMMAND;
			} 
			break;

		case COMMAND_TYPE::THROW:
			if(!input -> isKeyPush(KEY.THROW) && !input->isGamePadPush(player, GAMEPAD_Y))
			{
				iskeyState = true;
				commandSelect = COMMAND_TYPE::NOTCOMMAND;
			} 
			break;
	}
}

//=============================================================================
//�A�C�e���擾
//�����F�Ȃ�
//�߂�l�F�Ȃ�
//=============================================================================
void VsPlayer::getItem()
{
	//�A�C�e���ƌ��݂̈ʒu�����������
	if(D3DXVECTOR3(pos.x, 0, pos.z) == wallUnit -> retItem(pos.x,pos.z))
	{
		//�擾�����A�C�e������ۑ�
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
//��������ݒ�
//�����F�Ȃ�
//�߂�l�F�Ȃ�
//=============================================================================
void VsPlayer::selectThrowPos()
{
	//�G�t�F�N�g��O�Ɉړ�
	if(input -> isKeyPush(KEY.MOVE_UP) || input->isGamePadPush(player, GAMEPAD_DPAD_UP)) {		
		bomThrowPos += move;
		bomThrowEffectPos += move;
	}

	//���Ɉړ�
	if(input -> isKeyPush(KEY.MOVE_DOWN) || input->isGamePadPush(player, GAMEPAD_DPAD_DOWN)) {
		bomThrowPos -= move;
		bomThrowEffectPos -= move;
	}

	//�{���𓊂���
	if((input -> isKeyPush(KEY.PUT_BOM) || input->isGamePadPush(player, GAMEPAD_A))/*�{���֌W�̏���*/ && iskeyState/*���̃R�}���h���I������Ă��Ȃ�*/ && throwFlag/*�����鏈��*/ && isBomListState(itemType)/*�{�����X�g�̏�Ԃ�������*/) {
		//�w�肵���悪��Q���Ȃ�����
		if(!collObstacel(bomThrowEffectPos, throwFlag)) {
			//�{���𓊂�����������\��
			effectRenderFlag = false;
			//2�t���[���ڈȍ~�̏������֎~
			iskeyState = false;
			//�A�C�e���{����I���������ǂ������i�[����ϐ���true�ɂ���
			isItem = true;
			//�ǂ̃R�}���h��I�񂾂�����
			commandSelect = COMMAND_TYPE::BOM;

			bomThrowitr = bomThrowNumList.begin();
			//�g�����A�C�e���폜
			bomThrowitr = bomThrowNumList.erase(bomThrowitr);
		}
	}

	if((input -> isKeyPush(KEY.THROW) || input->isGamePadPush(player, GAMEPAD_Y)) && iskeyState) {
		//�{���̓�����ʒu�x�N�g���̏�����
		/*render�����̈�����bomThrowPos�����邽�߁A
		�����鏈������u�������ɐ؂�ւ����ۂɉe���̂Ȃ��悤�ɂ���*/
		bomThrowPos = D3DXVECTOR3(0,0,0);
		bomThrowEffectPos = pos;
		//�u��������I������
		throwFlag = false;	
		//�{���𓊂�����������\��
		effectRenderFlag = false;
		iskeyState = false;
	}
}

//=============================================================================
//  �J�����R���g���[��
//�@�����F�Ȃ�
//	�߂�l�F�Ȃ�
//=============================================================================
void VsPlayer::cameraCtrl()
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
	if(player == PLAYER1)
	{
		cameraA->setPos(vCamPos);
		cameraA->setTarget(target);
	} else {
		cameraB->setPos(vCamPos);
		cameraB->setTarget(target);
	}
}

//=============================================================================
//�J������h�炷���Z�b�g
//�����F�����ʒu(�߂��ق����D�悳���), �h�炷�傫��
//�����F�Ȃ�
//=============================================================================
void VsPlayer::setShakeCameraInfo(D3DXVECTOR3 expPos, float shakeSize)
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

//=============================================================================
//�J������h�炷
//�����F�Ȃ�
//�����F�Ȃ�
//=============================================================================
void VsPlayer::shakeCamera()
{ 	
	//�h��閽�߂��o����
	if(isShake) {
		// �h�炵���W�̎Z�o
		float hight = ((1.0f-(minRange*0.01f)) / (1.0f+(shakeTime+2.0f) / DIVFRAME)) * shakeDir;
	    
        shakePos = D3DXVECTOR3(0, hight, 0);

		shakeTime++;

		//���̃t���[������������t�����Ɉړ�
		if(shakeTime % DIVFRAME == 0) {
			shakeDir *= -1.0;
		}

		//�h�炷���Ԃ��߂�����
		if(shakeTime >= SHAKETIMERANGE){
			shakeTime = 0;
			minRange = 999.0f; 
			shakePos = D3DXVECTOR3(0, 0, 0);
			shakeDir = 1.0f;
			isShake = false;
		}
	}
}

//=============================================================================
//��Q���ɑ΂��鏈��������
//�����F�R�X�g��ݒ肷�邩�ǂ����A�L�����N�^�[�ԍ�
//�߂�l�F�Ȃ�
//=============================================================================
bool VsPlayer::collObstacel(D3DXVECTOR3 pos, bool isThrow)
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
	bool isCharacter = false;
	if(player == PLAYER1) {
		//�{����u�����v���C���[�̔ԍ�
		if(player2->pos == D3DXVECTOR3(blockX, pos.y, blockZ)) {
			isCharacter = true;
		}
	} else {
		if(player1->pos == D3DXVECTOR3(blockX, pos.y, blockZ)) {
			isCharacter = true;
		}
	}

	if(isCharacter)
	{
		isHit = true;
	} 

	return isHit;
}

//=============================================================================
//�ʒu�𐮐��ɐ�����
//�����F�Ȃ�
//�߂�l�F�Ȃ�
//=============================================================================
void VsPlayer::adjustmentPos()
{
	float decimalX, decimalZ;
	decimalX = fabs(pos.x) - fabs(floor(pos.x));
	decimalZ = fabs(pos.z) - fabs(floor(pos.z));
	if(decimalX >= 0.5f || decimalZ >= 0.5f)
		pos += move * 0.5f;
}

//=============================================================================
//�ʒu���t�B�[���h�����𔻒�
//�����F�Ȃ�
//�߂�l�F�Ȃ�
//=============================================================================
void VsPlayer::inRange()
{
	if(pos.z > MAXRANGE) {
		pos.z = MAXRANGE;
	} else if(pos.x > MAXRANGE) {
		pos.x = MAXRANGE;
	} else if(pos.z < MINRANGE) {
		pos.z = MINRANGE;
	} else if(pos.x < MINRANGE) {
		pos.x = MINRANGE;
	} 

	float blockX = toIntPos(pos.x)+toIntMove(move.x), blockZ = toIntPos(pos.z)+toIntMove(move.z);

	if(throwFlag) {
		if(blockZ > pos.z) {
			if(bomThrowEffectPos.z > MAXRANGE) {
				bomThrowEffectPos.z = MAXRANGE;
			} else if(bomThrowEffectPos.z < pos.z) {
				bomThrowEffectPos.z = blockZ;
			}
		}
		
		if(blockX > pos.x) {
			if(bomThrowEffectPos.x > MAXRANGE) {
				bomThrowEffectPos.x = MAXRANGE;
			} else if(bomThrowEffectPos.x < pos.x) {
				bomThrowEffectPos.x = blockX;
			}
		}
		
		if(blockZ < pos.z) { 
			if(bomThrowEffectPos.z < MINRANGE) {
				bomThrowEffectPos.z = MINRANGE;
			} else if(bomThrowEffectPos.z > pos.z) {
				bomThrowEffectPos.z = blockZ;
			}
		}
		
		if(blockX < pos.x) {  
			if(bomThrowEffectPos.x < MINRANGE) {
				bomThrowEffectPos.x = MINRANGE;
			} else if(bomThrowEffectPos.x > pos.x) {
				bomThrowEffectPos.x = blockX;
			}
		}
	}
}

//=============================================================================
//�e�{�����X�g���󂩂ǂ�����Ԃ�
//�����F�Ȃ�
//�߂�l�F���݂��邩�ǂ���
//=============================================================================
bool VsPlayer::isBomListState(int	itenType)
{
	switch (itenType)
	{
	case DEFAULTITEM:
		return true;
		break;
	case PLUSBOMITEM:
		if(!plusBomNumList.empty())
			return true;
		else
			return false;
		break;
	case PARALYZEBOMITEM:
		if(!paralyzeBomNumList.empty())
			return true;
		else
			return false;
		break;
	case THROWBOMITEM:
		return false;

	}

	return false;
}