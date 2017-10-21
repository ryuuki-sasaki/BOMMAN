// Programming 2D Games
// Copyright (c) 2011 by:
// Charles Kelly
// Chapter 3 graphics.cpp v1.0

#include "graphics.h"

//=============================================================================
// �R���X�g���N�^
//=============================================================================
Graphics::Graphics()
{
    direct3d = NULL;		//�N���X�̃|�C���^������ꍇ�̓R���X�g���N�^�łm�t�k�k
    device3d = NULL;
	sprite = NULL;
    fullscreen = false;		
    width = GAME_WIDTH;    // width & height �� initialize() �ŏ�������
    height = GAME_HEIGHT;
	backColor = SETCOLOR_ARGB(255, 0, 0, 128);		//�w�i�F
}

//=============================================================================
// �f�X�g���N�^
//=============================================================================
Graphics::~Graphics()
{
    releaseAll();		//���	
}

//=============================================================================
// ���ׂĊJ��
//=============================================================================
void Graphics::releaseAll()
{
	SAFE_RELEASE(sprite);
    SAFE_RELEASE(device3d);		//��ʂ̊J��
    SAFE_RELEASE(direct3d);		//�{�̂̊J���i�J���͖{�̂��Ō�j
}

//=============================================================================
// DirectX graphics ��������
// �G���[�� GameError ���X���[ 
//=============================================================================
void Graphics::initialize(HWND hw, int w, int h, bool full)
{
    hwnd = hw;
    width = w;
    height = h;
    fullscreen = full;

    //Direct3D��������
    direct3d = Direct3DCreate9(D3D_SDK_VERSION);		//�{�̂��쐬
    if (direct3d == NULL)								//�쐬�����s��������
        throw(GameError(gameErrorNS::FATAL_ERROR, "Direct3D�̏������Ɏ��s"));	//�Ăяo������throw�@GameError�͈���2��

    initD3Dpp();       // D3D presentation parameters ��������
	if(fullscreen)     // �t���X�N���[�����[�h�Ȃ�
    {
        if(isAdapterCompatible())   // is the adapter compatible
            // set the refresh rate with a compatible one
            d3dpp.FullScreen_RefreshRateInHz = pMode.RefreshRate;
        else
            throw(GameError(gameErrorNS::FATAL_ERROR, 
            "The graphics device does not support the specified resolution and/or format."));
    }

     // graphics card �� hardware texturing and lighting and vertex shaders ���T�|�[�g���Ă鎞�@�\����
    D3DCAPS9 caps;
    DWORD behavior;
    result = direct3d->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps);
    // device �� HW T&L �� 1.1 vertex shaders in hardware��
    // �T�|�[�g���Ă��Ȃ��Ƃ��@software vertex processing�ɐ؂�ւ���
    if( (caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) == 0 ||
            caps.VertexShaderVersion < D3DVS_VERSION(1,1) )
        behavior = D3DCREATE_SOFTWARE_VERTEXPROCESSING;  // software �������g������
    else		
		//HARDWARE�Ŋ�{�͑Ή��ł��邪�A�Ή����Ă��Ȃ��ꍇSOFTWARE
        behavior = D3DCREATE_HARDWARE_VERTEXPROCESSING;  // hardware �������g������
	
	/*
    if (FAILED(result))		//���s��������
        throw(GameError(gameErrorNS::FATAL_ERROR, "Direct3D device�i�Q�[����ʁj�̍쐬���s"));
	*/
	//Direct3D�f�o�C�X���쐬
	result = direct3d->CreateDevice(
		D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		hwnd,
		behavior,
		&d3dpp,
		&device3d);

	  if (FAILED(result))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error creating Direct3D device"));
 
	  result = D3DXCreateSprite(device3d, &sprite);
	  if (FAILED(result))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error creating Direct3D sprite"));
	
	if(FAILED(result))
		throw(GameError(gameErrorNS::FATAL_ERROR,
			"Direct3ddevice�̍쐬�Ɏ��s���܂���"));

		result = D3DXCreateSprite(device3d,&sprite);

		if(FAILED(result))
			throw(GameError(gameErrorNS::FATAL_ERROR,
			"Direct3ddevice�̍쐬�Ɏ��s���܂���"));

		//�A���t�@�u�����h
		device3d->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		device3d->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
		device3d->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
		//�A���t�@�e�X�g�i���̓����x�ȉ��Ȃ�`�悵�Ȃ��j
		device3d->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
		device3d->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
		device3d->SetRenderState(D3DRS_ALPHAREF, 0);				//�����x��0�Ȃ�`�悵�Ȃ�

}

//=============================================================================
// Initialize D3D presentation parameters
//=============================================================================
void Graphics::initD3Dpp()
{
    try{												
        ZeroMemory(&d3dpp, sizeof(d3dpp));              // fill the structure with 0
        // fill in the parameters we need
        d3dpp.BackBufferWidth   = width;
        d3dpp.BackBufferHeight  = height;
        if(fullscreen)                                  // if fullscreen
            d3dpp.BackBufferFormat  = D3DFMT_X8R8G8B8;  // 24 bit color
        else
            d3dpp.BackBufferFormat  = D3DFMT_UNKNOWN;   // use desktop setting
        d3dpp.BackBufferCount   = 1;
        d3dpp.SwapEffect        = D3DSWAPEFFECT_DISCARD;
        d3dpp.hDeviceWindow     = hwnd;
        d3dpp.Windowed          = (!fullscreen);
        d3dpp.PresentationInterval   = D3DPRESENT_INTERVAL_IMMEDIATE;
		//Z�o�b�t�@���g�����g��Ȃ���
		d3dpp.EnableAutoDepthStencil = TRUE;
		//Z�o�b�t�@�̐ݒ�
		d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    } catch(...)		//���s������
    {
        throw(GameError(gameErrorNS::FATAL_ERROR,		//throw�i�j
                "D3D presentation parameters���������s"));
    }
}

//=============================================================================
//�e�N�X�`�����f�t�H���g��D3D�������ɓǂݍ��ށi�ʏ�̃e�N�X�`���Ŏg�p�j
//�G���W���g�p���ɓ����I�ɂ̂ݎg�p����
//�Q�[���e�N�X�`���̓ǂݍ��݂ɂ́ATextureManager�N���X���g�p����
//���s�O�Ffilename�́A�e�N�X�`���t�@�C���̖��O
//		�@transcolor�͓����Ƃ��Ĉ����F
//���s��Fwidth��height = �e�N�X�`���̐��@
//		  texture�̓e�N�X�`�����w��
//HRESULT��߂�
//=============================================================================

HRESULT Graphics::loadTexture(const char *filename, COLOR_ARGB transcolor,
                                UINT &width, UINT &height, LP_TEXTURE &texture)
{
    // �t�@�C������ǂݍ��ނ��߂̍\����
    D3DXIMAGE_INFO info;
    result = E_FAIL;

    try{
        if(filename == NULL)
        {
            texture = NULL;
            return D3DERR_INVALIDCALL;
        }
    
        // ���ƍ������t�@�C������擾
        result = D3DXGetImageInfoFromFile(filename, &info);
        if (result != D3D_OK)
            return result;
        width = info.Width;
        height = info.Height;
    
        // �t�@�C����ǂݍ���ŁA�V�����e�N�X�`�����쐬
        result = D3DXCreateTextureFromFileEx( 
            device3d,           //3D �f�o�C�X
            filename,           //�摜�t�@�C���̖��O
            info.Width,         //�e�N�X�`���̕�
            info.Height,        //�e�N�X�`���̍���
            1,                  //mip-map levels (1 for no chain)
            0,                  //usage
            D3DFMT_UNKNOWN,     //�T�[�t�F�X�t�H�[�}�b�g (default)
            D3DPOOL_DEFAULT,    //�e�N�X�`���p�̃������̎��
            D3DX_DEFAULT,       //�摜�t�B���^
            D3DX_DEFAULT,       //mip filter
            transcolor,         //�����p�̐F�L�[
            &info,              //bitmap file info (from loaded file)
            NULL,               //�J���[�p���b�g
            &texture );         //��肭������texture�Ƃ����ϐ��ɌĂэ��񂾉摜�̏�񂪓���

    } catch(...)
    {
        throw(GameError(gameErrorNS::FATAL_ERROR, "Graphics::loadTexture�͎��s���܂���"));
    }
    return result;
}
//=============================================================================
// Display the backbuffer
//=============================================================================
HRESULT Graphics::showBackbuffer()
{
    result = E_FAIL;     // default to fail, replace on success
	// Display backbuffer to screen
    /*
	(this function will be moved in later versions)
    Clear the backbuffer to lime green 
    */
    result = device3d->Present(NULL, NULL, NULL, NULL);		//���s�������ۂ�
	return result;
}

//=============================================================================
// Checks the adapter to see if it is compatible with the BackBuffer height,
// width and refresh rate specified in d3dpp. Fills in the pMode structure with
// the format of the compatible mode, if found.
// Pre: d3dpp is initialized.
// Post: Returns true if compatible mode found and pMode structure is filled.
//       Returns false if no compatible mode found.
//=============================================================================
bool Graphics::isAdapterCompatible()
{
    UINT modes = direct3d->GetAdapterModeCount(D3DADAPTER_DEFAULT, 
                                            d3dpp.BackBufferFormat);
    for(UINT i=0; i<modes; i++)
    {
        result = direct3d->EnumAdapterModes( D3DADAPTER_DEFAULT, 
                                        d3dpp.BackBufferFormat,
                                        i, &pMode);
        if( pMode.Height == d3dpp.BackBufferHeight &&
            pMode.Width  == d3dpp.BackBufferWidth &&
            pMode.RefreshRate >= d3dpp.FullScreen_RefreshRateInHz)
            return true;
    }
    return false;
}

//=============================================================================
// Draw the sprite described in SpriteData structure
// Color is optional, it is applied like a filter, WHITE is default (no change)
// Pre : sprite->Begin() is called
// Post: sprite->End() is called
// spriteData.rect defines the portion of spriteData.texture to draw
//   spriteData.rect.right must be right edge + 1
//   spriteData.rect.bottom must be bottom edge + 1
//=============================================================================
void Graphics::drawSprite(const SpriteData &spriteData, COLOR_ARGB color)
{
    if(spriteData.texture == NULL)      // �e�N�X�`�����Ȃ��ꍇ
        return;

    // sprite�@�̒��S�����
    D3DXVECTOR2 spriteCenter=D3DXVECTOR2((float)(spriteData.width/2*spriteData.scale),
                                        (float)(spriteData.height/2*spriteData.scale));
    // sprite �̉�ʈʒu
    D3DXVECTOR2 translate=D3DXVECTOR2((float)spriteData.x,(float)spriteData.y);
    // X,Y�@�̊g��k��
    D3DXVECTOR2 scaling(spriteData.scale,spriteData.scale);
    if (spriteData.flipHorizontal)  // �������]����ꍇ
    {
        scaling.x *= -1;            // X �𕉂ɂ��Ĕ��]
        // ���]���ꂽ�摜�̒��S�擾
        spriteCenter.x -= (float)(spriteData.width*spriteData.scale);
        // ���[�𒆐S�ɔ��]���N����̂Ŕ��]���ꂽ�摜��
        // ���Ɠ����ʒu�ɂ��邽�߉E�Ɉړ�
        translate.x += (float)(spriteData.width*spriteData.scale);
    }
    if (spriteData.flipVertical)    // �������]����ꍇ
    {
        scaling.y *= -1;            // Y �𕉂ɂ��Ĕ��]
        // ���]���ꂽ�摜�̒��S�擾
        spriteCenter.y -= (float)(spriteData.height*spriteData.scale);
        // ��[�𒆐S�ɔ��]���N����̂Ŕ��]���ꂽ�摜��
        // ���Ɠ����ʒu�ɂ��邽�߉��Ɉړ�
        translate.y += (float)(spriteData.height*spriteData.scale);
    }
    // sprite �̉�]�A�g��k���A�z�u���s�����߂̔z����쐬
    D3DXMATRIX matrix;
    D3DXMatrixTransformation2D(
        &matrix,                // �s��
        NULL,                   // �g��k�����s���Ƃ����_�͍���̂܂�
        0.0f,                   // �g��k�����̉�]�Ȃ�
        &scaling,               // �g��k���̔{��
        &spriteCenter,          // ��]���S
        (float)(spriteData.angle),  // ��]�p�x
        &translate);            // X,Y �ʒu

	//�摜������
	if( spriteData.isBackGround == true )
	{
		D3DXMATRIX mTrans;
		D3DXMatrixTranslation(&mTrans, 0, 0, 1);
		matrix *= mTrans;
	}

    // �s����X�v���C�g�ɓK�p
    sprite->SetTransform(&matrix);

    // �X�v���C�g��`��
    sprite->Draw(spriteData.texture,&spriteData.rect,NULL,NULL,color);
}

//=============================================================================
//�O���t�B�b�N�X���������Ă��Ȃ������`�F�b�N
//=============================================================================
HRESULT Graphics::getDeviceState()
{ 
    result = E_FAIL;    // ���s���f�t�H���g�Ƃ��A�������ɒu������
    if (device3d == NULL)
        return  result;
    result = device3d->TestCooperativeLevel();	//���Ȃ����D3D_Ok������@�_���Ȃ炻�̑�����
    return result;
}

//=============================================================================
//�O���t�B�b�N�X�f�o�C�X�����Z�b�g
//=============================================================================
HRESULT Graphics::reset()
{
    result = E_FAIL;    // ���s���f�t�H���g�Ƃ��A�������ɒu������
    initD3Dpp();        // D3D�v���[���e�[�V�����p�����[�^��������
	sprite->OnLostDevice();
    result = device3d->Reset(&d3dpp);   // �O���t�B�b�N�X�f�o�C�X�̃��Z�b�g�����݂�

    sprite->OnResetDevice();
    return result;
}

//=============================================================================
// Toggle window or fullscreen mode
// Pre: All user created D3DPOOL_DEFAULT surfaces are freed.
// Post: All user surfaces are recreated.
//=============================================================================
void Graphics::changeDisplayMode(graphicsNS::DISPLAY_MODE mode)
{
    try{
        switch(mode)
        {
        case graphicsNS::FULLSCREEN:
            if(fullscreen)      // if already in fullscreen mode
                return;
            fullscreen = true; break;
        case graphicsNS::WINDOW:
            if(fullscreen == false) // if already in window mode
                return;
            fullscreen = false; break;
        default:        // default to toggle window/fullscreen
            fullscreen = !fullscreen;
        }
        reset();
        if(fullscreen)  // fullscreen
        {
            SetWindowLong(hwnd, GWL_STYLE,  WS_EX_TOPMOST | WS_VISIBLE | WS_POPUP);
        }
        else            // windowed
        {
            SetWindowLong(hwnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);
            SetWindowPos(hwnd, HWND_TOP, 0,0,GAME_WIDTH,GAME_HEIGHT,
                SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);

            // Adjust window size so client area is GAME_WIDTH x GAME_HEIGHT
            RECT clientRect;
            GetClientRect(hwnd, &clientRect);   // get size of client area of window
            MoveWindow(hwnd,
                       0,                                           // Left
                       0,                                           // Top
                       GAME_WIDTH+(GAME_WIDTH-clientRect.right),    // Right
                       GAME_HEIGHT+(GAME_HEIGHT-clientRect.bottom), // Bottom
                       TRUE);                                       // Repaint the window
        }

    } catch(...)
    {
        // An error occured, try windowed mode 
        SetWindowLong(hwnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);
        SetWindowPos(hwnd, HWND_TOP, 0,0,GAME_WIDTH,GAME_HEIGHT,
            SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);

        // Adjust window size so client area is GAME_WIDTH x GAME_HEIGHT
        RECT clientRect;
        GetClientRect(hwnd, &clientRect);   // get size of client area of window
        MoveWindow(hwnd,
                    0,                                           // Left
                    0,                                           // Top
                    GAME_WIDTH+(GAME_WIDTH-clientRect.right),    // Right
                    GAME_HEIGHT+(GAME_HEIGHT-clientRect.bottom), // Bottom
                    TRUE);                                       // Repaint the window
    }
}


/*
void Graphics::drawSprite(LP_TEXTURE texture)
{
	RECT r = { 0,0,200,200 };
	sprite->Draw(texture,&r,NULL,NULL,D3DCOLOR_ARGB(255,255,255,255) );
}
*/