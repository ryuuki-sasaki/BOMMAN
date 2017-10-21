// Programming 2D Games
// Copyright (c) 2011 by: 
// Charles Kelly
// textureManager.h v1.0

#ifndef _TEXTUREMANAGER_H       // Prevent multiple definitions if this 
#define _TEXTUREMANAGER_H       // file is included in more than one place
#define WIN32_LEAN_AND_MEAN

#include "graphics.h"
#include "constants.h"

class TextureManager
{
    // TextureManager properties
  private:
    UINT       width;       // 幅
    UINT       height;      // 高さ
    LP_TEXTURE texture;     // テクスチャへのポインタ　（LP_TEXTURE～はgraphics.hで定義している)　★
    const char *file;       // ファイル名		//const=定数
    Graphics *graphics;     // グラフィックスへのポインタの保存　★
    bool    initialized;    // 初期化
    HRESULT hr;             // 成功したか失敗したか

  public:
    // Constructor
    TextureManager();

    // Destructor
    virtual ~TextureManager();

    // テクスチャにポインタを返す
    LP_TEXTURE getTexture() const {return texture;}

    // テクスチャの幅を返す
    UINT getWidth() const {return width;}

    // テクスチャの高さを返す
    UINT getHeight() const {return height;}


    // Initialize the textureManager
    // Pre: *g points to Graphics object
    //      *file points to name of texture file to load
    // Post: The texture file is loaded
    virtual bool initialize(Graphics *g, const char *file);

    // Release resources
    virtual void onLostDevice();		//ゲーム終了の時

    // Restore resourses
    virtual void onResetDevice();		//何らかの原因で中断したとき
};

//テクスチャ用移動行列作成関数
void matrixTrans2D(D3DXMATRIX* m, float x, float y);

#endif

