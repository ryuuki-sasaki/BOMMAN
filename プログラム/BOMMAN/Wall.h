//=============================================================================
//  ��
//�@�X�V���F2016/10/30
//	����ҁF���X�ؗ��M
//=============================================================================
#pragma once
#include "unitbase.h"
#include "constants.h"
#include "textureManager.h"
#include "Poly.h"

class vsCpuPlayer;
class VsPlayer;
class NormalCpu;
class readFile;

const int WALLARRAYRANGE = 51;	//�ǂ�z�u����͈�
#define OBSTACLELCOST 100	//�Ǔ��̏�Q���̃R�X�g

class Wall : public UnitBase
{
public:	
	typedef struct 
	{
		D3DXVECTOR3	pos;			//�ǂ̈ʒu
		D3DXVECTOR3	itemPos;		//�A�C�e���̈ʒu
		int			isItem;			//�A�C�e�����i�[����ϐ��i0�̏ꍇ�̓A�C�e���Ȃ�����ȊO�͑Ή�����ԍ��̃A�C�e���i�[�j
		int			cpuCost;		//cpu�̍s�������肷��R�X�g
		int         expRangeNum;	//�����͈͂̔ԍ�
		bool		isExpRange;		//�����͈͓����H
	}data;
	int				 wallArr[WALLARRAYRANGE][WALLARRAYRANGE];		//�ǂ̗L�����i�[����z��
	data			 wallData[WALLARRAYRANGE][WALLARRAYRANGE];		//�ǂ̏����i�[����z��

private:	
	TextureManager	 texLargeexplosionBom;				//�唚���{���e�N�X�`��
	Poly			 largeexplosionPoly;				//�唚���e�N�X�`����\��|���S��
	TextureManager	 texPlusBom;						//�v���X�{���e�N�X�`��
	Poly			 plusBomPoly;						//�v���X�{���e�N�X�`����\��|���S��
	TextureManager	 texParalyzeBom;					//Ⴢ�{���e�N�X�`��
	Poly		     paralyzeBomPoly;					//Ⴢ�{���e�N�X�`����\��|���S��
	TextureManager	 texThrow;							//�X���[�e�N�X�`��
	Poly		     bomThrowPoly;						//�X���[�{���e�N�X�`����\��|���S��
	TextureManager	 texWall;							//�ǃe�N�X�`��
	Poly		     wallPoly;							//�ǃe�N�X�`����\��|���S��
	TextureManager	 texNonBreakWall;					//�S�ǃe�N�X�`��
	Poly		     nonBreakWallPoly;					//�S�ǃe�N�X�`����\��|���S��
	D3DXMATRIX matScale,matBillboard,matTrans;
	readFile* mapTableData; 
	readFile* nonBreakBlockMapData;
	DWORD putWallCount;									//�^�C���A�b�v��ɒu���ǂ̃J�E���g
	bool isTimeUp;										//�^�C���A�b�v������
	int nonBreakBlockMapTable[4800];					//�t�@�C���̏����i�[����z��
	int blockMapTable[51][51];	
	vsCpuPlayer*	vsCpuPlayerUnit;					//��CPU�v���C���[���j�b�g�̏����i�[����ϐ�
	NormalCpu*		cpuUnit[3];							//CPU���j�b�g�̏����i�[����ϐ�	
	VsPlayer		*player1, *player2;					//�ΐl�v���C���[���j�b�g�̏����i�[����ϐ�	

public:
	Wall(void);
	~Wall(void);
	void initialize();
	void update();
	void initList();
	void render(float frameTime, int cameraNum );							//�ǂ�`�悷��֐�	
	void randomWallArrange();												//�ǂ�L���������_���Ɍ��߂�֐�
	template <typename T> int retWallArr(T x, T z){return wallArr[(int)x][(int)z];}		//�ǂ̈ʒu��Ԃ�
	int retCost(float x, float z){return wallData[(int)x][(int)z].cpuCost;}				//CPU�R�X�g��Ԃ�
	D3DXVECTOR3 retItem(float x, float z){return wallData[(int)x][(int)z].itemPos;}		//�A�C�e���̈ʒu��Ԃ�
	bool retIsExpRange(float x, float z){return wallData[(int)x][(int)z].isExpRange;}	//�ԍ��̈ʒu���͈͓���
	int retExpRangeNum(float x, float z){return wallData[(int)x][(int)z].expRangeNum;}	//�����͈͂̔ԍ���Ԃ�
	float toIntPos(float pos) {return (pos<0) ? (float)(-1.0 * floor(fabs(pos) + 0.5)) : (float)(floor(pos + 0.5));}	//�ʒu�𐮐��ɂ���
	int retItemNumber(float posX, float posZ);								//�A�C�e���ԍ���Ԃ�
	void setBillboard(int x, int z, float scaleX, float scaleY, int cNum);	//�r���{�[�h�̐ݒ�
	void timeUp();															//���Ԑ�����̏���
	void notWallProcess(int x, int z, int cNum);							//�ǂ��z�u����Ă��Ȃ��ꏊ�̏���
	void setIsTimeUp(bool timeUp){isTimeUp = timeUp;}						//�^�C���A�b�v�������ݒ�
	void collisionOnTheWall(D3DXVECTOR2 pos);			//�L�����N�^�[���^�C���A�b�v��̕ǂɏՓ˂�����
	void collisions(UnitBase* target);					//���j�b�g�̏����擾
};

