#pragma once

namespace Char
{


class Object
{
public:
    Object();
    virtual ~Object();
    void setCoordReference(double x, double y);
    void getCoordReference(double& x, double& y);
    void updateVelocity();
    void setPos(double x, double y, bool save = true);
    virtual void setFacing(bool value);
    bool getFacing() const;
    bool isFreezed() const;
    void setFreeze(bool value);
    double getDeltaX() const;
    double getDeltaY() const;
    double getDeltaVelX() const;
    double getDeltaVelY() const;
    double getVelX() const;
    double getVelY() const;
    double getPosX() const;
    double getPosY() const;
    void velAdd(double xv, double yv);
    double getPrevVelX() const;
    double getPrevVelY() const;
    double getPrevPosX() const;
    double getPrevPosY() const;
    void setVelX(double value);
    void setVelY(double value);
    void setPosX(double value);
    void setPosY(double value);
    double getScaleX() const;
    double getScaleY() const;
    void setScaleX(double value);
    void setScaleY(double value);
protected:
    bool isPosFreezed;
    double xCoordRef;
    double yCoordRef;
    double drawPosX;
    double drawPosY;
    double posX;
    double posY;
    double prevPosX;
    double prevPosY;
    double velX;
    double velY;
    double prevVelX;
    double prevVelY;
    double scaleX;
    double scaleY;
    bool facing;
};

}
