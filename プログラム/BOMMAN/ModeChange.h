//=============================================================================
//  対戦モード
//　更新日：2015/8/1
//	制作者：佐々木隆貴
//=============================================================================
#pragma once

class modeChange
{
private:
	int selectMode;		//どの対戦モードを選んだか

private:
	modeChange::modeChange(){selectMode = 0;}

public:
	static modeChange* GetInstance(){static modeChange mode; return &mode;}		//インスタンスを返すGetInstance関数
	void setMode(int change){selectMode = change;}
	int getMode(){return selectMode;}
};