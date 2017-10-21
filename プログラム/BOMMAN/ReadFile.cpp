//=============================================================================
//  CSV�t�@�C���ǂݍ���
//�@�쐬���F2015/9/1
//�@�X�V���F2016/10/30
//	����ҁF���X�ؗ��M
//=============================================================================
#include "readFile.h"
#include <Windows.h>

readFile::readFile()
{
	data = nullptr;
}

//=============================================================================
//  CSV�t�@�C���ǂݍ���
//�@�����F�t�@�C����
//	�߂�l�F�ǂݍ��ݐ������ǂ���
//=============================================================================
bool readFile::read(char* fileName)
{
	//�t�@�C�����J��
	HANDLE hFile;
	hFile = CreateFile(
		fileName,				//�t�@�C����
		GENERIC_READ,			//�A�N�Z�X���[�h�i�ǂݍ��ݗp�j
		0,						//���L�i�Ȃ��j
		NULL,					//�Z�L�����e�B����
		OPEN_EXISTING,			//�J�����@�i�t�@�C�����Ȃ���Ύ��s�j
		FILE_ATTRIBUTE_NORMAL,	//�����ƃt���O�i�ݒ�Ȃ��j
		NULL);					//�g�������Ȃ�

	//�J���Ȃ������璆�f
	if(hFile == NULL) return false;

	//�t�@�C���̃T�C�Y���擾
	DWORD fileSize = GetFileSize(hFile, NULL);

	//�t�@�C���̃T�C�Y�����������m��
	data = new char[fileSize];

	//�S���ǂݍ���
	DWORD dwBytes = 0;		//�ǂݍ��݈ʒu
	ReadFile(
		hFile,				//�t�@�C���n���h��
		data,				//�f�[�^������ϐ�
		fileSize,			//�ǂݍ��ރT�C�Y
		&dwBytes,			//�ǂݍ��񂾃T�C�Y�i�ǂݍ��݈ʒu�j
		NULL);				//�I�[�o�[���b�v�h�\���́i����͎g��Ȃ��j

	//�t�@�C�������
	CloseHandle(hFile);

	return true;

}

//=============================================================================
//  �R���}��؂�ŏ����擾
//�@�����F�t�@�C���f�[�^�A�v�f�ԍ��̓Y��
//	�߂�l�F�ǂݍ��܂ꂽ���
//=============================================================================
int readFile::getToComma(int* index)
{
	char chr[256];
	int i=0;

	//�u,�v�܂łP��������chr�ɂ����
	while(data[*index] != ',' && data[*index] != '\n')
	{
		chr[i] = data[*index];		//�����Ń��m�̎�ނ�\���ԍ���chr[i]�ɂ͂���
		i++;
		(*index)++;
	}

	//�Ō�Ɂu\0�v������
	chr[i] = '\0';
	i++;
	(*index)++;

	//�����ɕϊ�
	int value;
	value = atoi(chr);

	return value;
}

//=============================================================================
//  data�z��폜
//�@�����F�Ȃ�
//	�߂�l�F�Ȃ�
//=============================================================================
void readFile::deleteDataArray()
{
	delete[] data;
}