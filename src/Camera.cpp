#include <cmath>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include "Camera.hpp"
#include "Console.hpp"
#include "Display.hpp"
#include "Character.hpp"

using namespace std;

int _CX = 0, _CY = 0;

Camera::Camera()
{
	startX = 0;
	startY = 0;
	posX = 0;
	posY = 0;
	tension = 50;
	boundLow = 0;
	boundHigh = 0;
	boundLeft = -100;
	boundRight = 100;
	velX = 0;
	velY = 0;
	player1 = NULL;
	player2 = NULL;
	shakeAmplitude = 0;
	shakeFrequency = 0;
	shakeTime = 0;
}

Camera::~Camera()
{

}

void Camera::drawDebug()
{
    al_draw_line(tension, 0, tension, 480, al_map_rgba(255, 0, 0, 100), 1);
    al_draw_line(640-tension, 0, 640-tension, 480, al_map_rgba(255, 0, 0, 100), 1);
}

void Camera::setConfig(const CameraInfo& values)
{
    startX = values.startX;
    startY = values.startY;
    tension = values.tension;
    boundHigh = values.boundHigh;
    boundLow = values.boundLow;
    boundLeft = values.boundLeft;
    boundRight = values.boundRight;
    _CX = startX;
    _CY = startY;
}

void Camera::setPlayers(Char::Character *p1, Char::Character *p2)
{
	player1 = p1;
	player2 = p2;
}

int Camera::getX() const
{
	return posX;
}

int Camera::getY() const
{
	return posY;
}

void Camera::setStartX(int x)
{
	startX = x;
}
void Camera::setStartY(int y)
{
	startY = y;
}

void Camera::reset()
{
    posX = startX;
    posY = startY;
}

void Camera::update()
{
    Char::Character *left = NULL, *right = NULL, *upper = NULL;

    if (player1->getLeftBodyEdgeDistance() <= player2->getLeftBodyEdgeDistance())
    {
        left = player1;
        right = player2;
    }
    else
    {
        left = player2;
        right = player1;
    }

    if (player1->getPosY() < player2->getPosY())
    {
        upper = player1;
    }
    else
    {
        upper = player2;
    }

    const Char::SizeConstants& leftSz = left->getSize();
    const Char::SizeConstants& rightSz = right->getSize();

#if 0
    if (left->getLeftBodyEdgeDistance() < tension && right->getRightBodyEdgeDistance() > 0)
    {
        if (left->getPrevPosX() > left->getPosX())
        {
            posX += (int)(left->getPrevPosX() - left->getPosX()) * left->getScaleX();
        }
    }
    if (left->getLeftBodyEdgeDistance() > 0 && right->getRightBodyEdgeDistance() < tension)
    {
        if (right->getPrevPosX() < right->getPosX())
        {
            posX += (int)(right->getPrevPosX() - right->getPosX()) * right->getScaleX();
        }
    }
#else
    double deltaXs = abs(right->getPosX() * right->getScaleX() - left->getPosX() * left->getScaleX());

    int lpx = (left->getPosX()) * left->getScaleX() + Display::getWidth() / 2 + posX;
    int rpx = (right->getPosX()) * right->getScaleX() + Display::getWidth() / 2 + posX;

    double dx = abs(Display::getWidth() - deltaXs) / 2;
    posX =  dx - (left->getPosX() * left->getScaleX()) - Display::getWidth() / 2;

#endif
    posY -= (upper->getPosY() - upper->getPrevPosY()) * .23;

    if (posX > boundRight)
        posX = boundRight;
    else
    if (posX < boundLeft)
        posX = boundLeft;

    if (posY > boundHigh)
        posY = boundHigh;
    else
    if (posY < boundLow)
        posY = boundLow;

    if (shakeTime)
        updateShake();
    _CX = posX;
    _CY = posY;
}

void Camera::addShakeTime(int ticks)
{
    shakeTime += ticks;
}

void Camera::addShakeFreq(double freq)
{
    shakeFrequency += freq;
}

void Camera::addShakeAmpl(double ampl)
{
    shakeAmplitude += ampl;
}

void Camera::addShakePhase(double phase)
{
    shakePhase += phase;
}

void Camera::updateShake()
{

    // no máximo 60 vibrações por segundo
    const double sampleFreq = 60.0;
    const double period = 1.0 / sampleFreq;
    const double t = (shakeTime + (shakePhase * M_PI / 180.0)) * shakeFrequency;
    const double yd = sin(2 * M_PI * t * period);
    const double v = yd * shakeAmplitude;
    posY += v;
    shakeTime--;
    Console::print("shaking... %f", v);
    if (shakeTime <= 0) {
        shakeAmplitude = 0;
        shakeFrequency = 0;
        shakeTime = 0;
        shakePhase = 0;

    }
}
