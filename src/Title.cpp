#include <iostream>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include "Title.hpp"
#include "Game.hpp"
#include "Display.hpp"
#include "Stage.hpp"
#include "Audio.hpp"

using namespace std;

Title::Title(Game& env) : gameEnviroment(env)
{
    entrys.clear();
    entrys.push_back(MenuEntry {0, "Versus"});
    entrys.push_back(MenuEntry {1, "Sobre"});
    entrys.push_back(MenuEntry {2, "Sair"});
    time = 0;
}

Title::~Title()
{
}

void Title::reset()
{
    stage.reset();
}

bool Title::open(string filename)
{
    cout << "Carregando tela de título " << filename << endl;
    if (!stage.open(filename))
    {
        cout << "Erro ao carregar tela de título" << endl;
        return false;
    }

    sounds.open("data/title/title.snd");
    option = 0;
    lastUp = false;
    lastDown = false;
    lastEnter = false;
    lastBack = false;
    isOnAbout = false;
    exitGame = false;
    return true;
}

int Title::update()
{
    al_clear_to_color(al_map_rgb(0, 0, 0));
    stage.update();
    stage.drawBack(0, 0);
    stage.drawFront(0, 0);

    if (isOnAbout)
        drawAbout();
    else
        drawMenu();
    handleInput();
    time++;
    if (exitGame)
        return Game::GAME_STATE_EXIT;
    return Game::GAME_STATE_TITLE;
}

void Title::handleInput()
{
    bool downPressed = Input::readSymbol(0, Input::K_DOWN);
    bool upPressed = Input::readSymbol(0, Input::K_UP);
    bool enterPressed = Input::readSymbol(0, Input::K_A);
    bool backPressed = Input::readSymbol(0, Input::K_B);

    if (!isOnAbout)
    {

        if (downPressed && !lastDown)
        {
            option = (option + 1) % entrys.size();
            sounds.playSound(0, 1);
        }
        if (upPressed && !lastUp)
        {
            if (option <= 0)
                option = entrys.size() - 1;
            else
                option = (option - 1) % entrys.size();
            sounds.playSound(0, 1);
        }

        if (enterPressed && !lastEnter)
        {
            sounds.playSound(0, 0);
            switch (option)
            {
            case 0:
                gameEnviroment.changeState(Game::GAME_STATE_SELECT);
                Input::clearSymbols(0);
                Input::clearSymbols(1);
                break;
            case 1:
                isOnAbout = true;
                break;
            case 2:
                gameEnviroment.changeState(Game::GAME_STATE_EXIT);
                break;
            }


        }
    }
    else
    {
        if (backPressed && !lastBack)
        {
            sounds.playSound(0, 0);
            isOnAbout = false;
        }
    }

    lastDown = downPressed;
    lastUp = upPressed;
    lastEnter = enterPressed;
    lastBack = backPressed;
}

void Title::drawMenu()
{

    const int topSpace = 10;
    const int botSpace = 10;
    const int leftSpace = 10;
    const int rightSpace = 10;
    const int textHeight = Display::textLineHeight(menuFont) + 5;

    int sx = 50;
    int sy = 320;
    int fy = textHeight * entrys.size();
    int fx = 0;

    for (auto entry : entrys)
    {
        int lx = Display::textWidth(menuFont, entry.str);
        if (lx >= fx)
            fx = lx;
    }
    Display::drawRectFill(sx - leftSpace, sy - topSpace, fx + rightSpace * 2, fy + botSpace, al_map_rgba(0, 0, 0, 100));


    for (size_t i = 0; i < entrys.size(); i++)
    {
        const char *str = entrys[i].str;
        int x = Display::textWidth(menuFont, str) / 2;
        ALLEGRO_COLOR color;
        if ((size_t) option == i)
            color = al_map_rgb(255, 255, 0);
        else
        {
            double dist = abs((int)i - option);
            color = al_map_rgba(255, 0, 0, 0);
        }
        Display::drawText(menuFont,
                          color,
                          (sx + fx / 2) - x,
                          sy + i * textHeight,
                          entrys[i].str);
    }
}

void Title::drawAbout()
{
    const int w = Display::getWidth();
    const int h = Display::getHeight();

    int sx = 30;
    int sy = 30;
    int fx = w - sx * 2;
    int fy = h - sx * 2;

    Display::drawRectFill(sx, sy, fx, fy, al_map_rgba(0, 0, 0, 200));

    sx += 10;
    sy += 10;

    int dy = Display::textLineHeight(INFO_FONT);

    ALLEGRO_COLOR color1 = al_map_rgb(220, 220, 220);
    ALLEGRO_COLOR color2 = al_map_rgb(220, 220, 0);


#define print_t(str)    Display::drawText(INFO_FONT, color2, sx, sy, str);   sy += dy
#define print(str)      Display::drawText(INFO_FONT, color1, sx, sy, str);   sy += dy
    print_t("Street Fighter Clone");
    print("");
    print_t("Informações:");
    print("Trabalho para o 1° semestre de 2013, nas disciplinas de Computação Gráfica,");
    print("Linguagem de Programação e Algoritmos e Estruturas de Dados II.");
    print("");
    print_t("Desenvolvedor:");
    print("Marcos Roberto A. Medeiros");
    print("");
    print_t("Tecnologias:");
    print("C++11,  Allegro5, Lua Embarcada, formatos de arquivo SFF, AIR, SND e OGG");
    print("como codec para as músicas de fundo (BGM)");
    print("");
    print_t("Comentários:");
    print("O objetivo inicial do jogo é primeiramente o próprio motor dele,");
    print("onde os cenários e personagens são programados através de uma");
    print("linguagem que define estados que o motor consegue lidar, através");
    print("de uma máquina virtual, baseada em uma máquina de estados finitos.");
    print("Essa engine é fortemente inspirada por outra engine chamada MUGEN,");
    print("tanto é que, a linguagem de estados é muito semelhante a encontrada");
    print("nela, bem como os arquivos de sons e imagens.");
    print("");
    print("A marca Street Fighter, bem como os sprites e músicas desde jogo");
    print("são propriedades da CAPCOM.");
#undef print
#undef print_t
}
