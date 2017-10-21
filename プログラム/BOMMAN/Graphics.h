// Programming 2D Games
// Copyright (c) 2011 by:
// Charles Kelly
// Chapter 3 graphics.h v1.0

#ifndef _GRAPHICS_H             // prevent multiple definitions if this 
#define _GRAPHICS_H             // ..file is included in more than one place
#define WIN32_LEAN_AND_MEAN

#ifdef _DEBUG
#define D3D_DEBUG_INFO
#endif
#include <d3d9.h>
#include <d3dx9.h>
#include "constants.h"
#include "gameError.h"

// DirectX pointer types
#define LP_TEXTURE  LPDIRECT3DTEXTURE9
#define LP_SPRITE   LPD3DXSPRITE
#define LP_3DDEVICE LPDIRECT3DDEVICE9		
#define LP_3D       LPDIRECT3D9				

// Color defines
#define COLOR_ARGB DWORD
#define SETCOLOR_ARGB(a,r,g,b) \
    ((COLOR_ARGB)((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)))

#define LP_SPRITE LPD3DXSPRITE	//�Ē�`�����X�v���C�g�̃|�C���^

namespace graphicsNS
{
    // Some common colors
    // ARGB numbers range from 0 through 255
    // A = Alpha channel (transparency where 255 is opaque)
    // R = Red, G = Green, B = Blue
    const COLOR_ARGB ORANGE  = D3DCOLOR_ARGB(255,255,165,  0);
    const COLOR_ARGB BROWN   = D3DCOLOR_ARGB(255,139, 69, 19);
    const COLOR_ARGB LTGRAY  = D3DCOLOR_ARGB(255,192,192,192);
    const COLOR_ARGB GRAY    = D3DCOLOR_ARGB(255,128,128,128);
    const COLOR_ARGB OLIVE   = D3DCOLOR_ARGB(255,128,128,  0);
    const COLOR_ARGB PURPLE  = D3DCOLOR_ARGB(255,128,  0,128);
    const COLOR_ARGB MAROON  = D3DCOLOR_ARGB(255,128,  0,  0);
    const COLOR_ARGB TEAL    = D3DCOLOR_ARGB(255,  0,128,128);
    const COLOR_ARGB GREEN   = D3DCOLOR_ARGB(255,  0,128,  0);
    const COLOR_ARGB NAVY    = D3DCOLOR_ARGB(255,  0,  0,128);
    const COLOR_ARGB WHITE   = D3DCOLOR_ARGB(255,255,255,255);
    const COLOR_ARGB YELLOW  = D3DCOLOR_ARGB(255,255,255,  0);
    const COLOR_ARGB MAGENTA = D3DCOLOR_ARGB(255,255,  0,255);
    const COLOR_ARGB RED     = D3DCOLOR_ARGB(255,255,  0,  0);
    const COLOR_ARGB CYAN    = D3DCOLOR_ARGB(255,  0,255,255);
    const COLOR_ARGB LIME    = D3DCOLOR_ARGB(255,  0,255,  0);
    const COLOR_ARGB BLUE    = D3DCOLOR_ARGB(255,  0,  0,255);
    const COLOR_ARGB BLACK   = D3DCOLOR_ARGB(255,  0,  0,  0);
    const COLOR_ARGB FILTER  = D3DCOLOR_ARGB(  0,  0,  0,  0);  // use to specify drawing with colorFilter
    const COLOR_ARGB ALPHA25 = D3DCOLOR_ARGB( 64,255,255,255);  // AND with color to get 25% alpha
    const COLOR_ARGB ALPHA50 = D3DCOLOR_ARGB(128,255,255,255);  // AND with color to get 50% alpha
    const COLOR_ARGB BACK_COLOR = NAVY;                         // background color of game

    enum DISPLAY_MODE{TOGGLE, FULLSCREEN, WINDOW};
}

// SpriteData: The properties required by Graphics::drawSprite to draw a sprite
struct SpriteData
{
    int         width;      // �X�v���C�g���i�s�N�Z���P�ʁj
    int         height;     //�X�v���C�g�����i�s�N�Z���P�ʁj
    float       x;          // ��ʈʒu (�X�v���C�g�̍����)
    float       y;
    float       scale;      // �i���j<1 �͏k��, >1 �͊g��
    float       angle;      // ��]�p�x�i���W�A���P�ʁj180�@���@3.14���W�A���A�@360�@���@2�΃��W�A��
    RECT        rect;       // �摜�̈ꕔ��I������Ƃ�
    LP_TEXTURE  texture;    // �e�N�X�`���ւ̃|�C���^
    bool        flipHorizontal; // ture ���������ɔ��]
    bool        flipVertical;   // true ���������ɔ��]
	bool		isBackGround;	//�w�i���ǂ���
};

extern LPD3DXEFFECT	 pEffect;	//�V�F�[�_�����邽�߂̕ϐ�

class Graphics
{
private:
    // DirectX pointers and stuff
    LP_3D       direct3d;	//�{��
    LP_3DDEVICE device3d;	//���
	LP_SPRITE   sprite;		//��
    D3DPRESENT_PARAMETERS d3dpp; //�p�����[�^
	D3DDISPLAYMODE pMode;

    // other variables
    HRESULT     result;          //���������s
    HWND        hwnd;
    bool        fullscreen;
    int         width;
    int         height;
	COLOR_ARGB  backColor;		 //�w�i�F

    // (For internal engine use only. No user serviceable parts inside.)
    // Initialize D3D presentation parameters
    void    initD3Dpp();		 //d3dppn�\���̂ɒl��ݒ�

public:
    // Constructor
    Graphics();

    // Destructor
    virtual ~Graphics();

    // Releases direct3d and device3d.
    void    releaseAll();		//���ׂĂ��J��

    // Initialize DirectX graphics
    // Throws GameError on error
    // Pre: hw = handle to window
    //      width = width in pixels
    //      height = height in pixels
    //      fullscreen = true for full screen, false for window
    void    initialize(HWND hw, int width, int height, bool fullscreen);		//������
	
    // Load the texture into default D3D memory (normal texture use)
    // For internal engine use only. Use the TextureManager class to load game textures.
    // Pre: filename = name of texture file.
    //      transcolor = transparent color
    // Post: width and height = size of texture
    //       texture points to texture
    HRESULT loadTexture(const char * filename, COLOR_ARGB transcolor, UINT &width, UINT &height, LP_TEXTURE &texture);

    // Display the offscreen backbuffer to the screen.
    HRESULT showBackbuffer();
	HRESULT getDeviceState();
	HRESULT reset();// Reset the graphics device.

    // Checks the adapter to see if it is compatible with the BackBuffer height,
    // width and refresh rate specified in d3dpp. Fills in the pMode structure with
    // the format of the compatible mode, if found.
    // Pre: d3dpp is initialized.
    // Post: Returns true if compatible mode found and pMode structure is filled.
    //       Returns false if no compatible mode found.
    bool    isAdapterCompatible();

    // Draw the sprite described in SpriteData structure.
    // color is optional, it is applied as a filter, WHITE is default (no change).
    // Creates a sprite Begin/End pair.
    // Pre: spriteData.rect defines the portion of spriteData.texture to draw
    //      spriteData.rect.right must be right edge + 1
    //      spriteData.rect.bottom must be bottom edge + 1
    void    drawSprite(const SpriteData &spriteData,           // sprite to draw
                       COLOR_ARGB color = graphicsNS::WHITE);      // default to white color filter (no change)

	// Toggle, fullscreen or window display mode
    // Pre: All user created D3DPOOL_DEFAULT surfaces are freed.
    // Post: All user surfaces are recreated.
    void    changeDisplayMode(graphicsNS::DISPLAY_MODE mode = graphicsNS::TOGGLE);

    // get functions
    // Return direct3d.
    LP_3D   get3D()             { return direct3d; }

    // Return device3d.
    LP_3DDEVICE get3Ddevice()   { return device3d; }

    // Return sprite
    LP_SPRITE   getSprite()     { return sprite; }

    // Return handle to device context (window).
    HDC     getDC()             { return GetDC(hwnd); }

    // Test for lost device
    //HRESULT getDeviceState();

    // Return fullscreen
    bool    getFullscreen()     { return fullscreen; }
 
    // Set color used to clear screen
    void setBackColor(COLOR_ARGB c) {backColor = c;}
	/*
    // Display the offscreen backbuffer to the screen.
    HRESULT showBackbuffer();		//�o�b�N�o�b�t�@�\��

	HRESULT getDeviceState();		//�O���t�B�b�N�X�f�o�C�X�̏����̃`�F�b�N
	HRESULT reset();				//�O���t�B�b�N�X�f�o�C�X�̃��Z�b�g
	HRESULT loadTexture(const char *filename, COLOR_ARGB transcolor,
                                UINT &width, UINT &height, LP_TEXTURE &texture);
	*/
	//=============================================================================
    // �o�b�N�o�b�t�@���N���A���āADirectX��BeginScene()���Ăяo��
    //=============================================================================
    HRESULT beginScene() 
    {
        result = E_FAIL;
        if(device3d == NULL)
            return result;
        //�o�b�N�o�b�t�@��backColor�ŃN���A����
        device3d->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, backColor, 1.0F, 0);
        result = device3d->BeginScene();          //�`��̂��߂̃V�[�����J�n����
        return result;
    }

	//-----------------------------------------------------------------------------
	//SpriteBegin
	//-----------------------------------------------------------------------------
	void spriteBegin()
	{
		sprite->Begin(D3DXSPRITE_ALPHABLEND);	//D3DXSPRITE_ALPHABLEND...�������ȂƂ����`��
	}

	//-----------------------------------------------------------------------------
	//SpriteEnd
	//-----------------------------------------------------------------------------
	void spriteEnd()
	{
		sprite->End();
	}

    //=============================================================================
    // DirectX��EndScene()���Ăяo��
    //=============================================================================
    HRESULT endScene() 
    {
        result = E_FAIL;
        if(device3d)
            result = device3d->EndScene();
        return result;
    }

	

	

	//-----------------------------------------------------------------------------
	//DrawSprite
	//-----------------------------------------------------------------------------
	//void Graphics::drawSprite(LP_TEXTURE texture);
};

#endif

