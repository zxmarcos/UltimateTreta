#ifndef __ANIMATIONS_HPP
#define __ANIMATIONS_HPP

#include <string>
#include <fstream>
#include <vector>
#include <map>
#include "Collision.hpp"
#include "Util.hpp"
#include "Tokenizer.hpp"
using namespace std;

namespace Anim
{


class AnimLoader;
class Frame;
class Action;
class Animator;

//=============================================================================
// Action
//=============================================================================
class Frame
{
    friend class Animator;
    friend class AnimLoader;
    friend class Action;
public:
    Frame(short g, short i, short x, short y, short t, bool hf = false, bool vf = false, bool add=false);
    ~Frame();
private:
    Col::CollisionList *clsn1;
    Col::CollisionList *clsn2;
    Col::CollisionList *clsn1Default;
    Col::CollisionList *clsn2Default;
    short group;
    short index;
    short xaxis;
    short yaxis;
    short time;
    bool vflip;
    bool hflip;
    bool addBlend;
    ImplementObjectCounter();
};

class Action
{
    friend class Animator;
    friend class AnimLoader;
public:
    Action(int n);
    ~Action();
private:
    int number;
    size_t loopstart;
    std::vector<Frame*> vFrames;
    ImplementObjectCounter();
};

class AnimLoader
{
public:
    AnimLoader();
    ~AnimLoader();

    const Action *get(int number);
    bool open(string name);
    bool exist(int number);
private:
    string fileName;
    Tok::Tokenizer lex;
    Frame *frame;
    Action *action;
    Col::CollisionList *clsn1;
    Col::CollisionList *clsn2;
    Col::CollisionList *clsn1Default;
    Col::CollisionList *clsn2Default;
    bool isDefaultClsn;
    int totalFrames;
    int loopstart;
    map<int, Action*> mapActions;

    void createAction(int number);
    void setLoopstart();
    void createClsnList(int type, bool def, size_t size);
    void createClsn(int type, int idx, int x1, int y1, int x2, int y2);
    void createFrame(int g, int i, int x, int y, int t, bool h, bool v, bool a);
};

class Animator
{
public:
    Animator(AnimLoader &loader_);
    ~Animator();
    bool open(string name);
    bool set(int no, int elem=0);
    void update();
    int number();
    int group();
    int index();
    int x();
    int y();
    bool fliph();
    bool flipv();
    bool additive();
    int frameNumber();
    int frameTick();
    int elapsed();
    bool isLastTick() const;
    const vector<Col::CollisionBox> getAttackBoxes(float x = 1, float y = 1, bool facing = false) const;
    const vector<Col::CollisionBox> getDefenseBoxes(float x = 1, float y = 1, bool facing = false) const;
private:
    AnimLoader &loader;
    const Frame *frame;
    const Action *action;
    int time;
    int frameTime;
    int frameNo;
    ImplementObjectCounter();
};

}


#endif
