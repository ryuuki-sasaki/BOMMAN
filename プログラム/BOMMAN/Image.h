// Programming 2D Games
// Copyright (c) 2011 by: 
// Charles Kelly
// image.h v1.2

#ifndef _IMAGE_H                // Prevent multiple definitions if this 
#define _IMAGE_H                // file is included in more than one place
#define WIN32_LEAN_AND_MEAN

#include "textureManager.h"
#include "constants.h"

class Image		//（ｐ154-155）
{
    // Image properties
  protected:
    Graphics *graphics;     // グラフィックスクラスのポインタ
    TextureManager *textureManager; // texture manager型のポインタ
    // spriteData contains the data required to draw the image by Graphics::drawSprite()
    SpriteData spriteData;  // SpriteData is defined in		"graphics.hのstruct SpriteData"
    COLOR_ARGB colorFilter; // applied as a color filter (use WHITE for no change)
    int     cols;           // number of cols (1 to n) in multi-frame sprite
    int     startFrame;     // 開始フレーム
    int     endFrame;       // 終了フレーム
    int     currentFrame;   // 現在のフレーム
    float   frameDelay;     // 何秒ごとにアニメーションを切り替えるか
    float   animTimer;      // animation timer
    HRESULT hr;             // 何かをやったときに入れる変数
    bool    loop;           // アニメーションを繰り返す true、false アニメーションが最後のフレームでとまり、animCompleteにtureが入る　
    bool    visible;        // 見えるか見えないか
    bool    initialized;    // 初期化が終わったかどうか
    bool    animComplete;   // アニメーション中　ture アニメーションが終わった　false

  public:
    // Constructor
    Image();
    // Destructor
    virtual ~Image();

    ////////////////////////////////////////
    //           Get functions            //
    ////////////////////////////////////////

    // SpriteData構造体への参照を戻す.
    const virtual SpriteData& getSpriteInfo() {return spriteData;}

    // visible パラメータを戻す.
    virtual bool  getVisible()  {return visible;}

    // X を戻す.
    virtual float getX()        {return spriteData.x;}

    // Y を戻す.
    virtual float getY()        {return spriteData.y;}

    // 倍率を戻す.
    virtual float getScale()    {return spriteData.scale;}

    // 幅を戻す.
    virtual int   getWidth()    {return spriteData.width;}

    // 高さを戻す.
    virtual int   getHeight()   {return spriteData.height;}

    // 中心 Xを戻す.
    virtual float getCenterX()      {return spriteData.x + spriteData.width/2*getScale();}

    // 中心 Yを戻す.
    virtual float getCenterY()      {return spriteData.y + spriteData.height/2*getScale();}

    // 角度（度）を戻す.
    virtual float getDegrees()      {return spriteData.angle*(180.0f/(float)PI);}

    // 角度（ラジアン）を戻す.
    virtual float getRadians()      {return spriteData.angle;}

    // フレーム間隔を戻す.
    virtual float getFrameDelay()   {return frameDelay;}

    // 開始フレーム番号を戻す.
    virtual int   getStartFrame()   {return startFrame;}

    // 終了フレーム番号を戻す.
    virtual int   getEndFrame()     {return endFrame;}

    // 現在のフレーム番号を戻す.
    virtual int   getCurrentFrame() {return currentFrame;}

    // ImageのRECT構造体を戻す.
    virtual RECT  getSpriteDataRect() {return spriteData.rect;}

    // アニメーション完了状態を戻す.
    virtual bool  getAnimationComplete() {return animComplete;}

    // colorFilterを戻す.
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
    virtual void draw(COLOR_ARGB color = graphicsNS::WHITE);	//draw関数のオーバーロード　（1）　（引数　＝　～とかくと、引数を省略した場合～が入る）
																//graphicsNSはgraphics.h
    // Draw this image using the specified SpriteData.
    //   The current SpriteData.rect is used to select the texture.
    virtual void draw(SpriteData sd, COLOR_ARGB color = graphicsNS::WHITE); // draw with SpriteData using color as filter	（2）

    // Update the animation. frameTime is used to regulate the speed.
    virtual void update(float frameTime);

	void SetIsBackGround(bool b) 
		{spriteData.isBackGround = b;}
};

#endif

