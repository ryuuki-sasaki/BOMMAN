// Programming 2D Games
// Copyright (c) 2011 by:
// Charles Kelly
// Chapter 3 graphics.cpp v1.0

#include "graphics.h"

//=============================================================================
// コンストラクタ
//=============================================================================
Graphics::Graphics()
{
    direct3d = NULL;		//クラスのポインタがある場合はコンストラクタでＮＵＬＬ
    device3d = NULL;
	sprite = NULL;
    fullscreen = false;		
    width = GAME_WIDTH;    // width & height を initialize() で書き換え
    height = GAME_HEIGHT;
	backColor = SETCOLOR_ARGB(255, 0, 0, 128);		//背景色
}

//=============================================================================
// デストラクタ
//=============================================================================
Graphics::~Graphics()
{
    releaseAll();		//解放	
}

//=============================================================================
// すべて開放
//=============================================================================
void Graphics::releaseAll()
{
	SAFE_RELEASE(sprite);
    SAFE_RELEASE(device3d);		//画面の開放
    SAFE_RELEASE(direct3d);		//本体の開放（開放は本体が最後）
}

//=============================================================================
// DirectX graphics を初期化
// エラー時 GameError をスロー 
//=============================================================================
void Graphics::initialize(HWND hw, int w, int h, bool full)
{
    hwnd = hw;
    width = w;
    height = h;
    fullscreen = full;

    //Direct3Dを初期化
    direct3d = Direct3DCreate9(D3D_SDK_VERSION);		//本体を作成
    if (direct3d == NULL)								//作成が失敗だったら
        throw(GameError(gameErrorNS::FATAL_ERROR, "Direct3Dの初期化に失敗"));	//呼び出し元にthrow　GameErrorは引数2つ

    initD3Dpp();       // D3D presentation parameters を初期化
	if(fullscreen)     // フルスクリーンモードなら
    {
        if(isAdapterCompatible())   // is the adapter compatible
            // set the refresh rate with a compatible one
            d3dpp.FullScreen_RefreshRateInHz = pMode.RefreshRate;
        else
            throw(GameError(gameErrorNS::FATAL_ERROR, 
            "The graphics device does not support the specified resolution and/or format."));
    }

     // graphics card が hardware texturing and lighting and vertex shaders をサポートしてる時機能する
    D3DCAPS9 caps;
    DWORD behavior;
    result = direct3d->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps);
    // device が HW T&L か 1.1 vertex shaders in hardwareを
    // サポートしていないとき　software vertex processingに切り替える
    if( (caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) == 0 ||
            caps.VertexShaderVersion < D3DVS_VERSION(1,1) )
        behavior = D3DCREATE_SOFTWARE_VERTEXPROCESSING;  // software だけが使う処理
    else		
		//HARDWAREで基本は対応できるが、対応していない場合SOFTWARE
        behavior = D3DCREATE_HARDWARE_VERTEXPROCESSING;  // hardware だけが使う処理
	
	/*
    if (FAILED(result))		//失敗だったら
        throw(GameError(gameErrorNS::FATAL_ERROR, "Direct3D device（ゲーム画面）の作成失敗"));
	*/
	//Direct3Dデバイスを作成
	result = direct3d->CreateDevice(
		D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		hwnd,
		behavior,
		&d3dpp,
		&device3d);

	  if (FAILED(result))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error creating Direct3D device"));
 
	  result = D3DXCreateSprite(device3d, &sprite);
	  if (FAILED(result))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error creating Direct3D sprite"));
	
	if(FAILED(result))
		throw(GameError(gameErrorNS::FATAL_ERROR,
			"Direct3ddeviceの作成に失敗しました"));

		result = D3DXCreateSprite(device3d,&sprite);

		if(FAILED(result))
			throw(GameError(gameErrorNS::FATAL_ERROR,
			"Direct3ddeviceの作成に失敗しました"));

		//アルファブレンド
		device3d->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		device3d->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
		device3d->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
		//アルファテスト（一定の透明度以下なら描画しない）
		device3d->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
		device3d->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
		device3d->SetRenderState(D3DRS_ALPHAREF, 0);				//透明度が0なら描画しない

}

//=============================================================================
// Initialize D3D presentation parameters
//=============================================================================
void Graphics::initD3Dpp()
{
    try{												
        ZeroMemory(&d3dpp, sizeof(d3dpp));              // fill the structure with 0
        // fill in the parameters we need
        d3dpp.BackBufferWidth   = width;
        d3dpp.BackBufferHeight  = height;
        if(fullscreen)                                  // if fullscreen
            d3dpp.BackBufferFormat  = D3DFMT_X8R8G8B8;  // 24 bit color
        else
            d3dpp.BackBufferFormat  = D3DFMT_UNKNOWN;   // use desktop setting
        d3dpp.BackBufferCount   = 1;
        d3dpp.SwapEffect        = D3DSWAPEFFECT_DISCARD;
        d3dpp.hDeviceWindow     = hwnd;
        d3dpp.Windowed          = (!fullscreen);
        d3dpp.PresentationInterval   = D3DPRESENT_INTERVAL_IMMEDIATE;
		//Zバッファを使うか使わないか
		d3dpp.EnableAutoDepthStencil = TRUE;
		//Zバッファの設定
		d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    } catch(...)		//失敗したら
    {
        throw(GameError(gameErrorNS::FATAL_ERROR,		//throw（）
                "D3D presentation parameters初期化失敗"));
    }
}

//=============================================================================
//テクスチャをデフォルトのD3Dメモリに読み込む（通常のテクスチャで使用）
//エンジン使用時に内部的にのみ使用する
//ゲームテクスチャの読み込みには、TextureManagerクラスを使用する
//実行前：filenameは、テクスチャファイルの名前
//		　transcolorは透明として扱う色
//実行後：widthとheight = テクスチャの寸法
//		  textureはテクスチャを指す
//HRESULTを戻す
//=============================================================================

HRESULT Graphics::loadTexture(const char *filename, COLOR_ARGB transcolor,
                                UINT &width, UINT &height, LP_TEXTURE &texture)
{
    // ファイル情報を読み込むための構造体
    D3DXIMAGE_INFO info;
    result = E_FAIL;

    try{
        if(filename == NULL)
        {
            texture = NULL;
            return D3DERR_INVALIDCALL;
        }
    
        // 幅と高さをファイルから取得
        result = D3DXGetImageInfoFromFile(filename, &info);
        if (result != D3D_OK)
            return result;
        width = info.Width;
        height = info.Height;
    
        // ファイルを読み込んで、新しいテクスチャを作成
        result = D3DXCreateTextureFromFileEx( 
            device3d,           //3D デバイス
            filename,           //画像ファイルの名前
            info.Width,         //テクスチャの幅
            info.Height,        //テクスチャの高さ
            1,                  //mip-map levels (1 for no chain)
            0,                  //usage
            D3DFMT_UNKNOWN,     //サーフェスフォーマット (default)
            D3DPOOL_DEFAULT,    //テクスチャ用のメモリの種類
            D3DX_DEFAULT,       //画像フィルタ
            D3DX_DEFAULT,       //mip filter
            transcolor,         //透明用の色キー
            &info,              //bitmap file info (from loaded file)
            NULL,               //カラーパレット
            &texture );         //上手くいけばtextureという変数に呼び込んだ画像の情報が入る

    } catch(...)
    {
        throw(GameError(gameErrorNS::FATAL_ERROR, "Graphics::loadTextureは失敗しました"));
    }
    return result;
}
//=============================================================================
// Display the backbuffer
//=============================================================================
HRESULT Graphics::showBackbuffer()
{
    result = E_FAIL;     // default to fail, replace on success
	// Display backbuffer to screen
    /*
	(this function will be moved in later versions)
    Clear the backbuffer to lime green 
    */
    result = device3d->Present(NULL, NULL, NULL, NULL);		//失敗したか否か
	return result;
}

//=============================================================================
// Checks the adapter to see if it is compatible with the BackBuffer height,
// width and refresh rate specified in d3dpp. Fills in the pMode structure with
// the format of the compatible mode, if found.
// Pre: d3dpp is initialized.
// Post: Returns true if compatible mode found and pMode structure is filled.
//       Returns false if no compatible mode found.
//=============================================================================
bool Graphics::isAdapterCompatible()
{
    UINT modes = direct3d->GetAdapterModeCount(D3DADAPTER_DEFAULT, 
                                            d3dpp.BackBufferFormat);
    for(UINT i=0; i<modes; i++)
    {
        result = direct3d->EnumAdapterModes( D3DADAPTER_DEFAULT, 
                                        d3dpp.BackBufferFormat,
                                        i, &pMode);
        if( pMode.Height == d3dpp.BackBufferHeight &&
            pMode.Width  == d3dpp.BackBufferWidth &&
            pMode.RefreshRate >= d3dpp.FullScreen_RefreshRateInHz)
            return true;
    }
    return false;
}

//=============================================================================
// Draw the sprite described in SpriteData structure
// Color is optional, it is applied like a filter, WHITE is default (no change)
// Pre : sprite->Begin() is called
// Post: sprite->End() is called
// spriteData.rect defines the portion of spriteData.texture to draw
//   spriteData.rect.right must be right edge + 1
//   spriteData.rect.bottom must be bottom edge + 1
//=============================================================================
void Graphics::drawSprite(const SpriteData &spriteData, COLOR_ARGB color)
{
    if(spriteData.texture == NULL)      // テクスチャがない場合
        return;

    // sprite　の中心を特定
    D3DXVECTOR2 spriteCenter=D3DXVECTOR2((float)(spriteData.width/2*spriteData.scale),
                                        (float)(spriteData.height/2*spriteData.scale));
    // sprite の画面位置
    D3DXVECTOR2 translate=D3DXVECTOR2((float)spriteData.x,(float)spriteData.y);
    // X,Y　の拡大縮小
    D3DXVECTOR2 scaling(spriteData.scale,spriteData.scale);
    if (spriteData.flipHorizontal)  // 水平反転する場合
    {
        scaling.x *= -1;            // X を負にして反転
        // 反転された画像の中心取得
        spriteCenter.x -= (float)(spriteData.width*spriteData.scale);
        // 左端を中心に反転が起きるので反転された画像を
        // 元と同じ位置にするため右に移動
        translate.x += (float)(spriteData.width*spriteData.scale);
    }
    if (spriteData.flipVertical)    // 垂直反転する場合
    {
        scaling.y *= -1;            // Y を負にして反転
        // 反転された画像の中心取得
        spriteCenter.y -= (float)(spriteData.height*spriteData.scale);
        // 上端を中心に反転が起きるので反転された画像を
        // 元と同じ位置にするため下に移動
        translate.y += (float)(spriteData.height*spriteData.scale);
    }
    // sprite の回転、拡大縮小、配置を行うための配列を作成
    D3DXMATRIX matrix;
    D3DXMatrixTransformation2D(
        &matrix,                // 行列
        NULL,                   // 拡大縮小を行うとき原点は左上のまま
        0.0f,                   // 拡大縮小時の回転なし
        &scaling,               // 拡大縮小の倍率
        &spriteCenter,          // 回転中心
        (float)(spriteData.angle),  // 回転角度
        &translate);            // X,Y 位置

	//画像を奥へ
	if( spriteData.isBackGround == true )
	{
		D3DXMATRIX mTrans;
		D3DXMatrixTranslation(&mTrans, 0, 0, 1);
		matrix *= mTrans;
	}

    // 行列をスプライトに適用
    sprite->SetTransform(&matrix);

    // スプライトを描画
    sprite->Draw(spriteData.texture,&spriteData.rect,NULL,NULL,color);
}

//=============================================================================
//グラフィックスが消失していないかをチェック
//=============================================================================
HRESULT Graphics::getDeviceState()
{ 
    result = E_FAIL;    // 失敗をデフォルトとし、成功時に置き換え
    if (device3d == NULL)
        return  result;
    result = device3d->TestCooperativeLevel();	//問題なければD3D_Okが入る　ダメならその他入る
    return result;
}

//=============================================================================
//グラフィックスデバイスをリセット
//=============================================================================
HRESULT Graphics::reset()
{
    result = E_FAIL;    // 失敗をデフォルトとし、成功時に置き換え
    initD3Dpp();        // D3Dプレゼンテーションパラメータを初期化
	sprite->OnLostDevice();
    result = device3d->Reset(&d3dpp);   // グラフィックスデバイスのリセットを試みる

    sprite->OnResetDevice();
    return result;
}

//=============================================================================
// Toggle window or fullscreen mode
// Pre: All user created D3DPOOL_DEFAULT surfaces are freed.
// Post: All user surfaces are recreated.
//=============================================================================
void Graphics::changeDisplayMode(graphicsNS::DISPLAY_MODE mode)
{
    try{
        switch(mode)
        {
        case graphicsNS::FULLSCREEN:
            if(fullscreen)      // if already in fullscreen mode
                return;
            fullscreen = true; break;
        case graphicsNS::WINDOW:
            if(fullscreen == false) // if already in window mode
                return;
            fullscreen = false; break;
        default:        // default to toggle window/fullscreen
            fullscreen = !fullscreen;
        }
        reset();
        if(fullscreen)  // fullscreen
        {
            SetWindowLong(hwnd, GWL_STYLE,  WS_EX_TOPMOST | WS_VISIBLE | WS_POPUP);
        }
        else            // windowed
        {
            SetWindowLong(hwnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);
            SetWindowPos(hwnd, HWND_TOP, 0,0,GAME_WIDTH,GAME_HEIGHT,
                SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);

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

    } catch(...)
    {
        // An error occured, try windowed mode 
        SetWindowLong(hwnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);
        SetWindowPos(hwnd, HWND_TOP, 0,0,GAME_WIDTH,GAME_HEIGHT,
            SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);

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
}


/*
void Graphics::drawSprite(LP_TEXTURE texture)
{
	RECT r = { 0,0,200,200 };
	sprite->Draw(texture,&r,NULL,NULL,D3DCOLOR_ARGB(255,255,255,255) );
}
*/