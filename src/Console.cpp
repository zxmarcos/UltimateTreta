#include <string>
#include <list>
#include <cstdarg>
#include <cstdio>
#include <allegro5/allegro.h>
#include "Console.hpp"
#include "Display.hpp"

namespace Console
{
static int width = 640;
static int height = 200;
static size_t maxLines = 30;
static int realHeight = 300;
static bool isVisible = false;
static bool isAnimating = false;
static bool closeAnim = false;
static int state = 0;
list<string> consoleLines;

bool isOpen()
{
    return isVisible;
}

bool toogle()
{
    if (isOpen())
        close();
    else
        open();
    state ^= 1;
    return true;
}

bool open()
{
    if (!isAnimating)
    {
        closeAnim = false;
        isAnimating = true;
        realHeight = height;
        height = 0;
        isVisible = true;
        return true;
    }
    return false;
}

bool close()
{
    if (!isAnimating)
    {
        closeAnim = true;
        realHeight = height;
        isAnimating = true;
        return true;
    }
    return false;
}

bool append(const string str)
{
    while (consoleLines.size() >= maxLines)
    {
        consoleLines.pop_front();
    }
    consoleLines.push_back(str);
    return true;
}

void setSize(int w, int h)
{
    if (w >= 0)
        width = w;
    if (h >= 0)
        height = h;
}

void draw()
{
    if (!isVisible)
        return;

    if (isAnimating)
    {
        if (!closeAnim)
        {
            height += 20;
            if (height >= realHeight)
            {
                isAnimating = false;
                height = realHeight;
            }
        }
        else
        {
            height -= 20;
            if (height <= 0)
            {
                isAnimating = false;
                isVisible = false;
                height = realHeight;
                return;
            }
        }
    }

    Display::drawRectFill(0, 0, width, height, al_map_rgba(0, 0, 0, 200));
    int ydelta = Display::textLineHeight(0);
    int y = height - ydelta;

    for (auto i = consoleLines.rbegin(); i != consoleLines.rend(); i++)
    {
        Display::drawText(0, al_map_rgb(255, 255, 255), 0, y, (*i).c_str());
        y -= ydelta - 1;
        if (y < 0)
            break;
    }

}

void print(const char *str, ...)
{
    char buffer[1024];
    va_list va;
    va_start(va, str);
    vsprintf(buffer, str, va);
    va_end(va);
    append(string(buffer));
}

};
