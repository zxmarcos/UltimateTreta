//=============================================================================
// Marcos Medeiros
//=============================================================================
#ifndef __DISPLAY_HPP
#define __DISPLAY_HPP

#include <allegro5/allegro.h>

namespace Display
{

#define SYS_FONT    0
#define NORMAL_FONT    1
#define INFO_FONT      2

enum BlendModes
{
	Normal = 0,
	Additive,
};

extern ALLEGRO_DISPLAY *screen;
ALLEGRO_BITMAP *createBitmap(int w, int h);
bool setVideoMode(int width, int height, int bpp = 32);
void showBackBuffer();
void resetFrameCounter();
bool showDebug();
bool showBoxes();
void toogleDebug();
void toogleBoxes();
int getFrameCounter();
void destroy();
int getWidth();
int getHeight();
void drawText(int font, ALLEGRO_COLOR color, int x, int y, const char *str);
void drawRect(int x, int y, int w, int h, ALLEGRO_COLOR color);
void drawRectFill(int x, int y, int w, int h, ALLEGRO_COLOR color);
int textLineHeight(int font);
int textWidth(int font, const char *str);
void setBlendMode(int mode);
// funções relacionadas ao framerate
void setFrameRate(double rate);
void begin();
void end();
void toogleFrameRate();
};

#endif
