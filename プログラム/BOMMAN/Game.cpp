// Programming 2D Games
// Copyright (c) 2011 by: 
// Charles Kelly
// Chapter 5 game.cpp v1.0

#include "game.h"
//=============================================================================
//コンストラクタ
//=============================================================================
Game::Game()
{
    input = new Input();        // キーボードの入力を即時に初期化
    // その他の初期化は後でinput->initialize()を呼び出して処理
    paused = false;             // ゲームは一時停止中でない
    graphics = NULL;
    initialized = false;
}

//=============================================================================
//デストラクタ
//=============================================================================
Game::~Game()
{
	deleteAll();		//予約されていたメモリをすべて解放
	ShowCursor(true);	//カーソルを表示（カーソルを消していた場合表示する）
}

//=============================================================================
//Windowsメッセージハンドラ
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
//ゲームを初期化
//エラー時にGameErrorをスロー
//=============================================================================
void Game::initialize(HWND hw)
{
    hwnd = hw;                                  //ウィンドウハンドルを保存

	//コントローラの接続状態を得る
	input->checkControllers();

    // グラフィックスを初期化
    graphics = new Graphics();
    //GameErrorをスロー (スロー・・・例外）
    graphics->initialize(hwnd, GAME_WIDTH, GAME_HEIGHT, FULLSCREEN);

    // 高分解能タイマーの準備を試みる
    if(QueryPerformanceFrequency(&timerFreq) == false)
        throw(GameError(gameErrorNS::FATAL_ERROR, "タイマー準備失敗"));

    QueryPerformanceCounter(&timeStart);        // 時間取得を開始

    initialized = true;
}

//=============================================================================
//ゲームアイテムをレンダー
//=============================================================================
void Game::renderGame()
{
    //レンダリングを開始
    if (SUCCEEDED(graphics->beginScene()))
    {
        // renderは、継承したクラス側で記述する必要のある純粋仮想関数です。
       
        render();               //派生クラスのrenderを呼び出す

        //レンダリングを終了
        graphics->endScene();
    }
    handleLostGraphicsDevice();

    //バックバッファを画面に表示
    graphics->showBackbuffer();
}


//=============================================================================
//消失したグラフィックスデバイスを処理
//=============================================================================
void Game::handleLostGraphicsDevice()
{
    //デバイスの消失をテストし、それに応じて処理を実行
    hr = graphics->getDeviceState();
    if(FAILED(hr))                  // グラフィクスデバイスが有効でない場合
    {
        // デバイスが消失しており、リセットできる状態にない場合
        if(hr == D3DERR_DEVICELOST)
        {
            Sleep(100);             // CPU時間を明け渡す（100ミリ秒）100ミリ秒寝てるようにする
            return;
        } 
        // デバイスが消失しているが、リセットできる状態にある場合
        else if(hr == D3DERR_DEVICENOTRESET)
        {
            releaseAll();			//resetの前にすべてを開放
            hr = graphics->reset(); // グラフィックデバイスのリセットを試みる
            if(FAILED(hr))          // リセットが失敗した場合
                return;
            resetAll();
        }
        else
            return;                 // その他のデバイスエラー
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
//Winmain内のメインのメッセージループで繰り返し呼び出される
//=============================================================================
void Game::run(HWND hwnd)
{
	// グラフィックスが初期化されていない場合
    if(graphics == NULL)								
        return;

    // 最後のフレームからの時間を計算、frameTimeに保存
    QueryPerformanceCounter(&timeEnd);

	//前回更新してからいままで何ミリ秒たったか
    frameTime = (float)(timeEnd.QuadPart - timeStart.QuadPart ) / 
                (float)timerFreq.QuadPart;

    // 省電力コード（winmm.libが必要）
    // 希望するフレームレートに対して経過時間が短い場合
    if (frameTime < MIN_FRAME_TIME)									//十分時間が経過していない場合
    {
        sleepTime = (DWORD)((MIN_FRAME_TIME - frameTime)*1000);
        timeBeginPeriod(1);         // 1msの分解能をWindowsタイマーに要求
        Sleep(sleepTime);           // sleepTime間CPU解放
        timeEndPeriod(1);           // 1msのタイマー終了
        return;
    }

    if (frameTime > 0.0)
        fps = (fps*0.99f) + (0.01f/frameTime);  // 平均fps（実際どれくらいの速度で動いているのか）

    if (frameTime > MAX_FRAME_TIME) // フレームレートが非常に遅い場合
        frameTime = MAX_FRAME_TIME; // 最大frameTimeを制限

    timeStart = timeEnd;			//次呼ばれたとき前回のつづきからできる　
	
	//FPS計測
	#if	defined(DEBUG) | defined(_DEBUG)
	static int count = 0;				
	static int time = timeGetTime();		 //timeGettime() = ウィンドウズが起動してからの時間(ミリ秒ではいる)
	count++;								 //１秒間に何回呼ばれたか
	if(timeGetTime() - time >= 1000)		//もし今の時間を見て、それから前回の時間を引いたとき1000以上ならば　つまり１秒経過したらという意味
	{
		char str[16];						//文字列を入れるための変数
		wsprintf(str, "FPS=%d", count);		//文字列を数値に変える
		SetWindowText(hwnd, str);			//ウィンドウのタイトルバーを変える(ウィンドウハンドル、表示したいやつ)
		count = 0;				
		time = timeGetTime();				//またタイムをとる
	}
	#endif

	input->readControllers();       // コントローラの状態を読み取る（どのボタンが押されたかなど）

    // update(), ai(), and collisions() は純粋仮想関数です。
    // これらの関数は、Gameクラスを継承しているクラス側で記述する必要があります。
    if (!paused)                    // 一時停止中でない場合
    {
        update();                   // すべてのゲームアイテムを更新 
		collisions();               // ゲームアイテムの情報を渡す 
		ai();                       // 人工知能
    }
    renderGame();                   //すべてのゲームアイテムを描画	（ポーズであろうがなかろうが）
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
