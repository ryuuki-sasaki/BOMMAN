//=============================================================================
//ゲームを扱うクラス
//作成日：2014/09/08
//更新日：2016/10/19
//製作者：佐々木隆貴
//=============================================================================
#pragma once
#include "textureManager.h"
#include "image.h"
#include "game.h"

class SceneBase;
class Audio;

class PlayGame : public Game
{
private:
	SceneBase* nowScene;	//現在のシーン(SceneBase*)
	GameScene  prevScene;	//前のシーン  (GameScene)
	Audio*	   audio;		//オーディオ

	//ロード画面
	TextureManager loadTex;			//ロードテクスチャ
	Image          loadImg;			//イメージ

	BOOL           sceneChangFlg;	//チェンジ注はtrue

public:
	//コンストラクタ
	PlayGame(void);
	//デストラクタ
	virtual ~PlayGame(void);
	//ゲームを初期化
	void initialize(HWND hwnd);
	void update();			//Gameからの純粋仮想関数をオーバーライドする
	void ai();				//同じく
	void collisions();		//同じく
	void render();			//同じく
	void releaseAll();
	void resetAll();
	void playSceneCamera();
};

