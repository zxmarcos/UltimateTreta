#include <string>
#include <iostream>
#include <algorithm>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include "Sprites.hpp"
#include "SpriteEffect.hpp"
#include "Animations.hpp"
#include "Tokenizer.hpp"
#include "Stage.hpp"
#include "Console.hpp"
#include "Display.hpp"
#include "Camera.hpp"

namespace Stage
{

StageLoader::StageLoader()
{
    scaleX = 2.0;
    scaleY = 2.0;
    p1startX = -70;
    p2startX = 70;
    p1startY = 0;
    p2startY = 0;
    camera.boundHigh = 0;
    camera.boundLow = 0;
    camera.startX = 0;
    camera.startY = 0;
    camera.tension = 50;
    camera.boundLeft = 0;
    camera.boundRight = 0;
    rightLimit = 200;
    leftLimit = -200;
    music = false;
    playingMusic = false;
    backLayers.clear();
    frontLayers.clear();
    volume = 1.0;
}

StageLoader::~StageLoader()
{
    cout << "=============================================" << endl;
    cout << "   Liberando cenário..." << fileName << endl;
    cout << "=============================================" << endl;
    for (auto i : frontLayers)
        delete i;
    for (auto i : backLayers)
        delete i;
    safeDelete(music);
}

bool StageLoader::open(string name)
{
    fileName = name;
    cout << "=============================================" << endl;
    cout << "  carregando cenário " << fileName << "..." << endl;
    cout << "=============================================" << endl;
    if (!lex.open(name))
    {
        cerr << "Nao foi possivel abrir o arquivo" << endl;
        return false;
    }
    while (!lex.eof())
    {
        if (lex.check("["))
        {
            if (lex.check("def"))
            {
                lex.check("]");
                parseDefSection();
            }
            else if (lex.check("info"))
            {
                lex.check("]");
                parseInfoSection();
            }
            else if (lex.check("layer"))
            {
                lex.check("]");
                parseLayer();
            }
             else if (lex.check("camera"))
            {
                lex.check("]");
                parseCameraSection();
            }
            else
                ignoreSection();
        }
        else
            lex.consumeToken();
    }
    return true;
}

void StageLoader::parseDefSection()
{
    while (!lex.check("[", false) && !lex.eof())
    {
        // cout << "Definicioes" << endl;
        if (lex.check("name"))
        {
            lex.check("=");
            if (lex.checkLiteral())
            {
                name = lex.tok();
                Console::print("-- name = %s", name.c_str());
            }
        }
        else if (lex.check("sprites"))
        {
            lex.check("=");
            if (lex.checkLiteral())
            {
                sprites.open(lex.tok());
            }
        }
        else if (lex.check("animation"))
        {
            lex.check("=");
            if (lex.checkLiteral())
            {
                animations.open(lex.tok());
            }
        }
        else if (lex.check("music"))
        {
            lex.check("=");
            if (lex.checkLiteral())
            {
                music = new Audio::Music(lex.tok());
            }
        }
        else if (lex.check("music.volume"))
        {
            lex.check("=");
            lex.checkNumber();
            volume = lex.toFloat();
        }
        else
            break;
    }
}

void StageLoader::parseInfoSection()
{
    while (!lex.check("[", false) && !lex.eof())
    {
        // cout << "Definicioes" << endl;
        if (lex.check("p1start"))
        {
            lex.check("=");
            lex.checkNumber();
            p1startX = lex.toInt();
            lex.check(",");

            lex.checkNumber();
            p1startY = lex.toInt();
        }
        else if (lex.check("p2start"))
        {
            lex.check("=");
            lex.checkNumber();
            p2startX = lex.toInt();
            lex.check(",");

            lex.checkNumber();
            p2startY = lex.toInt();
        }
        else if (lex.check("xlimits"))
        {
            lex.check("=");
            lex.checkNumber();
            leftLimit = lex.toInt();
            lex.check(",");

            lex.checkNumber();
            rightLimit = lex.toInt();
        }
        else if (lex.check("ground"))
        {
            lex.check("=");
            lex.checkNumber();
            groundOffset = lex.toInt();
            Console::print("ground: %d", groundOffset);
        }
        else if (lex.check("scale"))
        {
            lex.check("=");
            lex.checkNumber();
            scaleX = lex.toFloat();
            lex.check(",");

            lex.checkNumber();
            scaleY = lex.toFloat();
        }
        else
            break;
    }
}

void StageLoader::parseCameraSection()
{
    while (!lex.check("[", false) && !lex.eof())
    {
        // cout << "Definicioes" << endl;
        if (lex.check("start"))
        {
            lex.check("=");

            lex.checkNumber();
            camera.startX = lex.toInt();
            lex.check(",");

            lex.checkNumber();
            camera.startY = lex.toInt();
        }
        else if (lex.check("tension"))
        {
            lex.check("=");
            lex.checkNumber();
            camera.tension = lex.toInt();
        }
        else if (lex.check("boundlow"))
        {
            lex.check("=");
            lex.checkNumber();
            camera.boundLow = lex.toInt();
        }
        else if (lex.check("boundhigh"))
        {
            lex.check("=");
            lex.checkNumber();
            camera.boundHigh = lex.toInt();
        }
        else if (lex.check("boundleft"))
        {
            lex.check("=");
            lex.checkNumber();
            camera.boundLeft = lex.toInt();
        }
        else if (lex.check("boundright"))
        {
            lex.check("=");
            lex.checkNumber();
            camera.boundRight = lex.toInt();
        }
        else
            break;
    }
}

void StageLoader::parseLayer()
{
    Layer *layer = new Layer(this);
    while (!lex.check("[", false) && !lex.eof())
    {
        // cout << "Definicioes" << endl;
        if (lex.check("type"))
        {
            lex.check("=");
            if (lex.check("normal"))
                layer->type = LAYER_NORMAL;
            else if (lex.check("animation"))
                layer->type = LAYER_ANIM;
             else if (lex.check("parallax"))
                layer->type = LAYER_PARALLAX;
            else
            {
                cerr << "tipo invalido de layer" << endl;
                ignoreSection();
            }
        }
        else if (lex.check("spriteno"))
        {
            lex.check("=");

            lex.checkNumber();
            layer->spriteGroup = lex.toInt();
            lex.check(",");

            lex.checkNumber();
            layer->spriteIndex = lex.toInt();

        }
        else if (lex.check("pos"))
        {
            lex.check("=");

            lex.checkNumber();
            layer->posX = lex.toInt();
            lex.check(",");

            lex.checkNumber();
            layer->posY = lex.toInt();

        }
        else if (lex.check("delta"))
        {
            lex.check("=");

            lex.checkNumber();
            layer->deltaX = lex.toFloat();
            lex.check(",");

            lex.checkNumber();
            layer->deltaY = lex.toFloat();

        }
        else if (lex.check("pdelta"))
        {
            lex.check("=");

            lex.checkNumber();
            layer->pDeltaX = lex.toFloat();
            lex.check(",");

            lex.checkNumber();
            layer->pDeltaY = lex.toFloat();

        }
        else if (lex.check("sprscale"))
        {
            lex.check("=");

            lex.checkNumber();
            layer->sprScaleX = lex.toFloat();
            lex.check(",");

            lex.checkNumber();
            layer->sprScaleY = lex.toFloat();

        }
        else if (lex.check("tile"))
        {
            lex.check("=");

            lex.checkNumber();
            layer->tileX = lex.toInt();
            lex.check(",");

            lex.checkNumber();
            layer->tileY = lex.toInt();
        }
        else if (lex.check("vel"))
        {
            lex.check("=");

            lex.checkNumber();
            layer->velX = lex.toFloat();
            lex.check(",");

            lex.checkNumber();
            layer->velY = lex.toFloat();
           // Console::print("vel %f,%f", layer->velX, layer->velY);
        }
        else if (lex.check("priority"))
        {
            lex.check("=");
            lex.checkNumber();
            layer->priority = lex.toInt();
        }
        else if (lex.check("action"))
        {
            lex.check("=");
            lex.checkNumber();
            layer->action = lex.toInt();
        }
        else if (lex.check("front"))
        {
            lex.check("=");
            lex.checkNumber();
            layer->front = lex.toInt();
        }
        else
            break;
    }

    // se o layer for animado precisamos criar um animador
    if (layer->type == LAYER_ANIM)
    {
        layer->animator = new Anim::Animator(animations);
        layer->animator->set(layer->action);
    }
    layer->posXinit = layer->posX;
    layer->posYinit = layer->posY;
    if (layer->front)
        frontLayers.push_back(layer);
    else
        backLayers.push_back(layer);
}

void StageLoader::ignoreSection()
{
    cout << "ignorando secao " << lex.tok() << endl;
    while (!lex.check("[", false) && !lex.eof())
        lex.consumeToken();
}

void StageLoader::drawBack(float posX, float posY)
{
    for (auto layer : backLayers)
        layer->draw(posX, posY);
  //  al_draw_line(0, (posY + groundOffset) * scaleY, 640, (posY + groundOffset) * scaleY, al_map_rgba(255, 0, 0, 100), 1);
}

void StageLoader::drawFront(float posX, float posY)
{
    for (auto layer : frontLayers)
        layer->draw(posX, posY);
}

void StageLoader::update()
{
    if (!playingMusic)
    {
        Audio::playMusic(music, volume);
        playingMusic = true;
    }
    for (auto layer : backLayers)
        layer->update();
    for (auto layer : frontLayers)
        layer->update();
}

void StageLoader::reset()
{
    playingMusic = false;
}

int StageLoader::getGroundOffset() const
{
    return groundOffset;
}
int StageLoader::getP1StartX() const
{
    return p1startX;
}
int StageLoader::getP1StartY() const
{
    return p1startY;
}
int StageLoader::getP2StartX() const
{
    return p2startX;
}
int StageLoader::getP2StartY() const
{
    return p2startY;
}
float StageLoader::getScaleX() const
{
    return scaleX;
}
float StageLoader::getScaleY() const
{
    return scaleY;

}
int StageLoader::getLeftLimit() const
{
    return leftLimit;
}
int StageLoader::getRightLimit() const
{
    return rightLimit;
}

const CameraInfo& StageLoader::getCamera() const
{
    return camera;
}

Layer::Layer(StageLoader *loader_)
{
    type = LAYER_NORMAL;
    posX = 0;
    posY = 0;
    velX = 0;
    velY = 0;
    action = 0;
    spriteGroup = 0;
    spriteIndex = 0;
    tileX = 0;
    tileY = 0;
    priority = 0;
    loader = loader_;
    animator = NULL;
    posXinit = posX;
    posYinit = posY;
    deltaX = 1;
    deltaY = 1;
    pDeltaX = 1;
    pDeltaY = 1;
    sprScaleX = 1;
    sprScaleY = 1;
    front = 0;
}

Layer::~Layer()
{
}

void Layer::reset()
{
    posX = posXinit;
    posY = posYinit;
}

void Layer::update()
{
    if (animator)
        animator->update();
    posX += velX;
    posY += velY;
}

void Layer::draw(float drawPosX, float drawPosY)
{
    int ax = 0;
    int ay = 0;

    float scalex = loader->scaleX;
    float scaley = loader->scaleY;
    const Spr::Sprite *spr = NULL;
    Spr::Effect effect;

    effect.setTileX(tileX);
    effect.setTileY(tileY);
    effect.setParallax(type == LAYER_PARALLAX);
    // Verifica se é uma animação
    if (type == LAYER_ANIM)
    {
        spr = loader->sprites.get(animator->group(), animator->index());
        ax = animator->x();
        ay = animator->y();
        effect.setAdditive(animator->additive());
        effect.setFlipVertical(animator->flipv());
        effect.setFlipHorizontal(animator->fliph());
    }
    else
        spr = loader->sprites.get(spriteGroup, spriteIndex);

    // Se o sprite não existir apenas retornamos...
    if (!spr)
        return;

    effect.setScaleX(scalex * sprScaleX);
    effect.setScaleY(scaley * sprScaleY);
    float xref = Display::getWidth() / 2.0;
    float yref = 0;
    float sprPosX = xref + (drawPosX * deltaX + (posX + ax) * scalex);
    float sprPosY = yref + (drawPosY * deltaY + (posY + ay) * scaley);

    effect.setDisplaceX(drawPosX * pDeltaX);
    effect.setDisplaceY(drawPosY * pDeltaY);

    if (spr)
    {
        spr->draw(sprPosX, sprPosY, effect);
    }
}



}
