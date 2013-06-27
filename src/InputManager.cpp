//==============================================================================
// Sistema de entrada
// Marcos Medeiros
//==============================================================================
#include <allegro5/allegro.h>
#include "InputManager.hpp"

namespace Input
{

#define JOYSTICK_FLAG

// BindTable, onde um simbolo de entrada corresponde a uma tecla
int inputBindTable[2][K_MAX] =
{
	{
		[K_START]	= ALLEGRO_KEY_ENTER,
		[K_SELECT]	= ALLEGRO_KEY_BACKSPACE,
		[K_UP]		= ALLEGRO_KEY_UP,
		[K_DOWN]	= ALLEGRO_KEY_DOWN,
		[K_BACK]	= ALLEGRO_KEY_LEFT,
		[K_FOWARD]	= ALLEGRO_KEY_RIGHT,
		[K_A]		= ALLEGRO_KEY_A,
		[K_B]		= ALLEGRO_KEY_S,
		[K_C]		= ALLEGRO_KEY_D,
		[K_X]		= ALLEGRO_KEY_Z,
		[K_Y]		= ALLEGRO_KEY_X,
		[K_Z]		= ALLEGRO_KEY_C,
	},
	{
		[K_START]	= ALLEGRO_KEY_INSERT,
		[K_SELECT]	= ALLEGRO_KEY_DELETE,
		[K_UP]		= ALLEGRO_KEY_I,
		[K_DOWN]	= ALLEGRO_KEY_K,
		[K_BACK]	= ALLEGRO_KEY_J,
		[K_FOWARD]	= ALLEGRO_KEY_L,
		[K_A]		= ALLEGRO_KEY_F,
		[K_B]		= ALLEGRO_KEY_G,
		[K_C]		= ALLEGRO_KEY_H,
		[K_X]		= ALLEGRO_KEY_V,
		[K_Y]		= ALLEGRO_KEY_B,
		[K_Z]		= ALLEGRO_KEY_N,
	},
};


static int keyboardRawState[ALLEGRO_KEY_MAX] = { 0 };
// Tolerância de repetição para o botão ser solto
const unsigned symbolToleranceToRelease = 1;
// Tolerância de repetição para o botão ser marcado como segurado
const unsigned symbolToleranceToHold = 1;
bool playerFacing[2] = { false, false };


//==============================================================================
// Retorna o nome de um simbolo
//==============================================================================
const char *getSymbolName(int symbol)
{
    if (symbol >= K_MAX)
        return NULL;
    static const char *symNames[K_MAX] = {
        "start", "select", "U", "D", "B", "F",
        "a", "b", "c", "x", "y", "z"
    };
    return symNames[symbol];
}

//==============================================================================
// Modifica o estado de uma tecla
//==============================================================================
void setRawKeyState(unsigned keycode, int status)
{
	if (keycode >= ALLEGRO_KEY_MAX)
		return;
	keyboardRawState[keycode] = status;
}

//==============================================================================
// Retorna se uma tecla está pressionada ou não
//==============================================================================
int getRawKeyState(int keycode)
{
	if (keycode >= ALLEGRO_KEY_MAX)
		return 0;
	return keyboardRawState[keycode];
}

//==============================================================================
// Função utilizada para encontrar a primeira tecla pressionada
//==============================================================================
int readRawKey()
{
	for (int i = 0; i < ALLEGRO_KEY_MAX; i++)
		if (keyboardRawState[i])
			return i;
	return 0;
}


//==============================================================================
// Mapeia um simbolo a uma tecla
//==============================================================================
bool bindKeyToSymbol(int player, unsigned keycode, int symbol)
{
	if (player >= 2 || keycode >= ALLEGRO_KEY_MAX)
		return false;
	inputBindTable[player][symbol] = keycode;
	return true;
}

//==============================================================================
// Lê um simbolo em baixo nivel
//==============================================================================
int readSymbol(int player, int symbol)
{
	if (player >= 2 || symbol >= K_MAX)
		return 0;

	// Controla o facing (controles invertidos na horizontal)
	#if 1
	if (playerFacing[player])
	{
		if (symbol == K_FOWARD || symbol == K_BACK)
		{
			if (symbol == K_FOWARD)
				return getRawKeyState(inputBindTable[player][K_BACK]);
			else
				return getRawKeyState(inputBindTable[player][K_FOWARD]);
		}

	}
	#endif
	return getRawKeyState(inputBindTable[player][symbol]);
}

void clearSymbols(int player)
{
    for (int i = 0; i < K_MAX; i++)
        setRawKeyState(inputBindTable[player][i], 0);
}

//==============================================================================
// Configura o estado de facing onde o player tem as entradas FOWARD e BACK
// invertidas quando são lidas
//==============================================================================
void setPlayerInputFacing(int player, bool facing)
{
	if (player >= 2)
		return;
	playerFacing[player] = facing;
}

}
