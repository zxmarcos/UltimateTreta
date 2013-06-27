#ifndef __EXPRESSIONCOMPILER_HPP__
#define __EXPRESSIONCOMPILER_HPP__

#include <vector>
#include <utility>
#include <string>
#include <unordered_map>
#include "ExpressionTokenizer.hpp"
#include "Util.hpp"
using namespace std;

namespace Stat
{

class ExpressionCompiler;
class VirtualMachine;
struct VmOperation;

// Expressão compilada
class ExpressionValue
{
    friend class ExpressionCompiler;
    friend class VirtualMachine;
public:
    ExpressionValue();
    ~ExpressionValue();
    void dasm();
private:
    vector<VmOperation*> stream;
    ImplementObjectCounter();
};

typedef void (ExpressionCompiler::*TriggerAvailCallback)(int opcode);
typedef pair<int, TriggerAvailCallback> TriggerAvailPair;

// Compilador de expressões
class ExpressionCompiler
{
public:
    ExpressionCompiler();
    ~ExpressionCompiler();
    ExpressionValue *compile(string str);
private:
    Stat::StTokenizer lex;
    void emit(int op);
    void emit(int op, int arg);
    void emit(int op, float arg);
    void emit(int op, string arg);
    void emit(int op, ExpressionValue *cond, ExpressionValue *iftrue, ExpressionValue *iffalse);
    void availStage0();
    void availStage1();
    void availStage2();
    void availStage3();
    void availStage4();
    void availStage5();
    void availStage6();
    void availStage7();
    void availStage8();
    void availStage9();
    void availStage10();
    void availStage11();
    void availStage12();
    void availStage13();
    static const unordered_map<string, TriggerAvailPair> triggerAvailTable;
    void availTrigger0(int opcode);
    void availTrigger1(int opcode);
    void availTriggerCmd(int opcode);
    void availTriggerStateType(int opcode);
    void availTriggerPosVel(int opcode);
    void availTriggerIfElse(int opcode);
    int triggerCode;
    ExpressionValue *expr;
};

}

#endif
