#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>
#include <allegro5/allegro.h>
#include "InputMachine.hpp"
#include "InputManager.hpp"
#include "Display.hpp"
#include "Fight.hpp"
#include "Console.hpp"

#define LOG_ACCEPT  1

using namespace std;

namespace Input
{

InputMachine::InputMachine(int bindToPlayer) : playerInputId(bindToPlayer)
{
    Command *cmd = new Command("qcfx");
    cmd->add(new SingleKey(Down));
    cmd->add(new SingleKey(DownFoward));
    cmd->add(new SingleKey(Foward));
    cmd->add(new SingleKey(x));
    cmd->setTime(15);
    commands.push_back(cmd);

    cmd = new Command("qcfy");
    cmd->add(new SingleKey(Down));
    cmd->add(new SingleKey(DownFoward));
    cmd->add(new SingleKey(Foward));
    cmd->add(new SingleKey(y));
    cmd->setTime(15);
    commands.push_back(cmd);

    cmd = new Command("qcfz");
    cmd->add(new SingleKey(Down));
    cmd->add(new SingleKey(DownFoward));
    cmd->add(new SingleKey(Foward));
    cmd->add(new SingleKey(z));
    cmd->setTime(15);
    commands.push_back(cmd);

    cmd = new Command("qcba");
    cmd->add(new SingleKey(Down));
    cmd->add(new SingleKey(DownBack));
    cmd->add(new SingleKey(Back));
    cmd->add(new SingleKey(a));
    cmd->setTime(15);
    commands.push_back(cmd);

    cmd = new Command("qcbb");
    cmd->add(new SingleKey(Down));
    cmd->add(new SingleKey(DownBack));
    cmd->add(new SingleKey(Back));
    cmd->add(new SingleKey(b));
    cmd->setTime(15);
    commands.push_back(cmd);

    cmd = new Command("qcbc");
    cmd->add(new SingleKey(Down));
    cmd->add(new SingleKey(DownBack));
    cmd->add(new SingleKey(Back));
    cmd->add(new SingleKey(c));
    cmd->setTime(15);
    commands.push_back(cmd);

    cmd = new Command("pdgx");
    cmd->add(new SingleKey(Foward));
    cmd->add(new SingleKey(Down));
    cmd->add(new SingleKey(DownFoward));
    cmd->add(new SingleKey(x));
    cmd->setTime(30);
    commands.push_back(cmd);

    cmd = new Command("holdback");
    cmd->add(new SingleKeyHold(Back, true));
    cmd->setTime(1);
    commands.push_back(cmd);

    cmd = new Command("holdfwd");
    cmd->add(new SingleKeyHold(Foward, true));
    cmd->setTime(1);
    commands.push_back(cmd);

    cmd = new Command("holddown");
    cmd->add(new SingleKeyHold(Down, true));
    cmd->setTime(1);
    commands.push_back(cmd);

    cmd = new Command("holdup");
    cmd->add(new SingleKeyHold(Up, true));
    cmd->setTime(1);
    commands.push_back(cmd);

    cmd = new Command("x");
    cmd->add(new SingleKey(x));
    cmd->setTime(1);
    commands.push_back(cmd);

    cmd = new Command("y");
    cmd->add(new SingleKey(y));
    cmd->setTime(1);
    commands.push_back(cmd);

    cmd = new Command("z");
    cmd->add(new SingleKey(z));
    cmd->setTime(1);
    commands.push_back(cmd);

    cmd = new Command("a");
    cmd->add(new SingleKey(a));
    cmd->setTime(1);
    commands.push_back(cmd);

    cmd = new Command("b");
    cmd->add(new SingleKey(b));
    cmd->setTime(1);
    commands.push_back(cmd);

    cmd = new Command("c");
    cmd->add(new SingleKey(c));
    cmd->setTime(1);
    commands.push_back(cmd);

    setInputNo(0);
}

InputMachine::~InputMachine()
{
    for (auto i : commands)
        delete i;
}

void InputMachine::setInputNo(int n)
{
    playerInputId = n;
}

void InputMachine::update()
{
    moment.clear();
    moment.F = (readSymbol(playerInputId, K_FOWARD)) ? true : false;
    moment.B = (readSymbol(playerInputId, K_BACK)) ? true : false;
    moment.D = (readSymbol(playerInputId, K_DOWN)) ? true : false;
    moment.U = (readSymbol(playerInputId, K_UP)) ? true : false;
    moment.a = (readSymbol(playerInputId, K_A)) ? true : false;
    moment.b = (readSymbol(playerInputId, K_B)) ? true : false;
    moment.c = (readSymbol(playerInputId, K_C)) ? true : false;
    moment.x = (readSymbol(playerInputId, K_X)) ? true : false;
    moment.y = (readSymbol(playerInputId, K_Y)) ? true : false;
    moment.z = (readSymbol(playerInputId, K_Z)) ? true : false;

    if (moment.D && moment.F)
    {
        moment.D = false;
        moment.F = false;
        moment.DF = true;
    }
    if (moment.D && moment.B)
    {
        moment.D = false;
        moment.B = false;
        moment.DB = true;
    }
    if (moment.U && moment.F)
    {
        moment.U = false;
        moment.F = false;
        moment.UF = true;
    }
    if (moment.U && moment.B)
    {
        moment.U = false;
        moment.B = false;
        moment.UB = true;
    }

    asserted.clear();
    for (auto cmd : commands)
    {
        if (cmd->match(moment))
        {
            asserted.push_back(cmd->toString());
#if LOG_ACCEPT
            Console::print("%d Comando aceito: %s", Fight::getTime(), cmd->toString().c_str());
#endif
        }
    }
}

bool InputMachine::isCommandAsserted(const string& name)
{
    if (find(asserted.begin(), asserted.end(), name) != asserted.end())
        return true;
    return false;
}

SingleKey::SingleKey(int key_, bool fourDir) : key(key_), fourDirection(fourDir)
{
}

SingleKey::~SingleKey()
{
}

bool SingleKey::match(const InputMoment& last, const InputMoment& now) const
{
    if (fourDirection)
    {
        switch (key)
        {
        case Up:
            return (now.U || now.UF || now.UB) && ((!last.U) || !(last.UF) || !(last.UB));
        case Down:
            return (now.D || now.DF || now.DB) && ((!last.D) || !(last.DF) || !(last.DB));
        case Foward:
            return (now.F || now.UF || now.DF) && ((!last.F) || !(last.UF) || !(last.DF));
        case Back:
            return (now.B || now.UB || now.DB) && ((!last.B) || (!last.UB) || !(last.DB));
        default:
            goto _normal;
        }
    }
    else
    {
_normal:
        switch (key)
        {
        case Up:
            return now.U && (!last.U);
        case Down:
            return now.D && (!last.D);
        case Back:
            return now.B && (!last.B);
        case Foward:
            return now.F && (!last.F);
        case DownFoward:
            return now.DF && (!last.DF);
        case DownBack:
            return now.DB && (!last.DB);
        case UpFoward:
            return now.UF && (!last.UF);
        case UpBack:
            return now.UB && (!last.UB);
        case a:
            return now.a && (!last.a);
        case b:
            return now.b && (!last.b);
        case c:
            return now.c && (!last.c);
        case x:
            return now.x && (!last.x);
        case y:
            return now.y && (!last.y);
        case z:
            return now.z && (!last.z);
        }
    }
    return false;
}

string SingleKey::toString() const
{
    switch (key)
    {
    case Up:
        return "U";
    case Down:
        return "D";
    case Back:
        return "B";
    case Foward:
        return "F";
    case DownFoward:
        return "DF";
    case DownBack:
        return "DB";
    case UpFoward:
        return "UF";
    case UpBack:
        return "UB";
    case a:
        return "a";
    case b:
        return "b";
    case c:
        return "c";
    case x:
        return "x";
    case y:
        return "y";
    case z:
        return "z";
    }
    return "";
}


DualKey::DualKey(SingleKey *a_, SingleKey *b_) : SingleKey(0), a(a_), b(b_)
{
}

DualKey::~DualKey()
{
    safeDelete(a);
    safeDelete(b);
}

bool DualKey::match(const InputMoment& last, const InputMoment& now) const
{
    bool ra = (a) ? a->match(last, now) : false;
    bool rb = (b) ? b->match(last, now) : false;
    return (ra && rb);
}

string DualKey::toString() const
{
    return (a->toString() + "+" + b->toString());
}


SingleKeyHold::SingleKeyHold(int key_, bool fourDir) : SingleKey(key_, fourDir)
{
}

SingleKeyHold::~SingleKeyHold()
{
}

bool SingleKeyHold::match(const InputMoment& last, const InputMoment& now) const
{
    if (fourDirection)
    {
        switch (key)
        {
        case Up:
            return (now.U || now.UF || now.UB) && (last.U || last.UF || last.UB);
        case Down:
            return (now.D || now.DF || now.DB) && (last.D || last.DF || last.DB);
        case Foward:
            return (now.F || now.UF || now.DF) && (last.F || last.UF || last.DF);
        case Back:
            return (now.B || now.UB || now.DB) && (last.B || last.UB || last.DB);
        default:
            goto _normal;
        }
    }
    else
    {

_normal:
        switch (key)
        {
        case Up:
            return (last.U) && (last.U == now.U);
        case Down:
            return (last.D) && (last.D == now.D);
        case Back:
            return (last.B) && (last.B == now.B);
        case Foward:
            return (last.F) && (last.F == now.F);
        case DownFoward:
            return (last.DF) && (last.DF == now.DF);
        case DownBack:
            return (last.DB) && (last.DB == now.DB);
        case UpFoward:
            return (last.UF) && (last.UF == now.UF);
        case UpBack:
            return (last.UB) && (last.UB == now.UB);
        case a:
            return (last.a) && (last.a == now.a);
        case b:
            return (last.b) && (last.b == now.b);
        case c:
            return (last.c) && (last.c == now.c);
        case x:
            return (last.x) && (last.x == now.x);
        case y:
            return (last.y) && (last.y == now.y);
        case z:
            return (last.z) && (last.z == now.z);
        }
    }
    return false;
}

string SingleKeyHold::toString() const
{
    return string("/") + SingleKey::toString();
}


SingleKeyRelease::SingleKeyRelease(int key_) : SingleKey(key_)
{
}

SingleKeyRelease::~SingleKeyRelease()
{
}

bool SingleKeyRelease::match(const InputMoment& last, const InputMoment& now) const
{
    switch (key)
    {
    case Up:
        return (last.U && !now.U);
    case Down:
        return (last.D && !now.D);
    case Back:
        return (last.B && !now.B);
    case Foward:
        return (last.F && !now.F);
    case DownFoward:
        return (last.DF && !now.DF);
    case DownBack:
        return (last.DB && !now.DB);
    case UpFoward:
        return (last.UF && !now.UF);
    case UpBack:
        return (last.UB && !now.UB);
    case a:
        return (last.a && !now.a);
    case b:
        return (last.b && !now.b);
    case c:
        return (last.c && !now.c);
    case x:
        return (last.x && !now.x);
    case y:
        return (last.y && !now.y);
    case z:
        return (last.z && !now.z);
    }
    return false;
}

string SingleKeyRelease::toString() const
{
    return string("~") + SingleKey::toString();
}

string Command::toString() const
{
    return name;
}


Command::Command(string name_) : name(name_)
{
    currentTick = -1;
    maxTime = 2;
    startTime = 0;
    accepting = false;
    pattern.clear();
    current = pattern.begin();
    last.clear();
}

Command::~Command()
{
    for (auto i : pattern)
        delete i;
}

// Verifica se o comando foi aceito
bool Command::match(const InputMoment& now)
{
    bool ok = false;
    //cout << "currentTick: " << currentTick <<"/" << maxTime << endl;
    // não logrou exito na execução do comando no periodo determinado
    if (currentTick >= maxTime)
    {
        current = pattern.begin();
        currentTick = -1;
        // está aceitando estados?
        accepting = false;
    }
    SingleKey *key = *current;

    if (current != pattern.end())
    {
        //cout << "matching: " << key->toString() << endl;
        if (key->match(last, now))
        {
            if (current == pattern.begin())
                accepting = true;
            current++;
            // Se for o último simbolo então reiniciamos e aceitamos
            // se estiver dentro do tempo correto
            if (current == pattern.end())
            {
                current = pattern.begin();
                currentTick = -1;
                accepting = false;
                if (currentTick <= maxTime)
                    ok = true;
            }
        }
    }
    if (accepting)
        currentTick++;
    last = now;
    return ok;
}

}
