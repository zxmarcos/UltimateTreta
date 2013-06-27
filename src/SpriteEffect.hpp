#ifndef __SPRITEEFFECT_HPP__
#define __SPRITEEFFECT_HPP__

namespace Spr
{


class Effect
{
public:
	Effect();
	~Effect();
	bool isScaled() const;
	bool isFlipped() const;
	void setFlipVertical(bool n);
	void setFlipHorizontal(bool n);
	bool getFlipVertical() const;
	bool getFlipHorizontal() const;
	void setScaleX(float scale);
	void setScaleY(float scale);
	float getScaleX() const;
	float getScaleY() const;
	bool getTileX() const;
	bool getTileY() const;
	void setTileX(bool n);
	void setTileY(bool n);
	bool isAdditive() const;
	void setAdditive(bool n);
	void setParallax(bool n);
	bool getParallax() const;
	void setDisplaceX(float v);
	float getDisplaceX() const;
	void setDisplaceY(float v);
	float getDisplaceY() const;
private:
	float dispx;
	float dispy;
	bool parallax;
	float scaleX;
	float scaleY;
	bool flipVertical;
	bool flipHorizontal;
	bool tileX;
	bool tileY;
	bool addBlending;
};

}

#endif
