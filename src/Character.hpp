#pragma once

#include <string>
#include <lua.hpp>
#include <list>
#include "Sprites.hpp"
#include "Animations.hpp"
#include "VirtualMachine.hpp"
#include "StateCompiler.hpp"
#include "InputMachine.hpp"
#include "Object.hpp"
#include "Sounds.hpp"

class Fight;

using namespace std;
namespace Char
{

typedef struct SizeConstants
{
    int groundFront;
    int groundBack;
} SizeConstants;

class Projectile;

typedef struct HitInfo
{
    int damage;
    int power;
    int stateno;
    int p2stateno;
    int strength;
    struct
    {
        double freq = 0.0;
        double phase = 0.0;
        double amplitude = 0.0;
        int time = 0;
    } shake;
    struct
    {
        bool play = false;
        int group;
        int index;
    } sound;
    struct
    {
        int p2stateno;
    } air;
    struct
    {
        int p2stateno;
    } crouch;
    double velx;
    double vely;
} HitInfo;

typedef struct ProjectileInfo
{
    int anim = 0;
    double x = 0.0;
    double y = 0.0;
    double velX = 0.0;
    double velY = 0.0;
    struct
    {
        int anim = 0;
        struct
        {
            double freq = 0.0;
            double phase = 0.0;
            double amplitude = 0.0;
            int time = 0;
        } shake;
        struct
        {
            bool play = false;
            int group;
            int index;
        } sound;
    } hit;
} ProjectileInfo;



class Character : public Object
{
public:
    Character(Fight& env);
    ~Character();
    bool open(string name);
    void update();
    void drawDebugInfo(int ns = 0);
    void setPlayerNumber(int no);
    void gravityAccel();
    void draw(int camx, int camy);
    void drawBoxes(int xref, int yref);
    bool canTurn();
    void createProjectile(const ProjectileInfo& proj);
    int getPower();
    void setPower(int power);
    int getPowerMax();
    void setPowerMax(int power);
    const vector<Col::CollisionBox> getAttackBoxes() const;
    const vector<Col::CollisionBox> getDefenseBoxes() const;
    const SizeConstants& getSize() const;
    int getRightEdgeDistance();
    int getLeftEdgeDistance();
    int getRightBodyEdgeDistance();
    int getLeftBodyEdgeDistance();
    bool didCommand(const string str);
    void changeAnim(int actioNo, int elem = 0);
    void changeState(int stateNo);
    int getStateType() const;
    void setStateType(int value);
    Stat::StateMoveType getStateMoveType() const;
    void setStateMoveType(Stat::StateMoveType value);
    int getStateTime() const;
    int getStateNumber() const;
    int getAnimNo() const;
    int getAnimFrameNo() const;
    int getAnimFrameTime() const;
    int getAnimTime() const;
    bool hasState(int stateNo);
    bool hasAnim(int actionNo);
    virtual void setFacing(bool value);
    double getP2DistX() const;
    double getP2DistY() const;
    void setP2DistX(double value);
    void setP2DistY(double value);
    Stat::VirtualMachine *getVM();
    void playSound(int group, int index);
    void setVar(unsigned var, double value);
    double getVar(int var);
    void addHitDef(HitInfo *info);
    HitInfo *getOneHit();
private:
    void clearHitdefs();
    static const unsigned n_max_vars = 100;
    double vars[n_max_vars];
    int power;
    int powerMax;
    Fight& fightEnviroment;
    SizeConstants sizeConst;
    bool isLoaded;
    int inputNo;
    Snd::Sounds sounds;
    Spr::SpriteLoader sprites;
    Anim::AnimLoader animations;
    Anim::Animator *animator;
    Stat::VirtualMachine *vm;
    Stat::StateCompiler *stateCompiler;
    lua_State *luaVm;
    Input::InputMachine *cmd;
    const Stat::State *state;
    const Stat::State *stateSm1;
    string charName;
    int stateNumber;
    int prevStateNumber;
    int type;
    int physics;
    Stat::StateMoveType moveType;
    int ctrl;
    int time;
    double drawPosX;
    double drawPosY;
    double p2distX;
    double p2distY;
    std::list<HitInfo *> hitDefs;
};

class Projectile : public Object
{
public:
    typedef enum
    {
        Alive = 0,
        Explod,
        Dead
    } ProjectileState;

    Projectile(Fight& env, Spr::SpriteLoader& spr_, Anim::AnimLoader& anim_, Character& owner_, const ProjectileInfo& info_);
    ~Projectile();
    const vector<Col::CollisionBox> getAttackBoxes() const;
    ProjectileState getState() const;

    bool isOwner(Character &c);
    void update();
    void draw(int camx, int camy);
    void drawBoxes(int xref, int yref);
    void hit();
    Character& getOwner();
private:
    Character& owner;
    Fight& fightEnviroment;
    ProjectileInfo info;
    ProjectileState state;
    bool diesOnNextFrame;
    Anim::Animator animator;
    Spr::SpriteLoader& sprites;

};



}

// #endif // __CHARACTER_HPP__
