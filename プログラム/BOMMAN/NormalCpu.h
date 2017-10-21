//=============================================================================
//  NormalCpu
//�@�X�V���F2016/10/30
//	����ҁF���X�ؗ��M
//=============================================================================
#pragma once
#include "Cpu.h"
#include "constants.h"
#include <list>
#include <vector>
using namespace std;

class vsCpuPlayer;
class normalBom;
class Wall;
class Model;
class readFile;

#define	COMMAND_NUM 3	//�R�}���h��
#define POINT 9			//�o�߃|�C���g��

class NormalCpu :
	public Cpu
{
public:
	D3DXVECTOR3  pos;	//�ʒu
	bool		isLose; //���S�������ǂ����̔���t���O
	list<int>		 plusBomNumList;		//�v���X�{���̌����i�[����z��
	list<int>		 paralyzeBomNumList;	//Ⴢ�{���̌����i�[����z��
	list<int>		 bomThrowNumList;		//�X���[�{���̌����i�[����z��
	bool            isStop;					//������~���邩�ǂ���

protected:
	struct aStar
	{
		D3DXVECTOR3	tilePos;							//�ʒu
		int			cost;								//�R�X�g
		int			heuristic;							//�q���[���X�e�B�b�N
		int			score;								//�X�R�A
		aStar*		parentTile;							//�e�^�C���̃|�C���^
	};

	struct prevTileInfo
	{
		D3DXVECTOR3 prevTilePos;
		int			prevTileDir;
	};

	list<aStar>		 openList;							//�I�[�v�����X�g
	list<aStar>		 closeList;							//�N���[�Y���X�g	
	list<aStar>::iterator openListitr;					//�C�e���[�^�쐬
	list<aStar>::iterator closeListitr;
	list<D3DXVECTOR3> parentList;						//�e�^�C�����X�g
	Model*			 model;								//�N���X�^�̃I�u�W�F�N�g��������Ƃ��|�C���^�ɂ���Ƃ����������idelete��Y��Ȃ��j	
	float			 angle;								//�p�x					
	D3DXVECTOR3		 startPos;							//�X�^�[�g�ʒu
	D3DXVECTOR3		 goalPos;							//�S�[���ʒu
	int				 isControl;							//���݂�AI�̏��
	list<int>::iterator	plusBomitr;						//�C�e���[�^�쐬
	list<int>::iterator	paralyzeitr;					
	list<int>::iterator	bomThrowitr;					
	bool			isItem;								//�A�C�e���{����I���������ǂ����𔻒�
	int				itemType;							//�A�C�e���̎��
	int				processBomType;						//�������̃{��
	D3DXVECTOR3		putBomPos;							//�{����u�����ꏊ
	D3DXVECTOR3		prevPos;							//��r����ȑO�̈ʒu
	bool			charaSearch;						//�L�����N�^�[���^�[�Q�b�g�ɂ���
	int				charaNum;							//�L�����N�^�[�ԍ����i�[����ϐ�
	aStar			pushData;							//���X�g�Ƀv�b�V�����邽�߂̏����i�[���邽�߂̕ϐ�
	int				frameCount;							//�o�߃t���[�����J�E���g
	float			moveSpeed;							//�ړ����x
	int				stopCount;							//��~���Ԃ��J�E���g
	float			bomPutAngle;						//�{����u�����߂̃A���O��
	bool			throwFlag;							//�����鏈�����H
	D3DXVECTOR3		bomThrowPos;						//�������̈ʒu
	int				loopNum;							//���[�v��
	int				toTargetCost;						//�^�[�Q�b�g�܂ł̋����R�X�g
	D3DXVECTOR3		target;								//�S�[���ƂȂ�^�[�Q�b�g�̈ʒu
	bool			isTarget;							//�^�[�Q�b�g�̌�����
	int				prevWallCount;						//�^�[�Q�b�g���̕ǂ̎���̕ǐ�
	float			wallTargetMul;						//�ǃ^�[�Q�b�g�̃R�X�g�ɂ����鐔
	float			charaTargetMul;						//�L�����N�^�[�^�[�Q�b�g�̃R�X�g�ɂ����鐔
	D3DXVECTOR3		prevTargetPos;						//�O��^�[�Q�b�g�ɂ����ʒu
	bool			isOpenListEmpty;					//�I�[�v�����X�g����ɂȂ������@
	vector<prevTileInfo> prevPosList;					//�ʂ����o�H���i�[���郊�X�g
	int				prevTileState;						//�O��ʂ����^�C���ɑ΂��鏈���̏��
	int				prevDirIndex;						//�O��̕����������v�f�ԍ�
	D3DXVECTOR3		prevTile;							//�O��̈ʒu
	int				addExpRange;						//�����͈͂̉��Z��
	bool			isExitExpMove;						//�����͈͂������
	int				moveDir;							//���ݐi�s���Ă������
	int				prevMoveDir;						//�O��i�s��������
	readFile*		searchTargetRangeXData;				//�^�[�Q�b�g��{������͈�
	readFile*		searchTargetRangeZData;
	readFile*		targetDistCostData;					//�^�[�Q�b�g�Ƃ̋����R�X�g
	int				searchTargetRangeXTable[112];		//�t�@�C���̏����i�[����z��
	int				searchTargetRangeZTable[112];
	int				targetDistCostTable[112];
	normalBom*		bomUnit[3];							//�e��{���I�u�W�F�N�g�̏����i�[����z��		
	Wall*			wallUnit;							//�E�H�[�����j�b�g�̏����i�[����ϐ�
	vsCpuPlayer*	vsCpuPlayerUnit;					//��CPU�v���C���[���j�b�g�̏����i�[����ϐ�
	NormalCpu*		cpuUnit[3];							//CPU���j�b�g�̏����i�[����ϐ�

public:
	NormalCpu(void);

protected:
	~NormalCpu(void);	
	void initialize();
	void render(float frameTime, int cameraNum);		//�`�揈��
	void update();
	void collisions(UnitBase* target);					//���j�b�g�̏����擾
	void ai();											//AI����		
	void control();										//�ړ�
	void getItem();										//�A�C�e���擾
	void stopAi();										//AI�̏������~
	void adjustmentPos();								//�ʒu�𐮐��ɂ���							
	bool collObstacel(D3DXVECTOR3 conpare);				//��Q���ւ̏Փˏ���
	float toIntMove(float move) {return (move<0) ? (float)(-1.0 * floor(fabs(move) + 0.5)) : (float)(floor(move + 0.5));}	//Move�̒����𐮐��ɂ���
	float toIntPos(float pos) {return (pos<0) ? (float)(-1.0 * floor(fabs(pos) + 0.5)) : (float)(floor(pos + 0.5));}		//�ʒu�𐮐��ɂ���
	void initList();									//�e���X�g��������
	bool isThrowTarget();								//�������̈ʒu���擾
	bool searchPutPos(int dir, D3DXVECTOR3 sumDirection[], D3DXVECTOR3 conparePosList[]);	//�{����u����ʒu���t�Z���ĒT��
	void notRenderBom();								//�`�撆�̃{�������邩
	void searchTarget(float posX,float posY, int cost);	//�^�[�Q�b�g��T��
	void searchWallTarget(float posX,float posZ);		//���������̕ǂ̒��Ŏ��͂W�}�X�ɕǂ��������̂��^�[�Q�b�g�ɂ���
	void bomProcess();									//�{���ɑ΂��鏈��
	void getPath();										//�ړ��o�H���擾
	D3DXVECTOR3 retTarget(D3DXVECTOR3 pos);				//�S�[����Ԃ�
	void putBom();										//�{����u��
	void initAstar();									//AI���Ԃ������������
	bool isObstacel(D3DXVECTOR3 conpare);				//��Q�������݂��邩		
	bool situationCheck();								//�L�����N�^�[�̎��͂̏󋵂�����
	bool inExpRange(D3DXVECTOR3 conpare);				//�L�����N�^�[�������͈͓����𔻒肷�ăR�X�g���ɂ��邩�̐���𔻒�
	void exitExpRange();								//�����͈͂��痣��邽�߂̒T��
	void exitExpProcess(D3DXVECTOR3 dir, int dirIndex);	//�����͈͂��痣��邽�߂̏���
	bool equalPrevPosState(D3DXVECTOR3 conpare);		//��r�ʒu���O��ʂ������Ɠ�������
	void plusExpRange() { addExpRange++; }				//�����͈͂̉��Z
	D3DXVECTOR3 calcMove(float angle);					//�i�s�����x�N�g�������߂�
	D3DXVECTOR3 retTargetOfCharacterPos();				//�S�[���ʒu�Ƃ��ăL�����N�^�[�̈ʒu��Ԃ�
	float truncationSecondDecimalPlace(float pos);		//�����_���ȉ��؂�̂�
	bool isPutBom(D3DXVECTOR3 bomPutPos);				//���݂̈ʒu����{����u�����ꍇ�A�����������邩����
};

