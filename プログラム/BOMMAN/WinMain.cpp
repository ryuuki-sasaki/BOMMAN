// Programming 2D Games
// Copyright (c) 2011 by:
// Charles Kelly
// Chapter 3 DirectX Window v1.0
// winmain.cpp

#define _CRTDBG_MAP_ALLOC       // ���������[�N�idelete���Ă��Ȃ�new���Ȃ����Ȃǁj�����o����
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <stdlib.h>             // for detecting memory leaks
#include <crtdbg.h>             // for detecting memory leaks
#include "PlayGame.h"
#include "graphics.h"
//#pragma comment(lib,"winmm.lib")
//#pragma comment(lib,"Xinput.lib")
//#pragma comment(lib,"d3d9.lib")
//#pragma comment(lib,"d3dx9.lib")

// �v���g�^�C�v�֐�
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int); 
bool CreateMainWindow(HWND &, HINSTANCE, int);
LRESULT WINAPI WinProc(HWND, UINT, WPARAM, LPARAM); 

// �O���[�o���ϐ�

HINSTANCE hinst;

PlayGame *game = NULL;
//=============================================================================
// �E�B���h�E�Y�A�v���P�[�V�����̊J�n�_
//=============================================================================
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    LPSTR lpCmdLine, int nCmdShow)
{
    // ���������[�N�𒲂ׂăf�o�b�N�r���h
    #if defined(DEBUG) | defined(_DEBUG)		//�f�o�b�O�̎��Ƀ��������[�N�̌��o
        _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
		//_CrtSetBreakAlloc(399143);
    #endif

    MSG	 msg;

	timeBeginPeriod(1);	//Win�v���O�����̎��ԒP�ʂ����߂�

	//Create the game,sets up message handler
	game = new PlayGame;
    HWND hwnd = NULL;

	// �E�B���h�E���쐬
    if (!CreateMainWindow(hwnd, hInstance, nCmdShow))
        return 1;

    try{
		game->initialize(hwnd);		

        // ���C�����b�Z�[�W���[�v
        int done = 0;
        while (!done)
        {
            // PeekMessage,��u���b�N�����ŃE�B���h�E���b�Z�[�W�𒲂ׂ�
            if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) 
            {
                // �N�C�b�g���b�Z�[�W��҂��󂯂�
                if (msg.message == WM_QUIT)
                    done = 1;

                // ��ǂ��āA���b�Z�[�W��WinProc�ɓn��
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            } else
                game->run(hwnd);	//�Q�[�����[�v�����s �irun...1�t���[���ɂ��Ȃ���΂Ȃ�Ȃ����Ƃ�S�����j
        }
       // SAFE_DELETE (graphics);     // free memory before exit
		SAFE_DELETE(game);  // �I���O�Ƀ��������
        return msg.wParam;
    }
    catch(const GameError &err)			//�Q�[���G���[�N���X�̃G���[�̎�
    {
		game->deleteAll();
        DestroyWindow(hwnd);
        MessageBox(NULL, err.getMessage(), "Error", MB_OK);
    }
    catch(...)							//����ȊO�̃G���[�̎�
    {
		 game->deleteAll();
        DestroyWindow(hwnd);
        MessageBox(NULL, "�s���ȃG���[���������܂���.", "Error", MB_OK);
    }

    return 0;
}


//=============================================================================
// �E�B���h�E�Y�C�x���g�E�R�[���o�b�N�@�\
//=============================================================================
LRESULT WINAPI WinProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	return (game->messageHandler(hWnd, msg, wParam, lParam));
}

//=============================================================================
// Create the window
// Returns: false on error
//=============================================================================
bool CreateMainWindow(HWND &hwnd, HINSTANCE hInstance, int nCmdShow) 
{ 
    WNDCLASSEX wcx; 
 
    // window class �\���� main window �L�q���� parameters �Őݒ�
    wcx.cbSize = sizeof(wcx);				 // �\���̃T�C�Y 
    wcx.style = CS_HREDRAW | CS_VREDRAW;    // �T�C�Y�ύX�ōĕ`��
    wcx.lpfnWndProc = WinProc;          // window procedure ���w��
    wcx.cbClsExtra = 0;                 // class memory �Ȃ�
    wcx.cbWndExtra = 0;                 // window memory �Ȃ�
    wcx.hInstance = hInstance;          // instance �ւ̃n���h��
    wcx.hIcon = NULL; 
    wcx.hCursor = LoadCursor(NULL,IDC_ARROW);   // ���O��`�J�[�\��
    wcx.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);    // black background 
    wcx.lpszMenuName =  NULL;           // name of menu resource 
    wcx.lpszClassName = CLASS_NAME;     // name of window class 
    wcx.hIconSm = NULL;                 // small class icon 
 
    // window class �o�^. 
    // RegisterClassEx �G���[��0��Ԃ�.
    if (RegisterClassEx(&wcx) == 0)    // �G���[��
        return false;

	RECT r = {0, 0, GAME_WIDTH, GAME_HEIGHT};
	AdjustWindowRect(&r, WS_OVERLAPPEDWINDOW, false);

    // Create window
    hwnd = CreateWindow(
        CLASS_NAME,             // name of the window class
        GAME_TITLE,             // title bar text
        WS_OVERLAPPEDWINDOW,    // window style
        CW_USEDEFAULT,          // window �̐��� position �̃f�t�H���g
        CW_USEDEFAULT,          // window �̐��� position �̃f�t�H���g
		r.right - r.left,       // window �̕�
		r.bottom - r.top,        // window �̍���
        (HWND) NULL,            // �ewindow �Ȃ�
        (HMENU) NULL,           // menu �Ȃ�
        hInstance,              // application instance �ւ̃n���h��
        (LPVOID) NULL);         // window parameters �Ȃ�

    // �E�B���h�E�쐬���G���[��������
    if (!hwnd)
        return false;

	if(!FULLSCREEN)             // if window
    {
        // Adjust window size so client area is GAME_WIDTH x GAME_HEIGHT
        RECT clientRect;
        GetClientRect(hwnd, &clientRect);   // get size of client area of window
        MoveWindow(hwnd,
                   0,                                           // Left
                   0,                                           // Top
                   GAME_WIDTH+(GAME_WIDTH-clientRect.right),    // Right
                   GAME_HEIGHT+(GAME_HEIGHT-clientRect.bottom), // Bottom
                   TRUE);                                       // Repaint the window
    }

    // window �\��
    ShowWindow(hwnd, nCmdShow);

    // window procedure �� WM_PAINT message ����
    UpdateWindow(hwnd);
    return true;
}

