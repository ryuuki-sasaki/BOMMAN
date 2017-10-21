//=============================================================================
//タイトルを扱うクラス
//作成日：2014/09/02
//更新日：2014/09/02
//製作者：佐々木隆貴
//=============================================================================
#pragma once	//二重定義防止（VSのみ）
#include "constants.h"	
#include "textureManager.h"
#include "image.h"
#include "UnitBase.h"

class Audio;

class Title : public UnitBase
{
	Audio*	   audio;		//オーディオ
	//テクスチャ
	TextureManager texture;
	TextureManager texCursor;
	TextureManager texs_Cursor;
	TextureManager texOption;
	//イメージ
	Image		   image;
	Image		   imageCursor;
	Image		   images_Cursor;	
	Image		   imageOption;	

	int			   cursor;
	int			   optionCursor;
	bool		   isSet;

public:
	Title(void);						//コンストラクタは最初の1回しか呼ばれない（プレイヤーの初期位置をコンストラクタで定義した場合1回クリアしたものを2回目プレイしようとするとすでにゴールしている）
	~Title(void);						//デストラクタ
	void initialize();					//初期化
	void render(float frameTime, int cameraNum);		//描画
	void update();						//キーが押されたら次のシーンへ
	void setOption();					//オプションの設定
};

