//=============================================================================
//  �v���X�{��
//�@�X�V���F2016/10/30
//	����ҁF���X�ؗ��M
//=============================================================================
#include "plusBom.h"
#include "modeChange.h"
#include "Camera.h"
#include "vsCpuPlayer.h"
#include "PsyCpu.h"
#include "SpeedCpu.h"
#include "NormalCpu.h"
#include "audio.h"
#include "Wall.h"
#include "effect.h"
#include "Model.h"
#include "VsPlayer.h"

#define RENDERTIME 90			//�`�掞��
#define PUTWALL 1				//�ǗL��
#define NOTWALL 0				//�ǂȂ�
#define CHARANUM 3				//�L�����N�^�[��
#define OBSTACLELCOST 100		//�Ǔ��̏�Q����CPU�R�X�g
#define NONCOST 0				//�R�X�g�Ȃ�
#define ITEMCOST -3				//�A�C�e���̃R�X�g
#define GROUNDHEIGHT -0.5f		//�n�ʂ̍���
#define BEGIN_SPEED 0.3f		//�C�e����
#define CAMERA1 1				//�J�����ԍ�
#define PICSIZE 1				//�e�N�X�`���T�C�Y
#define UPDATEANIM 1.0f			//�A�j���[�V�����X�V
#define SCALE 5					//�傫��
#define RANGE 2					//�͈�
#define MAXRANGE 49				//�ő�͈�
#define MINRANGE 1				//�Œ�͈�
#define UPDATEANIM 1.0f			//�A�j���[�V�����X�V
#define ADDTIME 5				//���Ԓǉ���
const float gravity = 0.002f;

//�{���̎��
enum BOM_NAME
{
	NORMAL,
	PLUS,
	PARALYZE
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
plusBom::plusBom(void)
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
void plusBom::initializeMain(void)
{
	//�G�t�F�N�g�ǂݍ���
	fire = new effect;
	fire->Load(graphics, "pictures\\hibana.png", 3, 3);

	//�G�t�F�N�g�ǂݍ���
	exp = new effect;
	exp->Load(graphics, "pictures\\exp.png", 6, 4);

	initList();

	count = 0;
	bomType = BOM_NAME::PLUS;

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
void plusBom::initialize(void)
{
	SAFE_DELETE(fire);
	SAFE_DELETE(exp);
	initializeMain();
}

//=============================================================================
//�f�X�g���N�^
//=============================================================================
plusBom::~plusBom(void)
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
void plusBom::collisions(UnitBase* target)
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
void plusBom::update()
{	
	bomType = BOM_NAME::PLUS;
	//�G�t�F�N�g�X�V
	fire->fixPosUpdate(UPDATEANIM);
	exp->fixPosUpdate(UPDATEANIM);
	//�{���̏����̐i�߂�
	renderFlow();
	explosionFlow();
}

//=============================================================================
//�e���X�g�̏�Ԃ�������
//�����F�Ȃ�
//�߂�l�F�Ȃ�
//=============================================================================
void plusBom::initList()
{
	if(!explosionProcessList.empty()) {
		explosionProcessList.clear();
	}

	for(int i = 0; i <= CHARANUM; i++)
	{
		if(!putCharaNum[i].dataList.empty())
		{
			putCharaNum[i].dataList.clear();
		}				
	}
}

//=============================================================================
//�v���X�{���̏����擾
//�����F�{���̈ʒu�A�ړ����A�L�����N�^�[���A/*�����鏈���Ȃ�*/�������A�����邩�ǂ���
//�߂�l�F�Ȃ�
//=============================================================================
void plusBom::addBom(D3DXVECTOR3 pos/*�{���̈ʒu�x�N�g��*/, D3DXVECTOR3 move/*�ړ��x�N�g��*/,int addExpRabge/*�����͈͉��Z��*/, int charaNum /*�L�����N�^�[��*/, D3DXVECTOR3 bomThrowPos/*�������̍��W*/, bool throwFlag/*�{���𓊂��邩�H*/)
{	
	plusBomData pushData;

	{
		pushData.bomPos					= pos;		
		pushData.isBomRenderComp		= false;																						
		pushData.count					= 1;																							
		pushData.cost					= OBSTACLELCOST;
		pushData.throwPos				= D3DXVECTOR3((pos.x-move.x)+(fabs(bomThrowPos.x)*move.x), -0.5f, (pos.z-move.z)+(fabs(bomThrowPos.z)*move.z));
		pushData.isThrowFlag			= throwFlag;
		pushData.addRange				= addExpRabge;

		if(throwFlag) {
			//������L�����N�^�[�Ɠ������ʒu����
			pushData.bomPos			= D3DXVECTOR3(pos.x-move.x, -0.5f, pos.z-move.z);
			//�p���擾
			theta					= getAngle(bomThrowPos, BEGIN_SPEED);
			//���x���v�Z
			pushData.speed			= D3DXVECTOR3( BEGIN_SPEED * cosf( theta ) /** cosf( -putCharaNum[CHARANUM].theta )*/,
													BEGIN_SPEED * sinf( theta ),
													BEGIN_SPEED * cosf( theta ) /** sinf( putCharaNum[CHARANUM].theta )*/ );
			pushData.speed.x *= move.x;
			pushData.speed.z *= move.z;
		}	
	}
	//�{���f�[�^���
	putCharaNum[charaNum].dataList.push_back(pushData);

	if(!throwFlag) { 
		//�����͈͂ɃR�X�g�ݒ�
		setExpRange(true, charaNum);
	}
}

//=============================================================================
//�{����`��̗���
//�����F�Ȃ�
//�߂�l�F�Ȃ�
//=============================================================================
void plusBom::renderFlow()
{	
	//�L�����N�^�[�����[�v
	for(int i = 0; i <= CHARANUM; i++)
	{
		//�����v���X�{�������݂��Ă��Ȃ����
		if(putCharaNum[i].dataList.empty())
			continue;

		putCharaNum[i].dataiter = putCharaNum[i].dataList.begin();

		while(putCharaNum[i].dataiter != putCharaNum[i].dataList.end())
		{	
			//�����鏈���Ȃ��
			if(putCharaNum[i].dataiter -> isThrowFlag)
			{
				throwProcess(i);
			} else /*�����鏈���łȂ����*/ {		
				//�J�E���g���Z
				putCharaNum[i].dataiter -> count++;
				//�G�t�F�N�g��`�悵�����^�C�~���O
				if(putCharaNum[i].dataiter -> count % 9 == 0 
					&& putCharaNum[i].dataiter -> count < RENDERTIME)
				{
					//�G�t�F�N�g�ǉ�
					addEffect(putCharaNum[i].dataiter->bomPos, FIRE, i);
				}
			}
			putCharaNum[i].dataiter++;
		}	
		
		putCharaNum[i].dataiter = putCharaNum[i].dataList.begin();	
	}
}

//=============================================================================
//�{����`��
//�����F�t���[���A�J�����ԍ�
//�߂�l�F�Ȃ�
//=============================================================================
void plusBom::render(float frameTime, int cameraNum)
{	
	//�L�����N�^�[�����[�v
	for(int i = 0; i <= CHARANUM; i++)
	{
		//�����v���X�{�������݂��Ă��Ȃ����
		if(putCharaNum[i].dataList.empty())
			continue;

		putCharaNum[i].dataiter = putCharaNum[i].dataList.begin();

		while(putCharaNum[i].dataiter != putCharaNum[i].dataList.end())
		{	
			if(putCharaNum[i].dataiter -> count == RENDERTIME) {
				//�G�t�F�N�g�ǉ�
				addEffect(putCharaNum[i].dataiter->bomPos, EXP, i);

			} else if(putCharaNum[i].dataiter -> count >= RENDERTIME+ADDTIME) {
				//�J������h�炷
				callCameraShake(putCharaNum[i].dataiter->bomPos);
				initRender(i);
			}

			D3DXMATRIX mat;
			//�ړ��s��
			D3DXMatrixTranslation(&mat,putCharaNum[i].dataiter -> bomPos.x, putCharaNum[i].dataiter -> bomPos.y, putCharaNum[i].dataiter -> bomPos.z);

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

			putCharaNum[i].dataiter++;
		}
	
		putCharaNum[i].dataiter = putCharaNum[i].dataList.begin();	
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
void plusBom::addEffect(D3DXVECTOR3 pos, int effectType, int chara)
{
	pos.y+=1.5f; 
	int add = putCharaNum[chara].dataiter -> addRange;

	//�G�t�F�N�g�ǉ�
	switch (effectType) {	
	case EFFECT::EXP:
		if(bomType == NORMAL || bomType == PLUS) {
			pos.y += 1.0f + ((float)add * 0.5f);
			exp->fixPosAdd(pos,5.0f + add);
		} else if(bomType == PARALYZE) {
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
void plusBom::throwProcess(int putChara)
{
	putCharaNum[putChara].dataiter -> bomPos += putCharaNum[putChara].dataiter -> speed;
	putCharaNum[putChara].dataiter -> speed.y -= gravity;

	//�{���̂��������n�ʈȉ���
	if(putCharaNum[putChara].dataiter -> bomPos.y < GROUNDHEIGHT)
	{	
		D3DXVECTOR3 bomPos = putCharaNum[putChara].dataiter -> bomPos;
		//�ʒu�𐮐��ɂ���y�����ɒn�ʂ̍�������
		putCharaNum[putChara].bomPos = D3DXVECTOR3(toIntPos(bomPos.x), -0.5f, toIntPos(bomPos.z));
		//�����邩�ǂ����̃t���O��false��
		putCharaNum[putChara].dataiter -> isThrowFlag = false;
		//�����͈͂ɃR�X�g�ݒ�
		setExpRange(true, putChara);
		//�G�t�F�N�g�ǉ�
		addEffect(putCharaNum[putChara].dataiter->bomPos, FIRE, putChara);
	}
}

//=============================================================================
//�\����񏉊���
//�����F�L�����N�^�[�ԍ�
//�߂�l�F�Ȃ�
//=============================================================================
void plusBom::initRender(int putChara)
{
	//�{���̕`�悪�I���������H
	putCharaNum[putChara].dataiter -> isBomRenderComp = true;
	//�J�E���g������
	putCharaNum[putChara].dataiter -> count = 0;
	putCharaNum[putChara].dataiter -> cost	 = 0;
}

//=============================================================================
//���������̗���
//�����F�Ȃ�
//�߂�l�F�Ȃ�
//=============================================================================
void plusBom::explosionFlow()
{
	//�v���X�{���̏���
	//�L�����N�^�[�����[�v
	for(int i = 0; i <= CHARANUM; i++)
	{	
		//�{�����u����Ă����
		if(!(putCharaNum[i].dataList.empty()))
		{
			putCharaNum[i].dataiter = putCharaNum[i].dataList.begin();
			//�`�揈�����I�����Ă����
			if(putCharaNum[i].dataiter->isBomRenderComp)
			{
				//BGM�Đ�
				audio->playCue("������1");
				explosion(i);
				//�����͈͂ɃR�X�g�ݒ�
				setExpRange(false, i);
			} 
		}
	}
}

//=============================================================================
//�{���̔�������
//�����F�ǂ̔z�u���A�L�����N�^�[�ԍ�
//�߂�l�F�Ȃ�
//=============================================================================
void plusBom::explosion(int processCharaNum/*����������L�����N�^�[�ԍ�*/)
{	
	int blockX;
	int blockZ;
	int add = putCharaNum[processCharaNum].dataiter->addRange;

	//�����͈͕����[�v
	for(int hight = -(RANGE + add); hight <= (RANGE + add); hight++)
	{	
		for(int width = -(RANGE + add); width <= (RANGE + add); width++) 
		{	
			//�v���X�{���̏���
			//�n���ꂽ�L�����N�^�[�ԍ��̃��X�g����łȂ����
			if(!(putCharaNum[processCharaNum].dataList.empty()))
			{
				//�n���ꂽ�L�����N�^�[�ԍ��̃{���̕`�揈�����������Ă����
				if(putCharaNum[processCharaNum].dataiter->isBomRenderComp)
				{
					//�{���̈ʒu�Ɣ��蕪�����Z
					blockX = (int)(putCharaNum[processCharaNum].dataiter->bomPos.x+width);
					blockZ = (int)(putCharaNum[processCharaNum].dataiter->bomPos.z+hight);
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
	
	//���������{�������X�g����폜
	//���X�g����ł͂Ȃ�
	if(!(putCharaNum[processCharaNum].dataList.empty()))
	{
		//�n���ꂽ�L�����N�^�[�ԍ��̃{���̕`�揈�����������Ă����
		if(putCharaNum[processCharaNum].dataiter->isBomRenderComp)
		{
			//�v�f�폜
			putCharaNum[processCharaNum].dataiter = putCharaNum[processCharaNum].dataList.erase(putCharaNum[processCharaNum].dataiter);
		}
	}
}

//=============================================================================
//�{�������݂��邩��Ԃ�
//�����F�{�������݂��邩��m�肽�����W
//�߂�l�F���݂��邩
//=============================================================================
int plusBom::retIsBom(float x, float z)
{
	//�v���X�{���̈ʒu
	//�L�����N�^�[�����[�v
	for(int i = 0; i < 4; i++)
	{
		putCharaNum[i].compareitr = putCharaNum[i].dataList.begin();
		//�������Ă���{�������[�v
		while(putCharaNum[i].compareitr != putCharaNum[i].dataList.end())
		{
			//���߂�ʒu�Ƀ{�������݂����
			if(toIntPos(putCharaNum[i].compareitr->bomPos.x) == toIntPos(x) && toIntPos(putCharaNum[i].compareitr->bomPos.z) == toIntPos(z))
			{
				return putCharaNum[i].dataiter->cost;
			}
			putCharaNum[i].compareitr++;
		}
	}

	return 0;
}

//=============================================================================
//�����͈͂̃R�X�g��ݒ�
//�����F�R�X�g��ݒ肷�邩�ǂ����A�L�����N�^�[�ԍ�
//�߂�l�F�Ȃ�
//=============================================================================
void plusBom::setExpRange(bool isSet, int chara)
{
	count++;
	putCharaNum[chara].compareitr = putCharaNum[chara].dataList.begin();
	putCharaNum[chara].dataiter = putCharaNum[chara].dataList.begin();

	//�������Ă���{�������[�v
	while(putCharaNum[chara].compareitr != putCharaNum[chara].dataList.end())
	{
		int add = putCharaNum[chara].dataiter->addRange;

		//�����͈͕����[�v
		for(int hight = -(RANGE + add); hight <= (RANGE + add); hight++)
		{	
			for(int width = -(RANGE + add); width <= (RANGE + add); width++) 
			{		
				int blockX = (int)(putCharaNum[chara].compareitr -> bomPos.x+width);
				int blockZ = (int)(putCharaNum[chara].compareitr -> bomPos.z+hight);
		
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

		putCharaNum[chara].compareitr++;
	}
}
