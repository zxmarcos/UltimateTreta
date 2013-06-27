#ifndef __STAGES_HPP__
#define __STAGES_HPP__

#include <string>
#include <vector>
#include "Sprites.hpp"
#include "Animations.hpp"
#include "Tokenizer.hpp"
#include "Camera.hpp"
#include "Audio.hpp"

using namespace std;
namespace Stage
{

enum LAYER_TYPE
{
    LAYER_NORMAL = 0,
    LAYER_ANIM,
    LAYER_PARALLAX,
};

class StageLoader;
class Layer
{
    friend class StageLoader;
public:
    Layer(StageLoader *loader_);
    ~Layer();
    void reset();
    void update();
private:
    void draw(float drawPosX, float drawPosY);
    int type;
    int spriteGroup;
    int spriteIndex;
    float velX;
    float velY;
    float posX;
    float posY;
    float posXinit;
    float posYinit;
    float deltaX;
    float deltaY;
    float pDeltaX;
    float pDeltaY;
    float sprScaleX;
    float sprScaleY;
    int tileX;
    int tileY;
    int priority;
    int action;
    int front;
    StageLoader *loader;
    Anim::Animator *animator;
};

class StageLoader
{
    friend class Layer;
public:
    StageLoader();
    ~StageLoader();

    bool open(string name);
    void draw(float posX, float posY);
    void drawBack(float posX, float posY);
    void drawFront(float posX, float posY);
    void update();
    int getGroundOffset() const;
    int getP1StartX() const;
    int getP1StartY() const;
    int getP2StartX() const;
    int getP2StartY() const;
    int getLeftLimit() const;
    int getRightLimit() const;
    float getScaleX() const;
    float getScaleY() const;
    const CameraInfo& getCamera() const;
    void reset();
private:
    bool playingMusic;
    string name;
    string fileName;
    Audio::Music *music;
    float volume;
    float scaleX;
    float scaleY;
    int p1startX;
    int p1startY;
    int p2startX;
    int p2startY;
    int groundOffset;
    int leftLimit;
    int rightLimit;
    CameraInfo camera;
    Spr::SpriteLoader sprites;
    Anim::AnimLoader animations;
    void parseDefSection();
    void parseInfoSection();
    void parseCameraSection();
    void parseLayer();
    void ignoreSection();
    Tok::Tokenizer lex;
    vector<Layer*> frontLayers;
    vector<Layer*> backLayers;
};

}

#endif // __STAGES_HPP__
