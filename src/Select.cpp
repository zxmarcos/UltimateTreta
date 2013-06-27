#include <iostream>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include "Select.hpp"
#include "Display.hpp"

using namespace std;

Select::Select(Game& env) : gameEnviroment(env)
{
    parseSelect();
    rows = 2;
    columns = 4;
    cellHeight = 60;
    cellWidth = 60;
    portraitHeight = 125;
    portraitWidth = 100;

    playerLock[0] = false;
    playerLock[1] = false;
    players[0][0] = 0;
    players[0][1] = 0;
    players[1][0] = columns - 1;
    players[1][1] = 0;
    stageSelect = false;
    readyToFight = false;

    cellSpacing = 6;
    cellStartY = 300;
    cellStartX = Display::getWidth() / 2  - (columns * cellWidth + (columns - 1) * cellSpacing) / 2;
    currentStage = 0;
}

Select::~Select()
{
    for (auto s : characters)
        delete s;
    for (auto s : stages)
        delete s;
}

bool Select::open(string filename)
{
    stage.open(filename);
    sounds.open("data/select/select.snd");
    return true;
}

int Select::update()
{
    Input::setPlayerInputFacing(1);
    al_clear_to_color(al_map_rgb(0, 0, 0));
    stage.drawBack(0, 0);
    stage.drawFront(0, 0);
    drawPortraits();
    drawGrid();
    handleInput();
    stage.update();

    if (stageSelect)
        drawStageInfo();
    if (!stageSelect && (playerLock[0] && playerLock[1]))
        stageSelect = true;

    // vamos criar a luta e mudar o estado
    if (readyToFight)
    {
        string char1 = getCharacter(players[0][1], players[0][0])->getLoadPath();
        string char2 = getCharacter(players[1][1], players[1][0])->getLoadPath();
        string stbgn = stages[currentStage]->getLoadPath();
        gameEnviroment.createFight(char1, char2, stbgn);
        gameEnviroment.changeState(Game::GAME_STATE_BATTLE);
    }
    return 0;
}

void Select::reset()
{
    stage.reset();
    playerLock[0] = false;
    playerLock[1] = false;
    players[0][0] = 0;
    players[0][1] = 0;
    players[1][0] = columns - 1;
    players[1][1] = 0;
    for (int player = 0; player < 2; player++)
        for (int inp = 0; inp < Input::K_MAX; inp++)
            pinput[player].state[inp] = false;
    stageSelect = false;
    currentStage = 0;
    readyToFight = false;
}

SelectCharacter *Select::getCharacter(int row, int column)
{
    size_t pos = row * columns + column;
    if (pos >= characters.size())
        return NULL;
    return characters[pos];
}

void Select::handleInput()
{
    // se estivermos prontos para luta, não controlamos nada...
    if (readyToFight)
    {
        for (int player = 0; player < 2; player++)
            for (int inp = 0; inp < Input::K_MAX; inp++)
                pinput[player].state[inp] = false;
        return;
    }

    bool now[2][Input::K_MAX];
    for (int player = 0; player < 2; player++)
        for (int inp = 0; inp < Input::K_MAX; inp++)
            now[player][inp] = Input::readSymbol(player, inp) ? true : false;

    bool playLockSound = false;
    bool playChangeSound = false;
    for (int player = 0; player < 2; player++)
    {
        if (playerLock[player] && !stageSelect)
            continue;

        for (int inp = Input::K_A; inp < Input::K_MAX; inp++)
        {
            if (now[player][inp] && !pinput[player].state[inp])
            {
                if (getCharacter(players[player][1], players[player][0]))
                {
                    if (stageSelect)
                    {
                        readyToFight = true;
                        playLockSound = true;
                        goto _ok;
                    }
                    else
                    {
                        playLockSound = true;
                        playerLock[player] = true;
                    }
                    break;
                }
            }
        }
        // verifica se está escolhendo o player
        if (playerLock[player] && !stageSelect)
            continue;

        if (stageSelect)
        {
            if (now[player][Input::K_FOWARD] && !pinput[player].state[Input::K_FOWARD])
            {
                currentStage = (currentStage + 1) % stages.size();
                playChangeSound = true;
            }
            if (now[player][Input::K_BACK] && !pinput[player].state[Input::K_BACK])
            {
                if (--currentStage < 0)
                    currentStage = stages.size() - 1;
                playChangeSound = true;
            }
        }
        else
        {

            if (now[player][Input::K_FOWARD] && !pinput[player].state[Input::K_FOWARD])
            {
                players[player][0] = (players[player][0] + 1) % columns;
                playChangeSound = true;
            }
            if (now[player][Input::K_BACK] && !pinput[player].state[Input::K_BACK])
            {
                if (--players[player][0] < 0)
                    players[player][0] = columns - 1;
                playChangeSound = true;
            }
        }

        if (!stageSelect)
        {

            if (now[player][Input::K_DOWN] && !pinput[player].state[Input::K_DOWN])
            {
                players[player][1] = (players[player][1] + 1) % rows;
                playChangeSound = true;
            }
            if (now[player][Input::K_UP] && !pinput[player].state[Input::K_UP])
            {
                if (--players[player][1] < 0)
                    players[player][1] = rows- 1;
                playChangeSound = true;
            }
        }
    }
_ok:
    for (int player = 0; player < 2; player++)
        for (int inp = 0; inp < Input::K_MAX; inp++)
            pinput[player].state[inp] = now[player][inp];

    if (playChangeSound)
        sounds.playSound(0, 1);
    if (playLockSound)
        sounds.playSound(0, 0);

}

void Select::drawStageInfo()
{
    int h = Display::textLineHeight(NORMAL_FONT);
    int text_sy = cellStartY + rows * cellHeight + (rows - 1) * cellSpacing;
    int prev_sy = cellStartY - cellSpacing;
    ALLEGRO_COLOR color1 = al_map_rgb(255, 0, 0);
    ALLEGRO_COLOR color2 = al_map_rgb(255, 255, 0);
    ALLEGRO_COLOR color3 = al_map_rgb(200, 100, 0);

    Display::drawRectFill(0,
                          text_sy,
                          Display::getWidth(),
                          h,
                          al_map_rgba(0, 0, 0, 150));
    SelectStage *ss = stages.at(currentStage);
    if (ss)
    {
        ALLEGRO_BITMAP *bmp = ss->getPreview();
        if (bmp)
        {
            int ph = al_get_bitmap_height(bmp);
            int pw = al_get_bitmap_width(bmp);
            prev_sy -= ph;
            int x = Display::getWidth() / 2 - pw / 2;
            al_draw_bitmap(bmp, x, prev_sy, 0);
            al_draw_rectangle(x, prev_sy, x + pw, prev_sy + ph, color3, 3);
        }
        const char *title = ss->getName().c_str();
        int tw = Display::textWidth(NORMAL_FONT, title);
        int x = Display::getWidth() / 2 - tw / 2;
        Display::drawText(NORMAL_FONT, color1, x, text_sy, title);
        text_sy++;
        Display::drawText(NORMAL_FONT, color2, x, text_sy, title);
    }

}

void Select::drawPortraits()
{
    SelectCharacter *player = getCharacter(players[0][1], players[0][0]);
    if (player)
    {
        ALLEGRO_BITMAP *bmp = player->getBigPortrait();
        if (bmp)
        {
            int h = al_get_bitmap_height(bmp);
            al_draw_bitmap(bmp, 0, Display::getHeight() - h, 0);
        }
    }
    player = getCharacter(players[1][1], players[1][0]);
    if (player)
    {
        ALLEGRO_BITMAP *bmp = player->getBigPortrait();
        if (bmp)
        {
            int h = al_get_bitmap_height(bmp);
            int w = al_get_bitmap_width(bmp);
            al_draw_bitmap(bmp, Display::getWidth() - w, Display::getHeight() - h, ALLEGRO_FLIP_HORIZONTAL);
        }
    }
}

void Select::drawNames()
{
    int x = 0;
    int y = 0;
    int iy = cellStartY + (rows * cellHeight + (rows - 1) * cellSpacing) / 2 - (Display::textLineHeight(NORMAL_FONT) / 2);

    int p2x = cellStartX + columns * cellWidth + (columns - 1) * cellSpacing;

    ALLEGRO_COLOR color1 = al_map_rgb(255, 0, 0);
    ALLEGRO_COLOR color2 = al_map_rgb(255, 255, 0);
    SelectCharacter *player = getCharacter(players[0][1], players[0][0]);
    if (player)
    {
        string name = player->getName();
        x = cellStartX / 2 - Display::textWidth(NORMAL_FONT, name.c_str()) / 2;
        y = iy;
        Display::drawText(NORMAL_FONT, color1, x, y, name.c_str());
        y++;
        Display::drawText(NORMAL_FONT, color2, x, y, name.c_str());
    }
    player = getCharacter(players[1][1], players[1][0]);
    if (player)
    {
        string name = player->getName();
        x = p2x + (Display::getWidth() - p2x) / 2 - Display::textWidth(NORMAL_FONT, name.c_str()) / 2;
        y = iy;;
        Display::drawText(NORMAL_FONT, color1, x, y, name.c_str());
        y++;
        Display::drawText(NORMAL_FONT, color2, x, y, name.c_str());
    }
}

void Select::drawGrid()
{
    ALLEGRO_COLOR activep1 = playerLock[0] ? al_map_rgba(0, 255, 255, 150) : al_map_rgba(0, 0, 255, 150);
    ALLEGRO_COLOR activep2 = playerLock[1] ? al_map_rgba(255, 255, 0, 150) : al_map_rgba(255, 0, 0, 150);
    ALLEGRO_COLOR inative = al_map_rgba(200, 200, 200, 0);

    Display::drawRectFill(0,
                          cellStartY,
                          Display::getWidth(),
                          rows * cellHeight + (rows - 1) * cellSpacing,
                          al_map_rgba(0, 0, 0, 150));


    for (int y = 0; y < rows; y++)
    {
        for (int x = 0; x < columns; x++)
        {
            al_draw_filled_rectangle(cellStartX + x * cellWidth + cellSpacing * x,
                                     cellStartY + y * cellHeight + cellSpacing * y,
                                     cellStartX + x * cellWidth + cellSpacing * x + cellWidth,
                                     cellStartY + y * cellHeight + cellSpacing * y + cellHeight,
                                     al_map_rgba(0, 0, 0, 100));
            ALLEGRO_COLOR color;
            if (y == players[0][1] && x == players[0][0])
                color = activep1;
            else if (y == players[1][1] && x == players[1][0])
                color = activep2;
            else
                color = inative;

            al_draw_rectangle(cellStartX + x * cellWidth + cellSpacing * x,
                              cellStartY + y * cellHeight + cellSpacing * y,
                              cellStartX + x * cellWidth + cellSpacing * x + cellWidth,
                              cellStartY + y * cellHeight + cellSpacing * y + cellHeight,
                              color, 2);

            SelectCharacter *slot = getCharacter(y, x);
            if (!slot)
                continue;
            ALLEGRO_BITMAP *bmp = slot->getMiniPortrait();
            if (!bmp)
                continue;
            al_draw_scaled_bitmap(bmp,
                                  0,
                                  0,
                                  al_get_bitmap_width(bmp),
                                  al_get_bitmap_height(bmp),
                                  cellStartX + x * cellWidth + cellSpacing * x + 1,
                                  cellStartY + y * cellHeight + cellSpacing * y + 1,
                                  cellWidth - 2,
                                  cellHeight - 2,
                                  0);
        }
    }
    drawNames();

}

bool Select::parseSelect()
{
    cout << "=============================================" << endl;
    cout << "  carregando select.txt... " << endl;
    cout << "=============================================" << endl;
    //lex.setLogLine(true);
    if (!lex.open("data/select/select.cfg"))
    {
        cerr << "Nao foi possivel abrir o arquivo" << endl;
        exit(1);
    }

    while (!lex.eof())
    {
        if (lex.check("["))
        {
            if (lex.check("characters"))
            {
                lex.check("]");
                parseCharacterList();
            }
            else if (lex.check("stages"))
            {
                lex.check("]");
                parseStageList();
            }
            else
                ignoreSection();
        }
        else
            lex.consumeToken();
    }
    return true;
}

void Select::ignoreSection()
{
    cout << "ignorando secao " << lex.tok() << endl;
    while (!lex.check("[", false) && !lex.eof())
        lex.consumeToken();
}

void Select::parseCharacterList()
{
    while (!lex.check("[", false) && !lex.eof())
    {
        if (lex.checkLiteral())
        {
            string s = lex.tok(), name;
            cout << "Char: " << s << endl;
            charList.push_back(s);
            SelectCharacter *sc = new SelectCharacter();
            if (lex.check("="))
            {
                if (lex.checkLiteral())
                {
                    name = lex.tok();
                }
            }
            if (sc)
            {
                sc->load(s);
                sc->setName(name);
                characters.push_back(sc);
            }
        }
        else
            break;
    }
}

void Select::parseStageList()
{
    while (!lex.check("[", false) && !lex.eof())
    {
        //cout << "tok: " << lex.tok() << endl;
        if (lex.checkLiteral())
        {
            string s = lex.tok(), name;
            cout << "Stage: " << s << endl;
            stageList.push_back(s);
            SelectStage *ss = new SelectStage();
            if (lex.check("="))
            {
                if (lex.checkLiteral())
                {
                    name = lex.tok();
                }
            }
            if (ss)
            {
                ss->load(s);
                ss->setName(name);
                stages.push_back(ss);
            }
        }
        else
            break;
    }
}

SelectCharacter::SelectCharacter()
{
    big = NULL;
    mini = NULL;
    name = "";
}

SelectCharacter::~SelectCharacter()
{
    if (big)
        al_destroy_bitmap(big);
    if (mini)
        al_destroy_bitmap(mini);
}

bool SelectCharacter::load(string name)
{
    string path = "data/char/" + name + "/";
    charPath = path + name + ".lua";
    big = al_load_bitmap((path + "img1.png").c_str());
    mini = al_load_bitmap((path + "img0.png").c_str());
    return true;
}

ALLEGRO_BITMAP *SelectCharacter::getBigPortrait() const
{
    return big;
}
ALLEGRO_BITMAP *SelectCharacter::getMiniPortrait() const
{
    return mini;
}

string SelectCharacter::getName()
{
    return name;
}

void SelectCharacter::setName(string str)
{
    name = str;
}

string SelectCharacter::getLoadPath()
{
    return charPath;
}

SelectStage::SelectStage()
{
    preview = NULL;
    name = "";
}

SelectStage::~SelectStage()
{
    if (preview)
        al_destroy_bitmap(preview);
}

bool SelectStage::load(string name)
{
    string path = "data/stages/" + name + "/";
    stagePath = path + name + ".txt";
    preview = al_load_bitmap((path + "preview.png").c_str());
    return true;
}

ALLEGRO_BITMAP *SelectStage::getPreview() const
{
    return preview;
}

string SelectStage::getName()
{
    return name;
}

void SelectStage::setName(string str)
{
    name = str;
}


string SelectStage::getLoadPath()
{
    return stagePath;
}
