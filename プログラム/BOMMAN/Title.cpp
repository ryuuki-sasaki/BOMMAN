//=============================================================================
//�^�C�g���������N���X
//�쐬���F2014/9/02
//�X�V���F2014/8/30
//����ҁF���X�ؗ��M
//=============================================================================
#include "Title.h"
#include "input.h"
#include "modeChange.h"
#include "option.h"
#include "audio.h"

#define MODENUM			1		//�ΐ탂�[�h�̐�
#define PLUSSIZE		10		//�ʒu�̉��Z��
#define STAYTIME		30		//�ҋ@����30fps(=1�b�Ԃ�30�t���[��)
#define ADDSIZEX 480 / 2
#define ADDSIZEY 360 / 4

//=============================================================================
//�R���X�g���N�^
//=============================================================================
Title::Title(void)
{
	//�I�[�f�B�I
	audio = new Audio;

	//�I�[�f�B�I�ǂݍ���
	audio->initialize("Sounds\\Wave Bank.xwb", "Sounds\\Sound Bank.xsb");	

	//���[�h��ʓǂݍ���
	if (!texture.initialize(graphics,"pictures\\title.png"))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing title texture"));
    if (!image.initialize(graphics,GAME_WIDTH,GAME_HEIGHT,0,&texture))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing title"));

	if (!texCursor.initialize(graphics,"pictures\\cursor.png"))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing select bom texture"));
    if (!imageCursor.initialize(graphics,0,0,0,&texCursor))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error selectBom"));

	if (!texs_Cursor.initialize(graphics,"pictures\\s_cursor.png"))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing s_Cursor texture"));
    if (!images_Cursor.initialize(graphics,0,0,0,&texs_Cursor))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing s_Cursor"));

	if (!texOption.initialize(graphics,"pictures\\option.png"))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing option texture"));
    if (!imageOption.initialize(graphics,0,0,0,&texOption))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing option"));

	initialize();
}

//=============================================================================
// ������
// �����F �Ȃ�
// �߂�l�F�Ȃ�
//=============================================================================
void Title::initialize()
{
	//�e�N�X�`���̃|�W�V�����Z�b�g
	imageCursor.setX(90);
	imageCursor.setY(390+ADDSIZEY*2);

	imageOption.setX(300+ADDSIZEX);
	imageOption.setY(200+ADDSIZEY);

	cursor = 0;
	optionCursor = 0;
	isSet = false;
}

//=============================================================================
//�f�X�g���N�^
//=============================================================================
Title::~Title(void)
{
	texture.onLostDevice();
	SAFE_DELETE(audio);
}

//=============================================================================
//�@�\�F�摜��`��
//�����F�Ȃ�
//�ߒl�F�Ȃ�
//=============================================================================
void Title::render(float frameTime, int cameraNum)
{	
	//�e�N�X�`���`��
	image.draw();
	imageCursor.draw();

	if(isSet) {
		imageOption.draw();
		images_Cursor.draw();
	}
}

//=============================================================================
//�@�\�F�X�y�[�X�L�[�������ꂽ��v���C�V�[����
//�����F�Ȃ�
//�ߒl�F�Ȃ�
//=============================================================================
void Title::update()
{		
	if(!isSet) {
		//�ΐ탂�[�h�I��
		if(input->wasKeyPush(VK_RIGHT) || input->wasGamePadPush(1, GAMEPAD_DPAD_RIGHT))
		{	
			//BGM�Đ�
			audio->playCue("�J�[�\����2");
			switch(cursor)
			{
			case MODE::VS2P:
				modeChange::GetInstance() -> setMode(VSCPU);
				cursor = MODE::VSCPU;
				imageCursor.setX(450);
				break;
			case MODE::VSCPU:
				modeChange::GetInstance() -> setMode(OPTION);
				cursor = MODE::OPTION;
				imageCursor.setX(840);
				break;
			case MODE::OPTION:
				modeChange::GetInstance() -> setMode(VS2P);
				cursor = MODE::VS2P;
				imageCursor.setX(90);
				break;
			}
		}

		if(input->wasKeyPush(VK_LEFT) || input->wasGamePadPush(1, GAMEPAD_DPAD_LEFT))
		{
			//BGM�Đ�
			audio->playCue("�J�[�\����2");
			switch(cursor)
			{
			case MODE::VS2P:
				modeChange::GetInstance() -> setMode(OPTION);
				cursor = MODE::OPTION;
				imageCursor.setX(840);
				break;
			case MODE::VSCPU:
				modeChange::GetInstance() -> setMode(VS2P);
				cursor = MODE::VS2P;
				imageCursor.setX(90);
				break;
			case MODE::OPTION:
				modeChange::GetInstance() -> setMode(VSCPU);
				cursor = MODE::VSCPU;
				imageCursor.setX(450);
				break;
			}
		}

		if(input->wasKeyPush(VK_RETURN) || input->wasGamePadPush(1, GAMEPAD_B))		
		{
			//BGM�Đ�
			audio->playCue("�J�[�\����1");
		
			if(modeChange::GetInstance() -> getMode() <= MODENUM)
				gameScene = SC_PLAY;
			if(modeChange::GetInstance() -> getMode() == OPTION)
				images_Cursor.setX(320+ADDSIZEX);
				images_Cursor.setY(265+ADDSIZEY);
				optionCursor = 0;
				isSet = true;
		}
	}

	if(isSet) {
		setOption();
	}
}

//=============================================================================
//�@�\�F�I�v�V�����ݒ�
//�����F�Ȃ�
//�ߒl�F�Ȃ�
//=============================================================================
void Title::setOption()
{
	if(input->wasKeyPush(VK_DOWN) || input->wasGamePadPush(1, GAMEPAD_DPAD_DOWN))
	{	
		//BGM�Đ�
		audio->playCue("�J�[�\����2");
		switch(optionCursor)
		{
		case OPTION::NOSELECT:
			option::GetInstance() -> setMode(THUNDER);
			optionCursor = OPTION::THUNDER;
			images_Cursor.setY(290+ADDSIZEY);
			break;
		case OPTION::THUNDER:
			option::GetInstance() -> setMode(HURRICANE);
			optionCursor = OPTION::HURRICANE;
			images_Cursor.setY(320+ADDSIZEY);
			break;
		case OPTION::HURRICANE:
			option::GetInstance() -> setMode(ALLSELECT);
			optionCursor = OPTION::ALLSELECT;
			images_Cursor.setY(345+ADDSIZEY);
			break;
		case OPTION::ALLSELECT:
			option::GetInstance() -> setMode(NOSELECT);
			optionCursor = OPTION::NOSELECT;
			images_Cursor.setY(265+ADDSIZEY);
			break;
		}
	}

	if(input->wasKeyPush(VK_UP) || input->wasGamePadPush(1, GAMEPAD_DPAD_UP))
	{
		//BGM�Đ�
		audio->playCue("�J�[�\����2");
		switch(optionCursor)
		{
		case OPTION::NOSELECT:
			option::GetInstance() -> setMode(ALLSELECT);
			optionCursor = OPTION::ALLSELECT;
			images_Cursor.setY(345+ADDSIZEY);
			break;
		case OPTION::THUNDER:
			option::GetInstance() -> setMode(NOSELECT);
			optionCursor = OPTION::NOSELECT;
			images_Cursor.setY(265+ADDSIZEY);
			break;
		case OPTION::HURRICANE:
			option::GetInstance() -> setMode(THUNDER);
			optionCursor = OPTION::THUNDER;
			images_Cursor.setY(290+ADDSIZEY);
			break;
		case OPTION::ALLSELECT:
			option::GetInstance() -> setMode(HURRICANE);
			optionCursor = OPTION::HURRICANE;
			images_Cursor.setY(320+ADDSIZEY);
			break;
		}
	}

	if(input->wasKeyPush(VK_RETURN) || input->wasGamePadPush(1, GAMEPAD_B))		
	{
		//BGM�Đ�
		audio->playCue("�J�[�\����1");
		isSet = false;
	}
}