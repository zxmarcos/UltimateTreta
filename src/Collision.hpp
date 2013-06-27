#ifndef __COLLISION_HPP__
#define __COLLISION_HPP__

#include <vector>
#include <allegro5/allegro.h>
#include "Util.hpp"
using namespace std;

namespace Col
{

class CollisionBox
{
public:
    CollisionBox(int x1_, int y1_, int x2_, int y2_);
    ~CollisionBox();
    CollisionBox facing();
    CollisionBox scale(float x, float y);
    void draw(int x, int y, ALLEGRO_COLOR color);
    bool collide(const CollisionBox& with, int x, int y, int bx, int by) const;
private:
    int x1;
    int y1;
    int x2;
    int y2;
    ImplementObjectCounter();
};

void drawBoxes(const vector<CollisionBox>& list, int x, int y, ALLEGRO_COLOR color);
bool collide(const vector<CollisionBox>& a, int ax, int ay, const vector<CollisionBox>& b, int bx, int by);

class CollisionList
{
public:
    CollisionList(int type, size_t size);
    ~CollisionList();
    void add(CollisionBox *box, size_t idx);
    const vector<CollisionBox> getBoxes(float x = 1, float y = 1, bool facing = false) const;
private:
    int type;
    vector<CollisionBox*> vBoxes;
    ImplementObjectCounter();
};

}

#endif
