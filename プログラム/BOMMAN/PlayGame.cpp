//=============================================================================
//�Q�[���������N���X
//�쐬���F2014/09/08
//�X�V���F2016/10/19
//����ҁF���X�ؗ��M
//=============================================================================
#include "PlayGame.h"
#include "TitleScene.h"
#include "PlayScene.h"
#include "Camera.h"
#include "Hurricane.h"
#include "vsCpuPlayer.h"
#include "modeChange.h"
#include "SceneBase.h"
#include "audio.h"

#define CHARANUM 3		//�L�����N�^�[��

//�O���[�o���ϐ�
GameScene gameScene;			//���݂̃V�[��
Graphics *graphics;				//�O���t�B�b�N�^�̃N���X�|�C���^
Input	 *input;				//�C���v�b�g�^�̃N���X�|�C���^
Camera   *cameraA, *cameraB;	//�J�����^�̃N���X�|�C���^
GameSt	 gameSt;				//���݂̏��
GameRs	 gameRs;				//���݂̏��s

enum AI_STATE
{
	INIT,	//������
	CALC,	//�v�Z��
	MOVE,	//�ړ�����
	STAY,	//��~
	NOTAI = 100	//AI���g�p���Ȃ�
};

//=============================================================================
//�R���X�g���N�^
//=============================================================================
PlayGame::PlayGame(void)
{
	gameScene = SC_TITLE;	//���݂̃V�[��
	prevScene = SC_TITLE;	//��O�̃V�[��
	sceneChangFlg = FALSE;  //�V�[���؂�ւ��̃t���O
}

//=============================================================================
//�f�X�g���N�^
//=============================================================================
PlayGame::~PlayGame(void)
{
	releaseAll();
	SAFE_DELETE(audio);
}

//=============================================================================
// �Q�[���̏�����
// �����FHWND�@�E�B���h�E�n���h��
// �ߒl�F�Ȃ�
//=============================================================================
void PlayGame::initialize(HWND hwnd)
{
	Game::initialize(hwnd);

	//�ŏ��̓^�C�g���V�[��
	nowScene = new TitleScene();
	cameraA = new Camera;
	cameraB = new Camera;
	//�I�[�f�B�I
	audio = new Audio;

	//���[�h��ʓǂݍ���
	if (!loadTex.initialize(graphics,"pictures\\load.jpg"))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing load texture"));
    if (!loadImg.initialize(graphics,GAME_WIDTH,GAME_HEIGHT,0,&loadTex))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing load"));

	//�I�[�f�B�I�ǂݍ���
	audio->initialize("Sounds\\Wave Bank.xwb", "Sounds\\Sound Bank.xsb");

	//BGM�Đ�
	audio->playCue("JINGLE-22");

	//�J�����̐ݒ�i�P��ځj
	D3DXVECTOR3 vCamPos(0,3,-3);	//�J�����̈ʒu
	D3DXVECTOR3 vCamTarget(0,0,0);	//�J�����̏œ_�i�����_�j
	D3DXVECTOR3 vCamUp(0,1,0);		//�J�����̏����
	cameraA->setPos(vCamPos);
	cameraA->setTarget(vCamTarget);

	//�J�����̐ݒ�i�Q��ځj
	vCamPos = D3DXVECTOR3(0,3,-3);
	cameraB->setPos(vCamPos);
	cameraB->setTarget(vCamTarget);

	//�����_�����O�ݒ�
	graphics->get3Ddevice()->
				SetRenderState(D3DRS_ZENABLE, TRUE);		
	graphics->get3Ddevice()->
				SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	//���C�g
	graphics->get3Ddevice()->SetRenderState(D3DRS_LIGHTING, TRUE);
	D3DLIGHT9 light;
	D3DLIGHT9 spotLight;
	ZeroMemory(&light, sizeof(D3DLIGHT9));
	ZeroMemory(&spotLight, sizeof(D3DLIGHT9));
	light.Type = D3DLIGHT_DIRECTIONAL;
	spotLight.Type = D3DLIGHT_SPOT;

	light.Diffuse.r = 1.0f; 
	light.Diffuse.g = 1.0f; 
	light.Diffuse.b = 1.0f; 
	light.Diffuse.a = 1.0f; 
	spotLight.Diffuse.r = 1.0f; 
	spotLight.Diffuse.g = 1.0f; 
	spotLight.Diffuse.b = 1.0f; 
	spotLight.Diffuse.a = 1.0f; 

	light.Ambient.r = 1.0f;
	light.Ambient.g = 1.0f;
	light.Ambient.b = 1.0f;
	light.Ambient.a = 1.0f;
	spotLight.Ambient.r = 1.0f;
	spotLight.Ambient.g = 1.0f;
	spotLight.Ambient.b = 1.0f;
	spotLight.Ambient.a = 1.0f;

	light.Specular.r = 1.0f;
	light.Specular.g = 1.0f;
	light.Specular.b = 1.0f;
	light.Specular.a = 1.0f;
	spotLight.Specular.r = 1.0f;
	spotLight.Specular.g = 1.0f;
	spotLight.Specular.b = 1.0f;
	spotLight.Specular.a = 1.0f;

	light.Direction = D3DXVECTOR3(1, -1, 1);
	spotLight.Direction = D3DXVECTOR3(1, -1, 1);
	light.Range = 200.0f;
	spotLight.Range = 50.0f;
	
	graphics->get3Ddevice()->SetLight(0, &light);
	graphics->get3Ddevice()->SetLight(1, &spotLight);
	graphics->get3Ddevice()->LightEnable(0, TRUE);
	graphics->get3Ddevice()->LightEnable(1, TRUE);
}

//=============================================================================
// ���ׂẴQ�[���A�C�e���̍X�V
// �����F�Ȃ�
// �ߒl�F�Ȃ�
//=============================================================================
void PlayGame::update()
{
	//���݂̃V�[�����A�b�v�f�[�g
	nowScene->update();	

	//�V�[�����ς����(true)
	if(sceneChangFlg == TRUE)		
	{
		//������Ă���Scene��SceneBase���p�����Ă��邩��SceneBase�^��nowScene�ɑ���\
		//�V�[���̐؂�ւ�
		switch (gameScene)			
		{
		case SC_TITLE:
			SAFE_DELETE(nowScene);		//���݂̃V�[���̍폜
			audio->stopCue("Jazz 1");		//BGM��~
			audio->playCue("JINGLE-22");	//BGM�Đ�
			nowScene = new TitleScene();
			break;

		case SC_PLAY:
			SAFE_DELETE(nowScene);		//���݂̃V�[���̍폜			
			audio->stopCue("JINGLE-22");	//BGM��~
			audio->playCue("Jazz 1");		//BGM�Đ�
			playSceneCamera();
			nowScene = new PlayScene();
			break;

		case SC_INIT:
			gameScene = SC_PLAY;
			nowScene->initialize();
			break;
		}
		prevScene = gameScene;		//���݂̕\�����P�O�̕\����
		sceneChangFlg = FALSE;		
	}	
}

//=============================================================================
// AI�l�H�m�\
// �����F�Ȃ�
// �ߒl�F�Ȃ�
//=============================================================================
void PlayGame::ai()
{
	nowScene->ai();
}

//=============================================================================
//�Փ˂�����
// �����F�Ȃ�
// �ߒl�F�Ȃ�
//=============================================================================
void PlayGame::collisions()
{
	nowScene->collisions();
}

//=============================================================================
// �Q�[���A�C�e���������_�[
// �����F�Ȃ�
// �ߒl�F�Ȃ�
//=============================================================================
void PlayGame::render()		
{
	//�V�[�����ς����
	if(gameScene != prevScene)
	{
		//Scene�J�ڊԂ̃��[�h��ʕ`��
		loadImg.draw();			
		sceneChangFlg = TRUE;
		return;					//���̃t���[���̕`����X�g�b�v
	}

	//���݂̃V�[���̕`��
	nowScene->render(frameTime);	
}

//=============================================================================
// �O���t�B�b�N�X�f�o�C�X�����������Ƃ�
// �����F�Ȃ�
// �ߒl�F�Ȃ�
//=============================================================================
//�����@-�O���t�B�b�N�f�o�C�X�����������ꍇ
//�O���t�B�b�N�f�o�C�X�����Z�b�g�\�ɂ��邽��
//�\�񂳂�Ă����r�f�I�����������ׂĊJ��-
void PlayGame::releaseAll()
{
	Game::releaseAll();
	SAFE_DELETE(nowScene);
	SAFE_DELETE(cameraA);
	SAFE_DELETE(cameraB);
	return;
}

//=============================================================================
// �O���t�B�b�N�X�f�o�C�X�����Z�b�g���ꂽ��
// �����F�Ȃ�
// �ߒl�F�Ȃ�
//=============================================================================
//�����@-�O���t�B�b�N�f�o�C�X�����Z�b�g���ꂽ�ꍇ
//���ׂẴT�[�t�F�X���č쐬-
void PlayGame::resetAll()
{
	Game::resetAll();
	return;
}

//=============================================================================
// �v���C�V�[���̃J�����̐ݒ�
// �����F�Ȃ�
// �ߒl�F�Ȃ�
//=============================================================================
void PlayGame::playSceneCamera()
{
	//2P�ΐ�Ȃ�
	if(modeChange::GetInstance() -> getMode() == VS2P)
	{
		cameraA->changeViewport(0, 0, GAME_WIDTH / 2, GAME_HEIGHT);					//��ʂ��������ɂ���
		cameraB->changeViewport(GAME_WIDTH / 2, 0, GAME_WIDTH / 2, GAME_HEIGHT);	//��ʂ��E�����ɂ���
	} else /*CPU�ΐ�Ȃ�*/{
		cameraA->changeViewport(0, 0, GAME_WIDTH, GAME_HEIGHT);		
		cameraB->changeViewport(NULL, NULL, NULL, NULL);	//��ʂ��E�����ɂ���
	}	

	return;
}

