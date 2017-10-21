//=============================================================================
//  オプション
//　更新日：2015/8/1
//	制作者：佐々木隆貴
//=============================================================================
#pragma once

class option
{
private:
	int select;		

private:
	option::option(){select = 0;}

public:
	static option* GetInstance(){static option op; return &op;}		//インスタンスを返すGetInstance関数
	void setMode(int change){select = change;}
	int getMode(){return select;}
};