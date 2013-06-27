#ifndef __GAME_HPP__
#define __GAME_HPP__

#include <allegro5/allegro.h>
#include "Title.hpp"
#include "Fight.hpp"
#include "Select.hpp"

class Title;
class Fight;
class Select;

class Game
{
public:
    enum {
        GAME_STATE_EXIT = 0,
        GAME_STATE_TITLE,
        GAME_STATE_SELECT,
        GAME_STATE_BATTLE,
    };
    Game();
    ~Game();
    void init();
    void run();
    void close();
    void changeState(int state);
    void createFight(string char1, string char2, string stage);
private:
    int gameState;
    void updateSubsystems();
    bool isRunning;
    Fight *fight;
    Title *title;
    Select *select;
    ALLEGRO_EVENT_QUEUE *eventQueue;
    ALLEGRO_TIMER *timer;

};

#endif // __GAME_HPP__
