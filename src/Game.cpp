//==============================================================================
// A lógica principal do jogo
// Marcos Medeiros
//==============================================================================
#include <ctime>
#include <cstdlib>
#include <sstream>
#include <iostream>
#include "Game.hpp"
#include "Display.hpp"
#include "Audio.hpp"
#include "InputManager.hpp"
#include "Console.hpp"
#include "Fight.hpp"
#include "Title.hpp"

using namespace std;

Game::Game()
{
    fight = NULL;
    title = NULL;
    select = NULL;
    isRunning = false;
    eventQueue = NULL;
}

Game::~Game()
{
}

void Game::init()
{
	Display::setVideoMode(640, 480);
	Display::setFrameRate(60);
	Audio::init(10);
	srand(time(NULL));
	isRunning = true;
	// cria a fila de eventos
	eventQueue = al_create_event_queue();
	timer = al_create_timer(1.0);
	al_register_event_source(eventQueue, al_get_keyboard_event_source());
    al_register_event_source(eventQueue, al_get_display_event_source(Display::screen));
    al_register_event_source(eventQueue, al_get_timer_event_source(timer));

    title = new Title(*this);
    select = new Select(*this);
    title->open("data/title/title.txt");
    select->open("data/select/select.txt");

    // Inicia nosso temporalizador
    al_start_timer(timer);
    gameState = GAME_STATE_TITLE;
}

void Game::close()
{
    safeDelete(title);
    safeDelete(fight);
    safeDelete(select);
    Display::destroy();
}


void Game::run()
{
	while (isRunning)
	{
		updateSubsystems();
		Display::begin();

		switch (gameState)
		{
        case GAME_STATE_TITLE:
            title->update();
            break;
        case GAME_STATE_EXIT:
            isRunning = false;
            Audio::stopBGM();
            break;
        case GAME_STATE_BATTLE:
            if (fight)
                fight->update();
            break;
        case GAME_STATE_SELECT:
            select->update();
            break;
		}
		Console::draw();
		Display::showBackBuffer();
        Display::end();
	}

}

void Game::changeState(int state)
{
    if (state == GAME_STATE_BATTLE)
    {
        if (fight)
        {
            fight->reset();
            gameState = GAME_STATE_BATTLE;
            return;
        }
    }
    else
    {
        gameState = state;
        if (gameState == GAME_STATE_BATTLE)
            fight->reset();
        else if (gameState == GAME_STATE_SELECT)
            select->reset();
    }
}

void Game::createFight(string char1, string char2, string stage)
{
    if (fight)
        safeDelete(fight);
    fight = new Fight(*this);
    if (!fight->load(char1, char2, stage))
    {
        cout << "Não foi possível criar uma luta com: " << endl;
        cout << "player1: " << char1 << endl;
        cout << "player2: " << char2 << endl;
        cout << "stagebg: " << stage << endl;
        safeDelete(fight);
        exit(1);
    }
}

void Game::updateSubsystems()
{
	ALLEGRO_EVENT event;
	while (al_get_next_event(eventQueue, &event))
    {
        if (event.type == ALLEGRO_EVENT_TIMER)
        {
            if (gameState == GAME_STATE_BATTLE)
                if (fight)
                    fight->triggerSecond();
            int fps = Display::getFrameCounter();
            Display::resetFrameCounter();
            stringstream ss;
            ss << "FPS: " << fps;
            al_set_window_title(Display::screen, ss.str().c_str());
        }
        if (event.type == ALLEGRO_EVENT_KEY_UP)
        {
        	Input::setRawKeyState(event.keyboard.keycode, 0);
        }
        else if (event.type == ALLEGRO_EVENT_KEY_DOWN)
        {
            if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
            {
                if (gameState == GAME_STATE_TITLE)
                    isRunning = false;
                else if (gameState == GAME_STATE_BATTLE)
                {
                    select->reset();
                    changeState(GAME_STATE_SELECT);
                    safeDelete(fight);
                }
                else if (gameState == GAME_STATE_SELECT)
                {
                    title->reset();
                    changeState(GAME_STATE_TITLE);
                }
            }
            else if (event.keyboard.keycode == ALLEGRO_KEY_F1)
            {
                Display::toogleFrameRate();
            }
            else if (event.keyboard.keycode == ALLEGRO_KEY_F2)
            {
                Display::toogleDebug();
            }
            else if (event.keyboard.keycode == ALLEGRO_KEY_F3)
            {
                Display::toogleBoxes();
            }
            else if (event.keyboard.keycode == ALLEGRO_KEY_SPACE)
            {
                Console::toogle();
            }
            else
            {
            	Input::setRawKeyState(event.keyboard.keycode, 1);
            }

        }
        else if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
            isRunning = false;
    }
}

