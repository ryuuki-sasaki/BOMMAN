// Programming 2D Games
// Copyright (c) 2011 by:
// Charles Kelly
// Chapter 3 DirectX Window v1.0
// winmain.cpp

#define _CRTDBG_MAP_ALLOC       // メモリリーク（deleteしていないnewがないかなど）を検出する
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

// プロトタイプ関数
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int); 
bool CreateMainWindow(HWND &, HINSTANCE, int);
LRESULT WINAPI WinProc(HWND, UINT, WPARAM, LPARAM); 

// グローバル変数

HINSTANCE hinst;

PlayGame *game = NULL;
//=============================================================================
// ウィンドウズアプリケーションの開始点
//=============================================================================
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    LPSTR lpCmdLine, int nCmdShow)
{
    // メモリリークを調べてデバックビルド
    #if defined(DEBUG) | defined(_DEBUG)		//デバッグの時にメモリリークの検出
        _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
		//_CrtSetBreakAlloc(399143);
    #endif

    MSG	 msg;

	timeBeginPeriod(1);	//Winプログラムの時間単位を決める

	//Create the game,sets up message handler
	game = new PlayGame;
    HWND hwnd = NULL;

	// ウィンドウを作成
    if (!CreateMainWindow(hwnd, hInstance, nCmdShow))
        return 1;

    try{
		game->initialize(hwnd);		

        // メインメッセージループ
        int done = 0;
        while (!done)
        {
            // PeekMessage,非ブロック方式でウィンドウメッセージを調べる
            if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) 
            {
                // クイットメッセージを待ち受ける
                if (msg.message == WM_QUIT)
                    done = 1;

                // 解読して、メッセージをWinProcに渡す
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            } else
                game->run(hwnd);	//ゲームループを実行 （run...1フレームにやらなければならないことを全部やる）
        }
       // SAFE_DELETE (graphics);     // free memory before exit
		SAFE_DELETE(game);  // 終了前にメモリ解放
        return msg.wParam;
    }
    catch(const GameError &err)			//ゲームエラークラスのエラーの時
    {
		game->deleteAll();
        DestroyWindow(hwnd);
        MessageBox(NULL, err.getMessage(), "Error", MB_OK);
    }
    catch(...)							//それ以外のエラーの時
    {
		 game->deleteAll();
        DestroyWindow(hwnd);
        MessageBox(NULL, "不明なエラーが発生しました.", "Error", MB_OK);
    }

    return 0;
}


//=============================================================================
// ウィンドウズイベント・コールバック機能
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
 
    // window class 構造体 main window 記述する parameters で設定
    wcx.cbSize = sizeof(wcx);				 // 構造体サイズ 
    wcx.style = CS_HREDRAW | CS_VREDRAW;    // サイズ変更で再描画
    wcx.lpfnWndProc = WinProc;          // window procedure を指す
    wcx.cbClsExtra = 0;                 // class memory なし
    wcx.cbWndExtra = 0;                 // window memory なし
    wcx.hInstance = hInstance;          // instance へのハンドル
    wcx.hIcon = NULL; 
    wcx.hCursor = LoadCursor(NULL,IDC_ARROW);   // 事前定義カーソル
    wcx.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);    // black background 
    wcx.lpszMenuName =  NULL;           // name of menu resource 
    wcx.lpszClassName = CLASS_NAME;     // name of window class 
    wcx.hIconSm = NULL;                 // small class icon 
 
    // window class 登録. 
    // RegisterClassEx エラー時0を返す.
    if (RegisterClassEx(&wcx) == 0)    // エラー時
        return false;

	RECT r = {0, 0, GAME_WIDTH, GAME_HEIGHT};
	AdjustWindowRect(&r, WS_OVERLAPPEDWINDOW, false);

    // Create window
    hwnd = CreateWindow(
        CLASS_NAME,             // name of the window class
        GAME_TITLE,             // title bar text
        WS_OVERLAPPEDWINDOW,    // window style
        CW_USEDEFAULT,          // window の水平 position のデフォルト
        CW_USEDEFAULT,          // window の垂直 position のデフォルト
		r.right - r.left,       // window の幅
		r.bottom - r.top,        // window の高さ
        (HWND) NULL,            // 親window なし
        (HMENU) NULL,           // menu なし
        hInstance,              // application instance へのハンドル
        (LPVOID) NULL);         // window parameters なし

    // ウィンドウ作成がエラーだったら
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

    // window 表示
    ShowWindow(hwnd, nCmdShow);

    // window procedure に WM_PAINT message 送る
    UpdateWindow(hwnd);
    return true;
}

