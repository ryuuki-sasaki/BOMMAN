//=============================================================================
//  CSVファイル読み込み
//　作成日：2015/9/1
//　更新日：2016/10/30
//	制作者：佐々木隆貴
//=============================================================================
#pragma once
#include "constants.h"

class readFile
{
private:
	char* data;

public:
	readFile();
	bool read(char* fileName);	//ファイル読み込み
	int getToComma(int* index);	//コンマ区切りで情報を取得
	void deleteDataArray();		//data配列削除
};
