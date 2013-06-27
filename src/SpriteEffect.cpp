#include "SpriteEffect.hpp"

namespace Spr
{

Effect::Effect()
{
	scaleX = 1.0f;
	scaleY = 1.0f;
	tileX = false;
	tileY = false;
	flipHorizontal = false;
	flipVertical = false;
	addBlending = false;
	parallax = false;
	dispx = 0;
}

Effect::~Effect()
{
}

bool Effect::isScaled() const
{
	return (scaleX != 1.0f) || (scaleY != 1.0f);
}

bool Effect::isFlipped() const
{
	return flipVertical || flipHorizontal;
}

void Effect::setScaleX(float scale)
{
	if (scale < 0.0)
		return;
	scaleX = scale;
}

void Effect::setScaleY(float scale)
{
	if (scale < 0.0)
		return;
	scaleY = scale;
}

void Effect::setParallax(bool n)
{
	parallax = n;
}

bool Effect::getParallax() const
{
	return parallax;
}

void Effect::setFlipVertical(bool n)
{
	flipVertical = n;
}

void Effect::setFlipHorizontal(bool n)
{
	flipHorizontal = n;
}

bool Effect::getFlipVertical() const
{
	return flipVertical;
}

bool Effect::getFlipHorizontal() const
{
	return flipHorizontal;
}

void Effect::setTileX(bool n)
{
	tileX = n;
}

void Effect::setTileY(bool n)
{
	tileY = n;
}

bool Effect::getTileX() const
{
	return tileX;
}

bool Effect::getTileY() const
{
	return tileY;
}

float Effect::getScaleX() const
{
    return scaleX;
}

float Effect::getScaleY() const
{
    return scaleY;
}

bool Effect::isAdditive() const
{
    return addBlending;
}

void Effect::setAdditive(bool n)
{
	addBlending = n;
}

void Effect::setDisplaceX(float v)
{
	dispx =  v;
}
float Effect::getDisplaceX() const
{
	return dispx;
}

void Effect::setDisplaceY(float v)
{
	dispy =  v;
}
float Effect::getDisplaceY() const
{
	return dispy;
}

}
