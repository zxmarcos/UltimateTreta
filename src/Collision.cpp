//==============================================================================
// Funções relacionadas a colisões
// Marcos Medeiros
//==============================================================================
#include <cmath>
#include <sstream>
#include <string>
#include "Console.hpp"
#include <allegro5/allegro.h>
#include "Display.hpp"
#include "Collision.hpp"
#include "Util.hpp"

using namespace std;

namespace Col
{

//==============================================================================
// Inicia os contadores de objetos
//==============================================================================
InitObjectCounter(CollisionBox);
InitObjectCounter(CollisionList);

//==============================================================================
// ctor
//==============================================================================
CollisionBox::CollisionBox(int x1_, int y1_, int x2_, int y2_) :
    x1(x1_), y1(y1_), x2(x2_), y2(y2_)
{
    IncrementObjectCounter();
}

//==============================================================================
// dtor
//==============================================================================
CollisionBox::~CollisionBox()
{
    DecrementObjectCounter();
}

//==============================================================================
// Retorna a caixa de colisão para a posição invertida
//==============================================================================
CollisionBox CollisionBox::facing()
{
    return CollisionBox(-x2, y1, -x1, y2);
}


bool CollisionBox::collide(const CollisionBox& with, int ax, int ay, int bx, int by) const
{
    int ax1 = x1 + ax;
    int ax2 = x2 + ax;
    int ay1 = y1 + ay;
    int ay2 = y2 + ay;

    int bx1 = with.x1 + bx;
    int bx2 = with.x2 + bx;
    int by1 = with.y1 + by;
    int by2 = with.y2 + by;

    if ((ax1 < bx2 && ax2 > bx1 && ay1 < by2 && ay2 > by1))
    {
    //    Console::print("colisão (%d,%d,%d,%d)->(%d,%d,%d,%d)",ax1,ay1,ax2,ay2,bx1,by1,bx2,by2);
        return true;
    }
    return false;
}

//==============================================================================
// Retorna a caixa de colisão escalada
//==============================================================================
CollisionBox CollisionBox::scale(float x, float y)
{
    return CollisionBox(x1 * x, y1 * y, x2 * x, y2 * y);
}

void CollisionBox::draw(int x, int y, ALLEGRO_COLOR color)
{
    Display::drawRectFill(x + x1, y + y1, (x2 - x1), (y2 - y1), color);
    unsigned char r, g, b;
    al_unmap_rgb(color, &r, &g, &b);
    Display::drawRect(x + x1, y + y1, (x2 - x1), (y2 - y1), al_map_rgba(r, g, b, 140));
}

void drawBoxes(const vector<CollisionBox>& boxes, int x, int y, ALLEGRO_COLOR color)
{
    for (auto box : boxes)
        box.draw(x, y, color);
}

bool collide(const vector<CollisionBox>& a, int ax, int ay, const vector<CollisionBox>& b, int bx, int by)
{
    for (auto &av : a)
    {
        for (auto &bv : b)
        {
            if (av.collide(bv, ax, ay, bx, by))
                return true;
        }
    }
    return false;
}

//==============================================================================
// ctor
//==============================================================================
CollisionList::CollisionList(int type_, size_t size)
{
    type = type_;
    vBoxes.resize(size);
    IncrementObjectCounter();
}

//==============================================================================
// dtor
//==============================================================================
CollisionList::~CollisionList()
{
    while (!vBoxes.empty())
    {
        CollisionBox *box = vBoxes.back();
        vBoxes.pop_back();
        safeDelete(box);
    }
    DecrementObjectCounter();
}

//==============================================================================
// Adiciona uma caixa de colisão a lista
//==============================================================================
void CollisionList::add(CollisionBox *box, size_t idx)
{
    if (!box)
        return;
    // caso o valor do indice seja maior que o vetor, colocamos ele no fim
    if (idx >= vBoxes.size())
        vBoxes.push_back(box);
    else
        vBoxes[idx] = box;
}

const vector<CollisionBox> CollisionList::getBoxes(float x, float y, bool facing) const
{
    vector<CollisionBox> boxes;
    for (auto box : vBoxes)
    {
        if (facing)
            boxes.push_back(box->facing().scale(x, y));
        else
            boxes.push_back(box->scale(x, y));
    }
    return boxes;
}


}
