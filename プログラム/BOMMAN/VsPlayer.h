//=============================================================================
//  �v���C���[(2P�ΐ�p)
//�@�X�V���F2016/10/30
//	����ҁF���X�ؗ��M
//=============================================================================
#pragma once
#include "Player.h"
#include "constants.h"
#include "Poly.h"
#include "textureManager.h"
#include <list>
using namespace std;

class normalBom;
class Wall;
class NormalCpu;
class Model;
class Image;

class VsPlayer : public Player
{
public:
	D3DXVECTOR3      pos;								//�ʒu
	bool			 isStop;							//������~���邩�ǂ���
	bool			 isLose;							//���S����t���O
	list<int>		 plusBomNumList;					//�v���X�{���̌����i�[����z��
	list<int>		 paralyzeBomNumList;				//Ⴢ�{���̌����i�[����z��
	list<int>		 bomThrowNumList;					//Ⴢ�{���̌����i�[����z��
	list<int>::iterator	plusBomitr;						//�C�e���[�^�쐬
	list<int>::iterator	paralyzeitr;					//�C�e���[�^�쐬
	list<int>::iterator	bomThrowitr;					//�C�e���[�^�쐬
	bool			 isShake;							//�J������h�炷��
	int				 player;							//�v���C���[�ԍ�

protected:
	struct key
	{
		//�ړ��L�[
		unsigned int MOVE_UP;
		unsigned int MOVE_DOWN;
		unsigned int MOVE_RIGHT;
		unsigned int MOVE_LEFT;

		//�R�}���h�I��
		unsigned int COMMAND_SELECT_RIGHT;
		unsigned int COMMAND_SELECT_LEFT;

		//�{����u��
		unsigned int PUT_BOM;
		unsigned int THROW;
	}KEY;

	Model*			 model;								//�N���X�^�̃I�u�W�F�N�g��������Ƃ��|�C���^�ɂ���Ƃ����������idelete��Y��Ȃ��j		
	D3DXVECTOR3      move;								//�ړ��x�N�g��
	bool			 iskeyState;						//�L�[������������Ƃ��̏������t���[�����Ƃɍs���邽�߁A��x���������x�����������Ȃ��悤�ɐ��䂷��
	int				 commandSelect;						//�ǂ̃R�}���h��I��������
	float			 angle;								//�p�x
	D3DXVECTOR3		 bomThrowPos;						//�������̈ʒu
	D3DXVECTOR3		 bomThrowEffectPos;					//��������\���G�t�F�N�g�̈ʒu
	bool			 throwFlag;							//�����鏈�����H
	bool			 effectRenderFlag;					//�������̃G�t�F�N�g��\�����邩�ǂ���
	bool			 isItem;							//�A�C�e���{����I���������ǂ����𔻒�
	int				 itemType;							//�A�C�e���̎��
	int				 processBomType;					//�������̃{��
	int				 stopCount;							//��~���Ԃ̃J�E���g
	int				 putBomPlayer;						//�{����u�����v���C���[�̎���(2P�ΐ펞�p)
	TextureManager	 texBom;							//�{���e�N�X�`��
	Image*		     bomimage;							//�C���[�W
	TextureManager	 texPlusBom;						//�v���X�{���e�N�X�`��
	Image*		     plusBomimage;						//�C���[�W
	TextureManager	 texArrow;							//���e�N�X�`��
	Image*		     arrowimage;						//�C���[�W
	TextureManager	 texParalyzeBom;					//���e�N�X�`��
	Image*		     paralyzeBomimage;					//�C���[�W
	TextureManager	 texThrow;							//���e�N�X�`��
	Image*		     throwimage;						//�C���[�W
	TextureManager	 texItemNum;						//�A�C�e�����p�����e�N�X�`��
	Image*		     leftItemNumimage;					//�C���[�W
	Image*		     rightItemNumimage;					//�C���[�W
	Poly		     BomThrowPosEffectPoly;				//�摜��\��|���S��
	TextureManager	 texBomThrowPosEffect;				//�{���𓊂����������e�N�X�`��
	int				 moveControlCount;					//�L�����N�^�[����������܂ł̗V�т̎���
	D3DXVECTOR3		 shakePos;							//�J��������炵����̈ʒu
	float			 minRange;							//�����ʒu�ƃv���C���[�ʒu�̋����̒��ōł��Z������
	int				 shakeTime;							//�J������h�炷����
	float			 shakeDir;							//�J������h�炷����
	int				 addExpRange;						//�����͈͂̉��Z��
	bool			 isPushMoveButton;					//�ړ��{�^���������Ă��邩
	normalBom*		 bomUnit[3];						//�e��{���I�u�W�F�N�g�̏����i�[����z��		
	Wall*			 wallUnit;							//�E�H�[�����j�b�g�̏����i�[����ϐ�
	NormalCpu*		 cpuUnit[3];						//CPU���j�b�g�̏����i�[����ϐ�
	VsPlayer		 *player1, *player2;				//�ΐl�v���C���[���j�b�g�̏����i�[����ϐ�

public: 
	VsPlayer(int playerNum);
	~VsPlayer();
	void  render(float frameTime, int cameraNum);		//�`�揈��
	void  update();										//�X�V����
	void setShakeCameraInfo(D3DXVECTOR3 expPos, float shakeSize);	//�J������h�炷���Z�b�g

protected:
	void initialize();						
	void initList();									//���X�g�̏�����
	void control();										//�ړ�
	void cameraCtrl();									//�J��������
	void inRange();										//�v���C���[���t�B�[���h�͈͓̔���
	bool collObstacel(D3DXVECTOR3 pos, bool isThrow);	//��Q���ւ̏Փ˔���
	void selectThrowPos();								//�{���𓊂�����ݒ�
	void getItem();										//�A�C�e���擾
	bool isBomListState(int	itenType);					//�{�����X�g���󂩂ǂ���
	void bomProcess();									//�{���ɑ΂��鏈��
	void notRenderBom();								//�`�撆�̃{�������邩
	void renderNumber();								//�A�C�e�����\��
	void processedCommand(int command);					//�g�p�R�}���h�̌㏈��
	void selectCommand();								//�R�}���h�I��
	void shakeCamera();									//�J������h�炷
	void plusExpRange() { addExpRange++; }				//�����͈͂̉��Z
	void adjustmentPos();								//�ʒu�𐮐��ɂ���
	Model* GetModel(){return model;}					//�A�N�Z�X�֐�
	float toIntMove(float move) {return (move<0) ? (float)(-1.0 * floor(fabs(move) + 0.5)) : (float)(floor(move + 0.5));}	//Move�̒����𐮐��ɂ���
	float toIntPos(float pos) {return (pos<0) ? (float)(-1.0 * floor(fabs(pos) + 0.5)) : (float)(floor(pos + 0.5));}		//�ʒu�𐮐��ɂ���	
	void collisions(UnitBase* target);					//���j�b�g�̏����擾
};

