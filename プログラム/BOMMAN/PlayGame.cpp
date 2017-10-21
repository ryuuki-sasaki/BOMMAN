//=============================================================================
//ゲームを扱うクラス
//作成日：2014/09/08
//更新日：2016/10/19
//製作者：佐々木隆貴
//=============================================================================
#include "PlayGame.h"
#include "TitleScene.h"
#include "PlayScene.h"
#include "Camera.h"
#include "Hurricane.h"
#include "vsCpuPlayer.h"
#include "modeChange.h"
#include "SceneBase.h"
#include "audio.h"

#define CHARANUM 3		//キャラクター数

//グローバル変数
GameScene gameScene;			//現在のシーン
Graphics *graphics;				//グラフィック型のクラスポインタ
Input	 *input;				//インプット型のクラスポインタ
Camera   *cameraA, *cameraB;	//カメラ型のクラスポインタ
GameSt	 gameSt;				//現在の状態
GameRs	 gameRs;				//現在の勝敗

enum AI_STATE
{
	INIT,	//初期化
	CALC,	//計算中
	MOVE,	//移動処理
	STAY,	//停止
	NOTAI = 100	//AIを使用しない
};

//=============================================================================
//コンストラクタ
//=============================================================================
PlayGame::PlayGame(void)
{
	gameScene = SC_TITLE;	//現在のシーン
	prevScene = SC_TITLE;	//一つ前のシーン
	sceneChangFlg = FALSE;  //シーン切り替えのフラグ
}

//=============================================================================
//デストラクタ
//=============================================================================
PlayGame::~PlayGame(void)
{
	releaseAll();
	SAFE_DELETE(audio);
}

//=============================================================================
// ゲームの初期化
// 引数：HWND　ウィンドウハンドル
// 戻値：なし
//=============================================================================
void PlayGame::initialize(HWND hwnd)
{
	Game::initialize(hwnd);

	//最初はタイトルシーン
	nowScene = new TitleScene();
	cameraA = new Camera;
	cameraB = new Camera;
	//オーディオ
	audio = new Audio;

	//ロード画面読み込み
	if (!loadTex.initialize(graphics,"pictures\\load.jpg"))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing load texture"));
    if (!loadImg.initialize(graphics,GAME_WIDTH,GAME_HEIGHT,0,&loadTex))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing load"));

	//オーディオ読み込み
	audio->initialize("Sounds\\Wave Bank.xwb", "Sounds\\Sound Bank.xsb");

	//BGM再生
	audio->playCue("JINGLE-22");

	//カメラの設定（１台目）
	D3DXVECTOR3 vCamPos(0,3,-3);	//カメラの位置
	D3DXVECTOR3 vCamTarget(0,0,0);	//カメラの焦点（注視点）
	D3DXVECTOR3 vCamUp(0,1,0);		//カメラの上方向
	cameraA->setPos(vCamPos);
	cameraA->setTarget(vCamTarget);

	//カメラの設定（２台目）
	vCamPos = D3DXVECTOR3(0,3,-3);
	cameraB->setPos(vCamPos);
	cameraB->setTarget(vCamTarget);

	//レンダリング設定
	graphics->get3Ddevice()->
				SetRenderState(D3DRS_ZENABLE, TRUE);		
	graphics->get3Ddevice()->
				SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	//ライト
	graphics->get3Ddevice()->SetRenderState(D3DRS_LIGHTING, TRUE);
	D3DLIGHT9 light;
	D3DLIGHT9 spotLight;
	ZeroMemory(&light, sizeof(D3DLIGHT9));
	ZeroMemory(&spotLight, sizeof(D3DLIGHT9));
	light.Type = D3DLIGHT_DIRECTIONAL;
	spotLight.Type = D3DLIGHT_SPOT;

	light.Diffuse.r = 1.0f; 
	light.Diffuse.g = 1.0f; 
	light.Diffuse.b = 1.0f; 
	light.Diffuse.a = 1.0f; 
	spotLight.Diffuse.r = 1.0f; 
	spotLight.Diffuse.g = 1.0f; 
	spotLight.Diffuse.b = 1.0f; 
	spotLight.Diffuse.a = 1.0f; 

	light.Ambient.r = 1.0f;
	light.Ambient.g = 1.0f;
	light.Ambient.b = 1.0f;
	light.Ambient.a = 1.0f;
	spotLight.Ambient.r = 1.0f;
	spotLight.Ambient.g = 1.0f;
	spotLight.Ambient.b = 1.0f;
	spotLight.Ambient.a = 1.0f;

	light.Specular.r = 1.0f;
	light.Specular.g = 1.0f;
	light.Specular.b = 1.0f;
	light.Specular.a = 1.0f;
	spotLight.Specular.r = 1.0f;
	spotLight.Specular.g = 1.0f;
	spotLight.Specular.b = 1.0f;
	spotLight.Specular.a = 1.0f;

	light.Direction = D3DXVECTOR3(1, -1, 1);
	spotLight.Direction = D3DXVECTOR3(1, -1, 1);
	light.Range = 200.0f;
	spotLight.Range = 50.0f;
	
	graphics->get3Ddevice()->SetLight(0, &light);
	graphics->get3Ddevice()->SetLight(1, &spotLight);
	graphics->get3Ddevice()->LightEnable(0, TRUE);
	graphics->get3Ddevice()->LightEnable(1, TRUE);
}

//=============================================================================
// すべてのゲームアイテムの更新
// 引数：なし
// 戻値：なし
//=============================================================================
void PlayGame::update()
{
	//現在のシーンをアップデート
	nowScene->update();	

	//シーンが変わった(true)
	if(sceneChangFlg == TRUE)		
	{
		//代入しているSceneはSceneBaseを継承しているからSceneBase型のnowSceneに代入可能
		//シーンの切り替え
		switch (gameScene)			
		{
		case SC_TITLE:
			SAFE_DELETE(nowScene);		//現在のシーンの削除
			audio->stopCue("Jazz 1");		//BGM停止
			audio->playCue("JINGLE-22");	//BGM再生
			nowScene = new TitleScene();
			break;

		case SC_PLAY:
			SAFE_DELETE(nowScene);		//現在のシーンの削除			
			audio->stopCue("JINGLE-22");	//BGM停止
			audio->playCue("Jazz 1");		//BGM再生
			playSceneCamera();
			nowScene = new PlayScene();
			break;

		case SC_INIT:
			gameScene = SC_PLAY;
			nowScene->initialize();
			break;
		}
		prevScene = gameScene;		//現在の表示を１つ前の表示へ
		sceneChangFlg = FALSE;		
	}	
}

//=============================================================================
// AI人工知能
// 引数：なし
// 戻値：なし
//=============================================================================
void PlayGame::ai()
{
	nowScene->ai();
}

//=============================================================================
//衝突を処理
// 引数：なし
// 戻値：なし
//=============================================================================
void PlayGame::collisions()
{
	nowScene->collisions();
}

//=============================================================================
// ゲームアイテムをレンダー
// 引数：なし
// 戻値：なし
//=============================================================================
void PlayGame::render()		
{
	//シーンが変わった
	if(gameScene != prevScene)
	{
		//Scene遷移間のロード画面描画
		loadImg.draw();			
		sceneChangFlg = TRUE;
		return;					//このフレームの描画をストップ
	}

	//現在のシーンの描画
	nowScene->render(frameTime);	
}

//=============================================================================
// グラフィックスデバイスが消失したとき
// 引数：なし
// 戻値：なし
//=============================================================================
//メモ　-グラフィックデバイスが消失した場合
//グラフィックデバイスをリセット可能にするため
//予約されていたビデオメモリをすべて開放-
void PlayGame::releaseAll()
{
	Game::releaseAll();
	SAFE_DELETE(nowScene);
	SAFE_DELETE(cameraA);
	SAFE_DELETE(cameraB);
	return;
}

//=============================================================================
// グラフィックスデバイスがリセットされたら
// 引数：なし
// 戻値：なし
//=============================================================================
//メモ　-グラフィックデバイスがリセットされた場合
//すべてのサーフェスを再作成-
void PlayGame::resetAll()
{
	Game::resetAll();
	return;
}

//=============================================================================
// プレイシーンのカメラの設定
// 引数：なし
// 戻値：なし
//=============================================================================
void PlayGame::playSceneCamera()
{
	//2P対戦なら
	if(modeChange::GetInstance() -> getMode() == VS2P)
	{
		cameraA->changeViewport(0, 0, GAME_WIDTH / 2, GAME_HEIGHT);					//画面を左半分にする
		cameraB->changeViewport(GAME_WIDTH / 2, 0, GAME_WIDTH / 2, GAME_HEIGHT);	//画面を右半分にする
	} else /*CPU対戦なら*/{
		cameraA->changeViewport(0, 0, GAME_WIDTH, GAME_HEIGHT);		
		cameraB->changeViewport(NULL, NULL, NULL, NULL);	//画面を右半分にする
	}	

	return;
}

