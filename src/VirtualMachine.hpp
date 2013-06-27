#ifndef __VIRTUALMACHINE_HPP__
#define __VIRTUALMACHINE_HPP__


#include <vector>
#include <utility>
#include <cstdlib>

using namespace std;
#include "ExpressionCompiler.hpp"
namespace Char
{
class Character;
};


namespace Stat
{

class VirtualMachine;
class ExpressionValue;


enum VmOperationCode
{
    OP_PUSH = 0,
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_POW,
    OP_MOD,
    OP_NEG,
    OP_ASSIGN,
    OP_LOG_NEG,
    OP_LOG_OR,
    OP_LOG_XOR,
    OP_LOG_AND,
    OP_OR,
    OP_XOR,
    OP_AND,
    OP_NOT,
    OP_EQ,
    OP_NE,
    OP_GT,
    OP_GE,
    OP_LT,
    OP_LE,
    OP_ACOS,
    OP_ASIN,
    OP_ATAN,
    OP_COS,
    OP_SIN,
    OP_TAN,
    OP_ABS,
    OP_CEIL,
    OP_FLOOR,
    OP_E,
    OP_EXP,
    OP_LN,
    OP_LOG,
    OP_PI,
    OP_ANIM,
    OP_ANIMELEMNO,
    OP_ANIMELEMTIME,
    OP_ANIMEXIST,
    OP_ANIMTIME,
    OP_TIME,
    OP_COMMAND,
    OP_RANDOM,
    OP_STATENO,
    OP_STATETYPE,
    OP_POS,
    OP_VEL,
    OP_IFELSE,
    OP_VAR,
    VM_OP_MAX
};

enum VmOperationCodeArgType
{
    OP_ARG_NULL = 0,
    OP_ARG_INTEGER,
    OP_ARG_FLOAT,
    OP_ARG_STRING,
    OP_ARG_IFELSE,
};
enum VmValueType
{
    VM_VAL_INTEGER = 0,
    VM_VAL_FLOAT,
    VM_VAL_STRING,
};

//
struct VmOperation
{
    unsigned short type;
    unsigned char argType;
    union
    {
        int iValue;
        float fValue;
        char *strValue;
    };
    ExpressionValue *exprTrue;
    ExpressionValue *exprCond;
    ExpressionValue *exprFalse;
    VmOperation() {
        type = VM_VAL_INTEGER;
        iValue = 0;
        fValue = 0;
        strValue = NULL;
        exprTrue = NULL;
        exprFalse = NULL;
        exprCond = NULL;
    };
    ~VmOperation() {
        if (type == VM_VAL_STRING) {
            free(strValue);
        } else if (argType == OP_ARG_IFELSE) {
            safeDelete(exprTrue);
            safeDelete(exprCond);
            safeDelete(exprFalse);
        }
    };
};

struct VmValue
{
    unsigned char type;
    union
    {
        int iValue;
        float fValue;
        const char *strValue;
    };
    VmValue() {
        iValue = 0;
        fValue = 0;
        strValue = NULL;
    };
};



typedef bool (*VmOperationCallback)(VirtualMachine *);

// Máquina virtual que processa as informações
class VirtualMachine
{
public:
    VirtualMachine(Char::Character *player_);
    ~VirtualMachine();
    bool execute(ExpressionValue *expr, bool clearStack = true);
    int iValue();
    float fValue();
private:
    Char::Character *player;
    VmValue pop();
    void pushString(const string& value);
    void pushInteger(int value);
    void pushFloat(float value);
    vector<VmValue> stack;
    VmOperation *thisOperation;
    static bool opPush(VirtualMachine *vm);
    static bool opAdd(VirtualMachine *vm);
    static bool opSub(VirtualMachine *vm);
    static bool opMul(VirtualMachine *vm);
    static bool opDiv(VirtualMachine *vm);
    static bool opPow(VirtualMachine *vm);
    static bool opMod(VirtualMachine *vm);
    static bool opNeg(VirtualMachine *vm);
    static bool opAssign(VirtualMachine *vm);
    static bool opLogicalNeg(VirtualMachine *vm);
    static bool opLogicalOr(VirtualMachine *vm);
    static bool opLogicalXor(VirtualMachine *vm);
    static bool opLogicalAnd(VirtualMachine *vm);
    static bool opOr(VirtualMachine *vm);
    static bool opXor(VirtualMachine *vm);
    static bool opAnd(VirtualMachine *vm);
    static bool opNot(VirtualMachine *vm);
    static bool opEQ(VirtualMachine *vm);
    static bool opNE(VirtualMachine *vm);
    static bool opGT(VirtualMachine *vm);
    static bool opGE(VirtualMachine *vm);
    static bool opLT(VirtualMachine *vm);
    static bool opLE(VirtualMachine *vm);
    static bool opCos(VirtualMachine *vm);
    static bool opSin(VirtualMachine *vm);
    static bool opTan(VirtualMachine *vm);
    static bool opAcos(VirtualMachine *vm);
    static bool opAsin(VirtualMachine *vm);
    static bool opAtan(VirtualMachine *vm);
    static bool opCeil(VirtualMachine *vm);
    static bool opFloor(VirtualMachine *vm);
    static bool opPI(VirtualMachine *vm);
    static bool opE(VirtualMachine *vm);
    static bool opExp(VirtualMachine *vm);
    static bool opAbs(VirtualMachine *vm);
    static bool opLn(VirtualMachine *vm);
    static bool opLog(VirtualMachine *vm);
    static bool opAnim(VirtualMachine *vm);
    static bool opAnimTime(VirtualMachine *vm);
    static bool opAnimElemTime(VirtualMachine *vm);
    static bool opAnimElemNo(VirtualMachine *vm);
    static bool opAnimExist(VirtualMachine *vm);
    static bool opTime(VirtualMachine *vm);
    static bool opCommand(VirtualMachine *vm);
    static bool opRandom(VirtualMachine *vm);
    static bool opStateNo(VirtualMachine *vm);
    static bool opStateType(VirtualMachine *vm);
    static bool opPos(VirtualMachine *vm);
    static bool opVel(VirtualMachine *vm);
    static bool opIfElse(VirtualMachine *vm);
    static bool opVar(VirtualMachine *vm);
    static const VmOperationCallback opTable[VM_OP_MAX];
};

}

#endif // __VIRTUALMACHINE_HPP__
