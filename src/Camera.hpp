#ifndef __CAMERA_HPP__
#define __CAMERA_HPP__

#include "Character.hpp"


extern int _CX, _CY;
struct CameraInfo
{
    int startX;
    int startY;
    int tension;
    int boundLow;
    int boundHigh;
    int boundLeft;
    int boundRight;
};

class Camera
{
public:
    Camera();
    ~Camera();
    void setPlayers(Char::Character *p1, Char::Character *p2);
    int getX() const;
    int getY() const;
    void reset();
    void setStartX(int x);
    void setStartY(int y);
    void update();
    void setConfig(const CameraInfo& values);
    void drawDebug();
    void addShakeTime(int ticks);
    void addShakeFreq(double freq);
    void addShakeAmpl(double ampl);
    void addShakePhase(double phase);
private:
    void updateShake();
    int startX;
    int startY;
    int posX;
    int posY;
    int lastPosX;
    int lastPosY;
    int boundLeft;
    int boundRight;
    int boundHigh;
    int boundLow;
    int tension;
    float velX;
    float velY;
    Char::Character *player1;
    Char::Character *player2;
    double shakeFrequency;
    int shakeTime;
    double shakePhase;
    double shakeAmplitude;
};


#endif
