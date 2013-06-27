//=============================================================================
// Máquina virtual capaz de processar nosso código compilado
// Marcos Medeiros
//=============================================================================
#include <string>
#include <iostream>
#include <map>
#include <cmath>
#include "StateCompiler.hpp"
#include "ExpressionCompiler.hpp"
#include "VirtualMachine.hpp"
#include "Character.hpp"
#include "Fight.hpp"
using namespace std;

namespace Stat
{

// Tabela de callbacks das operações
const VmOperationCallback VirtualMachine::opTable[VM_OP_MAX] =
{
    [OP_PUSH]			= VirtualMachine::opPush,
    [OP_ADD]			= VirtualMachine::opAdd,
    [OP_SUB]			= VirtualMachine::opSub,
    [OP_MUL]			= VirtualMachine::opMul,
    [OP_DIV]			= VirtualMachine::opDiv,
    [OP_POW]			= VirtualMachine::opPow,
    [OP_MOD]			= VirtualMachine::opMod,
    [OP_NEG]			= VirtualMachine::opNeg,
    [OP_ASSIGN]			= VirtualMachine::opAssign,
    [OP_LOG_NEG]		= VirtualMachine::opLogicalNeg,
    [OP_LOG_OR]			= VirtualMachine::opLogicalOr,
    [OP_LOG_XOR]		= VirtualMachine::opLogicalXor,
    [OP_LOG_AND]		= VirtualMachine::opLogicalAnd,
    [OP_OR]				= VirtualMachine::opOr,
    [OP_XOR]			= VirtualMachine::opXor,
    [OP_AND]			= VirtualMachine::opAnd,
    [OP_NOT]			= VirtualMachine::opNot,
    [OP_EQ]				= VirtualMachine::opEQ,
    [OP_NE]				= VirtualMachine::opNE,
    [OP_GT]				= VirtualMachine::opGT,
    [OP_GE]				= VirtualMachine::opGE,
    [OP_LT]				= VirtualMachine::opLT,
    [OP_LE]				= VirtualMachine::opLE,
    [OP_ACOS]			= VirtualMachine::opAcos,
    [OP_ASIN]			= VirtualMachine::opAsin,
    [OP_ATAN]			= VirtualMachine::opAtan,
    [OP_COS]			= VirtualMachine::opCos,
    [OP_SIN]			= VirtualMachine::opSin,
    [OP_TAN]			= VirtualMachine::opTan,
    [OP_ABS]			= VirtualMachine::opAbs,
    [OP_CEIL]			= VirtualMachine::opCeil,
    [OP_FLOOR]			= VirtualMachine::opFloor,
    [OP_E]				= VirtualMachine::opE,
    [OP_EXP]			= VirtualMachine::opExp,
    [OP_LN]				= VirtualMachine::opLn,
    [OP_LOG]			= VirtualMachine::opLog,
    [OP_PI]				= VirtualMachine::opPI,
    [OP_ANIM]			= VirtualMachine::opAnim,
    [OP_ANIMELEMNO]		= VirtualMachine::opAnimElemNo,
    [OP_ANIMELEMTIME]	= VirtualMachine::opAnimElemTime,
    [OP_ANIMEXIST]		= VirtualMachine::opAnimExist,
    [OP_ANIMTIME]		= VirtualMachine::opAnimTime,
    [OP_TIME]           = VirtualMachine::opTime,
    [OP_COMMAND]        = VirtualMachine::opCommand,
    [OP_RANDOM]         = VirtualMachine::opRandom,
    [OP_STATENO]        = VirtualMachine::opStateNo,
    [OP_STATETYPE]      = VirtualMachine::opStateType,
    [OP_POS]            = VirtualMachine::opPos,
    [OP_VEL]            = VirtualMachine::opVel,
    [OP_IFELSE]         = VirtualMachine::opIfElse,
    [OP_VAR]            = VirtualMachine::opVar,
};

//=============================================================================
// ctor
//=============================================================================
VirtualMachine::VirtualMachine(Char::Character *player_) : player(player_)
{
    stack.reserve(10);
}

//=============================================================================
// dtor
//=============================================================================
VirtualMachine::~VirtualMachine()
{
}

//=============================================================================
// Desempilha um valor da pilha
//=============================================================================
VmValue VirtualMachine::pop()
{
    VmValue v;
    if (stack.empty())
    {
        v.type = VM_VAL_INTEGER;
        v.iValue = 0;
    }
    else
    {
        v = stack.back();
        stack.pop_back();
    }
#if 0
    if (v.type == VM_VAL_INTEGER)
        cout << "popi " << v.iValue << endl;
    else if (v.type == VM_VAL_FLOAT)
        cout << "popf " << v.fValue << endl;
#endif
    return v;
}

//=============================================================================
// Empilha um inteiro na pilha
//=============================================================================
void VirtualMachine::pushInteger(int value)
{
    VmValue v;
    v.type = VM_VAL_INTEGER;
    v.iValue = value;
    stack.push_back(v);
  //  cout << "pushi " << value << endl;
}

//=============================================================================
// Empilha um float na pilha
//=============================================================================
void VirtualMachine::pushFloat(float value)
{
    VmValue v;
    v.type = VM_VAL_FLOAT;
    v.fValue = value;
    stack.push_back(v);
 //   cout << "pushf " << value << endl;
}

void VirtualMachine::pushString(const string& value)
{
    VmValue v;
    v.type = VM_VAL_STRING;
    v.strValue = value.c_str();
    stack.push_back(v);
}


//=============================================================================
// Executa uma expressão
//=============================================================================
bool VirtualMachine::execute(ExpressionValue *expr, bool clearStack)
{
    if (clearStack)
        stack.clear();
    if (!expr) {
        cout << "NULL expr" << endl;
        return false;
    }
    for (auto op : expr->stream)
    {
        thisOperation = op;
        if (!op) {
            cout << "VM skip" << endl;
            continue;
        }
        // cout << op->type << endl;
        if (opTable[op->type](this))
        {
            stack.clear();
            cout << "VM Jam" << endl;
            return false;
        }
    }
    return true;
}

//=============================================================================
// Retorna o valor inteiro da expressão processada
//=============================================================================
int VirtualMachine::iValue()
{
    if (stack.empty())
        return 0;
    if (stack[0].type == VM_VAL_INTEGER)
        return stack[0].iValue;
    else if (stack[0].type == VM_VAL_FLOAT)
        return (int) stack[0].fValue;
    return 0;
}

//=============================================================================
// Retorna o valor decimal da expresão processada
//=============================================================================
float VirtualMachine::fValue()
{
    if (stack.empty())
        return 0.0;
    if (stack[0].type == VM_VAL_INTEGER)
        return (float) stack[0].iValue;
    else if (stack[0].type == VM_VAL_FLOAT)
        return stack[0].fValue;
    return 0;
}

//=============================================================================
// Operação: empilhar dados na pilha
//=============================================================================
bool VirtualMachine::opPush(VirtualMachine *vm)
{
    if (vm->thisOperation->argType == OP_ARG_INTEGER)
        vm->pushInteger(vm->thisOperation->iValue);
    else if (vm->thisOperation->argType == OP_ARG_FLOAT)
        vm->pushFloat(vm->thisOperation->fValue);
    else if (vm->thisOperation->argType == OP_ARG_STRING)
        vm->pushString(vm->thisOperation->strValue);
    return false;
}

//=============================================================================
// Operação: adicionar dois valores
//=============================================================================
bool VirtualMachine::opAdd(VirtualMachine *vm)
{
    VmValue a = vm->pop();
    VmValue b = vm->pop();
    if (a.type == VM_VAL_FLOAT || b.type == VM_VAL_FLOAT)
    {
        float v1 = (a.type == VM_VAL_INTEGER) ? ((float) a.iValue) : a.fValue;
        float v2 = (b.type == VM_VAL_INTEGER) ? ((float) b.iValue) : b.fValue;
        vm->pushFloat(v1 + v2);
    }
    else
        vm->pushInteger(a.iValue + b.iValue);
    return false;
}

//=============================================================================
// Operação: subtrair dois valores
//=============================================================================
bool VirtualMachine::opSub(VirtualMachine *vm)
{
    VmValue a = vm->pop();
    VmValue b = vm->pop();
    if (a.type == VM_VAL_FLOAT || b.type == VM_VAL_FLOAT)
    {
        float v1 = (a.type == VM_VAL_INTEGER) ? ((float) a.iValue) : a.fValue;
        float v2 = (b.type == VM_VAL_INTEGER) ? ((float) b.iValue) : b.fValue;
        vm->pushFloat(v2 - v1);
    }
    else
        vm->pushInteger(b.iValue - a.iValue);
    return false;
}

//=============================================================================
// Operação: multiplicar dois valores
//=============================================================================
bool VirtualMachine::opMul(VirtualMachine *vm)
{
    VmValue a = vm->pop();
    VmValue b = vm->pop();
    if (a.type == VM_VAL_FLOAT || b.type == VM_VAL_FLOAT)
    {
        float v1 = (a.type == VM_VAL_INTEGER) ? ((float) a.iValue) : a.fValue;
        float v2 = (b.type == VM_VAL_INTEGER) ? ((float) b.iValue) : b.fValue;
        vm->pushFloat(v1 * v2);
    }
    else
        vm->pushInteger(a.iValue * b.iValue);
    return false;
}

//=============================================================================
// Operação: efetuar divisão de dois valores
//=============================================================================
bool VirtualMachine::opDiv(VirtualMachine *vm)
{
    VmValue a = vm->pop();
    VmValue b = vm->pop();
    if (!a.iValue)
        return true;
    // verificar divisão por zero
    if (a.type == VM_VAL_FLOAT || b.type == VM_VAL_FLOAT)
    {
        float v1 = (a.type == VM_VAL_INTEGER) ? ((float) a.iValue) : a.fValue;
        float v2 = (b.type == VM_VAL_INTEGER) ? ((float) b.iValue) : b.fValue;
        vm->pushFloat(v2 / v1);
    }
    else
        vm->pushInteger(b.iValue / a.iValue);
    return false;
}

//=============================================================================
// Operação: efetuar exponenciação de um valor
//=============================================================================
bool VirtualMachine::opPow(VirtualMachine *vm)
{
    VmValue a = vm->pop();
    VmValue b = vm->pop();
    if (a.type == VM_VAL_FLOAT || b.type == VM_VAL_FLOAT)
    {
        float v1 = (a.type == VM_VAL_INTEGER) ? ((float) a.iValue) : a.fValue;
        float v2 = (b.type == VM_VAL_INTEGER) ? ((float) b.iValue) : b.fValue;
        vm->pushFloat(pow(v2, v1));
    }
    else
        vm->pushInteger(pow(b.iValue, a.iValue));
    return false;
}

//=============================================================================
// Operação: retornar o módulo de um divisão
//=============================================================================
bool VirtualMachine::opMod(VirtualMachine *vm)
{
    VmValue a = vm->pop();
    VmValue b = vm->pop();
    if (!a.iValue)
        return true;
    // verificar divisão por zero
    if (a.type == VM_VAL_FLOAT || b.type == VM_VAL_FLOAT)
    {
        float v1 = (a.type == VM_VAL_INTEGER) ? ((float) a.iValue) : a.fValue;
        float v2 = (b.type == VM_VAL_INTEGER) ? ((float) b.iValue) : b.fValue;
        vm->pushFloat(fmod(v2, v1));
    }
    else
        vm->pushInteger(b.iValue % a.iValue);
    return false;
}

//=============================================================================
// Operação: negação algébrica, trocar sinal do valor
//=============================================================================
bool VirtualMachine::opNeg(VirtualMachine *vm)
{
    VmValue a = vm->pop();
    if (a.type == VM_VAL_FLOAT)
        vm->pushFloat(-a.fValue);
    else
        vm->pushInteger(-a.iValue);
    return false;
}

//=============================================================================
// Operação: assimilar um valor (NÃO IMPLEMENTADO)
//=============================================================================
bool VirtualMachine::opAssign(VirtualMachine *vm)
{
    return false;
    // não faz nada...
}

//=============================================================================
// Operação: negação lógica
//=============================================================================
bool VirtualMachine::opLogicalNeg(VirtualMachine *vm)
{
    VmValue a = vm->pop();;
    if (a.type == VM_VAL_FLOAT)
        a.iValue = (int) a.fValue;
    if (a.iValue)
        vm->pushInteger(0);
    else
        vm->pushInteger(1);
    return false;
}

//=============================================================================
// Operação: OU lógico
//=============================================================================
bool VirtualMachine::opLogicalOr(VirtualMachine *vm)
{
    VmValue a = vm->pop();
    VmValue b = vm->pop();

    if (a.type == VM_VAL_FLOAT)
        a.iValue = (int) a.fValue;
    if (b.type == VM_VAL_FLOAT)
        b.iValue = (int) b.fValue;
    vm->pushInteger(a.iValue || b.iValue);
    return false;
}

//=============================================================================
// Operação: Exclusivo OU lógico
//=============================================================================
bool VirtualMachine::opLogicalXor(VirtualMachine *vm)
{
    VmValue a = vm->pop();
    VmValue b = vm->pop();

    if (a.type == VM_VAL_FLOAT)
        a.iValue = ((int) a.fValue) ? 1 : 0;
    if (b.type == VM_VAL_FLOAT)
        b.iValue = ((int) b.fValue) ? 1 : 0;

    vm->pushInteger(a.iValue ^ b.iValue);
    return false;
}

//=============================================================================
// Operação: E lógico
//=============================================================================
bool VirtualMachine::opLogicalAnd(VirtualMachine *vm)
{
    VmValue a = vm->pop();
    VmValue b = vm->pop();

    if (a.type == VM_VAL_FLOAT)
        a.iValue = ((int) a.fValue) ? 1 : 0;
    if (b.type == VM_VAL_FLOAT)
        b.iValue = ((int) b.fValue) ? 1 : 0;

    vm->pushInteger(a.iValue && b.iValue);
    return false;
}

//=============================================================================
// Operação: OU Booleano
//=============================================================================
bool VirtualMachine::opOr(VirtualMachine *vm)
{
    VmValue a = vm->pop();
    VmValue b = vm->pop();
    if (a.type == VM_VAL_FLOAT)
        a.iValue = (int) a.fValue;
    if (b.type == VM_VAL_FLOAT)
        b.iValue = (int) b.fValue;
    vm->pushInteger(a.iValue | b.iValue);
    return false;
}

//=============================================================================
// Operação: Exclusivo OU booleano
//=============================================================================
bool VirtualMachine::opXor(VirtualMachine *vm)
{
    VmValue a = vm->pop();
    VmValue b = vm->pop();
    if (a.type == VM_VAL_FLOAT)
        a.iValue = (int) a.fValue;
    if (b.type == VM_VAL_FLOAT)
        b.iValue = (int) b.fValue;
    vm->pushInteger(a.iValue ^ b.iValue);
    return false;
}

//=============================================================================
// Operação: E booleano
//=============================================================================
bool VirtualMachine::opAnd(VirtualMachine *vm)
{
    VmValue a = vm->pop();
    VmValue b = vm->pop();
    if (a.type == VM_VAL_FLOAT)
        a.iValue = (int) a.fValue;
    if (b.type == VM_VAL_FLOAT)
        b.iValue = (int) b.fValue;
    vm->pushInteger(a.iValue & b.iValue);
    return false;
}

//=============================================================================
// Operação: Negação booleana
//=============================================================================
bool VirtualMachine::opNot(VirtualMachine *vm)
{
    VmValue a = vm->pop();
    if (a.type == VM_VAL_FLOAT)
        a.iValue = (int) a.fValue;
    vm->pushInteger(~a.iValue);
    return false;
}

//=============================================================================
// Operação: igualdade
//=============================================================================
bool VirtualMachine::opEQ(VirtualMachine *vm)
{
    VmValue a = vm->pop();
    VmValue b = vm->pop();
    if (a.type == VM_VAL_FLOAT || b.type == VM_VAL_FLOAT)
    {
        float v1 = (a.type == VM_VAL_INTEGER) ? ((float) a.iValue) : a.fValue;
        float v2 = (b.type == VM_VAL_INTEGER) ? ((float) b.iValue) : b.fValue;
        vm->pushFloat(v2 == v1);
    }
    else if (a.type == VM_VAL_STRING && b.type == VM_VAL_STRING)
    {
        // cout << "cmp "<< a.strValue << ", " << b.strValue << endl;
        vm->pushInteger(!strcmp(a.strValue, b.strValue));
    }
    else
        vm->pushInteger(b.iValue == a.iValue);
    return false;
}

//=============================================================================
// Operação: desigualdade
//=============================================================================
bool VirtualMachine::opNE(VirtualMachine *vm)
{
    VmValue a = vm->pop();
    VmValue b = vm->pop();
    if (a.type == VM_VAL_FLOAT || b.type == VM_VAL_FLOAT)
    {
        float v1 = (a.type == VM_VAL_INTEGER) ? ((float) a.iValue) : a.fValue;
        float v2 = (b.type == VM_VAL_INTEGER) ? ((float) b.iValue) : b.fValue;
        vm->pushInteger(v2 != v1);
    }
    else
        vm->pushInteger(b.iValue != a.iValue);
    return false;
}

//=============================================================================
// Operação: maior que
//=============================================================================
bool VirtualMachine::opGT(VirtualMachine *vm)
{
    VmValue a = vm->pop();
    VmValue b = vm->pop();
    if (a.type == VM_VAL_FLOAT || b.type == VM_VAL_FLOAT)
    {
        float v1 = (a.type == VM_VAL_INTEGER) ? ((float) a.iValue) : a.fValue;
        float v2 = (b.type == VM_VAL_INTEGER) ? ((float) b.iValue) : b.fValue;
        vm->pushInteger(v2 > v1);
    }
    else
        vm->pushInteger(b.iValue > a.iValue);
    return false;
}

//=============================================================================
// Operação: maior ou igual que
//=============================================================================
bool VirtualMachine::opGE(VirtualMachine *vm)
{
    VmValue a = vm->pop();
    VmValue b = vm->pop();

    if (a.type == VM_VAL_FLOAT || b.type == VM_VAL_FLOAT)
    {

        float v1 = (a.type == VM_VAL_INTEGER) ? ((float) a.iValue) : a.fValue;
        float v2 = (b.type == VM_VAL_INTEGER) ? ((float) b.iValue) : b.fValue;
        // cout << v2 << ">=" << v1 << endl;
        vm->pushInteger(v2 >= v1);
    }
    else
        vm->pushInteger(b.iValue >= a.iValue);
    return false;
}

//=============================================================================
// Operação: menor que
//=============================================================================
bool VirtualMachine::opLT(VirtualMachine *vm)
{
    VmValue a = vm->pop();
    VmValue b = vm->pop();
    if (a.type == VM_VAL_FLOAT || b.type == VM_VAL_FLOAT)
    {
        float v1 = (a.type == VM_VAL_INTEGER) ? ((float) a.iValue) : a.fValue;
        float v2 = (b.type == VM_VAL_INTEGER) ? ((float) b.iValue) : b.fValue;
        vm->pushInteger(v2 < v1);
    }
    else
        vm->pushInteger(b.iValue < a.iValue);
    return false;
}

//=============================================================================
// Operação: menor ou igual que
//=============================================================================
bool VirtualMachine::opLE(VirtualMachine *vm)
{
    VmValue a = vm->pop();
    VmValue b = vm->pop();
    if (a.type == VM_VAL_FLOAT || b.type == VM_VAL_FLOAT)
    {
        float v1 = (a.type == VM_VAL_INTEGER) ? ((float) a.iValue) : a.fValue;
        float v2 = (b.type == VM_VAL_INTEGER) ? ((float) b.iValue) : b.fValue;
        vm->pushInteger(v2 <= v1);
    }
    else
        vm->pushInteger(b.iValue <= a.iValue);
    return false;
}

//=============================================================================
// Operação: cosseno
//=============================================================================
bool VirtualMachine::opCos(VirtualMachine *vm)
{
    VmValue a = vm->pop();
    if (a.type == VM_VAL_FLOAT)
        vm->pushFloat(cos(a.fValue));
    else
        vm->pushFloat(cos(a.iValue));
    return false;
}

//=============================================================================
// Operação: seno
//=============================================================================
bool VirtualMachine::opSin(VirtualMachine *vm)
{
    VmValue a = vm->pop();
    if (a.type == VM_VAL_FLOAT)
        vm->pushFloat(sin(a.fValue));
    else
        vm->pushFloat(sin(a.iValue));
    return false;
}

//=============================================================================
// Operação: tangente
//=============================================================================
bool VirtualMachine::opTan(VirtualMachine *vm)
{
    VmValue a = vm->pop();
    if (a.type == VM_VAL_FLOAT)
        vm->pushFloat(tan(a.fValue));
    else
        vm->pushFloat(tan(a.iValue));
    return false;
}

//=============================================================================
// Operação: arco cosseno
//=============================================================================
bool VirtualMachine::opAcos(VirtualMachine *vm)
{
    VmValue a = vm->pop();
    if (a.type == VM_VAL_FLOAT)
        vm->pushFloat(acos(a.fValue));
    else
        vm->pushFloat(acos(a.iValue));
    return false;
}

//=============================================================================
// Operação: arco seno
//=============================================================================
bool VirtualMachine::opAsin(VirtualMachine *vm)
{
    VmValue a = vm->pop();
    if (a.type == VM_VAL_FLOAT)
        vm->pushFloat(asin(a.fValue));
    else
        vm->pushFloat(asin(a.iValue));
    return false;
}

//=============================================================================
// Operação: arco tangente
//=============================================================================
bool VirtualMachine::opAtan(VirtualMachine *vm)
{
    VmValue a = vm->pop();
    if (a.type == VM_VAL_FLOAT)
        vm->pushFloat(atan(a.fValue));
    else
        vm->pushFloat(atan(a.iValue));
    return false;
}

//=============================================================================
// Operação: arredondar para cima
//=============================================================================
bool VirtualMachine::opCeil(VirtualMachine *vm)
{
    VmValue a = vm->pop();
    if (a.type == VM_VAL_FLOAT)
        vm->pushFloat(ceil(a.fValue));
    else
        vm->pushFloat(ceil(a.iValue));
    return false;
}

//=============================================================================
// Operação: arredondar para baixo
//=============================================================================
bool VirtualMachine::opFloor(VirtualMachine *vm)
{
    VmValue a = vm->pop();
    if (a.type == VM_VAL_FLOAT)
        vm->pushFloat(floor(a.fValue));
    else
        vm->pushFloat(floor(a.iValue));
    return false;
}

//=============================================================================
// Operação: PI
//=============================================================================
bool VirtualMachine::opPI(VirtualMachine *vm)
{
    vm->pushFloat(M_PI);
    return false;
}

//=============================================================================
// Operação: E
//=============================================================================
bool VirtualMachine::opE(VirtualMachine *vm)
{
    vm->pushFloat(M_E);
    return false;
}

//=============================================================================
// Operação: exponencial de E
//=============================================================================
bool VirtualMachine::opExp(VirtualMachine *vm)
{
    VmValue a = vm->pop();
    if (a.type == VM_VAL_FLOAT)
        vm->pushFloat(exp(a.fValue));
    else
        vm->pushFloat(exp(a.iValue));
    return false;
}

//=============================================================================
// Operação: valor absoluto
//=============================================================================
bool VirtualMachine::opAbs(VirtualMachine *vm)
{
    VmValue a = vm->pop();
    if (a.type == VM_VAL_FLOAT)
        vm->pushFloat(fabs(a.fValue));
    else
        vm->pushInteger(abs(a.iValue));
    return false;
}

//=============================================================================
// Operação: logaritmo natural
//=============================================================================
bool VirtualMachine::opLn(VirtualMachine *vm)
{
    VmValue a = vm->pop();
    if (a.type == VM_VAL_FLOAT)
        vm->pushFloat(log(a.fValue));
    else
        vm->pushFloat(log(a.iValue));
    return false;
}

//=============================================================================
// Operação: logaritmo na base 10
//=============================================================================
bool VirtualMachine::opLog(VirtualMachine *vm)
{
    VmValue a = vm->pop();
    if (a.type == VM_VAL_FLOAT)
        vm->pushFloat(log10(a.fValue));
    else
        vm->pushFloat(log10(a.iValue));
    return false;
}

//=============================================================================
// Retorna o número da animação atual
//=============================================================================
bool VirtualMachine::opAnim(VirtualMachine *vm)
{
    Char::Character *player = vm->player;
    vm->pushInteger(player->getAnimNo());
    return false;
}

//=============================================================================
// Retorna o tempo decorrido na animação atual
//=============================================================================
bool VirtualMachine::opAnimTime(VirtualMachine *vm)
{
    Char::Character *player = vm->player;
    vm->pushInteger(player->getAnimTime());
    return false;
}

//=============================================================================
// Retorna o tempo decorrido no frame atual
//=============================================================================
bool VirtualMachine::opAnimElemTime(VirtualMachine *vm)
{
    Char::Character *player = vm->player;
    VmValue a = vm->pop();
    if (a.type == VM_VAL_FLOAT)
        a.iValue = (int) a.fValue;
    if (player->getAnimFrameNo() == a.iValue)
    {
        vm->pushInteger(player->getAnimFrameTime());
    }
    else
        vm->pushInteger(-1);
    return false;
}

//=============================================================================
// Retorna o número do elemento atual
//=============================================================================
bool VirtualMachine::opAnimElemNo(VirtualMachine *vm)
{
    Char::Character *player = vm->player;
    vm->pop();
    vm->pushInteger(player->getAnimFrameNo());
    return false;
}

//=============================================================================
// Retorna se uma animação existe
//=============================================================================
bool VirtualMachine::opAnimExist(VirtualMachine *vm)
{
    Char::Character *player = vm->player;
    VmValue a = vm->pop();
    if (a.type == VM_VAL_FLOAT)
        a.iValue = (int) a.fValue;
    vm->pushInteger(player->hasAnim(a.iValue) ? 1 : 0);
    return false;
}

//=============================================================================
// Retorna o tempo decorrido no jogo
//=============================================================================
bool VirtualMachine::opTime(VirtualMachine *vm)
{
    Char::Character *player = vm->player;
    vm->pushInteger(player->getStateTime());
    return false;
}

//=============================================================================
// Retorna o comando atual
//=============================================================================
bool VirtualMachine::opCommand(VirtualMachine *vm)
{
    Char::Character *player = vm->player;
    VmValue cmdName = vm->pop();
    // erro na avaliação, faz a expressão retornar falso
    if (cmdName.type != VM_VAL_STRING)
        return true;
    int type = vm->thisOperation->iValue;
    if (type)
        vm->pushInteger(player->didCommand(cmdName.strValue));
    else
        vm->pushInteger(!player->didCommand(cmdName.strValue));
    return false;
}

//=============================================================================
// Retorna um número randomico entre 0 e 999
//=============================================================================
bool VirtualMachine::opRandom(VirtualMachine *vm)
{
    vm->pushInteger(rand() % 1000);
    return false;
}

//=============================================================================
// Retorna o número do estado atual
//=============================================================================
bool VirtualMachine::opStateNo(VirtualMachine *vm)
{
    Char::Character *player = vm->player;
    vm->pushInteger(player->getStateNumber());
    return false;
}

//=============================================================================
// Verifica o tipo do estado atual
//=============================================================================
bool VirtualMachine::opStateType(VirtualMachine *vm)
{
    Char::Character *player = vm->player;
    VmValue stype = vm->pop();
    // erro na avaliação, faz a expressão retornar falso
    if (stype.type != VM_VAL_INTEGER)
        return true;
    int type = vm->thisOperation->iValue;
    if (type)
        vm->pushInteger(player->getStateType() == stype.iValue);
    else
        vm->pushInteger(player->getStateType() != stype.iValue);
    return false;
}

//=============================================================================
// Retorna uma posição
//=============================================================================
bool VirtualMachine::opPos(VirtualMachine *vm)
{
    Char::Character *player = vm->player;
    int type = vm->thisOperation->iValue;
    // cout << "pos " << player->posY << endl;
    if (type)
        vm->pushFloat(player->getPosY());
    else {
        vm->pushFloat(player->getPosX());
    }

    return false;
}

//=============================================================================
// Retorna uma velocidade
//=============================================================================
bool VirtualMachine::opVel(VirtualMachine *vm)
{
    Char::Character *player = vm->player;
    int type = vm->thisOperation->iValue;
    // cout << "vel " << player->velY << endl;
    if (type)
        vm->pushFloat(player->getVelY());
    else
        vm->pushFloat(player->getVelX());
    return false;
}

bool VirtualMachine::opIfElse(VirtualMachine *vm)
{
    if (!vm->thisOperation->exprCond)
        return true;
    VmOperation *oper = vm->thisOperation;

    vm->execute(vm->thisOperation->exprCond, false);
    VmValue cond = vm->pop();
    if (cond.type == VM_VAL_FLOAT)
        cond.iValue = (int) cond.fValue;
    if (!oper->exprTrue || !oper->exprFalse) {
        return true;
    }

    if (cond.iValue)
        vm->execute(oper->exprTrue, false);
    else
        vm->execute(oper->exprFalse, false);
    return false;
}

//=============================================================================
// Retorna o tempo decorrido no jogo
//=============================================================================
bool VirtualMachine::opVar(VirtualMachine *vm)
{
    Char::Character *player = vm->player;

    VmValue var = vm->pop();
    if (var.type == VM_VAL_FLOAT)
        var.iValue = var.fValue;
    double  v = player->getVar(var.iValue);
    //cout << "Var(" << var.iValue << ") = " << v << endl;
    vm->pushFloat(v);
    return false;
}


}
