// Programming 2D Games
// Copyright (c) 2011 by: 
// Charles Kelly
// textureManager.cpp v1.0
// A TextureManager object loads and maintains one texture file. 
// Create a TextureManager object for each texture file in the game.

#include "textureManager.h"

//=============================================================================
// default constructor
//=============================================================================
TextureManager::TextureManager()
{
    texture = NULL;
    width = 0;
    height = 0;
    file = NULL;
    graphics = NULL;
    initialized = false;            // set true when successfully initialized
}

//=============================================================================
// destructor
//=============================================================================
TextureManager::~TextureManager()
{
    SAFE_RELEASE(texture);
}

//=============================================================================
// Loads the texture file from disk.
// Post: ����������true ���s������false
//=============================================================================
bool TextureManager::initialize(Graphics *g, const char *f)
{
    try{
        graphics = g;                       // the graphics object
        file = f;                           // the texture file

        hr = graphics->loadTexture(file, TRANSCOLOR, width, height, texture);
        if (FAILED(hr))
        {
            SAFE_RELEASE(texture);
            return false;
        }
    }
    catch(...) {return false;}
    initialized = true;                    // ����������
    return true;
}

//=============================================================================
// called when graphics device is lost
//=============================================================================
void TextureManager::onLostDevice()
{
    if (!initialized)
        return;
    SAFE_RELEASE(texture);	//�ǂݍ��񂾉摜�t�@�C���̊J��
}

//=============================================================================
// called when graphics device is reset
//=============================================================================
void TextureManager::onResetDevice()
{
    if (!initialized)
        return;
    graphics->loadTexture(file, TRANSCOLOR, width, height, texture);	//�X�N���[���T�C�Y���̕ύX�̂Ƃ��ɓǂݍ���
}


void matrixTrans2D(D3DXMATRIX* m, float x, float y)
{
	D3DXMatrixIdentity(m);
	//4*4�̍s���3.1�A3.2��x,y�ɂ���
	m->_31 = x;
	m->_32 = y;
}