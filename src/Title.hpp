#pragma once
#include "Stage.hpp"
#include "Game.hpp"
#include "Sounds.hpp"
#include <string>
#include <vector>

class Game;
using namespace std;

class Title
{
public:
    Title(Game& env);
    virtual ~Title();
    bool open(string filename);
    int update();
    void reset();
private:
    struct MenuEntry {
        int opt;
        const char *str;
        MenuEntry(int o = 0, const char *s = NULL) {
            opt = o;
            str = s;
        }
    };
    Snd::Sounds sounds;
    bool lastUp;
    bool lastDown;
    bool lastEnter;
    bool lastBack;
    bool isOnAbout;
    const int menuFont = 1;
    void drawMenu();
    void drawAbout();
    void handleInput();
    Game& gameEnviroment;
    bool exitGame;
    Stage::StageLoader stage;
    int option;
    int time;
    vector<MenuEntry> entrys;
};


