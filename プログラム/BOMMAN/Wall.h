//=============================================================================
//  壁
//　更新日：2016/10/30
//	制作者：佐々木隆貴
//=============================================================================
#pragma once
#include "unitbase.h"
#include "constants.h"
#include "textureManager.h"
#include "Poly.h"

class vsCpuPlayer;
class VsPlayer;
class NormalCpu;
class readFile;

const int WALLARRAYRANGE = 51;	//壁を配置する範囲
#define OBSTACLELCOST 100	//壁等の障害物のコスト

class Wall : public UnitBase
{
public:	
	typedef struct 
	{
		D3DXVECTOR3	pos;			//壁の位置
		D3DXVECTOR3	itemPos;		//アイテムの位置
		int			isItem;			//アイテムを格納する変数（0の場合はアイテムなしそれ以外は対応する番号のアイテム格納）
		int			cpuCost;		//cpuの行動を決定するコスト
		int         expRangeNum;	//爆発範囲の番号
		bool		isExpRange;		//爆発範囲内か？
	}data;
	int				 wallArr[WALLARRAYRANGE][WALLARRAYRANGE];		//壁の有無を格納する配列
	data			 wallData[WALLARRAYRANGE][WALLARRAYRANGE];		//壁の情報を格納する配列

private:	
	TextureManager	 texLargeexplosionBom;				//大爆発ボムテクスチャ
	Poly			 largeexplosionPoly;				//大爆発テクスチャを貼るポリゴン
	TextureManager	 texPlusBom;						//プラスボムテクスチャ
	Poly			 plusBomPoly;						//プラスボムテクスチャを貼るポリゴン
	TextureManager	 texParalyzeBom;					//痺れボムテクスチャ
	Poly		     paralyzeBomPoly;					//痺れボムテクスチャを貼るポリゴン
	TextureManager	 texThrow;							//スローテクスチャ
	Poly		     bomThrowPoly;						//スローボムテクスチャを貼るポリゴン
	TextureManager	 texWall;							//壁テクスチャ
	Poly		     wallPoly;							//壁テクスチャを貼るポリゴン
	TextureManager	 texNonBreakWall;					//鉄壁テクスチャ
	Poly		     nonBreakWallPoly;					//鉄壁テクスチャを貼るポリゴン
	D3DXMATRIX matScale,matBillboard,matTrans;
	readFile* mapTableData; 
	readFile* nonBreakBlockMapData;
	DWORD putWallCount;									//タイムアップ後に置く壁のカウント
	bool isTimeUp;										//タイムアップしたか
	int nonBreakBlockMapTable[4800];					//ファイルの情報を格納する配列
	int blockMapTable[51][51];	
	vsCpuPlayer*	vsCpuPlayerUnit;					//対CPUプレイヤーユニットの情報を格納する変数
	NormalCpu*		cpuUnit[3];							//CPUユニットの情報を格納する変数	
	VsPlayer		*player1, *player2;					//対人プレイヤーユニットの情報を格納する変数	

public:
	Wall(void);
	~Wall(void);
	void initialize();
	void update();
	void initList();
	void render(float frameTime, int cameraNum );							//壁を描画する関数	
	void randomWallArrange();												//壁を有無をランダムに決める関数
	template <typename T> int retWallArr(T x, T z){return wallArr[(int)x][(int)z];}		//壁の位置を返す
	int retCost(float x, float z){return wallData[(int)x][(int)z].cpuCost;}				//CPUコストを返す
	D3DXVECTOR3 retItem(float x, float z){return wallData[(int)x][(int)z].itemPos;}		//アイテムの位置を返す
	bool retIsExpRange(float x, float z){return wallData[(int)x][(int)z].isExpRange;}	//番号の位置が範囲内か
	int retExpRangeNum(float x, float z){return wallData[(int)x][(int)z].expRangeNum;}	//爆発範囲の番号を返す
	float toIntPos(float pos) {return (pos<0) ? (float)(-1.0 * floor(fabs(pos) + 0.5)) : (float)(floor(pos + 0.5));}	//位置を整数にする
	int retItemNumber(float posX, float posZ);								//アイテム番号を返す
	void setBillboard(int x, int z, float scaleX, float scaleY, int cNum);	//ビルボードの設定
	void timeUp();															//時間制限後の処理
	void notWallProcess(int x, int z, int cNum);							//壁が配置されていない場所の処理
	void setIsTimeUp(bool timeUp){isTimeUp = timeUp;}						//タイムアップしたか設定
	void collisionOnTheWall(D3DXVECTOR2 pos);			//キャラクターがタイムアップ後の壁に衝突したか
	void collisions(UnitBase* target);					//ユニットの情報を取得
};

