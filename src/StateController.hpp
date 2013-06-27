#ifndef __STATECONTROLLER_HPP__
#define __STATECONTROLLER_HPP__

#include <vector>
#include "Character.hpp"
#include "ExpressionCompiler.hpp"
#include "Util.hpp"
using namespace std;

namespace Stat
{

class VirtualMachine;

enum StateControllerOpcode
{
    SCTRL_NULL = 0,
    SCTRL_CHANGESTATE,
    SCTRL_CHANGEANIM,
    SCTRL_VELADD,
    SCTRL_VELSET,
    SCTRL_VELMUL,
    SCTRL_POSSET,
    SCTRL_POSADD,
    SCTRL_POSFREEZE,
    SCTRL_HITDEF,
    SCTRL_PROJECTILE,
    SCTRL_PLAYSOUND,
    SCTRL_VARSET,
};
// foward declaration.
class ExpressionValue;
class StateCompiler;
class State;

class StateController
{
    friend class Char::Character;
    friend class StateCompiler;
    friend class State;
public:
    StateController(int code_);
    virtual ~StateController();
    bool triggered(Char::Character *player);
private:

    virtual bool execute(Char::Character *player);
    vector<ExpressionValue*> triggerAll;
    vector<ExpressionValue*> triggers;
protected:
    VirtualMachine *getVM();
    int code;
    ImplementObjectCounter();
};

class ChangeState : public StateController
{
    friend class StateCompiler;
public:
    ChangeState(int code);
    virtual ~ChangeState();
private:
    virtual bool execute(Char::Character *player);
    ExpressionValue *value;
    ExpressionValue *ctrl;
};

class VelSet : public StateController
{
    friend class StateCompiler;
public:
    VelSet(int code);
    virtual ~VelSet();
private:
    virtual bool execute(Char::Character *player);
    ExpressionValue *x;
    ExpressionValue *y;
};

class VelAdd : public StateController
{
    friend class StateCompiler;
public:
    VelAdd(int code);
    virtual ~VelAdd();
private:
    virtual bool execute(Char::Character *player);
    ExpressionValue *x;
    ExpressionValue *y;
};

class VelMul : public StateController
{
    friend class StateCompiler;
public:
    VelMul(int code);
    virtual ~VelMul();
private:
    virtual bool execute(Char::Character *player);
    ExpressionValue *x;
    ExpressionValue *y;
};


class ChangeAnim : public StateController
{
    friend class StateCompiler;
public:
    ChangeAnim(int code);
    virtual ~ChangeAnim();
private:
    virtual bool execute(Char::Character *player);
    ExpressionValue *value;
    ExpressionValue *elem;
};

class PosSet : public StateController
{
    friend class StateCompiler;
public:
    PosSet(int code);
    virtual ~PosSet();
private:
    virtual bool execute(Char::Character *player);
    ExpressionValue *x;
    ExpressionValue *y;
};

class PosAdd : public StateController
{
    friend class StateCompiler;
public:
    PosAdd(int code);
    virtual ~PosAdd();
private:
    virtual bool execute(Char::Character *player);
    ExpressionValue *x;
    ExpressionValue *y;
};

class PosFreeze : public StateController
{
    friend class StateCompiler;
public:
    PosFreeze(int code);
    virtual ~PosFreeze();
private:
    virtual bool execute(Char::Character *player);
    ExpressionValue *value;
};

class HitDef : public StateController
{
    friend class StateCompiler;
public:
    HitDef(int code);
    virtual ~HitDef();
private:
    virtual bool execute(Char::Character *player);
    ExpressionValue *damage;
    ExpressionValue *power;
    ExpressionValue *stateno;
    ExpressionValue *p2stateno;
    ExpressionValue *velx;
    ExpressionValue *vely;
    ExpressionValue *strength;
    struct
    {
        ExpressionValue *freq;
        ExpressionValue *phase;
        ExpressionValue *amplitude;
        ExpressionValue *time;
    } shake;
    struct
    {
        ExpressionValue *group;
        ExpressionValue *index;
    } sound;
    struct
    {
        ExpressionValue *p2stateno;
    } air;
    struct
    {
        ExpressionValue *p2stateno;
    } crouch;
};

class Projectile : public StateController
{
    friend class StateCompiler;
public:
    Projectile(int code);
    virtual ~Projectile();
private:
    virtual bool execute(Char::Character *player);
    ExpressionValue *x;
    ExpressionValue *y;
    ExpressionValue *velX;
    ExpressionValue *velY;
    ExpressionValue *anim;
    ExpressionValue *hitAnim;
    ExpressionValue *hitSoundGroup;
    ExpressionValue *hitSoundIndex;
    ExpressionValue *shakeFreq;
    ExpressionValue *shakePhase;
    ExpressionValue *shakeTime;
    ExpressionValue *shakeAmpl;

};

class PlaySound : public StateController
{
    friend class StateCompiler;
public:
    PlaySound(int code);
    virtual ~PlaySound();
private:
    virtual bool execute(Char::Character *player);
    ExpressionValue *group;
    ExpressionValue *index;
};

class VarSet : public StateController
{
    friend class StateCompiler;
public:
    VarSet(int code);
    virtual ~VarSet();
private:
    virtual bool execute(Char::Character *player);
    ExpressionValue *var;
    ExpressionValue *value;
};


}

#endif
