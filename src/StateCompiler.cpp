//=============================================================================
// Compilador de estados
//=============================================================================
#include <string>
#include <iostream>
#include <unordered_map>
#include <utility>
#include "StateCompiler.hpp"
#include "StateController.hpp"
#include "Character.hpp"
#include "Util.hpp"
using namespace std;

namespace Stat
{

InitObjectCounter(State);

// Nossa tabela de hashing para as funções a serem chamadas
// para cada tipo de StateController
unordered_map<string, StateParserCallback> StateCompiler::parseTable =
{
    { "changeanim",		&StateCompiler::parseChangeAnim },
    { "changestate",	&StateCompiler::parseChangeState },
    { "veladd",			&StateCompiler::parseVelAdd },
    { "velset",			&StateCompiler::parseVelSet },
    { "velmul",			&StateCompiler::parseVelMul },
    { "posset",			&StateCompiler::parsePosSet },
    { "posadd",			&StateCompiler::parsePosAdd },
    { "posfreeze",		&StateCompiler::parsePosFreeze },
    { "null",			&StateCompiler::parseNull },
    { "hitdef",			&StateCompiler::parseHitDef },
    { "projectile",		&StateCompiler::parseProjectile },
    { "playsound",		&StateCompiler::parsePlaySound },
    { "varset",		    &StateCompiler::parseVarSet },
};

//=============================================================================
// Emite uma mensagem de erro sobre algo esperado
//=============================================================================
void StateCompiler::errorWait(string str)
{
    cout << "Esperado " << str << " na linha " << lex.getLine() << endl;
}

//=============================================================================
// ctor
//=============================================================================
State::State(int number_) : number(number_)
{
    anim = NULL;
    velX = NULL;
    velY = NULL;
    ctrl = NULL;
    moveType = STATE_MOVE_IDLE;
    physics = STATE_PHYSICS_STAND;
    sprPriority = NULL;
    IncrementObjectCounter();
}

//=============================================================================
// dtor
//=============================================================================
State::~State()
{
    for (auto i : sctrls)
        delete i;
    safeDelete(anim);
    safeDelete(velX);
    safeDelete(velY);
    safeDelete(ctrl);
    safeDelete(sprPriority);
    DecrementObjectCounter();
}

//=============================================================================
// Caminha pelos supercontrollers e verifica quais estão ativados
//=============================================================================
bool State::execute(Char::Character *player) const
{
    for (auto sc : sctrls)
    {
        int currentStateNo = player->getStateNumber();
        if (sc->triggered(player))
            sc->execute(player);
        if (currentStateNo != player->getStateNumber())
            return true;
    }
    return false;
}

//=============================================================================
// ctor
//=============================================================================
StateCompiler::StateCompiler()
{
    state = NULL;
    sctrl = NULL;
}

//=============================================================================
// dtor
//=============================================================================
StateCompiler::~StateCompiler()
{
    int nstates = State::getObjectCounter();
    int nsctrls = StateController::getObjectCounter();
    int nexprs = ExpressionValue::getObjectCounter();
    for (auto i = stateTable.begin(); i != stateTable.end(); i++)
    {
        delete i->second;
    }
    nstates -= State::getObjectCounter();
    nexprs -= ExpressionValue::getObjectCounter();
    nsctrls -= StateController::getObjectCounter();
    cout << nstates << " states desalocados com sucesso!" << endl;
    cout << nsctrls << " sctrls desalocados com sucesso!" << endl;
    cout << nexprs << " valores desalocados com sucesso!" << endl;
}

//=============================================================================
// Compila um arquivo de estados
//=============================================================================
bool StateCompiler::open(string name)
{
    int nstates = State::getObjectCounter();
    int nexprs = ExpressionValue::getObjectCounter();
    int nsctrls = StateController::getObjectCounter();
    if (!lex.open(name))
    {
        cerr << "Nao foi possivel abrir o arquivo" << endl;
        return false;
    }
    sctrl = NULL;
    state = NULL;
    while (!lex.eof())
    {
        if (lex.check("["))
        {
            if (lex.check("statedef"))
            {
                parseStatedef();
            }
            else if (lex.check("state"))
            {
                lex.rest();
                if (!lex.check("type"))
                {
                    cout << "Esperado definição de tipo para o sctrl!" << endl;
                    return false;
                }
                if (!lex.check("="))
                {
                    errorWait("=");
                    return false;
                }
                lex.getToken();
                string type = lex.tok();
                lex.consumeToken();
                // verifica se é um trigger
                auto ptr = parseTable.find(lex.tok());
                if (ptr != parseTable.end())
                {
                    StateParserCallback call = (*ptr).second;
                    if (!(this->*call)())
                        return false;
                }
                else
                {
                    cout << "Tipo de StateController invalido! " << type << endl;
                    exit(-1);
                }
                if (sctrl && state)
                    state->sctrls.push_back(sctrl);
                sctrl = NULL;
            }
            else
                lex.consumeToken();
        }
    }
    nstates = State::getObjectCounter() - nstates;
    nexprs = ExpressionValue::getObjectCounter() - nexprs;
    nsctrls = StateController::getObjectCounter() - nsctrls;
    cout << nstates << " states alocados com sucesso!" << endl;
    cout << nsctrls << " sctrls alocados com sucesso!" << endl;
    cout << nexprs << " valores alocados com sucesso!" << endl;
    return true;
}

//=============================================================================
// Verifica se um estado existe
//=============================================================================
bool StateCompiler::exist(int stateNo)
{
    if (stateTable[stateNo])
        return true;
    return false;
}

//=============================================================================
// Retorna um estado compilado
//=============================================================================
const State *StateCompiler::get(int stateNo)
{
    return stateTable[stateNo];
}

//==============================================================================
// Faz o parsing dos triggers de um StateController
//==============================================================================
bool StateCompiler::parseTriggers()
{
    int ntriggers = 0;
    // cout << "fazendo parsing dos triggers" << endl;
    do
    {
        lex.getToken();
        if (lex.check("triggerall"))
        {
            // cout << "novo triggerall" << endl;
            if (!lex.check("="))
            {
                errorWait("=");
                return false;
            }
            sctrl->triggerAll.push_back(getExpression());
        }
        else
        {
            if (lex.tok().substr(0, 7) == "trigger")
            {
                // cout << "novo trigger!" << endl;
                lex.consumeToken();
                if (!lex.check("="))
                {
                    errorWait("=");
                    return false;
                }
                sctrl->triggers.push_back(getExpression());
            }
            else
                break;
        }
        ntriggers++;
    }
    while (!lex.eof());
    if (!ntriggers)
    {
        cout << "E necessario pelo menos 1 trigger na linha " << lex.getLine() << endl;
        return false;
    }
    return true;
}

//==============================================================================
// Faz o parsing de um statedef
//==============================================================================
bool StateCompiler::parseStatedef()
{
    int number = 0;
    if (!lex.checkNumber())
    {
        cout << "Esperado numero do estado na linha " << lex.getLine() << endl;
        return false;
    }

    number = lex.toInt();
    lex.rest();
    // lex.check("]");

   // cout << "statedef " << number << endl;

    state = new State(number);
    if (!state)
        return false;
    if (stateTable[number])
    {
        cout << "ATENCAO: Sobreescrevendo estado ja inserido! " << number << endl;
        // faz alguma coisa para evitar o memorylake
    }
    stateTable[number] = state;

    while (!lex.eof() && !lex.check("[", false))
    {
        // type = str
        if (lex.check("type"))
        {
            if (!lex.check("="))
            {
                errorWait("=");
                return false;
            }
            if (lex.check("s"))
            {
                state->type = STATE_TYPE_STAND;
            }
            else if (lex.check("c"))
            {
                state->type = STATE_TYPE_CROUCH;
            }
            else if (lex.check("a"))
            {
                state->type = STATE_TYPE_AIR;
            }
            else
                lex.consumeToken();
        }
        else if (lex.check("movetype")) // movetype = str
        {
            lex.check("=");
            if (lex.check("i"))
            {
                state->moveType = STATE_MOVE_IDLE;
            }
            else if (lex.check("a"))
            {
                state->moveType = STATE_MOVE_ATTACK;
            }
            else if (lex.check("h"))
            {
                state->moveType = STATE_MOVE_HIT;
            }
            else if (lex.check("u"))
            {
                state->moveType = STATE_MOVE_UNCHANGED;
            }
            else
                lex.consumeToken();
        }
        else if (lex.check("physics")) // physics = str
        {
            lex.check("=");
            if (lex.check("s"))
            {
                state->physics = STATE_PHYSICS_STAND;
            }
            else if (lex.check("c"))
            {
                state->physics = STATE_PHYSICS_CROUCH;
            }
            else if (lex.check("a"))
            {
                state->physics = STATE_PHYSICS_AIR;
            }
            else
            {
                lex.consumeToken();
            }
        }
        else if (lex.check("anim")) // anim = expr
        {
            if (!lex.check("="))
            {
                errorWait("=");
                return false;
            }
            state->anim = getExpression();
        }
        else if (lex.check("velx"))
        {
            if (!lex.check("="))
            {
                errorWait("=");
                return false;
            }
            state->velX = getExpression();
        }
        else if (lex.check("vely"))
        {
            if (!lex.check("="))
            {
                errorWait("=");
                return false;
            }
            state->velY = getExpression();
        }
        else if (lex.check("ctrl"))
        {
            if (!lex.check("="))
            {
                errorWait("=");
                return false;
            }
            state->ctrl = getExpression();
        }
        else
        {
            lex.consumeToken();
        }
    }
    return true;
}

//==============================================================================
// Faz o parsing de uma expressão e ja retorna ela compilada
// a expressão irá ser o resto da linha
//==============================================================================
ExpressionValue *StateCompiler::getExpression()
{
    ExpressionValue *ex = exprCompiler.compile(lex.rest());
    //ex->dasm();
    return ex;
}


//=============================================================================
// Faz o parse de um StateController ChangeState
//=============================================================================
bool StateCompiler::parseChangeState()
{
    ChangeState *p = new ChangeState(SCTRL_CHANGESTATE);
    sctrl = p;
    // cout << "fazendo parsing do changestate" << endl;
    if (!parseTriggers())
        return false;
    while (!lex.eof())
    {
        if (lex.check("value"))
        {
            if (!lex.check("="))
            {
                errorWait("=");
                return false;
            }
            p->value = getExpression();
        }
        else if (lex.check("ctrl"))
        {
            if (!lex.check("="))
            {
                errorWait("=");
                return false;
            }
            p->ctrl = getExpression();
        }
        else
            break;
    }
    return true;
}

//=============================================================================
// Faz o parse de um StateController ChangeAnim
//=============================================================================
bool StateCompiler::parseChangeAnim()
{
    ChangeAnim *p = new ChangeAnim(SCTRL_CHANGEANIM);
    sctrl = p;
    // cout << "fazendo parsing do changeanim" << endl;
    if (!parseTriggers())
        return false;
    while (!lex.eof())
    {
        if (lex.check("value"))
        {
            if (!lex.check("="))
            {
                errorWait("=");
                return false;
            }
            p->value = getExpression();
        }
        else if (lex.check("elem"))
        {
            if (!lex.check("="))
            {
                errorWait("=");
                return false;
            }
            p->elem = getExpression();
        }
        else
            break;
    }
    return true;
}

//=============================================================================
// Faz o parse de um StateController VelSet
//=============================================================================
bool StateCompiler::parseVelSet()
{
    VelSet *p = new VelSet(SCTRL_VELSET);
    sctrl = p;
    // cout << "fazendo parsing do velset" << endl;
    if (!parseTriggers())
        return false;
    while (!lex.eof())
    {
        if (lex.check("x"))
        {
            if (!lex.check("="))
            {
                errorWait("=");
                return false;
            }
            p->x = getExpression();
        }
        else if (lex.check("y"))
        {
            if (!lex.check("="))
            {
                errorWait("=");
                return false;
            }
            p->y = getExpression();
        }
        else
            break;
    }
    return true;
}

//=============================================================================
// Faz o parse de um StateController VelAdd
//=============================================================================
bool StateCompiler::parseVelAdd()
{
    VelAdd *p = new VelAdd(SCTRL_VELADD);
    sctrl = p;
    // cout << "fazendo parsing do veladd" << endl;
    if (!parseTriggers())
        return false;
    while (!lex.eof())
    {
        if (lex.check("x"))
        {
            if (!lex.check("="))
            {
                errorWait("=");
                return false;
            }
            p->x = getExpression();
        }
        else if (lex.check("y"))
        {
            if (!lex.check("="))
            {
                errorWait("=");
                return false;
            }
            p->y = getExpression();
        }
        else
            break;
    }
    return true;
}

//=============================================================================
// Faz o parse de um StateController VelMul
//=============================================================================
bool StateCompiler::parseVelMul()
{
    VelMul*p = new VelMul(SCTRL_VELMUL);
    sctrl = p;
    // cout << "fazendo parsing do velmul" << endl;
    if (!parseTriggers())
        return false;
    while (!lex.eof())
    {
        if (lex.check("x"))
        {
            if (!lex.check("="))
            {
                errorWait("=");
                return false;
            }
            p->x = getExpression();
        }
        else if (lex.check("y"))
        {
            if (!lex.check("="))
            {
                errorWait("=");
                return false;
            }
            p->y = getExpression();
        }
        else
            break;
    }
    return true;
}

//=============================================================================
// Faz o parse de um StateController PosFreeze
//=============================================================================
bool StateCompiler::parsePosFreeze()
{
    PosFreeze *p = new PosFreeze(SCTRL_POSFREEZE);
    sctrl = p;
    // cout << "fazendo parsing do posfreeze" << endl;
    if (!parseTriggers())
        return false;
    while (!lex.eof())
    {
        if (lex.check("value"))
        {
            if (!lex.check("="))
            {
                errorWait("=");
                return false;
            }
            p->value = getExpression();
        }
        else
            break;
    }
    return true;
}
//=============================================================================
// Faz o parse de um StateController PosSet
//=============================================================================
bool StateCompiler::parsePosSet()
{
    PosSet *p = new PosSet(SCTRL_POSSET);
    sctrl = p;
    // cout << "fazendo parsing do posset" << endl;
    if (!parseTriggers())
        return false;
    while (!lex.eof())
    {
        if (lex.check("x"))
        {
            if (!lex.check("="))
            {
                errorWait("=");
                return false;
            }
            p->x = getExpression();
        }
        else if (lex.check("y"))
        {
            if (!lex.check("="))
            {
                errorWait("=");
                return false;
            }
            p->y = getExpression();
        }
        else
            break;
    }
    return true;
}

//=============================================================================
// Faz o parse de um StateController PosAdd
//=============================================================================
bool StateCompiler::parsePosAdd()
{
    PosAdd *p = new PosAdd(SCTRL_POSADD);
    sctrl = p;
    // cout << "fazendo parsing do posadd" << endl;
    if (!parseTriggers())
        return false;
    while (!lex.eof())
    {
        if (lex.check("x"))
        {
            if (!lex.check("="))
            {
                errorWait("=");
                return false;
            }
            p->x = getExpression();
        }
        else if (lex.check("y"))
        {
            if (!lex.check("="))
            {
                errorWait("=");
                return false;
            }
            p->y = getExpression();
        }
        else
            break;
    }
    return true;
}

//=============================================================================
// Faz o parse de um StateController HitDef
//=============================================================================
bool StateCompiler::parseHitDef()
{
    HitDef *p = new HitDef(SCTRL_HITDEF);
    sctrl = p;
    // cout << "fazendo parsing do changeanim" << endl;
    if (!parseTriggers())
        return false;
    while (!lex.eof())
    {
        if (lex.check("damage"))
        {
            if (!lex.check("="))
            {
                errorWait("=");
                return false;
            }
            p->damage = getExpression();
        }
        else if (lex.check("power"))
        {
            if (!lex.check("="))
            {
                errorWait("=");
                return false;
            }
            p->power = getExpression();
        }
        else if (lex.check("stateno"))
        {
            if (!lex.check("="))
            {
                errorWait("=");
                return false;
            }
            p->stateno = getExpression();
        }
        else if (lex.check("strength"))
        {
            if (!lex.check("="))
            {
                errorWait("=");
                return false;
            }
            p->strength = getExpression();
        }
        else if (lex.check("p2stateno"))
        {
            if (!lex.check("="))
            {
                errorWait("=");
                return false;
            }
            p->p2stateno = getExpression();
        }
        else if (lex.check("air.p2stateno"))
        {
            if (!lex.check("="))
            {
                errorWait("=");
                return false;
            }
            p->air.p2stateno = getExpression();
        }
        else if (lex.check("crouch.p2stateno"))
        {
            if (!lex.check("="))
            {
                errorWait("=");
                return false;
            }
            p->crouch.p2stateno = getExpression();
        }
        else if (lex.check("shake.freq"))
        {
            if (!lex.check("="))
            {
                errorWait("=");
                return false;
            }
            p->shake.freq = getExpression();
        }
        else if (lex.check("shake.amplitude"))
        {
            if (!lex.check("="))
            {
                errorWait("=");
                return false;
            }
            p->shake.amplitude = getExpression();
        }
        else if (lex.check("shake.phase"))
        {
            if (!lex.check("="))
            {
                errorWait("=");
                return false;
            }
            p->shake.phase = getExpression();
        }
        else if (lex.check("shake.time"))
        {
            if (!lex.check("="))
            {
                errorWait("=");
                return false;
            }
            p->shake.time = getExpression();
        }
        else if (lex.check("sound.group"))
        {
            if (!lex.check("="))
            {
                errorWait("=");
                return false;
            }
            p->sound.group = getExpression();
        }
        else if (lex.check("sound.index"))
        {
            if (!lex.check("="))
            {
                errorWait("=");
                return false;
            }
            p->sound.index = getExpression();
        }
        else if (lex.check("ground.velx"))
        {
            if (!lex.check("="))
            {
                errorWait("=");
                return false;
            }
            p->velx = getExpression();
        }
        else if (lex.check("ground.vely"))
        {
            if (!lex.check("="))
            {
                errorWait("=");
                return false;
            }
            p->vely = getExpression();
        }
        else
            break;
    }
    return true;
}

//=============================================================================
// Faz o parse de um StateController Projectile
//=============================================================================
bool StateCompiler::parseProjectile()
{
    Stat::Projectile *p = new Stat::Projectile(SCTRL_PROJECTILE);
    sctrl = p;
    cout << "fazendo parsing do projétil" << endl;
    if (!parseTriggers())
        return false;

    while (!lex.eof())
    {
        if (lex.check("x"))
        {
            if (!lex.check("="))
            {
                errorWait("=");
                return false;
            }
            p->x = getExpression();
        }
        else if (lex.check("y"))
        {
            if (!lex.check("="))
            {
                errorWait("=");
                return false;
            }
            p->y = getExpression();
        }
        else if (lex.check("velx"))
        {
            if (!lex.check("="))
            {
                errorWait("=");
                return false;
            }
            p->velX = getExpression();
        }
        else if (lex.check("vely"))
        {
            if (!lex.check("="))
            {
                errorWait("=");
                return false;
            }
            p->velY = getExpression();
        }
        else if (lex.check("anim"))
        {
            if (!lex.check("="))
            {
                errorWait("=");
                return false;
            }
            p->anim = getExpression();
        }
        else if (lex.check("hit.anim"))
        {
            if (!lex.check("="))
            {
                errorWait("=");
                return false;
            }
            p->hitAnim = getExpression();
        }
        else if (lex.check("hit.shake.time"))
        {
            if (!lex.check("="))
            {
                errorWait("=");
                return false;
            }
            p->shakeTime = getExpression();
        }
        else if (lex.check("hit.shake.freq"))
        {
            if (!lex.check("="))
            {
                errorWait("=");
                return false;
            }
            p->shakeFreq = getExpression();
        }
        else if (lex.check("hit.shake.amplitude"))
        {
            if (!lex.check("="))
            {
                errorWait("=");
                return false;
            }
            p->shakeAmpl = getExpression();
        }
        else if (lex.check("hit.shake.phase"))
        {
            if (!lex.check("="))
            {
                errorWait("=");
                return false;
            }
            p->shakePhase = getExpression();
        }
        else if (lex.check("hit.sound.group"))
        {
            if (!lex.check("="))
            {
                errorWait("=");
                return false;
            }
            p->hitSoundGroup = getExpression();
        }
        else if (lex.check("hit.sound.index"))
        {
            if (!lex.check("="))
            {
                errorWait("=");
                return false;
            }
            p->hitSoundIndex = getExpression();
        }
        else
            break;
    }
    return true;
}

//=============================================================================
// Faz o parse de um StateController PlaySound
//=============================================================================
bool StateCompiler::parsePlaySound()
{
    PlaySound *p = new PlaySound(SCTRL_PLAYSOUND);
    sctrl = p;
    // cout << "fazendo parsing do posset" << endl;
    if (!parseTriggers())
        return false;
    while (!lex.eof())
    {
        if (lex.check("group"))
        {
            if (!lex.check("="))
            {
                errorWait("=");
                return false;
            }
            p->group = getExpression();
        }
        else if (lex.check("index"))
        {
            if (!lex.check("="))
            {
                errorWait("=");
                return false;
            }
            p->index = getExpression();
        }
        else
            break;
    }
    return true;
}

//=============================================================================
// Faz o parse de um StateController VarSet
//=============================================================================
bool StateCompiler::parseVarSet()
{
    VarSet *p = new VarSet(SCTRL_VARSET);
    sctrl = p;
    // cout << "fazendo parsing do posset" << endl;
    if (!parseTriggers())
        return false;
    while (!lex.eof())
    {
        if (lex.check("var"))
        {
            if (!lex.check("="))
            {
                errorWait("=");
                return false;
            }
            p->var = getExpression();
        }
        else if (lex.check("value"))
        {
            if (!lex.check("="))
            {
                errorWait("=");
                return false;
            }
            p->value = getExpression();
        }
        else
            break;
    }
    return true;
}

//=============================================================================
// Faz o parse de um StateController null
//=============================================================================
bool StateCompiler::parseNull()
{
    sctrl = NULL;
    // cout << "fazendo parsing do velset" << endl;
    while (!lex.eof())
    {
        if (lex.check("statedef"))
        {
            lex.rewind();
            break;
        }
        else
            lex.consumeToken();
    }
    return true;
}

}
