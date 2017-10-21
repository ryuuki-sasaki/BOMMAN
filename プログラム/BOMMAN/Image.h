// Programming 2D Games
// Copyright (c) 2011 by: 
// Charles Kelly
// image.h v1.2

#ifndef _IMAGE_H                // Prevent multiple definitions if this 
#define _IMAGE_H                // file is included in more than one place
#define WIN32_LEAN_AND_MEAN

#include "textureManager.h"
#include "constants.h"

class Image		//�i��154-155�j
{
    // Image properties
  protected:
    Graphics *graphics;     // �O���t�B�b�N�X�N���X�̃|�C���^
    TextureManager *textureManager; // texture manager�^�̃|�C���^
    // spriteData contains the data required to draw the image by Graphics::drawSprite()
    SpriteData spriteData;  // SpriteData is defined in		"graphics.h��struct SpriteData"
    COLOR_ARGB colorFilter; // applied as a color filter (use WHITE for no change)
    int     cols;           // number of cols (1 to n) in multi-frame sprite
    int     startFrame;     // �J�n�t���[��
    int     endFrame;       // �I���t���[��
    int     currentFrame;   // ���݂̃t���[��
    float   frameDelay;     // ���b���ƂɃA�j���[�V������؂�ւ��邩
    float   animTimer;      // animation timer
    HRESULT hr;             // ������������Ƃ��ɓ����ϐ�
    bool    loop;           // �A�j���[�V�������J��Ԃ� true�Afalse �A�j���[�V�������Ō�̃t���[���łƂ܂�AanimComplete��ture������@
    bool    visible;        // �����邩�����Ȃ���
    bool    initialized;    // ���������I��������ǂ���
    bool    animComplete;   // �A�j���[�V�������@ture �A�j���[�V�������I������@false

  public:
    // Constructor
    Image();
    // Destructor
    virtual ~Image();

    ////////////////////////////////////////
    //           Get functions            //
    ////////////////////////////////////////

    // SpriteData�\���̂ւ̎Q�Ƃ�߂�.
    const virtual SpriteData& getSpriteInfo() {return spriteData;}

    // visible �p�����[�^��߂�.
    virtual bool  getVisible()  {return visible;}

    // X ��߂�.
    virtual float getX()        {return spriteData.x;}

    // Y ��߂�.
    virtual float getY()        {return spriteData.y;}

    // �{����߂�.
    virtual float getScale()    {return spriteData.scale;}

    // ����߂�.
    virtual int   getWidth()    {return spriteData.width;}

    // ������߂�.
    virtual int   getHeight()   {return spriteData.height;}

    // ���S X��߂�.
    virtual float getCenterX()      {return spriteData.x + spriteData.width/2*getScale();}

    // ���S Y��߂�.
    virtual float getCenterY()      {return spriteData.y + spriteData.height/2*getScale();}

    // �p�x�i�x�j��߂�.
    virtual float getDegrees()      {return spriteData.angle*(180.0f/(float)PI);}

    // �p�x�i���W�A���j��߂�.
    virtual float getRadians()      {return spriteData.angle;}

    // �t���[���Ԋu��߂�.
    virtual float getFrameDelay()   {return frameDelay;}

    // �J�n�t���[���ԍ���߂�.
    virtual int   getStartFrame()   {return startFrame;}

    // �I���t���[���ԍ���߂�.
    virtual int   getEndFrame()     {return endFrame;}

    // ���݂̃t���[���ԍ���߂�.
    virtual int   getCurrentFrame() {return currentFrame;}

    // Image��RECT�\���̂�߂�.
    virtual RECT  getSpriteDataRect() {return spriteData.rect;}

    // �A�j���[�V����������Ԃ�߂�.
    virtual bool  getAnimationComplete() {return animComplete;}

    // colorFilter��߂�.
    virtual COLOR_ARGB getColorFilter() {return colorFilter;}

    ////////////////////////////////////////
    //           Set functions            //
    ////////////////////////////////////////

    // Set X location.
    virtual void setX(float newX)   {spriteData.x = newX;}

    // Set Y location.
    virtual void setY(float newY)   {spriteData.y = newY;}

    // Set scale.
    virtual void setScale(float s)  {spriteData.scale = s;}

    // Set rotation angle in degrees.
    // 0 degrees is up. Angles progress clockwise.
    virtual void setDegrees(float deg)  {spriteData.angle = deg*((float)PI/180.0f);}

    // Set rotation angle in radians.
    // 0 radians is up. Angles progress clockwise.
    virtual void setRadians(float rad)  {spriteData.angle = rad;}

    // Set visible.
    virtual void setVisible(bool v) {visible = v;}

    // Set delay between frames of animation.
    virtual void setFrameDelay(float d) {frameDelay = d;}

    // Set starting and ending frames of animation.
    virtual void setFrames(int s, int e){startFrame = s; endFrame = e;}

    // Set current frame of animation.
    virtual void setCurrentFrame(int c);

    // Set spriteData.rect to draw currentFrame
    virtual void setRect(); 

    // Set spriteData.rect to r.
    virtual void setSpriteDataRect(RECT r)  {spriteData.rect = r;}

    // Set animation loop. lp = true to loop.
    virtual void setLoop(bool lp) {loop = lp;}

    // Set animation complete Boolean.
    virtual void setAnimationComplete(bool a) {animComplete = a;};

    // Set color filter. (use WHITE for no change)
    virtual void setColorFilter(COLOR_ARGB color) {colorFilter = color;}

    // Set TextureManager
    virtual void setTextureManager(TextureManager *textureM)
    { textureManager = textureM; }

    ////////////////////////////////////////
    //         Other functions            //
    ////////////////////////////////////////

    // Initialize Image
    // Pre: *g = pointer to Graphics object
    //      width = width of Image in pixels  (0 = use full texture width)
    //      height = height of Image in pixels (0 = use full texture height)
    //      ncols = number of columns in texture (1 to n) (0 same as 1)
    //      *textureM = pointer to TextureManager object
    virtual bool Image::initialize(Graphics *g, int width, int height, 
                                    int ncols, TextureManager *textureM);

    // Flip image horizontally (mirror)
    virtual void flipHorizontal(bool flip)  {spriteData.flipHorizontal = flip;}

    // Flip image vertically
    virtual void flipVertical(bool flip)    {spriteData.flipVertical = flip;}

    // Draw Image using color as filter. Default color is WHITE.
    virtual void draw(COLOR_ARGB color = graphicsNS::WHITE);	//draw�֐��̃I�[�o�[���[�h�@�i1�j�@�i�����@���@�`�Ƃ����ƁA�������ȗ������ꍇ�`������j
																//graphicsNS��graphics.h
    // Draw this image using the specified SpriteData.
    //   The current SpriteData.rect is used to select the texture.
    virtual void draw(SpriteData sd, COLOR_ARGB color = graphicsNS::WHITE); // draw with SpriteData using color as filter	�i2�j

    // Update the animation. frameTime is used to regulate the speed.
    virtual void update(float frameTime);

	void SetIsBackGround(bool b) 
		{spriteData.isBackGround = b;}
};

#endif

