#include "Object.hpp"
namespace Char
{



Object::Object()
{
    posX = 0;
    posY = 0;
    prevPosX = 0;
    prevPosY = 0;
    velX = 0;
    velY = 0;
    prevVelX = 0;
    prevVelY = 0;
    scaleX = 2.0f;
    scaleY = 2.0f;
    isPosFreezed = false;
    xCoordRef = 0;
    yCoordRef = 0;
    facing = false;
}

Object::~Object()
{
    //dtor
}

void Object::setFacing(bool value)
{
    facing = value;
}

void Object::updateVelocity()
{
    prevPosX = posX;
    prevPosY = posY;
    if (facing)
        posX -= velX;
    else
        posX += velX;
    posY += velY;
}

void Object::setPos(double x, double y, bool save)
{
    if (!save)
    {
        prevPosX = x;
        prevPosY = y;
    }
    posX = x;
    posY = y;
}


bool Object::isFreezed() const
{
    return isPosFreezed;
}

void Object::setFreeze(bool value)
{
    isPosFreezed = value;
}


bool Object::getFacing() const
{
    return facing;
}

double Object::getDeltaX() const
{
    return posX - prevPosX;
}

double Object::getDeltaY() const
{
    return posY - prevPosY;
}

double Object::getDeltaVelX() const
{
    return velX - prevVelX;
}

double Object::getDeltaVelY() const
{
    return velY - prevVelY;
}

double Object::getVelX() const
{
    return velX;
}

double Object::getVelY() const
{
    return velY;
}

double Object::getPosX() const
{
    return posX;
}

double Object::getPosY() const
{
    return posY;
}

double Object::getPrevVelX() const
{
    return prevVelX;
}

double Object::getPrevVelY() const
{
    return prevVelY;
}

double Object::getPrevPosX() const
{
    return prevPosX;
}

double Object::getPrevPosY() const
{
    return prevPosY;
}

void Object::setVelX(double value)
{
    prevVelX = velX;
    velX = value;
}

void Object::setVelY(double value)
{
    prevVelY = velY;
    velY = value;
}

void Object::setPosX(double value)
{
    prevPosX = value;
    posX = value;
}

void Object::setPosY(double value)
{
    prevPosY = posY;
    posY = value;
}

double Object::getScaleX() const
{
    return scaleX;
}
double Object::getScaleY() const
{
    return scaleY;
}
void Object::setScaleX(double value)
{
    scaleX = value;
}
void Object::setScaleY(double value)
{
    scaleY = value;
}

void Object::setCoordReference(double x, double y)
{
    xCoordRef = x;
    yCoordRef = y;
}

void Object::getCoordReference(double& x, double& y)
{
    x = xCoordRef;
    y = yCoordRef;
}

void Object::velAdd(double xv, double yv)
{
    velX += xv;
    velY += yv;
}

}
