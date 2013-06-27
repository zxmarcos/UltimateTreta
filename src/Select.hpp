#pragma once
#include "Stage.hpp"
#include "Game.hpp"
#include "Sounds.hpp"
#include "Tokenizer.hpp"
#include <string>
#include <vector>
#include <allegro5/allegro.h>

class Game;
using namespace std;

class SelectCharacter
{
public:
    SelectCharacter();
    ~SelectCharacter();
    bool load(string name);
    ALLEGRO_BITMAP *getBigPortrait() const;
    ALLEGRO_BITMAP *getMiniPortrait() const;
    string getName();
    void setName(string str);
    string getLoadPath();
private:
    ALLEGRO_BITMAP *big;
    ALLEGRO_BITMAP *mini;
    string charPath;
    string name;
};

class SelectStage
{
public:
    SelectStage();
    ~SelectStage();
    bool load(string name);
    ALLEGRO_BITMAP *getPreview() const;
    string getName();
    void setName(string str);
    string getLoadPath();
private:
    ALLEGRO_BITMAP *preview;
    string stagePath;
    string name;
};

class Select
{
public:
    Select(Game& env);
    virtual ~Select();
    bool open(string filename);
    int update();
    void reset();
private:
    bool parseSelect();
    void ignoreSection();
    void parseCharacterList();
    void parseStageList();
    void handleInput();
    void drawGrid();
    void drawPortraits();
    void drawNames();
    void drawStageInfo();
    SelectCharacter *getCharacter(int row, int column);
    Game& gameEnviroment;
    Stage::StageLoader stage;
    vector<string> charList;
    vector<string> stageList;
    vector<SelectCharacter*> characters;
    vector<SelectStage*> stages;
    Tok::Tokenizer lex;
    int rows;
    int columns;
    int cellWidth;
    int cellHeight;
    int portraitWidth;
    int portraitHeight;
    int cellSpacing;
    int cellStartX;
    int cellStartY;
    bool playerLock[2];
    Snd::Sounds sounds;
    bool stageSelect;
    int currentStage;
    bool readyToFight;
    struct {
        bool state[Input::K_MAX];
    } pinput[2];
    int players[2][2];
};
