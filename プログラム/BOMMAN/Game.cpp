// Programming 2D Games
// Copyright (c) 2011 by: 
// Charles Kelly
// Chapter 5 game.cpp v1.0

#include "game.h"
//=============================================================================
//�R���X�g���N�^
//=============================================================================
Game::Game()
{
    input = new Input();        // �L�[�{�[�h�̓��͂𑦎��ɏ�����
    // ���̑��̏������͌��input->initialize()���Ăяo���ď���
    paused = false;             // �Q�[���͈ꎞ��~���łȂ�
    graphics = NULL;
    initialized = false;
}

//=============================================================================
//�f�X�g���N�^
//=============================================================================
Game::~Game()
{
	deleteAll();		//�\�񂳂�Ă��������������ׂĉ��
	ShowCursor(true);	//�J�[�\����\���i�J�[�\���������Ă����ꍇ�\������j
}

//=============================================================================
//Windows���b�Z�[�W�n���h��
//=============================================================================
LRESULT Game::messageHandler( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    if(initialized)     // do not process messages if not initialized
    {
        switch( msg )
        {
            case WM_DESTROY:
                PostQuitMessage(0);        //tell Windows to kill this program
                return 0;
            case WM_KEYDOWN: case WM_SYSKEYDOWN:    // key down
                input->keyDown(wParam);
                return 0;
            case WM_KEYUP: case WM_SYSKEYUP:        // key up
                input->keyUp(wParam);
                return 0;
        }
    }
    return DefWindowProc( hwnd, msg, wParam, lParam );    // let Windows handle it
}

//=============================================================================
//�Q�[����������
//�G���[����GameError���X���[
//=============================================================================
void Game::initialize(HWND hw)
{
    hwnd = hw;                                  //�E�B���h�E�n���h����ۑ�

	//�R���g���[���̐ڑ���Ԃ𓾂�
	input->checkControllers();

    // �O���t�B�b�N�X��������
    graphics = new Graphics();
    //GameError���X���[ (�X���[�E�E�E��O�j
    graphics->initialize(hwnd, GAME_WIDTH, GAME_HEIGHT, FULLSCREEN);

    // ������\�^�C�}�[�̏��������݂�
    if(QueryPerformanceFrequency(&timerFreq) == false)
        throw(GameError(gameErrorNS::FATAL_ERROR, "�^�C�}�[�������s"));

    QueryPerformanceCounter(&timeStart);        // ���Ԏ擾���J�n

    initialized = true;
}

//=============================================================================
//�Q�[���A�C�e���������_�[
//=============================================================================
void Game::renderGame()
{
    //�����_�����O���J�n
    if (SUCCEEDED(graphics->beginScene()))
    {
        // render�́A�p�������N���X���ŋL�q����K�v�̂��鏃�����z�֐��ł��B
       
        render();               //�h���N���X��render���Ăяo��

        //�����_�����O���I��
        graphics->endScene();
    }
    handleLostGraphicsDevice();

    //�o�b�N�o�b�t�@����ʂɕ\��
    graphics->showBackbuffer();
}


//=============================================================================
//���������O���t�B�b�N�X�f�o�C�X������
//=============================================================================
void Game::handleLostGraphicsDevice()
{
    //�f�o�C�X�̏������e�X�g���A����ɉ����ď��������s
    hr = graphics->getDeviceState();
    if(FAILED(hr))                  // �O���t�B�N�X�f�o�C�X���L���łȂ��ꍇ
    {
        // �f�o�C�X���������Ă���A���Z�b�g�ł����ԂɂȂ��ꍇ
        if(hr == D3DERR_DEVICELOST)
        {
            Sleep(100);             // CPU���Ԃ𖾂��n���i100�~���b�j100�~���b�Q�Ă�悤�ɂ���
            return;
        } 
        // �f�o�C�X���������Ă��邪�A���Z�b�g�ł����Ԃɂ���ꍇ
        else if(hr == D3DERR_DEVICENOTRESET)
        {
            releaseAll();			//reset�̑O�ɂ��ׂĂ��J��
            hr = graphics->reset(); // �O���t�B�b�N�f�o�C�X�̃��Z�b�g�����݂�
            if(FAILED(hr))          // ���Z�b�g�����s�����ꍇ
                return;
            resetAll();
        }
        else
            return;                 // ���̑��̃f�o�C�X�G���[
    }
}

//=============================================================================
// Toggle window or fullscreen mode
//=============================================================================
void Game::setDisplayMode(graphicsNS::DISPLAY_MODE mode)
{
    releaseAll();                   // free all user created surfaces
    graphics->changeDisplayMode(mode);
    resetAll();                     // recreate surfaces
}

//=============================================================================
//Winmain���̃��C���̃��b�Z�[�W���[�v�ŌJ��Ԃ��Ăяo�����
//=============================================================================
void Game::run(HWND hwnd)
{
	// �O���t�B�b�N�X������������Ă��Ȃ��ꍇ
    if(graphics == NULL)								
        return;

    // �Ō�̃t���[������̎��Ԃ��v�Z�AframeTime�ɕۑ�
    QueryPerformanceCounter(&timeEnd);

	//�O��X�V���Ă��炢�܂܂ŉ��~���b��������
    frameTime = (float)(timeEnd.QuadPart - timeStart.QuadPart ) / 
                (float)timerFreq.QuadPart;

    // �ȓd�̓R�[�h�iwinmm.lib���K�v�j
    // ��]����t���[�����[�g�ɑ΂��Čo�ߎ��Ԃ��Z���ꍇ
    if (frameTime < MIN_FRAME_TIME)									//�\�����Ԃ��o�߂��Ă��Ȃ��ꍇ
    {
        sleepTime = (DWORD)((MIN_FRAME_TIME - frameTime)*1000);
        timeBeginPeriod(1);         // 1ms�̕���\��Windows�^�C�}�[�ɗv��
        Sleep(sleepTime);           // sleepTime��CPU���
        timeEndPeriod(1);           // 1ms�̃^�C�}�[�I��
        return;
    }

    if (frameTime > 0.0)
        fps = (fps*0.99f) + (0.01f/frameTime);  // ����fps�i���ۂǂꂭ�炢�̑��x�œ����Ă���̂��j

    if (frameTime > MAX_FRAME_TIME) // �t���[�����[�g�����ɒx���ꍇ
        frameTime = MAX_FRAME_TIME; // �ő�frameTime�𐧌�

    timeStart = timeEnd;			//���Ă΂ꂽ�Ƃ��O��̂Â�����ł���@
	
	//FPS�v��
	#if	defined(DEBUG) | defined(_DEBUG)
	static int count = 0;				
	static int time = timeGetTime();		 //timeGettime() = �E�B���h�E�Y���N�����Ă���̎���(�~���b�ł͂���)
	count++;								 //�P�b�Ԃɉ���Ă΂ꂽ��
	if(timeGetTime() - time >= 1000)		//�������̎��Ԃ����āA���ꂩ��O��̎��Ԃ��������Ƃ�1000�ȏ�Ȃ�΁@�܂�P�b�o�߂�����Ƃ����Ӗ�
	{
		char str[16];						//����������邽�߂̕ϐ�
		wsprintf(str, "FPS=%d", count);		//������𐔒l�ɕς���
		SetWindowText(hwnd, str);			//�E�B���h�E�̃^�C�g���o�[��ς���(�E�B���h�E�n���h���A�\�����������)
		count = 0;				
		time = timeGetTime();				//�܂��^�C�����Ƃ�
	}
	#endif

	input->readControllers();       // �R���g���[���̏�Ԃ�ǂݎ��i�ǂ̃{�^���������ꂽ���Ȃǁj

    // update(), ai(), and collisions() �͏������z�֐��ł��B
    // �����̊֐��́AGame�N���X���p�����Ă���N���X���ŋL�q����K�v������܂��B
    if (!paused)                    // �ꎞ��~���łȂ��ꍇ
    {
        update();                   // ���ׂẴQ�[���A�C�e�����X�V 
		collisions();               // �Q�[���A�C�e���̏���n�� 
		ai();                       // �l�H�m�\
    }
    renderGame();                   //���ׂẴQ�[���A�C�e����`��	�i�|�[�Y�ł��낤���Ȃ��낤���j
}

//=============================================================================
// The graphics device was lost.
// Release all reserved video memory so graphics device may be reset.
//=============================================================================
void Game::releaseAll()
{}

//=============================================================================
// Recreate all surfaces and reset all entities.
//=============================================================================
void Game::resetAll()
{}

//=============================================================================
// Delete all reserved memory
//=============================================================================
void Game::deleteAll()
{
    releaseAll();               // call onLostDevice() for every graphics item
    SAFE_DELETE(graphics);
    SAFE_DELETE(input);
    initialized = false;
}
