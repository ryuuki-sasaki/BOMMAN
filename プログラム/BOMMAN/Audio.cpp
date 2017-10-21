//=============================================================================
//  音を扱うクラス
//　作成日：2015/04/16
//　更新日：2015/07/10
//	制作者：佐々木隆貴
//=============================================================================

#include "audio.h"

//=============================================================================
// デフォルトコンストラクタ
//=============================================================================
Audio::Audio()
{
    xactEngine = NULL;
    waveBank = NULL;
    soundBank = NULL;
    cueI = 0;
    mapWaveBank = NULL;         // リリース・ファイルへの呼び出しUnmapViewOfFile()
    soundBankData = NULL;

    HRESULT hr = CoInitializeEx( NULL, COINIT_MULTITHREADED );
    if( SUCCEEDED( hr ) )
        coInitialized = true;
    else
        coInitialized = false;
}

//=============================================================================
// デストラクタ
//=============================================================================
Audio::~Audio()
{
    // 終了処理
    if( xactEngine )
    {
        xactEngine->ShutDown(); 
        xactEngine->Release();
    }

    if ( soundBankData )
        delete[] soundBankData;
    soundBankData = NULL;

     //xactEngine->ShutDown()の後メモリ開放
    if( mapWaveBank )
        UnmapViewOfFile( mapWaveBank );
    mapWaveBank = NULL;

    if( coInitialized )        //CoInitializeExが成功したら
        CoUninitialize();
}

//=============================================================================
// 初期化
// この関数の機能:
//      1. xactEngine->Initializeでエンジン初期化
//      2. wavebankを作成できる
//      3. soundbankを作成できる
//      4. cueが音の番号
// 引数：waveBankName ウェーブバンクの名前　
// 引数：soundBankName サウンドバンクの名前
// 戻り値：HRESULT 初期化実行結果
//=============================================================================
HRESULT Audio::initialize(char* waveBankName, char* soundBankName)
{
    HRESULT result = E_FAIL;
    HANDLE hFile;
    DWORD fileSize;
    DWORD bytesRead;
    HANDLE hMapFile;

	//coInitializedの初期化してなかったら
    if( coInitialized == false)
        return E_FAIL;

	//XACTエンジン（音を鳴らす本体）を準備
    result = XACT3CreateEngine( 0, &xactEngine );
    if( FAILED( result ) || xactEngine == NULL )
        return E_FAIL;

   // XACTランタイムの作成と初期化
    XACT_RUNTIME_PARAMETERS xactParams = {0};
    xactParams.lookAheadTime = XACT_ENGINE_LOOKAHEAD_DEFAULT;
    result = xactEngine->Initialize( &xactParams );
    if( FAILED( result ) )
        return result;

    //wavebankを読む
    result = E_FAIL; // 最初は失敗が入る、成功で書き換え
	//wavBankのファイルの読み込み（CreateFile、GENERIC_READでファイル読み込み）
    hFile = CreateFile( waveBankName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL );
    if( hFile != INVALID_HANDLE_VALUE )
    {
		//読み込んだファイルのサイズを取得
        fileSize = GetFileSize( hFile, NULL );
        if( fileSize != -1 )
        {
			//マッピングしたハンドルを作成
            hMapFile = CreateFileMapping( hFile, NULL, PAGE_READONLY, 0, fileSize, NULL );
            if( hMapFile )
            {
				//読み込んだファイルをコピー
                mapWaveBank = MapViewOfFile( hMapFile, FILE_MAP_READ, 0, 0, 0 );
				//waveBankにコピー
                if( mapWaveBank )
                    result = xactEngine->CreateInMemoryWaveBank( mapWaveBank, fileSize, 0, 0, &waveBank );
				//ファイルを閉じる
                CloseHandle( hMapFile );    // mapWaveBank maintains a handle on the file so close this unneeded handle
            }
        }
		//ファイルを閉じる
        CloseHandle( hFile );    // mapWaveBank maintains a handle on the file so close this unneeded handle
    }
    if( FAILED( result ) )
        return HRESULT_FROM_WIN32( ERROR_FILE_NOT_FOUND );

    //soundbankを読む
    result = E_FAIL;    // 最初は失敗が入る、成功で書き換え
	//soundBankのファイルの読み込み（CreateFile、GENERIC_READでファイル読み込み
	hFile = CreateFile( soundBankName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL );
    if( hFile != INVALID_HANDLE_VALUE )
    {
		//読み込んだファイルのサイズを取得
        fileSize = GetFileSize( hFile, NULL );
        if( fileSize != -1 )
        {
            soundBankData = new BYTE[fileSize];    // reserve memory for sound bank
            if( soundBankData )
            {
				//読み込んだファイルをコピー
                if( 0 != ReadFile( hFile, soundBankData, fileSize, &bytesRead, NULL ) )
                    result = xactEngine->CreateSoundBank( soundBankData, fileSize, 0, 0, &soundBank );
            }
        }
		//ファイルを閉じる
        CloseHandle( hFile );
    }
    if( FAILED( result ) )
        return HRESULT_FROM_WIN32( ERROR_FILE_NOT_FOUND );

    return S_OK;
}

//=============================================================================
// 実行する
// 引数：なし
// 戻り値：なし
//=============================================================================
void Audio::run()
{
	//サウンドエンジンが空ではなかったら
    if (xactEngine != NULL)
		 xactEngine->DoWork();	
}

//=============================================================================
// 再生する
// 引数：cue[]　再生する音の名前
// 戻り値：なし
//=============================================================================
//引数で音の番号を受け取る
void Audio::playCue(const char cue[])	
{
    if (soundBank == NULL)
        return;
    cueI = soundBank->GetCueIndex( cue );        //引数の名前から番号を取得
    soundBank->Play( cueI, 0, 0, NULL );
}

//=============================================================================
// 停止する
// 引数：cue[]　停止する音の名前
// 戻値：なし
//=============================================================================
//引数で音の番号を受け取る
void Audio::stopCue(const char cue[])	
{
    if (soundBank == NULL)
        return;
    cueI = soundBank->GetCueIndex( cue );        // 受け取った音をとめる
    soundBank->Stop( cueI, XACT_FLAG_SOUNDBANK_STOP_IMMEDIATE);
}
