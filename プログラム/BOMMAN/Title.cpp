//=============================================================================
//タイトルを扱うクラス
//作成日：2014/9/02
//更新日：2014/8/30
//製作者：佐々木隆貴
//=============================================================================
#include "Title.h"
#include "input.h"
#include "modeChange.h"
#include "option.h"
#include "audio.h"

#define MODENUM			1		//対戦モードの数
#define PLUSSIZE		10		//位置の加算分
#define STAYTIME		30		//待機時間30fps(=1秒間に30フレーム)
#define ADDSIZEX 480 / 2
#define ADDSIZEY 360 / 4

//=============================================================================
//コンストラクタ
//=============================================================================
Title::Title(void)
{
	//オーディオ
	audio = new Audio;

	//オーディオ読み込み
	audio->initialize("Sounds\\Wave Bank.xwb", "Sounds\\Sound Bank.xsb");	

	//ロード画面読み込み
	if (!texture.initialize(graphics,"pictures\\title.png"))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing title texture"));
    if (!image.initialize(graphics,GAME_WIDTH,GAME_HEIGHT,0,&texture))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing title"));

	if (!texCursor.initialize(graphics,"pictures\\cursor.png"))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing select bom texture"));
    if (!imageCursor.initialize(graphics,0,0,0,&texCursor))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error selectBom"));

	if (!texs_Cursor.initialize(graphics,"pictures\\s_cursor.png"))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing s_Cursor texture"));
    if (!images_Cursor.initialize(graphics,0,0,0,&texs_Cursor))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing s_Cursor"));

	if (!texOption.initialize(graphics,"pictures\\option.png"))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing option texture"));
    if (!imageOption.initialize(graphics,0,0,0,&texOption))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing option"));

	initialize();
}

//=============================================================================
// 初期化
// 引数： なし
// 戻り値：なし
//=============================================================================
void Title::initialize()
{
	//テクスチャのポジションセット
	imageCursor.setX(90);
	imageCursor.setY(390+ADDSIZEY*2);

	imageOption.setX(300+ADDSIZEX);
	imageOption.setY(200+ADDSIZEY);

	cursor = 0;
	optionCursor = 0;
	isSet = false;
}

//=============================================================================
//デストラクタ
//=============================================================================
Title::~Title(void)
{
	texture.onLostDevice();
	SAFE_DELETE(audio);
}

//=============================================================================
//機能：画像を描画
//引数：なし
//戻値：なし
//=============================================================================
void Title::render(float frameTime, int cameraNum)
{	
	//テクスチャ描画
	image.draw();
	imageCursor.draw();

	if(isSet) {
		imageOption.draw();
		images_Cursor.draw();
	}
}

//=============================================================================
//機能：スペースキーが押されたらプレイシーンへ
//引数：なし
//戻値：なし
//=============================================================================
void Title::update()
{		
	if(!isSet) {
		//対戦モード選択
		if(input->wasKeyPush(VK_RIGHT) || input->wasGamePadPush(1, GAMEPAD_DPAD_RIGHT))
		{	
			//BGM再生
			audio->playCue("カーソル音2");
			switch(cursor)
			{
			case MODE::VS2P:
				modeChange::GetInstance() -> setMode(VSCPU);
				cursor = MODE::VSCPU;
				imageCursor.setX(450);
				break;
			case MODE::VSCPU:
				modeChange::GetInstance() -> setMode(OPTION);
				cursor = MODE::OPTION;
				imageCursor.setX(840);
				break;
			case MODE::OPTION:
				modeChange::GetInstance() -> setMode(VS2P);
				cursor = MODE::VS2P;
				imageCursor.setX(90);
				break;
			}
		}

		if(input->wasKeyPush(VK_LEFT) || input->wasGamePadPush(1, GAMEPAD_DPAD_LEFT))
		{
			//BGM再生
			audio->playCue("カーソル音2");
			switch(cursor)
			{
			case MODE::VS2P:
				modeChange::GetInstance() -> setMode(OPTION);
				cursor = MODE::OPTION;
				imageCursor.setX(840);
				break;
			case MODE::VSCPU:
				modeChange::GetInstance() -> setMode(VS2P);
				cursor = MODE::VS2P;
				imageCursor.setX(90);
				break;
			case MODE::OPTION:
				modeChange::GetInstance() -> setMode(VSCPU);
				cursor = MODE::VSCPU;
				imageCursor.setX(450);
				break;
			}
		}

		if(input->wasKeyPush(VK_RETURN) || input->wasGamePadPush(1, GAMEPAD_B))		
		{
			//BGM再生
			audio->playCue("カーソル音1");
		
			if(modeChange::GetInstance() -> getMode() <= MODENUM)
				gameScene = SC_PLAY;
			if(modeChange::GetInstance() -> getMode() == OPTION)
				images_Cursor.setX(320+ADDSIZEX);
				images_Cursor.setY(265+ADDSIZEY);
				optionCursor = 0;
				isSet = true;
		}
	}

	if(isSet) {
		setOption();
	}
}

//=============================================================================
//機能：オプション設定
//引数：なし
//戻値：なし
//=============================================================================
void Title::setOption()
{
	if(input->wasKeyPush(VK_DOWN) || input->wasGamePadPush(1, GAMEPAD_DPAD_DOWN))
	{	
		//BGM再生
		audio->playCue("カーソル音2");
		switch(optionCursor)
		{
		case OPTION::NOSELECT:
			option::GetInstance() -> setMode(THUNDER);
			optionCursor = OPTION::THUNDER;
			images_Cursor.setY(290+ADDSIZEY);
			break;
		case OPTION::THUNDER:
			option::GetInstance() -> setMode(HURRICANE);
			optionCursor = OPTION::HURRICANE;
			images_Cursor.setY(320+ADDSIZEY);
			break;
		case OPTION::HURRICANE:
			option::GetInstance() -> setMode(ALLSELECT);
			optionCursor = OPTION::ALLSELECT;
			images_Cursor.setY(345+ADDSIZEY);
			break;
		case OPTION::ALLSELECT:
			option::GetInstance() -> setMode(NOSELECT);
			optionCursor = OPTION::NOSELECT;
			images_Cursor.setY(265+ADDSIZEY);
			break;
		}
	}

	if(input->wasKeyPush(VK_UP) || input->wasGamePadPush(1, GAMEPAD_DPAD_UP))
	{
		//BGM再生
		audio->playCue("カーソル音2");
		switch(optionCursor)
		{
		case OPTION::NOSELECT:
			option::GetInstance() -> setMode(ALLSELECT);
			optionCursor = OPTION::ALLSELECT;
			images_Cursor.setY(345+ADDSIZEY);
			break;
		case OPTION::THUNDER:
			option::GetInstance() -> setMode(NOSELECT);
			optionCursor = OPTION::NOSELECT;
			images_Cursor.setY(265+ADDSIZEY);
			break;
		case OPTION::HURRICANE:
			option::GetInstance() -> setMode(THUNDER);
			optionCursor = OPTION::THUNDER;
			images_Cursor.setY(290+ADDSIZEY);
			break;
		case OPTION::ALLSELECT:
			option::GetInstance() -> setMode(HURRICANE);
			optionCursor = OPTION::HURRICANE;
			images_Cursor.setY(320+ADDSIZEY);
			break;
		}
	}

	if(input->wasKeyPush(VK_RETURN) || input->wasGamePadPush(1, GAMEPAD_B))		
	{
		//BGM再生
		audio->playCue("カーソル音1");
		isSet = false;
	}
}