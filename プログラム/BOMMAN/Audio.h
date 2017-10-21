//=============================================================================
//  ���������N���X
//�@�쐬���F2015/04/16
//�@�X�V���F2015/07/10
//	����ҁF���X�ؗ��M
//=============================================================================
#ifndef _AUDIO_H                 
#define _AUDIO_H                
#define WIN32_LEAN_AND_MEAN

#include <xact3.h>
#include "constants.h"

class Audio
{
    // properties
  private:
    IXACT3Engine* xactEngine;   // ����炷���߂̋@�\�̖{��
    IXACT3WaveBank* waveBank;   // wave bank�̃|�C���^
    IXACT3SoundBank* soundBank; // sound bank�̃|�C���^
    XACTINDEX cueI;             // ��荞�񂾉�,�Ȃ̔ԍ�
    void* mapWaveBank;          // UnmapViewOfFile()�֓n���|�C���^
    void* soundBankData;
    bool coInitialized;         // coInitialize������������true������

  public:
    // �R���X�g���N�^
	Audio();

    // �f�X�g���N�^
    virtual ~Audio();

    // �����o�֐�

    // ������
    HRESULT initialize(char* waveBankName, char* soundBankName);

    // ���t���[���ǂݍ���
    void run();

	//���̍Đ�
    void playCue(const char cue[]);

	//���̒�~
    void stopCue(const char cue[]);
};

#endif
