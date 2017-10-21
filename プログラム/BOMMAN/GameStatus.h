//=============================================================================
//  ゲームの状態管理
//　作成日：2015/8/22
//　更新日：2016/10/30
//	制作者：佐々木隆貴
//=============================================================================
#pragma once
#include "UnitBase.h"
#include "constants.h"
#include "textureManager.h"

class vsCpuPlayer;
class VsPlayer;
class NormalCpu;
class Wall;
class Image;

class gameStatus : public UnitBase
{
	//カーソル位置
	enum Cursor
	{
		RESUME,
		RETRY,
		EXIT
	}cursor;

	//クラス作成
	//テクスチャ
	TextureManager texMenu;		//ポーズメニューテクスチャ
	Image	*menuImg;			
	TextureManager texCursor;	//カーソルテクスチャ
	Image	*cursorImg;
	TextureManager texs_Cursor;	//スモールカーソルテクスチャ
	Image	*s_cursorImg;
	TextureManager texResult;	//リザルトテクスチャ
	Image	*resultImg;
	TextureManager texString;	//勝敗文字テクスチャ
	Image	*stringImg;
	TextureManager texNum;		//ナンバーテクスチャ
	Image	*numImg, *numImg2, *numImg3;
	DWORD timeCount;			//ゲーム開始時間
	int set1;
	int set2;
	int set3;		
	vsCpuPlayer*	vsCpuPlayerUnit;	//対CPUプレイヤーユニットの情報を格納する変数
	NormalCpu*		cpuUnit[3];			//CPUユニットの情報を格納する変数
	Wall*			wallUnit;			//ウォールユニットの情報を格納する変数
	VsPlayer		*player1, *player2;	//対人プレイヤーユニットの情報を格納する変数

public:
	gameStatus(void);
	~gameStatus(void);
	void initialize();
	void render(float frameTime, int cNum);
	void update();

private:
	void resultRender(int cNum);
	void Pause();
	void End();
	void gameTimeCount();
	void loseDecision();		//死亡判定
	void collisions(UnitBase* target);	//ユニットの情報を取得	
};

