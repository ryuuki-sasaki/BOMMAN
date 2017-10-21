//=============================================================================
//  �ʏ�{��
//�@�X�V���F2016/10/30
//	����ҁF���X�ؗ��M
//=============================================================================
#include "normalBom.h"
#include "VsPlayer.h"
#include "vsCpuPlayer.h"
#include "NormalCpu.h"
#include "PsyCpu.h"
#include "SpeedCpu.h"
#include "modeChange.h"
#include "Camera.h"
#include "Wall.h"
#include "Model.h"
#include "audio.h"
#include "effect.h"

#define RENDERTIME 120			//�`�掞��
#define PUTWALL 1				//�ǗL��
#define NOTWALL 0				//�ǂȂ�
#define CHARANUM 3				//�L�����N�^�[��
#define OBSTACLELCOST 100		//�Ǔ��̏�Q����CPU�R�X�g
#define NONCOST 0				//�R�X�g�Ȃ�
#define ITEMCOST -3				//�A�C�e���̃R�X�g
#define GROUNDHEIGHT -0.5f		//�n�ʂ̍���
#define BEGIN_SPEED 0.3f		//�C�e����
#define PI 3.1415927f			//�~����
#define CAMERA1 1				//�J�����ԍ�
#define PICSIZE 1				//�e�N�X�`���T�C�Y
#define UPDATEANIM 1.0f			//�A�j���[�V�����X�V
#define SCALE 5					//�傫��
#define RANGE 2					//�͈�
#define MAXRANGE 49				//�ő�͈�
#define MINRANGE 1				//�Œ�͈�
#define ADDTIME 5				//���Ԓǉ���
const float gravity = 0.002f;

//�{���̕`��Ɋւ�����
enum BOM_RENDER_STATE
{
	RENDER,			//�`�揈��
	ACTIONEXPSlON,	//�����������s��
	DONAOTHING		//�Ȃ�ł��Ȃ�
};

//�L�����N�^�[�̖��O
enum CHARA_NAME
{
	PLAYER,
	PYE,
	NORMAL,
	SPEED
};

//�{���̖��O
enum BOM_NAME
{
	NORMALBOM,
	PLUSBOM,
	PARALYZEBOM
};	

//�G�t�F�N�g�̎��
enum EFFECT
{
	EXP,
	FIRE
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
normalBom::normalBom(void)
{
	//���f���ǂݍ���
	model = new Model;
	model->Load(graphics->get3Ddevice(),"Models\\Bom.fbx");

	//�I�[�f�B�I
	audio = new Audio;

	//�I�[�f�B�I�ǂݍ���
	audio->initialize("Sounds\\Wave Bank.xwb", "Sounds\\Sound Bank.xsb");
	
	initializeMain();
}

//=============================================================================
//������
//=============================================================================
void normalBom::initializeMain(void)
{
	//�G�t�F�N�g�ǂݍ���
	fire = new effect;
	fire->Load(graphics, "pictures\\hibana.png", 3, 3);

	//�G�t�F�N�g�ǂݍ���
	exp = new effect;
	exp->Load(graphics, "pictures\\exp.png", 6, 4);

	//�{���̏�ԏ�����
	for(int i = 0; i <= CHARANUM; i++)
	{
		putCharaNum[i].bomRenderState = DONAOTHING;
	}

	initList();

	count = 0;
	bomType = BOM_NAME::NORMALBOM;

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
void normalBom::initialize(void)
{
	SAFE_DELETE(fire);
	SAFE_DELETE(exp);
	initializeMain();
}

//=============================================================================
//�f�X�g���N�^
//=============================================================================
normalBom::~normalBom(void)
{
	SAFE_DELETE(model);
	SAFE_DELETE(fire);
	SAFE_DELETE(exp);
	SAFE_DELETE(audio);
}

//=============================================================================
//�e���X�g�̏�Ԃ�������
//�����F�Ȃ�
//�߂�l�F�Ȃ�
//=============================================================================
void normalBom::initList()
{
	if(!explosionProcessList.empty()) {
		explosionProcessList.clear();
	}

	for(int i = 0; i <= CHARANUM; i++)
	{			
		putCharaNum[i].bomPos			= D3DXVECTOR3( 0, 0, 0 );
		putCharaNum[i].bomRenderState	= DONAOTHING;
		putCharaNum[i].count			= 0;
		putCharaNum[i].cost				= 0;
		putCharaNum[i].throwPos			= D3DXVECTOR3( 0, 0, 0 );
		putCharaNum[i].isThrowFlag		= false;
		putCharaNum[i].theta			= 0;
		putCharaNum[i].speed			= D3DXVECTOR3( 0, 0, 0 );
		putCharaNum[i].addRange			= 0;
	}
}

//=============================================================================
//�V�[���̃��j�b�g���擾
//�����F�V�[���̃��j�b�g
//�߂�l�F�Ȃ�
//=============================================================================
//ScceneBase����Ăяo���ꂽUnitBase��collisions����^�[�Q�b�g�擾
//typeid...typeid(�^���܂��͌^�^�̕ϐ����j...���^�ō��ꂽ��
void normalBom::collisions(UnitBase* target)
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
void normalBom::update()
{	
	bomType = BOM_NAME::NORMALBOM;
	//�G�t�F�N�g�X�V
	fire->fixPosUpdate(UPDATEANIM);
	exp->fixPosUpdate(UPDATEANIM);
	//�{���̏����̐i�߂�
	renderFlow();
	explosionFlow();
}

//=============================================================================
//�{����ǉ�
//�����F�{���̈ʒu�A�ړ����A�L�����N�^�[���A/*�����鏈���Ȃ�*/�������A�����邩�ǂ���
//�߂�l�F�Ȃ�
//=============================================================================
void normalBom::addBom(D3DXVECTOR3 pos/*�{���̈ʒu�x�N�g��*/, D3DXVECTOR3 move/*�ړ��x�N�g��*/,int addExpRabge/*�����͈͉��Z��*/, int charaNum /*�L�����N�^�[��*/, D3DXVECTOR3 bomThrowPos/*�������̍��W*/, bool throwFlag/*�{���𓊂��邩�H*/)
{	
	putCharaNum[charaNum].bomPos			= pos;
	putCharaNum[charaNum].bomRenderState	= RENDER;
	putCharaNum[charaNum].count				= 1;
	putCharaNum[charaNum].cost				= OBSTACLELCOST;
	putCharaNum[charaNum].throwPos		    = D3DXVECTOR3((pos.x-move.x)+(fabs(bomThrowPos.x)*move.x), -0.5f, (pos.z-move.z)+(fabs(bomThrowPos.z)*move.z));
	putCharaNum[charaNum].isThrowFlag		= throwFlag;
	putCharaNum[charaNum].addRange			= addExpRabge;

	if(putCharaNum[charaNum].isThrowFlag) {
		//������L�����N�^�[�Ɠ������ʒu����
		putCharaNum[charaNum].bomPos		= D3DXVECTOR3(pos.x-move.x, -0.5f, pos.z-move.z);
		//�p���擾
		putCharaNum[charaNum].theta			= getAngle(bomThrowPos, BEGIN_SPEED);
		//���x���v�Z
		putCharaNum[charaNum].speed			= D3DXVECTOR3( BEGIN_SPEED * cosf( putCharaNum[charaNum].theta )/* * cosf( -putCharaNum[charaNum].theta )*/,
												BEGIN_SPEED * sinf( putCharaNum[charaNum].theta ),
												BEGIN_SPEED * cosf( putCharaNum[charaNum].theta )/* * sinf( putCharaNum[charaNum].theta ) */);

		putCharaNum[charaNum].speed.x *= move.x;
		putCharaNum[charaNum].speed.z *= move.z;
	}
	if(!putCharaNum[charaNum].isThrowFlag) { 
		//�����͈͂ɃR�X�g�ݒ�
		setExpRange(true, charaNum);
	}
}

//=============================================================================
//�{����`��̗���
//�����F�Ȃ�
//�߂�l�F�Ȃ�
//=============================================================================
void normalBom::renderFlow()
{
	//�L�����N�^�[�̐������[�v
	for(int i = 0; i <= CHARANUM; i++)
	{
		//�`��̃J�E���g���n�܂��Ă��Ȃ����
		if(putCharaNum[i].count < 1)
			continue;

		//�����鏈���Ȃ��
		if(putCharaNum[i].isThrowFlag)
		{
			//�����鏈��
			throwProcess(i);
		} else /*�����鏈���łȂ����*/ {
			//�J�E���g���Z
			putCharaNum[i].count++;
			//�G�t�F�N�g��ǉ��������^�C�~���O
			if(putCharaNum[i].count % 9 == 0 
				&& putCharaNum[i].count < RENDERTIME)
			{
				addEffect(putCharaNum[i].bomPos, FIRE, i);
			}
		}
	}
}

//=============================================================================
//�{����`��
//�����F�t���[���A�J�����ԍ�
//�߂�l�F�Ȃ�
//=============================================================================
void normalBom::render(float frameTime, int cameraNum)
{	
	//�L�����N�^�[�̐������[�v
	for(int i = 0; i <= CHARANUM; i++)
	{
		//�`��̃J�E���g���n�܂��Ă��Ȃ����
		if(putCharaNum[i].count < 1)
			continue;

		if(putCharaNum[i].count == RENDERTIME) {
			//�G�t�F�N�g�ǉ�
			addEffect(putCharaNum[i].bomPos, EXP, i);

		} else if(putCharaNum[i].count >= RENDERTIME+ADDTIME) {	
			//�J������h�炷
			callCameraShake(putCharaNum[i].bomPos);
			//�����������ɂ���
			putCharaNum[i].bomRenderState = ACTIONEXPSlON;
			initRender(i);
		}

		D3DXMATRIX mat;
		//�ړ��s��
		D3DXMatrixTranslation(&mat,putCharaNum[i].bomPos.x,putCharaNum[i].bomPos.y,putCharaNum[i].bomPos.z);

		if(cameraNum == 1 || modeChange::GetInstance() -> getMode() == VSCPU/*�ΐ탂�[�h��CPU�ΐ�*/) {
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
	}

	//�G�t�F�N�g�`��
	fire->fixPosDraw(cameraNum);
	exp->fixPosDraw(cameraNum);
}

//=============================================================================
//�G�t�F�N�g�ǉ�
//�����F�ʒu, �G�t�F�N�g�̎��, �L�����N�^�[�ԍ�
//�߂�l�F�Ȃ�
//=============================================================================
void normalBom::addEffect(D3DXVECTOR3 pos, int effectType, int chara)
{
	pos.y+=1.5f; 
	int add = putCharaNum[chara].addRange;

	//�G�t�F�N�g�ǉ�
	switch (effectType) {	
	case EFFECT::EXP:
		if(bomType == NORMALBOM || bomType == PLUSBOM) {
			pos.y += 1.0f + ((float)add * 0.5f);
			exp->fixPosAdd(pos,5.0f + add);
		} else if(bomType == PARALYZEBOM) {
			pos.y += 4.0f;
			exp->fixPosAdd(pos,11);
		}
		break;
	case EFFECT::FIRE:	
		fire->fixPosAdd(pos,(float)((rand() % PICSIZE)+1));
		break;
	}
}

//=============================================================================
//�����鏈��
//�����F�L�����N�^�[�ԍ�
//�߂�l�F�Ȃ�
//=============================================================================
void normalBom::throwProcess(int putChara)
{
	putCharaNum[putChara].bomPos += putCharaNum[putChara].speed;
	putCharaNum[putChara].speed.y -= gravity;

	//�{���̂��������n�ʈȉ���
	if(putCharaNum[putChara].bomPos.y < GROUNDHEIGHT)
	{	
		D3DXVECTOR3 bomPos = putCharaNum[putChara].bomPos;
		//�ʒu�𐮐��ɂ���y�����ɒn�ʂ̍�������
		putCharaNum[putChara].bomPos = D3DXVECTOR3(toIntPos(bomPos.x), -0.5f, toIntPos(bomPos.z));
		//�����邩�ǂ����̃t���O��false��
		putCharaNum[putChara].isThrowFlag = false;
		//�����͈͂ɃR�X�g�ݒ�
		setExpRange(true, putChara);
		//�G�t�F�N�g�ǉ�
		addEffect(putCharaNum[putChara].bomPos, FIRE, putChara);
	}
}

//=============================================================================
//�\����񏉊���
//�����F�L�����N�^�[�ԍ�
//�߂�l�F�Ȃ�
//=============================================================================
void normalBom::initRender(int putChara)
{
	//�J�E���g������
	putCharaNum[putChara].count = 0;
	putCharaNum[putChara].cost	= 0;
	//�{���������̃L�����N�^�[���X�g
	explosionProcessList.push_back(putChara);
}

//=============================================================================
//���������̗���
//�����F�Ȃ�
//�߂�l�F�Ȃ�
//=============================================================================
void normalBom::explosionFlow()
{
	//�ʏ�{���̏���
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
void normalBom::explosion(int processCharaNum/*����������L�����N�^�[�ԍ�*/)
{	
	int blockX;
	int blockZ;
	int add = putCharaNum[processCharaNum].addRange;

	//�����͈͕����[�v
	for(int hight = -(RANGE + add); hight <= (RANGE + add); hight++)
	{	
		for(int width = -(RANGE + add); width <= (RANGE + add); width++) 
		{	
			//�ʏ�{���̏���
			//�n���ꂽ�L�����N�^�[�ԍ��̃{���̕`�揈�����������Ă����
			if(putCharaNum[processCharaNum].bomRenderState == ACTIONEXPSlON) {
				//�{���̈ʒu�Ɣ��蕪�����Z
				blockX = (int)(putCharaNum[processCharaNum].bomPos.x+width);
				blockZ = (int)(putCharaNum[processCharaNum].bomPos.z+hight);
			}

			if(blockX < MINRANGE || blockX > MAXRANGE
				|| blockZ < MINRANGE || blockZ > MAXRANGE)
				continue;

			//�͈͓��ɕǂ����݂��邩����
			if(wallUnit -> retWallArr((float)blockX, (float)blockZ) == PUTWALL)
			{
				//�����0�������ăE�H�[���N���X�ŕ`�悳��Ȃ��悤�ɂ���
				wallUnit -> wallArr[blockX][blockZ] = NOTWALL;
			} else {
				//�͈͓��ɃL�����N�^�[�����݂��邩����
				characterInRange(blockX, blockZ);
			}
		}
	}	
}

//=============================================================================
//�L�����N�^�[���͈͓������菈��
//�����F�{���̔����͈͂̈ʒu
//�߂�l�F�Ȃ�
//=============================================================================
void normalBom::characterInRange(int x, int z)
{
	D3DXVECTOR3 explosionRange((float)x, 0, (float)z);

	//CPU�ΐ�
	if(modeChange::GetInstance() -> getMode() == VSCPU){
		//�eCPU�L�����N�^�[�̏��i�[
		NormalCpu* cpuChara[] = { cpuUnit[PYECPU],
					cpuUnit[SPEEDCPU],
					cpuUnit[NORMALCPU] };
		//CPU�L���������[�v
		for(int i = 0; i < CHARANUM; i++)
		{
			//�����͈͓��ɃL�����N�^�[�������
			if(explosionRange == D3DXVECTOR3(toIntPos(cpuChara[i]->pos.x), 0, toIntPos(cpuChara[i]->pos.z)))
			{
				//�L�����N�^-�̎��S�t���O��true��
				cpuChara[i]->isLose = true;
			}
		}

		//�����͈͓��Ƀv���C���[�������
		if(explosionRange == D3DXVECTOR3(toIntPos(vsCpuPlayerUnit ->pos.x), 0, toIntPos(vsCpuPlayerUnit ->pos.z)))
		{
			//�v���C��-�̎��S�t���O��true��
			vsCpuPlayerUnit ->isLose = true;
		}
	}

	//2P�ΐ�
	if(modeChange::GetInstance() -> getMode() == VS2P)
	{
		//�����͈͓��Ƀv���C���[1�������
		if(explosionRange == D3DXVECTOR3(player1->pos.x, 0, player1->pos.z))
		{
			//�v���C��-1�̎��S�t���O��true��
			player1->isLose = true;
		}

		//�����͈͓��Ƀv���C���[2�������
		if(explosionRange == D3DXVECTOR3(player2->pos.x, 0, player2->pos.z))
		{
			//�v���C��-2�̎��S�t���O��true��
			player2->isLose = true;
		}
	}
}

//=============================================================================
//�{�������݂��邩��Ԃ�
//�����F�{�������݂��邩��m�肽�����W
//�߂�l�F�Ȃ�
//=============================================================================
int normalBom::retIsBom(float x, float z)
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
void normalBom::setExpRange(bool isSet, int chara)
{
	count++;
	int add = putCharaNum[chara].addRange;

	//�����͈͕����[�v
	for(int hight = -(RANGE + add); hight <= (RANGE + add); hight++)
	{	
		for(int width = -(RANGE + add); width <= (RANGE + add); width++) 
		{	
			int blockX = (int)(putCharaNum[chara].bomPos.x+width);
			int blockZ = (int)(putCharaNum[chara].bomPos.z+hight);

			if(blockX < MINRANGE || blockX > MAXRANGE
				|| blockZ < MINRANGE || blockZ > MAXRANGE)
				continue;

			if(isSet) {
					wallUnit  -> wallData[blockX][blockZ].isExpRange = true;
					wallUnit  -> wallData[blockX][blockZ].expRangeNum = count;
			} else {
				if(wallUnit  -> wallData[blockX][blockZ].isExpRange) {
					wallUnit -> wallData[blockX][blockZ].isExpRange = false;
				}
			}
		}
	}
}

//=============================================================================
//�������֓��B���邽�߂̊p�x�v�Z
//�����F��������̈ʒu�A����
//�߂�l�F�Ȃ�
//=============================================================================
float normalBom::getAngle(D3DXVECTOR3 pos, float speed)
{
	float distance = sqrtf(pos.x * pos.x + pos.z * pos.z);	//�W�I�܂ł̋���
	float sin;

	//�A�[�N�T�C���̒��g�̌v�Z
	sin = distance/*����*/ * gravity/*�d��*/ / (speed/*����*/ * speed);

	//�͂��Ȃ��ꍇ�̏���
	//sin�֐��͏��1�̒l�����Ƃ�Ȃ�(�����ύX�ŋ����ω�)
	if( sin > 1.0f ) {
		sin = 1.0f;
	}

	//�p�x��Ԃ�
	//�~��������������ƂŁA�傫���ق��̊p�x��Ԃ����߁A���Ȑ��I�ȕ�������`��
	return 0.5f * /*( PI -*/ asinf( sin )/* )*/;
}

//=============================================================================
//�J������h�炷�������Ă�
//�����F�{���̈ʒu
//�߂�l�F�Ȃ�
//=============================================================================
void normalBom::callCameraShake(D3DXVECTOR3 pos)
{
	float shakeSize = 1.0f;

	//�J������h�炷
	//2P�ΐ�
	if(modeChange::GetInstance() -> getMode() == VS2P)
	{
		player1->setShakeCameraInfo(pos, shakeSize);
		player2->setShakeCameraInfo(pos, shakeSize);
	} else {
		vsCpuPlayerUnit ->setShakeCameraInfo(pos, shakeSize);
	}
}