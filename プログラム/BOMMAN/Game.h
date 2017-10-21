// Programming 2D Games
// Copyright (c) 2011 by: 
// Charles Kelly
// Chapter 5 game.h v1.0

#ifndef _GAME_H                 //このファイルが複数の箇所にインクルードされる場合に
#define _GAME_H                 //多重定義を防ぐ
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <Mmsystem.h>
#include "graphics.h"
#include "input.h"
#include "constants.h"
#include "gameError.h"

class Game
{
protected:
   //メンバー変数
	HWND     hwnd;				//ウィンドウハンドル
	HRESULT	 hr;				//何かの処理をした時の結果を入れる
	LARGE_INTEGER timeStart;	//パフォーマンスカウンタの開始値
	LARGE_INTEGER timeEnd;		//			〃			　終了値
	LARGE_INTEGER timerFreq;	//周波数
	float frameTime;			//最後のフレームに要した時間 
	float fps;					//フレームレート（1秒当たりのフレーム数）
	DWORD sleepTime;			//フレーム間でスリープする時間ms
	bool paused;				//ゲームが一時停止されている場合、true
	bool initialized;

public:
	//コンストラクタ
	Game();
	//デストラクタ
	virtual~Game();
	//メンバー関数

	LRESULT messageHandler(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lparam);	//ダイレクトＸはWindowsを無視するのが基本だけど無視できないときの処理
	//ゲームを初期化
	//hwndはウィンドウへのハンドル
	virtual void initialize(HWND hwnd);		
	//WinMain内のメインのメッセージループでranを繰り返し（1秒間にやる処理がここに入る）
	virtual void run(HWND);
	virtual void releaseAll();		//本体を開放する処理
	virtual void resetAll();		//フルスクリーンに切り替える古期にリセットするとき（？）
	virtual void deleteAll();		//本体削除
	virtual void renderGame();		//画面を描画

	//消失したグラフィックデバイスを処理（ゲーム画面が消失した時の処理）
	virtual void handleLostGraphicsDevice();

    // Set display mode (fullscreen, window or toggle)
    void setDisplayMode(graphicsNS::DISPLAY_MODE mode = graphicsNS::TOGGLE);

    //Graphicsへのポインタを戻す（アクセス関数）
	Graphics* getGraphics() {return graphics;}
	//Inputへのポインタを戻す（アクセス関数）
	Input*    getInput() {return input;}
	//ゲームを終了
	void exitGame() {PostMessage(hwnd,WM_DESTROY,0,0);}
	//純粋仮想関数の宣言
	//これらの関数は、Gameを継承する関数内で記述する必要がある
	//ゲームアイテムを更新
	virtual void update() = 0;	//移動やカウントなど画面に描画するもの以外で処理するもの
	//AI計算を実行
	virtual void ai() = 0;		//aiが必要なとき
	//ゲームアイテムの情報を渡す
	virtual void collisions() = 0;	//ゲームアイテムの情報を渡す
	/*
	//グラフィックスをレンダー
	graphics->SpriteBegin();
	//スプライトを描画
	graphics->SpriteEnd();
	*/
	//   draw non-sprites
	virtual void render() = 0;		//画面に絵を表示
};
#endif	_GAME_H

/*
virtual = 仮想関数　（Gameによってその場所を変更できる）（継承先で変更してもいい）
頭にvirtualがついて、後ろに=0がつくと純粋仮想関数
純粋仮想関数...継承先で変更しなければならない
*/