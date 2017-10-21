//=============================================================================
//  ���������N���X
//�@�쐬���F2015/04/16
//�@�X�V���F2015/07/10
//	����ҁF���X�ؗ��M
//=============================================================================

#include "audio.h"

//=============================================================================
// �f�t�H���g�R���X�g���N�^
//=============================================================================
Audio::Audio()
{
    xactEngine = NULL;
    waveBank = NULL;
    soundBank = NULL;
    cueI = 0;
    mapWaveBank = NULL;         // �����[�X�E�t�@�C���ւ̌Ăяo��UnmapViewOfFile()
    soundBankData = NULL;

    HRESULT hr = CoInitializeEx( NULL, COINIT_MULTITHREADED );
    if( SUCCEEDED( hr ) )
        coInitialized = true;
    else
        coInitialized = false;
}

//=============================================================================
// �f�X�g���N�^
//=============================================================================
Audio::~Audio()
{
    // �I������
    if( xactEngine )
    {
        xactEngine->ShutDown(); 
        xactEngine->Release();
    }

    if ( soundBankData )
        delete[] soundBankData;
    soundBankData = NULL;

     //xactEngine->ShutDown()�̌チ�����J��
    if( mapWaveBank )
        UnmapViewOfFile( mapWaveBank );
    mapWaveBank = NULL;

    if( coInitialized )        //CoInitializeEx������������
        CoUninitialize();
}

//=============================================================================
// ������
// ���̊֐��̋@�\:
//      1. xactEngine->Initialize�ŃG���W��������
//      2. wavebank���쐬�ł���
//      3. soundbank���쐬�ł���
//      4. cue�����̔ԍ�
// �����FwaveBankName �E�F�[�u�o���N�̖��O�@
// �����FsoundBankName �T�E���h�o���N�̖��O
// �߂�l�FHRESULT ���������s����
//=============================================================================
HRESULT Audio::initialize(char* waveBankName, char* soundBankName)
{
    HRESULT result = E_FAIL;
    HANDLE hFile;
    DWORD fileSize;
    DWORD bytesRead;
    HANDLE hMapFile;

	//coInitialized�̏��������ĂȂ�������
    if( coInitialized == false)
        return E_FAIL;

	//XACT�G���W���i����炷�{�́j������
    result = XACT3CreateEngine( 0, &xactEngine );
    if( FAILED( result ) || xactEngine == NULL )
        return E_FAIL;

   // XACT�����^�C���̍쐬�Ə�����
    XACT_RUNTIME_PARAMETERS xactParams = {0};
    xactParams.lookAheadTime = XACT_ENGINE_LOOKAHEAD_DEFAULT;
    result = xactEngine->Initialize( &xactParams );
    if( FAILED( result ) )
        return result;

    //wavebank��ǂ�
    result = E_FAIL; // �ŏ��͎��s������A�����ŏ�������
	//wavBank�̃t�@�C���̓ǂݍ��݁iCreateFile�AGENERIC_READ�Ńt�@�C���ǂݍ��݁j
    hFile = CreateFile( waveBankName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL );
    if( hFile != INVALID_HANDLE_VALUE )
    {
		//�ǂݍ��񂾃t�@�C���̃T�C�Y���擾
        fileSize = GetFileSize( hFile, NULL );
        if( fileSize != -1 )
        {
			//�}�b�s���O�����n���h�����쐬
            hMapFile = CreateFileMapping( hFile, NULL, PAGE_READONLY, 0, fileSize, NULL );
            if( hMapFile )
            {
				//�ǂݍ��񂾃t�@�C�����R�s�[
                mapWaveBank = MapViewOfFile( hMapFile, FILE_MAP_READ, 0, 0, 0 );
				//waveBank�ɃR�s�[
                if( mapWaveBank )
                    result = xactEngine->CreateInMemoryWaveBank( mapWaveBank, fileSize, 0, 0, &waveBank );
				//�t�@�C�������
                CloseHandle( hMapFile );    // mapWaveBank maintains a handle on the file so close this unneeded handle
            }
        }
		//�t�@�C�������
        CloseHandle( hFile );    // mapWaveBank maintains a handle on the file so close this unneeded handle
    }
    if( FAILED( result ) )
        return HRESULT_FROM_WIN32( ERROR_FILE_NOT_FOUND );

    //soundbank��ǂ�
    result = E_FAIL;    // �ŏ��͎��s������A�����ŏ�������
	//soundBank�̃t�@�C���̓ǂݍ��݁iCreateFile�AGENERIC_READ�Ńt�@�C���ǂݍ���
	hFile = CreateFile( soundBankName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL );
    if( hFile != INVALID_HANDLE_VALUE )
    {
		//�ǂݍ��񂾃t�@�C���̃T�C�Y���擾
        fileSize = GetFileSize( hFile, NULL );
        if( fileSize != -1 )
        {
            soundBankData = new BYTE[fileSize];    // reserve memory for sound bank
            if( soundBankData )
            {
				//�ǂݍ��񂾃t�@�C�����R�s�[
                if( 0 != ReadFile( hFile, soundBankData, fileSize, &bytesRead, NULL ) )
                    result = xactEngine->CreateSoundBank( soundBankData, fileSize, 0, 0, &soundBank );
            }
        }
		//�t�@�C�������
        CloseHandle( hFile );
    }
    if( FAILED( result ) )
        return HRESULT_FROM_WIN32( ERROR_FILE_NOT_FOUND );

    return S_OK;
}

//=============================================================================
// ���s����
// �����F�Ȃ�
// �߂�l�F�Ȃ�
//=============================================================================
void Audio::run()
{
	//�T�E���h�G���W������ł͂Ȃ�������
    if (xactEngine != NULL)
		 xactEngine->DoWork();	
}

//=============================================================================
// �Đ�����
// �����Fcue[]�@�Đ����鉹�̖��O
// �߂�l�F�Ȃ�
//=============================================================================
//�����ŉ��̔ԍ����󂯎��
void Audio::playCue(const char cue[])	
{
    if (soundBank == NULL)
        return;
    cueI = soundBank->GetCueIndex( cue );        //�����̖��O����ԍ����擾
    soundBank->Play( cueI, 0, 0, NULL );
}

//=============================================================================
// ��~����
// �����Fcue[]�@��~���鉹�̖��O
// �ߒl�F�Ȃ�
//=============================================================================
//�����ŉ��̔ԍ����󂯎��
void Audio::stopCue(const char cue[])	
{
    if (soundBank == NULL)
        return;
    cueI = soundBank->GetCueIndex( cue );        // �󂯎���������Ƃ߂�
    soundBank->Stop( cueI, XACT_FLAG_SOUNDBANK_STOP_IMMEDIATE);
}
