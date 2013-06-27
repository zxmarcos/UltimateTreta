//=============================================================================
// Compilador de expressões
// Esse compilador inteiro é um trabalho braçal e mental enorme.
// A melhor coisa dessa implementação é eu ter aprendido como utilizar
// métodos de classes como callbacks, uma sintaxe estranha que eu ainda
// não conhecia e que serviu perfeitamente ao propósito. Além de utilizar
// o novíssimo unordered_map (hash table) do padrão C++0x
//=============================================================================
#include <string>
#include <iostream>
#include <cstdlib>
#include <unordered_map>
#include <utility>
#include <cstring>
#include "ExpressionCompiler.hpp"
#include "VirtualMachine.hpp"
#include "StateCompiler.hpp"
#include "Util.hpp"

using namespace std;

namespace Stat
{

InitObjectCounter(ExpressionValue);

//=============================================================================
// Tabela de avaliação para triggers
//=============================================================================
const unordered_map<string, TriggerAvailPair> ExpressionCompiler::triggerAvailTable =
{
    {"acos",			make_pair(OP_ACOS,				&ExpressionCompiler::availTrigger1)	},
    {"asin",			make_pair(OP_ASIN,				&ExpressionCompiler::availTrigger1)	},
    {"atan",			make_pair(OP_ATAN,				&ExpressionCompiler::availTrigger1)	},
    {"cos",				make_pair(OP_COS,				&ExpressionCompiler::availTrigger1) },
    {"sin",				make_pair(OP_SIN,				&ExpressionCompiler::availTrigger1) },
    {"tan",				make_pair(OP_TAN,				&ExpressionCompiler::availTrigger1) },
    {"floor",			make_pair(OP_FLOOR,				&ExpressionCompiler::availTrigger1) },
    {"ceil",			make_pair(OP_CEIL,				&ExpressionCompiler::availTrigger1) },
    {"abs",				make_pair(OP_ABS,				&ExpressionCompiler::availTrigger1) },
    {"exp",				make_pair(OP_EXP,				&ExpressionCompiler::availTrigger1) },
    {"pi",				make_pair(OP_PI,				&ExpressionCompiler::availTrigger0) },
    {"e",				make_pair(OP_E,					&ExpressionCompiler::availTrigger0) },
    {"time",			make_pair(OP_TIME,				&ExpressionCompiler::availTrigger0) },
    {"anim",			make_pair(OP_ANIM,				&ExpressionCompiler::availTrigger0) },
    {"animelemno",		make_pair(OP_ANIMELEMNO,		&ExpressionCompiler::availTrigger1) },
    {"animelemtime",	make_pair(OP_ANIMELEMTIME,		&ExpressionCompiler::availTrigger1) },
    {"animexist",		make_pair(OP_ANIMEXIST, 		&ExpressionCompiler::availTrigger1) },
    {"animtime",        make_pair(OP_ANIMTIME,          &ExpressionCompiler::availTrigger0) },
    {"command",         make_pair(OP_COMMAND,           &ExpressionCompiler::availTriggerCmd) },
    {"random",          make_pair(OP_RANDOM,            &ExpressionCompiler::availTrigger0) },
    {"stateno",         make_pair(OP_STATENO,           &ExpressionCompiler::availTrigger0) },
    {"statetype",       make_pair(OP_STATETYPE,         &ExpressionCompiler::availTriggerStateType) },
    {"pos",             make_pair(OP_POS,               &ExpressionCompiler::availTriggerPosVel) },
    {"vel",             make_pair(OP_VEL,               &ExpressionCompiler::availTriggerPosVel) },
    {"ifelse",          make_pair(OP_IFELSE,			&ExpressionCompiler::availTriggerIfElse) },
    {"var",             make_pair(OP_VAR,   			&ExpressionCompiler::availTrigger1) },
};

//=============================================================================
// ctor
//=============================================================================
ExpressionCompiler::ExpressionCompiler()
{
}

//=============================================================================
// dtor
//=============================================================================
ExpressionCompiler::~ExpressionCompiler()
{
}

//=============================================================================
// Compila uma expressão
//=============================================================================
ExpressionValue *ExpressionCompiler::compile(string str)
{
    ExpressionValue *cmd = new ExpressionValue();
    expr = cmd;
    // cout << "Compilando: " << str << endl;
    lex.setLineBuf(str);
    availStage0();
    return cmd;
}

//=============================================================================
// Avalia um trigger que não recebe nenhum parametro
//=============================================================================
void ExpressionCompiler::availTrigger0(int opcode)
{
    emit(opcode);
}

//=============================================================================
// Avalia um trigger que recebe uma expressão de parametro
//=============================================================================
void ExpressionCompiler::availTrigger1(int opcode)
{
    if (lex.check("("))
    {
        availStage0();
        lex.check(")");
        emit(opcode);
    }
}

void ExpressionCompiler::availTriggerCmd(int opcode)
{
    int type = 0;
    if (lex.check("="))
        type = 1;
    else if (lex.check("!="))
        type = 0;
    else goto _error;
    if (!lex.checkLiteral())
        goto _error;

    emit(OP_PUSH, lex.tok());
    emit(opcode, type);
    return;
_error:
    cout << "Erro fatal: avaliando trigger command" << endl;
    cout << "sintaxe: command (=, !=) \"commandname\"" << endl;
    exit(-1);
}

void ExpressionCompiler::availTriggerStateType(int opcode)
{
    int type = 0;
    int stype = STATE_TYPE_STAND;
    if (lex.check("="))
        type = 1;
    else if (lex.check("!="))
        type = 0;
    else goto _error;

    if (lex.check("S"))
        stype = STATE_TYPE_STAND;
    else if (lex.check("C"))
        stype = STATE_TYPE_CROUCH;
    else if (lex.check("A"))
        stype = STATE_TYPE_AIR;
    else
        goto _error;

    emit(OP_PUSH, stype);
    emit(opcode, type);
    return;
_error:
    cout << "Erro fatal: avaliando trigger statetype" << endl;
    cout << "sintaxe: statetype (=, !=) (A, C, S)" << endl;
    exit(-1);
}

void ExpressionCompiler::availTriggerPosVel(int opcode)
{
    int type = 0; // x
    if (lex.check("x"))
        type = 0;
    else if (lex.check("y"))
        type = 1;
    else
        goto _error;
    // cout << "Avaliando vel/pos" << endl;
    emit(opcode, type);
    return;
_error:
    cout << "Erro fatal: avaliando trigger (pos/vel)" << endl;
    cout << "sintaxe: (pos/vel) (x, y)" << endl;
    exit(-1);
}

void ExpressionCompiler::availTriggerIfElse(int opcode)
{
    if (lex.check("("))
    {
        ExpressionValue *exprOriginal = expr;
        ExpressionValue *exprCond = new ExpressionValue();
        ExpressionValue *exprTrue = new ExpressionValue();
        ExpressionValue *exprFalse = new ExpressionValue();
        expr = exprCond;
        availStage0();
        if (!lex.check(",")) {
            goto _error;
        }

        expr = exprTrue;
        availStage0();
        if (!lex.check(",")) {
            goto _error;
        }
        //expr->dasm();

        expr = exprFalse;
        availStage0();
        if (!lex.check(")")) {
            goto _error;
        }
        //expr->dasm();
        expr = exprOriginal;
        emit(opcode, exprCond, exprTrue, exprFalse);
    }
    return;
_error:
    cout << "Erro fatal: avaliando trigger ifelse" << endl;
    cout << "sintaxe: ifelse (expr_cond, true_expr, false_expr)" << endl;
    exit(-1);
}

//=============================================================================
// Avalia o Ou Lógico
//=============================================================================
void ExpressionCompiler::availStage0()
{
    availStage1();
    if (lex.check("||"))
    {
        availStage0();
        emit(OP_LOG_OR);
    }
    else
        return;
}

//=============================================================================
// Avalia o Ou Exclusivo Lógico
//=============================================================================
void ExpressionCompiler::availStage1()
{
    availStage2();
    if (lex.check("^^"))
    {
        availStage1();
        emit(OP_LOG_XOR);
    }
    else
        return;
}

//=============================================================================
// Avalia o E Lógico
//=============================================================================
void ExpressionCompiler::availStage2()
{
    availStage3();
    if (lex.check("&&"))
    {
        availStage2();
        emit(OP_LOG_AND);
    }
    else
        return;
}

//=============================================================================
// Avalia o bit OR
//=============================================================================
void ExpressionCompiler::availStage3()
{
    availStage4();
    if (lex.check("|"))
    {
        availStage3();
        emit(OP_OR);
    }
    else
        return;
}

//=============================================================================
// Avalia o bit XOR
//=============================================================================
void ExpressionCompiler::availStage4()
{
    availStage5();
    if (lex.check("^"))
    {
        availStage4();
        emit(OP_XOR);
    }
    else
        return;
}

//=============================================================================
// Avalia o bit AND
//=============================================================================
void ExpressionCompiler::availStage5()
{
    availStage6();
    if (lex.check("&"))
    {
        availStage5();
        emit(OP_AND);
    }
    else
        return;
}

//=============================================================================
// ...
//=============================================================================
void ExpressionCompiler::availStage6()
{
    availStage7();
    if (lex.check(":="))
    {
        availStage6();
        emit(OP_ASSIGN);
    }
    else
        return;
}

//=============================================================================
// Avalia igualdade e desigualdade
//=============================================================================
void ExpressionCompiler::availStage7()
{
    availStage8();
    if (lex.check("="))
    {
        availStage7();
        emit(OP_EQ);
    }
    else if (lex.check("!="))
    {
        availStage7();
        emit(OP_NE);
    }
    else
        return;

    // Intervalos
}

//=============================================================================
// Avalia comparações
//=============================================================================
void ExpressionCompiler::availStage8()
{
    availStage9();
    if (lex.check(">"))
    {
        availStage8();
        emit(OP_GT);
    }
    else if (lex.check(">="))
    {
        availStage8();
        emit(OP_GE);
    }
    else if (lex.check("<"))
    {
        availStage8();
        emit(OP_LT);
    }
    else if (lex.check("<="))
    {
        availStage8();
        emit(OP_LE);
    }
    else
        return;
}

//=============================================================================
// Avalia soma e subtração
//=============================================================================
void ExpressionCompiler::availStage9()
{
    availStage10();
    if (lex.check("+"))
    {
        availStage9();
        emit(OP_ADD);
    }
    else if (lex.check("-"))
    {
        availStage9();
        emit(OP_SUB);
    }
    else
        return;
}

//=============================================================================
// Avalia multiplicação, divisão e módulo de divisão
//=============================================================================
void ExpressionCompiler::availStage10()
{
    availStage11();
    if (lex.check("*"))
    {
        availStage10();
        emit(OP_MUL);
    }
    else if (lex.check("/"))
    {
        availStage10();
        emit(OP_DIV);
    }
    else if (lex.check("%"))
    {
        availStage10();
        emit(OP_MOD);
    }
    else
        return;
}

//=============================================================================
// Avalia a exponenciação
//=============================================================================
void ExpressionCompiler::availStage11()
{
    availStage12();
    if (lex.check("**"))
    {
        availStage11();
        emit(OP_POW);
    }
    else
        return;
}

//=============================================================================
// Avalia os unários
//=============================================================================
void ExpressionCompiler::availStage12()
{
#if 1
    if (lex.check("-"))
    {
        availStage12();
        emit(OP_NEG);
    }
    else if (lex.check("+"))
    {
        availStage12();
    }
    else
#endif
        if (lex.check("!"))
        {
            availStage12();
            emit(OP_LOG_NEG);
        }
        else if (lex.check("~"))
        {
            availStage12();
            emit(OP_NOT);
        }
        else availStage13();
}

//=============================================================================
// Avalia triggers, subexpressões e números, nosso átomos
//=============================================================================
void ExpressionCompiler::availStage13()
{
    if (lex.checkNumber())
    {
        if (lex.isFloatNumber())
            emit(OP_PUSH, lex.toFloat());
        else
            emit(OP_PUSH, lex.toInt());
    }
    else if (lex.checkLiteral())
    {
        emit(OP_PUSH, lex.tok());
    }
    else if (lex.check("("))
    {
        availStage0();
        lex.check(")");
    }
    else
    {
        // verifica se é um trigger
        auto ptr = triggerAvailTable.find(lex.tok());
        if (ptr != triggerAvailTable.end())
        {
            // precisamos consumir o token já que ele é válido
            lex.consumeToken();

            TriggerAvailPair mp = (*ptr).second;
            TriggerAvailCallback call = mp.second;
            int opcode = mp.first;
            (this->*call)(opcode);
        }
    }
}

//=============================================================================
// Emit um opcode sem argumentos
//=============================================================================
void ExpressionCompiler::emit(int op)
{
    VmOperation *cmd = new VmOperation();
    cmd->type = op;
    cmd->argType = OP_ARG_NULL;
    expr->stream.push_back(cmd);
}

//=============================================================================
// Emit um opcode com argumento inteiro
//=============================================================================
void ExpressionCompiler::emit(int op, int arg)
{
    VmOperation *cmd = new VmOperation();
    cmd->type = op;
    cmd->argType = OP_ARG_INTEGER;
    cmd->iValue = arg;
    expr->stream.push_back(cmd);
}

//=============================================================================
// Emit um opcode com argumento float
//=============================================================================
void ExpressionCompiler::emit(int op, float arg)
{
    VmOperation *cmd = new VmOperation();
    cmd->type = op;
    cmd->argType = OP_ARG_FLOAT;
    cmd->fValue = arg;
    expr->stream.push_back(cmd);
}

//=============================================================================
// Emit um opcode com argumento inteiro
//=============================================================================
void ExpressionCompiler::emit(int op, string arg)
{
    VmOperation *cmd = new VmOperation();
    cmd->type = op;
    cmd->argType = OP_ARG_STRING;
    cmd->strValue = strdup(arg.c_str()); // converte para o const char*
    expr->stream.push_back(cmd);
}

void ExpressionCompiler::emit(int op, ExpressionValue *cond, ExpressionValue *iftrue, ExpressionValue *iffalse)
{
    VmOperation *cmd = new VmOperation();
    cmd->type = op;
    cmd->argType = OP_ARG_IFELSE;
    cmd->exprTrue = iftrue;
    cmd->exprFalse = iffalse;
    cmd->exprCond = cond;
    expr->stream.push_back(cmd);
}

void ExpressionValue::dasm()
{
    for (auto op : stream)
    {
        switch (op->type)
        {
        case OP_PUSH:
            if (op->argType == OP_ARG_FLOAT)
                cout << "push " << op->fValue;
            else if (op->argType == OP_ARG_INTEGER)
                cout << "push " << op->iValue;
            else if (op->argType == OP_ARG_STRING)
                cout << "push " << op->strValue;
            break;
        case OP_ADD:
            cout << "add";
            break;
        case OP_SUB:
            cout << "sub";
            break;
        case OP_MUL:
            cout << "mul";
            break;
        case OP_DIV:
            cout << "div";
            break;
        case OP_POW:
            cout << "pow";
            break;
        case OP_MOD:
            cout << "mod";
            break;
        case OP_NEG:
            cout << "neg";
            break;
        case OP_ASSIGN:
            cout << "assign";
            break;
        case OP_LOG_NEG:
            cout << "log_neg";
            break;
        case OP_LOG_OR:
            cout << "log_or";
            break;
        case OP_LOG_XOR:
            cout << "log_xor";
            break;
        case OP_LOG_AND:
            cout << "log_and";
            break;
        case OP_OR:
            cout << "or";
            break;
        case OP_XOR:
            cout << "xor";
            break;
        case OP_AND:
            cout << "and";
            break;
        case OP_NOT:
            cout << "not";
            break;
        case OP_EQ:
            cout << "eq";
            break;
        case OP_NE:
            cout << "ne";
            break;
        case OP_GT:
            cout << "gt";
            break;
        case OP_GE:
            cout << "ge";
            break;
        case OP_LT:
            cout << "lt";
            break;
        case OP_LE:
            cout << "le";
            break;
        case OP_ACOS:
            cout << "acos";
            break;
        case OP_ASIN:
            cout << "asin";
            break;
        case OP_ATAN:
            cout << "atan";
            break;
        case OP_COS:
            cout << "cos";
            break;
        case OP_SIN:
            cout << "sin";
            break;
        case OP_TAN:
            cout << "tan";
            break;
        case OP_ABS:
            cout << "abs";
            break;
        case OP_CEIL:
            cout << "ceil";
            break;
        case OP_FLOOR:
            cout << "floor";
            break;
        case OP_E:
            cout << "e";
            break;
        case OP_EXP:
            cout << "exp";
            break;
        case OP_LN:
            cout << "ln";
            break;
        case OP_LOG:
            cout << "log";
            break;
        case OP_PI:
            cout << "pi";
            break;
        case OP_ANIM:
            cout << "anim";
            break;
        case OP_ANIMELEMNO:
            cout << "animElemNo";
            break;
        case OP_ANIMELEMTIME:
            cout << "animElemtime";
            break;
        case OP_ANIMEXIST:
            cout << "animExist";
            break;
        case OP_ANIMTIME:
            cout << "animTime";
            break;
        case OP_TIME:
            cout << "time";
            break;
        case OP_COMMAND:
            cout << "command";
            break;
        case OP_RANDOM:
            cout << "random";
            break;
        case OP_STATENO:
            cout << "stateno";
            break;
        case OP_STATETYPE:
            cout << "statetype";
            break;
        case OP_POS:
            cout << "pos " << (op->iValue ? "y" : "x");
            break;
        case OP_VEL:
            cout << "vel " << (op->iValue ? "y" : "x");
            break;
        case OP_IFELSE:
            cout << "if:" << endl;
            op->exprTrue->dasm();
            cout << "else:" << endl;
            op->exprFalse->dasm();
            break;
        default:
            cout << "operacao desconhecida";
            break;
        }
        cout << endl;
    }

}

ExpressionValue::ExpressionValue()
{
    IncrementObjectCounter();
    stream.clear();
}

ExpressionValue::~ExpressionValue()
{
    for (auto i : stream)
        delete i;
    DecrementObjectCounter();

}





}
