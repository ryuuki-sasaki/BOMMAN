// Programming 2D Games
// Copyright (c) 2011 by: 
// Charles Kelly
// textureManager.h v1.0

#ifndef _TEXTUREMANAGER_H       // Prevent multiple definitions if this 
#define _TEXTUREMANAGER_H       // file is included in more than one place
#define WIN32_LEAN_AND_MEAN

#include "graphics.h"
#include "constants.h"

class TextureManager
{
    // TextureManager properties
  private:
    UINT       width;       // ��
    UINT       height;      // ����
    LP_TEXTURE texture;     // �e�N�X�`���ւ̃|�C���^�@�iLP_TEXTURE�`��graphics.h�Œ�`���Ă���)�@��
    const char *file;       // �t�@�C����		//const=�萔
    Graphics *graphics;     // �O���t�B�b�N�X�ւ̃|�C���^�̕ۑ��@��
    bool    initialized;    // ������
    HRESULT hr;             // �������������s������

  public:
    // Constructor
    TextureManager();

    // Destructor
    virtual ~TextureManager();

    // �e�N�X�`���Ƀ|�C���^��Ԃ�
    LP_TEXTURE getTexture() const {return texture;}

    // �e�N�X�`���̕���Ԃ�
    UINT getWidth() const {return width;}

    // �e�N�X�`���̍�����Ԃ�
    UINT getHeight() const {return height;}


    // Initialize the textureManager
    // Pre: *g points to Graphics object
    //      *file points to name of texture file to load
    // Post: The texture file is loaded
    virtual bool initialize(Graphics *g, const char *file);

    // Release resources
    virtual void onLostDevice();		//�Q�[���I���̎�

    // Restore resourses
    virtual void onResetDevice();		//���炩�̌����Œ��f�����Ƃ�
};

//�e�N�X�`���p�ړ��s��쐬�֐�
void matrixTrans2D(D3DXMATRIX* m, float x, float y);

#endif

