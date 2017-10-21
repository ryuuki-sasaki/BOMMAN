//=============================================================================
//  NormalCpu
//　更新日：2016/10/30
//	制作者：佐々木隆貴
//=============================================================================
#pragma once
#include "Cpu.h"
#include "constants.h"
#include <list>
#include <vector>
using namespace std;

class vsCpuPlayer;
class normalBom;
class Wall;
class Model;
class readFile;

#define	COMMAND_NUM 3	//コマンド数
#define POINT 9			//経過ポイント数

class NormalCpu :
	public Cpu
{
public:
	D3DXVECTOR3  pos;	//位置
	bool		isLose; //死亡したかどうかの判定フラグ
	list<int>		 plusBomNumList;		//プラスボムの個数を格納する配列
	list<int>		 paralyzeBomNumList;	//痺れボムの個数を格納する配列
	list<int>		 bomThrowNumList;		//スローボムの個数を格納する配列
	bool            isStop;					//動作を停止するかどうか

protected:
	struct aStar
	{
		D3DXVECTOR3	tilePos;							//位置
		int			cost;								//コスト
		int			heuristic;							//ヒューリスティック
		int			score;								//スコア
		aStar*		parentTile;							//親タイルのポインタ
	};

	struct prevTileInfo
	{
		D3DXVECTOR3 prevTilePos;
		int			prevTileDir;
	};

	list<aStar>		 openList;							//オープンリスト
	list<aStar>		 closeList;							//クローズリスト	
	list<aStar>::iterator openListitr;					//イテレータ作成
	list<aStar>::iterator closeListitr;
	list<D3DXVECTOR3> parentList;						//親タイルリスト
	Model*			 model;								//クラス型のオブジェクトを作ったときポインタにするとかっこいい（deleteを忘れない）	
	float			 angle;								//角度					
	D3DXVECTOR3		 startPos;							//スタート位置
	D3DXVECTOR3		 goalPos;							//ゴール位置
	int				 isControl;							//現在のAIの状態
	list<int>::iterator	plusBomitr;						//イテレータ作成
	list<int>::iterator	paralyzeitr;					
	list<int>::iterator	bomThrowitr;					
	bool			isItem;								//アイテムボムを選択したかどうかを判定
	int				itemType;							//アイテムの種類
	int				processBomType;						//処理中のボム
	D3DXVECTOR3		putBomPos;							//ボムを置いた場所
	D3DXVECTOR3		prevPos;							//比較する以前の位置
	bool			charaSearch;						//キャラクターをターゲットにした
	int				charaNum;							//キャラクター番号を格納する変数
	aStar			pushData;							//リストにプッシュするための情報を格納するための変数
	int				frameCount;							//経過フレームをカウント
	float			moveSpeed;							//移動速度
	int				stopCount;							//停止時間をカウント
	float			bomPutAngle;						//ボムを置くためのアングル
	bool			throwFlag;							//投げる処理か？
	D3DXVECTOR3		bomThrowPos;						//投げる先の位置
	int				loopNum;							//ループ回数
	int				toTargetCost;						//ターゲットまでの距離コスト
	D3DXVECTOR3		target;								//ゴールとなるターゲットの位置
	bool			isTarget;							//ターゲットの見つけた
	int				prevWallCount;						//ターゲット候補の壁の周りの壁数
	float			wallTargetMul;						//壁ターゲットのコストにかける数
	float			charaTargetMul;						//キャラクターターゲットのコストにかける数
	D3DXVECTOR3		prevTargetPos;						//前回ターゲットにした位置
	bool			isOpenListEmpty;					//オープンリストが空になったか　
	vector<prevTileInfo> prevPosList;					//通った経路を格納するリスト
	int				prevTileState;						//前回通ったタイルに対する処理の状態
	int				prevDirIndex;						//前回の方向を示す要素番号
	D3DXVECTOR3		prevTile;							//前回の位置
	int				addExpRange;						//爆発範囲の加算分
	bool			isExitExpMove;						//爆発範囲から避難中か
	int				moveDir;							//現在進行している方向
	int				prevMoveDir;						//前回進行した方向
	readFile*		searchTargetRangeXData;				//ターゲットを捜査する範囲
	readFile*		searchTargetRangeZData;
	readFile*		targetDistCostData;					//ターゲットとの距離コスト
	int				searchTargetRangeXTable[112];		//ファイルの情報を格納する配列
	int				searchTargetRangeZTable[112];
	int				targetDistCostTable[112];
	normalBom*		bomUnit[3];							//各種ボムオブジェクトの情報を格納する配列		
	Wall*			wallUnit;							//ウォールユニットの情報を格納する変数
	vsCpuPlayer*	vsCpuPlayerUnit;					//対CPUプレイヤーユニットの情報を格納する変数
	NormalCpu*		cpuUnit[3];							//CPUユニットの情報を格納する変数

public:
	NormalCpu(void);

protected:
	~NormalCpu(void);	
	void initialize();
	void render(float frameTime, int cameraNum);		//描画処理
	void update();
	void collisions(UnitBase* target);					//ユニットの情報を取得
	void ai();											//AI処理		
	void control();										//移動
	void getItem();										//アイテム取得
	void stopAi();										//AIの処理を停止
	void adjustmentPos();								//位置を整数にする							
	bool collObstacel(D3DXVECTOR3 conpare);				//障害物への衝突処理
	float toIntMove(float move) {return (move<0) ? (float)(-1.0 * floor(fabs(move) + 0.5)) : (float)(floor(move + 0.5));}	//Moveの長さを整数にする
	float toIntPos(float pos) {return (pos<0) ? (float)(-1.0 * floor(fabs(pos) + 0.5)) : (float)(floor(pos + 0.5));}		//位置を整数にする
	void initList();									//各リストを初期化
	bool isThrowTarget();								//投げる先の位置を取得
	bool searchPutPos(int dir, D3DXVECTOR3 sumDirection[], D3DXVECTOR3 conparePosList[]);	//ボムを置ける位置を逆算して探索
	void notRenderBom();								//描画中のボムがあるか
	void searchTarget(float posX,float posY, int cost);	//ターゲットを探す
	void searchWallTarget(float posX,float posZ);		//同じ距離の壁の中で周囲８マスに壁が多いものをターゲットにする
	void bomProcess();									//ボムに対する処理
	void getPath();										//移動経路を取得
	D3DXVECTOR3 retTarget(D3DXVECTOR3 pos);				//ゴールを返す
	void putBom();										//ボムを置く
	void initAstar();									//AI時間する情報を初期化
	bool isObstacel(D3DXVECTOR3 conpare);				//障害物が存在するか		
	bool situationCheck();								//キャラクターの周囲の状況を見る
	bool inExpRange(D3DXVECTOR3 conpare);				//キャラクターが爆発範囲内かを判定すてコスト候補にするかの是非を判定
	void exitExpRange();								//爆発範囲から離れるための探索
	void exitExpProcess(D3DXVECTOR3 dir, int dirIndex);	//爆発範囲から離れるための処理
	bool equalPrevPosState(D3DXVECTOR3 conpare);		//比較位置が前回通った道と等しいか
	void plusExpRange() { addExpRange++; }				//爆発範囲の加算
	D3DXVECTOR3 calcMove(float angle);					//進行方向ベクトルを求める
	D3DXVECTOR3 retTargetOfCharacterPos();				//ゴール位置としてキャラクターの位置を返す
	float truncationSecondDecimalPlace(float pos);		//小数点第二以下切り捨て
	bool isPutBom(D3DXVECTOR3 bomPutPos);				//現在の位置からボムを置いた場合、逃げ道があるか判定
};

