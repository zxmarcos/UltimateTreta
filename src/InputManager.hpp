#ifndef __INPUTMANAGER_HPP__
#define __INPUTMANAGER_HPP__

#include <allegro5/allegro.h>

namespace Input
{

enum InputSymbol
{
    K_START = 0,
    K_SELECT,
    K_UP,
    K_DOWN,
    K_BACK,
    K_FOWARD,
    K_A,
    K_B,
    K_C,
    K_X,
    K_Y,
    K_Z,
    K_MAX
};

const char *getSymbolName(int symbol);

// Definições de mais baixo nivel no sistema, apenas armazena as informações
// se uma tecla foi escrita como pressionada ou não
void setRawKeyState(unsigned keycode, int status);
int getRawKeyState(int keycode);
int readRawKey();


// Partes de mais alto nivel, controlando estados como KEYDOWN, KEYUP, KEYHOLD
// Entrada dinâmica com bind
void setPlayerInputFacing(int player, bool facing = false);
bool bindKeyToSymbol(int player, int keycode, int symbol);
int readSymbol(int player, int symbol);
void clearSymbols(int player);

}

#endif // __INPUTMANAGER_HPP__
