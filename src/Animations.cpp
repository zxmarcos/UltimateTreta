//==============================================================================
// Todas as funções, classes relacionadas a animação estão nesse arquivo
// Marcos Medeiros
//==============================================================================
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <iomanip>
#include <cctype>
#include <map>
#include <vector>
#include <set>
#include "Util.hpp"
#include "Animations.hpp"
#include "Collision.hpp"
#include "Console.hpp"
#include "Tokenizer.hpp"

#define LOG_TOKENS	0
#define LOG_ACTIONS 0
#define LOG_FRAMES  0
#define LOG_CLSN    0
#define LOG_LOOPS   0
using namespace std;

// Frescura do code::blocks em dizer ao compilador que é puritano em
// relação ao padrão ANSI do C...

#undef __STRICT_ANSI__
#include <string.h>
#define strcmpi strcasecmp

namespace Anim
{

InitObjectCounter(Action);
InitObjectCounter(Frame);
InitObjectCounter(Animator);


//==============================================================================
// A idéia do frame não possuir nenhum método é porque somente nosso carregador
// pode modifica-la.
// ctor
//==============================================================================
Frame::Frame(short g, short i, short x, short y, short t, bool hf, bool vf, bool add)
{
    group = g;
    index = i;
    xaxis = x;
    yaxis = y;
    time = t;
    clsn1 = NULL;
    clsn2 = NULL;
    clsn1Default = NULL;
    clsn2Default = NULL;
    vflip = vf;
    hflip = hf;
    addBlend = add;
    IncrementObjectCounter();
}

//==============================================================================
// dtor
//==============================================================================
Frame::~Frame()
{
    safeDelete(clsn1);
    safeDelete(clsn2);
    DecrementObjectCounter();
}

//==============================================================================
// ctor
// Assim como a classe Frame, essa é uma classe em que só nossa classe principal
// de animação pode modificar seus valores.
//==============================================================================

Action::Action(int n)
{
    number = n;
    loopstart = 0;
    IncrementObjectCounter();
}

//==============================================================================
// dtor
// Como as clsn padrões são compartilhadas, precisamos tomar conta de desalocar
// somente uma vez cada objeto do tipo
//==============================================================================
Action::~Action()
{
    cout << "Liberando action " << number << endl;
    Frame *ptr = NULL;
    set<Col::CollisionList *> clptrs;
    set<Frame *> frptrs;
    // somente destruimos as clsn compartilhadas e os frames
    while (!vFrames.empty()) {
        ptr = vFrames.back();
        vFrames.pop_back();
        if (ptr)
            frptrs.insert(ptr);
        if (ptr->clsn1Default)
            clptrs.insert(ptr->clsn1Default);
        if (ptr->clsn2Default)
            clptrs.insert(ptr->clsn2Default);
    }
    for (auto i : clptrs)
        delete i;
    for (auto i : frptrs)
        delete i;
    DecrementObjectCounter();
}

//==============================================================================
// /////////////////////////////////////////////////////////////////////////////
//                            Parser
// /////////////////////////////////////////////////////////////////////////////
//==============================================================================

//==============================================================================
// ctor
//==============================================================================
AnimLoader::AnimLoader()
{
    frame = NULL;
    action = NULL;
    clsn1 = NULL;
    clsn2 = NULL;
    clsn1Default = NULL;
    clsn2Default = NULL;
}

//==============================================================================
// dtor
//==============================================================================
AnimLoader::~AnimLoader()
{
    cout << "Liberando animações " << fileName << endl;
    int nactions = Action::getObjectCounter();
    int nboxes = Col::CollisionBox::getObjectCounter();
    int nframes = Frame::getObjectCounter();
    for (auto i = mapActions.begin(); i != mapActions.end(); i++)
    {
        delete i->second;
    }
    nactions = (nactions - Action::getObjectCounter());
    nboxes = (nboxes - Col::CollisionBox::getObjectCounter());
    nframes = (nframes - Frame::getObjectCounter());
    cout << nactions << " actions desalocados com sucesso!" << endl;
    cout << nframes << " frames desalocados com sucesso!" << endl;
    cout << nboxes << " boxes desalocados com sucesso!" << endl;
}

//==============================================================================
// Cria um novo quadro
//==============================================================================
void AnimLoader::createFrame(int g, int i, int x, int y, int t, bool h, bool v, bool a)
{
    frame = new Frame(g, i, x, y, t, h, v, a);
    frame->clsn1 = clsn1;
    frame->clsn2 = clsn2;
    frame->clsn1Default = clsn1Default;
    frame->clsn2Default = clsn2Default;
    action->vFrames.push_back(frame);
    totalFrames++;
    // essas clsn's são exclusivas desse frame
    clsn1 = NULL;
    clsn2 = NULL;
}

//==============================================================================
// Cria um novo action
//==============================================================================
void AnimLoader::createAction(int number)
{
    totalFrames = 0;
    loopstart = 0;
    action = new Action(number);
    mapActions[number] = action;
    action->loopstart = 0;
    clsn1 = NULL;
    clsn2 = NULL;
    clsn1Default = NULL;
    clsn2Default = NULL;
}

//==============================================================================
// Define o próximo frame como começo do loop
//==============================================================================
void AnimLoader::setLoopstart()
{
    loopstart = totalFrames;
    if (action)
        action->loopstart = loopstart;
#if LOG_LOOPS
    cout << "Loopstart em " << action->loopstart << endl;
#endif
}

void AnimLoader::createClsnList(int type, bool def, size_t size)
{
    Col::CollisionList *list = new Col::CollisionList(type, size);
    isDefaultClsn = def;
    if (isDefaultClsn)
    {
        switch (type)
        {
        case 1:
            clsn1Default = list;
            break;
        case 2:
            clsn2Default = list;
            break;
        }
    }
    else
    {
        switch (type)
        {
        case 1:
            clsn1 = list;
            break;
        case 2:
            clsn2 = list;
            break;
        }
    }
#if LOG_CLSN
    cout << "clsnList " << type << " " << def << endl;
#endif
}

void AnimLoader::createClsn(int type, int idx, int x1, int y1, int x2, int y2)
{
    Col::CollisionBox *box = new Col::CollisionBox(x1, y1, x2, y2);
#if LOG_CLSN
    cout << type << " ["<<idx<<"] "<<x1 << "," << y1 << "," << x2 << ","<< y2 << endl;
#endif
    switch (type)
    {
    case 1:
        if (isDefaultClsn)
            clsn1Default->add(box, idx);
        else
            clsn1->add(box, idx);
        break;
    case 2:
        if (isDefaultClsn)
            clsn2Default->add(box, idx);
        else
            clsn2->add(box, idx);
        break;
    default:
        delete box;
        break;
    }
}

//=============================================================================
// Nosso parser, responsável por interpretar o arquivo e criar nossas
// classes que representação actions, frames e caixas de colisão.
//=============================================================================
bool AnimLoader::open(string name)
{
    Console::print("AirLoader: carregando animacoes: %s", name.c_str());
    cout << "Carregando arquivo de animacoes: " << name << endl;

    if (!lex.open(name))
    {
        Console::print("AirLoader: nao foi possivel carregar: %s", name.c_str());
        cerr << "Nao foi possivel carregar o arquivo " << name << endl;
        return false;
    }

    fileName = name;

    int nactions = Action::getObjectCounter();
    int nboxes = Col::CollisionBox::getObjectCounter();
    int nframes = Frame::getObjectCounter();


    while (!lex.eof())
    {
        int actionNo = -1;
        if (lex.check("["))
        {
            lex.check("begin");
            lex.check("action");
            lex.checkNumber();
            actionNo = lex.toInt();
            lex.check("]");
#if LOG_ACTIONS
            cout << "action " << actionNo << endl;
#endif
            createAction(actionNo);

        }
        else if (lex.check("clsn1") || lex.check("clsn1default") ||
                 lex.check("clsn2") || lex.check("clsn2default"))
        {
            int type = lex.tok()[4] - 0x30;
            bool def = false;

            if (!strcmpi(lex.tok().c_str(), "clsn1default") || !strcmpi(lex.tok().c_str(), "clsn2default"))
                def = true;
            lex.check(":");
            int boxes = 0;
            lex.checkNumber();
            boxes = lex.toInt();

            createClsnList(type, def, boxes);

            for (int i = 0; i < boxes; i++)
            {
                lex.check("clsn1");
                lex.check("clsn2");

                lex.check("[");
                lex.checkNumber();
                if (lex.toInt() != i)
                {
                    cout << "Caixa na posicao errada! esperado " << i <<
                         ", recebido " << lex.toInt() << endl;
                }
                lex.check("]");
                lex.check("=");

                int x1, y1, x2, y2;

                lex.checkNumber();
                x1 = lex.toInt();
                lex.check(",");

                lex.checkNumber();
                y1 = lex.toInt();
                lex.check(",");

                lex.checkNumber();
                x2 = lex.toInt();
                lex.check(",");

                lex.checkNumber();
                y2 = lex.toInt();
#if LOG_CLSN
                cout << "clsn[" << i << "]=" << x1 << "," << y1 << ","
                     << x2 << "," << y2 << endl;
#endif
                createClsn(type, i, x1, y1, x2, y2);
            }
        }
        else if (lex.check("loopstart"))
        {
            setLoopstart();
            // coloca o flag de loop nesse frame
        }
        else if (lex.checkNumber())
        {
            int group, index, xaxis, yaxis, ticks;

            group = lex.toInt();

            lex.check(",");
            lex.checkNumber();
            index = lex.toInt();

            lex.check(",");
            lex.checkNumber();
            xaxis = lex.toInt();

            lex.check(",");
            lex.checkNumber();
            yaxis = lex.toInt();

            lex.check(",");
            lex.checkNumber();
            ticks = lex.toInt();

            bool vf = false;
            bool hf = false;
            bool ad = false;

            // outros parametros
            if (lex.check(","))
            {
                if (lex.check("H"))
                {
                    hf = true;
                }
                else if (lex.check("V"))
                {
                    vf = true;
                }
                else if (lex.check("VH") || lex.check("HV"))
                {
                    hf = true;
                    vf = true;
                }
                if (lex.check(","))
                {
                    if (lex.check("A")) {
                        ad = true;
                    }
                }
            }
#if LOG_FRAMES
            cout << "frame:" << group << "," << index << "," << xaxis
                 << "," << yaxis << "," << ticks << endl;
#endif
            createFrame(group, index, xaxis, yaxis, ticks, hf, vf, ad);
        }
        else
        {
            cout << "Ignorando " << lex.tok() << endl;
            // apenas ignora e pula para o próximo token
            lex.consumeToken();
        }
    }
    nactions = (Action::getObjectCounter() - nactions);
    nboxes = (Col::CollisionBox::getObjectCounter() - nboxes);
    nframes = (Frame::getObjectCounter() - nframes);
    cout << nactions << " actions alocados com sucesso!" << endl;
    cout << nframes << " frames alocados com sucesso!" << endl;
    cout << nboxes << " boxes alocados com sucesso!" << endl;
    Console::print("-- Actions: %d, Clsns: %d, Frames: %d", nactions, nboxes, nframes);
    Console::print("AirLoader: ok");
    return true;
}

//==============================================================================
// Retorna um action da nossa lista
//==============================================================================
const Action *AnimLoader::get(int number)
{
    if (mapActions[number])
        return mapActions[number];
    return NULL;
}

bool AnimLoader::exist(int number)
{
    if (mapActions[number])
        return true;
    return false;
}


//==============================================================================
// /////////////////////////////////////////////////////////////////////////////
//                           Animador
// /////////////////////////////////////////////////////////////////////////////
//==============================================================================

//==============================================================================
// ctor
//==============================================================================
Animator::Animator(AnimLoader &loader_) : loader(loader_)
{
    frame = NULL;
    action = NULL;
    time = 0;
    frameTime = 0;
    frameNo = 0;
    IncrementObjectCounter();
}

//==============================================================================
// dtor
//==============================================================================
Animator::~Animator()
{
    DecrementObjectCounter();
}

//==============================================================================
// Define o action que o animador utilizará.
//==============================================================================
bool Animator::set(int n, int elem)
{
    action = loader.get(n);
    if (action)
    {
        if ((elem < (int) action->vFrames.size()) && elem >= 0)
            frameNo = elem;
        else
            frameNo = 0;
        frameTime = 0;
        time = 0;
        if (action->vFrames.size())
            frame = action->vFrames[0];
        return true;
    }
    time = 0;
    return false;
}

//==============================================================================
// Atualiza a animação atual
//==============================================================================
void Animator::update()
{
    if (!action)
        return;
    if (action->vFrames.empty())
        return;
    frame = action->vFrames[frameNo];
    if (frameTime >= frame->time && (frame->time > 0))
    {
        frameTime = 0;
        frameNo++;
        // verifica se é o último elemento da action
        if ((unsigned)frameNo >= action->vFrames.size())
        {
            frameNo = action->loopstart;
        }
    }
    else
        frameTime++;
    time++;
}

//==============================================================================
// Retorna se é o último tick da animação
//==============================================================================
bool Animator::isLastTick() const
{
    if (!action)
        return false;
    if (action->vFrames.empty())
        return false;
    if ((unsigned) frameNo == (action->vFrames.size() - 1))
        if (frameTime == (frame->time - 1))
            return true;
    return false;
}

//==============================================================================
// Retorna o grupo do quadro atual
//==============================================================================
int Animator::group()
{
    if (!frame)
        return -1;
    return frame->group;
}

//==============================================================================
// Retorna o indice do quadro atual
//==============================================================================
int Animator::index()
{
    if (!frame)
        return -1;
    return frame->index;
}

//==============================================================================
// Retorna o alinhamento em x do quadro atual
//==============================================================================
int Animator::x()
{
    if (!frame)
        return -1;
    return frame->xaxis;
}

//==============================================================================
// Retorna o alinhamento em y do quadro atual
//==============================================================================
int Animator::y()
{
    if (!frame)
        return -1;
    return frame->yaxis;
}

//==============================================================================
// Retorna se o quadro atual é utilizado adição para composição
//==============================================================================
bool Animator::additive()
{
    if (!frame)
        return false;
    return frame->addBlend;
}

//==============================================================================
// Retorna se o quadro atual é invertido horizontalmente
//==============================================================================
bool Animator::fliph()
{
    if (!frame)
        return false;
    return frame->hflip;
}

//==============================================================================
// Retorna se o quadro atual é invertido verticalmente
//==============================================================================
bool Animator::flipv()
{
    if (!frame)
        return false;
    return frame->vflip;
}

//==============================================================================
// Retorna o número da animação atual
//==============================================================================
int Animator::number()
{
    if (!action)
        return -1;
    return action->number;
}

const vector<Col::CollisionBox> Animator::getAttackBoxes(float x, float y, bool facing) const
{
    vector<Col::CollisionBox> boxes;
    if (frame->clsn1Default)
    {
        const vector<Col::CollisionBox>& v = frame->clsn1Default->getBoxes(x, y, facing);
        boxes.insert(boxes.end(), v.begin(), v.end());
    }
    if (frame->clsn1)
    {
        const vector<Col::CollisionBox>& v = frame->clsn1->getBoxes(x, y, facing);
        boxes.insert(boxes.end(), v.begin(), v.end());
    }
    return boxes;
}

const vector<Col::CollisionBox> Animator::getDefenseBoxes(float x, float y, bool facing) const
{
    vector<Col::CollisionBox> boxes;
    if (frame->clsn2Default)
    {
        const vector<Col::CollisionBox>& v = frame->clsn2Default->getBoxes(x, y, facing);
        boxes.insert(boxes.end(), v.begin(), v.end());
    }
    if (frame->clsn2)
    {
        const vector<Col::CollisionBox>& v = frame->clsn2->getBoxes(x, y, facing);
        boxes.insert(boxes.end(), v.begin(), v.end());
    }
    return boxes;
}

//==============================================================================
// Retorna o número do quadro na animação atual
//==============================================================================
int Animator::frameNumber()
{
    if (!action)
        return -1;
    return frameNo;
}

int Animator::elapsed()
{
    return time;
}

int Animator::frameTick()
{
    return frameTime;
}

}
