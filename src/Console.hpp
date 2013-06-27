#ifndef __CONSOLE_HPP__
#define __CONSOLE_HPP__

#include <string>
#include <cstdarg>
using namespace std;

namespace Console
{
bool append(const string str);
void setSize(int w, int h);
void draw();
bool isOpen();
bool open();
bool close();
bool toogle();
void print(const char *str, ...);
};

#endif
