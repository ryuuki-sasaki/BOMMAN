//=============================================================================
//  プレイヤー(2P対専用)
//　更新日：2016/10/30
//	制作者：佐々木隆貴
//=============================================================================
#pragma once
#include "Player.h"
#include "constants.h"
#include "Poly.h"
#include "textureManager.h"
#include <list>
using namespace std;

class normalBom;
class Wall;
class NormalCpu;
class Model;
class Image;

class VsPlayer : public Player
{
public:
	D3DXVECTOR3      pos;								//位置
	bool			 isStop;							//動作を停止するかどうか
	bool			 isLose;							//死亡判定フラグ
	list<int>		 plusBomNumList;					//プラスボムの個数を格納する配列
	list<int>		 paralyzeBomNumList;				//痺れボムの個数を格納する配列
	list<int>		 bomThrowNumList;					//痺れボムの個数を格納する配列
	list<int>::iterator	plusBomitr;						//イテレータ作成
	list<int>::iterator	paralyzeitr;					//イテレータ作成
	list<int>::iterator	bomThrowitr;					//イテレータ作成
	bool			 isShake;							//カメラを揺らすか
	int				 player;							//プレイヤー番号

protected:
	struct key
	{
		//移動キー
		unsigned int MOVE_UP;
		unsigned int MOVE_DOWN;
		unsigned int MOVE_RIGHT;
		unsigned int MOVE_LEFT;

		//コマンド選択
		unsigned int COMMAND_SELECT_RIGHT;
		unsigned int COMMAND_SELECT_LEFT;

		//ボムを置く
		unsigned int PUT_BOM;
		unsigned int THROW;
	}KEY;

	Model*			 model;								//クラス型のオブジェクトを作ったときポインタにするとかっこいい（deleteを忘れない）		
	D3DXVECTOR3      move;								//移動ベクトル
	bool			 iskeyState;						//キーを押し続けるとその処理がフレームごとに行われるため、一度押したら一度しか処理しないように制御する
	int				 commandSelect;						//どのコマンドを選択したか
	float			 angle;								//角度
	D3DXVECTOR3		 bomThrowPos;						//投げる先の位置
	D3DXVECTOR3		 bomThrowEffectPos;					//投げる先を表すエフェクトの位置
	bool			 throwFlag;							//投げる処理か？
	bool			 effectRenderFlag;					//投げる先のエフェクトを表示するかどうか
	bool			 isItem;							//アイテムボムを選択したかどうかを判定
	int				 itemType;							//アイテムの種類
	int				 processBomType;					//処理中のボム
	int				 stopCount;							//停止時間のカウント
	int				 putBomPlayer;						//ボムを置いたプレイヤーの識別(2P対戦時用)
	TextureManager	 texBom;							//ボムテクスチャ
	Image*		     bomimage;							//イメージ
	TextureManager	 texPlusBom;						//プラスボムテクスチャ
	Image*		     plusBomimage;						//イメージ
	TextureManager	 texArrow;							//矢印テクスチャ
	Image*		     arrowimage;						//イメージ
	TextureManager	 texParalyzeBom;					//矢印テクスチャ
	Image*		     paralyzeBomimage;					//イメージ
	TextureManager	 texThrow;							//矢印テクスチャ
	Image*		     throwimage;						//イメージ
	TextureManager	 texItemNum;						//アイテム数用数字テクスチャ
	Image*		     leftItemNumimage;					//イメージ
	Image*		     rightItemNumimage;					//イメージ
	Poly		     BomThrowPosEffectPoly;				//画像を貼るポリゴン
	TextureManager	 texBomThrowPosEffect;				//ボムを投げる先を示すテクスチャ
	int				 moveControlCount;					//キャラクターが書道するまでの遊びの時間
	D3DXVECTOR3		 shakePos;							//カメラをゆらした後の位置
	float			 minRange;							//爆発位置とプレイヤー位置の距離の中で最も短い長さ
	int				 shakeTime;							//カメラを揺らす時間
	float			 shakeDir;							//カメラを揺らす方向
	int				 addExpRange;						//爆発範囲の加算分
	bool			 isPushMoveButton;					//移動ボタンを押しているか
	normalBom*		 bomUnit[3];						//各種ボムオブジェクトの情報を格納する配列		
	Wall*			 wallUnit;							//ウォールユニットの情報を格納する変数
	NormalCpu*		 cpuUnit[3];						//CPUユニットの情報を格納する変数
	VsPlayer		 *player1, *player2;				//対人プレイヤーユニットの情報を格納する変数

public: 
	VsPlayer(int playerNum);
	~VsPlayer();
	void  render(float frameTime, int cameraNum);		//描画処理
	void  update();										//更新処理
	void setShakeCameraInfo(D3DXVECTOR3 expPos, float shakeSize);	//カメラを揺らす情報セット

protected:
	void initialize();						
	void initList();									//リストの初期化
	void control();										//移動
	void cameraCtrl();									//カメラ操作
	void inRange();										//プレイヤーがフィールドの範囲内か
	bool collObstacel(D3DXVECTOR3 pos, bool isThrow);	//障害物への衝突判定
	void selectThrowPos();								//ボムを投げる先を設定
	void getItem();										//アイテム取得
	bool isBomListState(int	itenType);					//ボムリストが空かどうか
	void bomProcess();									//ボムに対する処理
	void notRenderBom();								//描画中のボムがあるか
	void renderNumber();								//アイテム数表示
	void processedCommand(int command);					//使用コマンドの後処理
	void selectCommand();								//コマンド選択
	void shakeCamera();									//カメラを揺らす
	void plusExpRange() { addExpRange++; }				//爆発範囲の加算
	void adjustmentPos();								//位置を整数にする
	Model* GetModel(){return model;}					//アクセス関数
	float toIntMove(float move) {return (move<0) ? (float)(-1.0 * floor(fabs(move) + 0.5)) : (float)(floor(move + 0.5));}	//Moveの長さを整数にする
	float toIntPos(float pos) {return (pos<0) ? (float)(-1.0 * floor(fabs(pos) + 0.5)) : (float)(floor(pos + 0.5));}		//位置を整数にする	
	void collisions(UnitBase* target);					//ユニットの情報を取得
};

