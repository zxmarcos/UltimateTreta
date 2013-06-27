#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <utility>
#include "ExpressionTokenizer.hpp"
#include "Tokenizer.hpp"
#include "ExpressionCompiler.hpp"
#include "Util.hpp"
using namespace std;
namespace Char
{
class Character;
};
namespace Stat
{


class StateCompiler;
class StateController;
class ExpressionValue;

typedef enum StateType
{
    STATE_TYPE_STAND = 0,
    STATE_TYPE_CROUCH,
    STATE_TYPE_AIR,
} StateType;

typedef enum StateMoveType
{
    STATE_MOVE_IDLE = 0,
    STATE_MOVE_ATTACK,
    STATE_MOVE_HIT,
    STATE_MOVE_UNCHANGED,
} StateMoveType;

typedef enum StatePhysics
{
    STATE_PHYSICS_STAND = 0,
    STATE_PHYSICS_CROUCH,
    STATE_PHYSICS_AIR,
} StatePhysics;

class State
{
    friend class StateCompiler;
public:
    State(int number_);
    ~State();
    bool execute(Char::Character *player) const;
    int number;
    StateType type;
    StatePhysics physics;
    StateMoveType moveType;
    ExpressionValue *anim;
    ExpressionValue *velX;
    ExpressionValue *velY;
    ExpressionValue *ctrl;
    ExpressionValue *sprPriority;
    vector<StateController*> sctrls;
private:
    ImplementObjectCounter();
};

typedef bool (StateCompiler::*StateParserCallback)();

class StateCompiler
{
public:
    StateCompiler();
    ~StateCompiler();
    bool open(string name);
    bool exist(int stateNo);
    const State *get(int stateNo);
private:
    bool parseTriggers();
    bool parseChangeState();
    bool parseChangeAnim();
    bool parseVelSet();
    bool parseVelAdd();
    bool parseVelMul();
    bool parsePosSet();
    bool parsePosAdd();
    bool parsePosFreeze();
    bool parseNull();
    bool parseHitDef();
    bool parseProjectile();
    bool parsePlaySound();
    bool parseVarSet();

    static unordered_map<string, StateParserCallback> parseTable;
    void errorWait(string str);
    ExpressionValue *getExpression();
    bool parseStatedef();
    Tok::Tokenizer lex;
    State *state;
    StateController *sctrl;
    ExpressionCompiler exprCompiler;
    unordered_map<int, State*> stateTable;
};

}

// #endif // __STCOMPILER_HPP__
