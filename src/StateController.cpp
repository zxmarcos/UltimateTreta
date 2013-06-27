#include "StateController.hpp"
#include "ExpressionCompiler.hpp"
#include "Util.hpp"
#include "Console.hpp"
#include <iostream>
#include "Character.hpp"

using namespace std;
namespace Stat
{

InitObjectCounter(StateController);

//=============================================================================
// STATECONTROLLER
//=============================================================================
StateController::StateController(int code_) : code(code_)
{
    IncrementObjectCounter();
}

StateController::~StateController()
{
    for (auto i : triggerAll)
        delete i;
    for (auto i : triggers)
        delete i;
    DecrementObjectCounter();
}

bool StateController::execute(Char::Character *player)
{
    return true;
}

//=============================================================================
// Verifica se o StateController pode ser ativado
//=============================================================================
bool StateController::triggered(Char::Character *player)
{
    bool triggerAllOk = true;
    VirtualMachine *vm = player->getVM();
    for (auto tr : triggerAll)
    {
        vm->execute(tr);
        if (!vm->iValue())
        {
            triggerAllOk = false;
            break;
        }
    }
    if (!triggerAllOk)
        return false;
    for (auto tr : triggers)
    {
        vm->execute(tr);
        if (vm->iValue())
        {
            return true;
        }
    }
    return false;
}

#define EvalInt(vm_, expr_, p_)     \
    if (expr_) {                    \
        (vm_)->execute(expr_);      \
        (p_) = (vm_)->iValue();     \
    }

#define EvalFloat(vm_, expr_, p_)   \
    if (expr_) {                    \
        (vm_)->execute(expr_);      \
        (p_) = (vm_)->fValue();     \
    }

#define EvalBool(vm_, expr_, p_)                \
    if (expr_) {                                \
        (vm_)->execute(expr_);                  \
        (p_) = (vm_)->iValue() ? true : false;  \
    }

//=============================================================================
// CHANGESTATE
//=============================================================================
ChangeState::ChangeState(int code) : StateController(code)
{
    value = NULL;
    ctrl = NULL;
}

ChangeState::~ChangeState()
{
    safeDelete(value);
    safeDelete(ctrl);
}

bool ChangeState::execute(Char::Character *player)
{
    if (!player)
        return false;

    VirtualMachine *vm = player->getVM();

    vm->execute(value);
    player->changeState(vm->iValue());
    return true;
}

//=============================================================================
// VELSET
//=============================================================================
VelSet::VelSet(int code) : StateController(code)
{
    x = NULL;
    y = NULL;
}

VelSet::~VelSet()
{
    safeDelete(x);
    safeDelete(y);
}

bool VelSet::execute(Char::Character *player)
{
    if (!player)
        return false;
    VirtualMachine *vm = player->getVM();
    if (x)
    {
        vm->execute(x);
        player->setVelX(vm->fValue());
    }
    if (y)
    {
        vm->execute(y);
        player->setVelY(vm->fValue());
    }
    return true;
}

//=============================================================================
// VELMUL
//=============================================================================
VelMul::VelMul(int code) : StateController(code)
{
    x = NULL;
    y = NULL;
}

VelMul::~VelMul()
{
    safeDelete(x);
    safeDelete(y);
}

bool VelMul::execute(Char::Character *player)
{
    if (!player)
        return false;
    VirtualMachine *vm = player->getVM();
    if (x)
    {
        vm->execute(x);
        player->setVelX(player->getVelX() * vm->fValue());
    }
    if (y)
    {
        vm->execute(y);
        player->setVelY(player->getVelY() * vm->fValue());
    }
    return true;
}

//=============================================================================
// VELADD
//=============================================================================
VelAdd::VelAdd(int code) : StateController(code)
{
    x = NULL;
    y = NULL;
}

VelAdd::~VelAdd()
{
    safeDelete(x);
    safeDelete(y);
}

bool VelAdd::execute(Char::Character *player)
{
    if (!player)
        return false;
    VirtualMachine *vm = player->getVM();
    if (x)
    {
        vm->execute(x);
        player->setVelX(player->getVelX() + vm->fValue());
    }
    if (y)
    {
        vm->execute(y);
        player->setVelY(player->getVelY() + vm->fValue());
    }
    return true;
}

//=============================================================================
// CHANGEANIM
//=============================================================================
ChangeAnim::ChangeAnim(int code) : StateController(code)
{
    value = NULL;
    elem = NULL;
}

ChangeAnim::~ChangeAnim()
{
    safeDelete(value);
    safeDelete(elem);
}

bool ChangeAnim::execute(Char::Character *player)
{
    if (!player)
        return false;
    VirtualMachine *vm = player->getVM();

    vm->execute(value);
    int iAnim = vm->iValue();
    int iElem = 0;
    if (elem)
    {
        vm->execute(elem);
        iElem = vm->iValue();
    }

    player->changeAnim(iAnim, iElem);
    return true;
}
//=============================================================================
// POSSET
//=============================================================================
PosSet::PosSet(int code) : StateController(code)
{
    x = NULL;
    y = NULL;
}

PosSet::~PosSet()
{
    safeDelete(x);
    safeDelete(y);
}

bool PosSet::execute(Char::Character *player)
{
    if (!player)
        return false;
    if (player->isFreezed())
        return true;

    VirtualMachine *vm = player->getVM();

    if (x)
    {
        vm->execute(x);
        player->setPosX(vm->fValue());
    }
    if (y)
    {
        vm->execute(y);
        player->setPosY(vm->fValue());
    }
    return true;
}
//=============================================================================
// POSADD
//=============================================================================
PosAdd::PosAdd(int code) : StateController(code)
{
    x = NULL;
    y = NULL;
}

PosAdd::~PosAdd()
{
    safeDelete(x);
    safeDelete(y);
}

bool PosAdd::execute(Char::Character *player)
{
    if (!player)
        return false;
    if (player->isFreezed())
        return true;

    VirtualMachine *vm = player->getVM();

    if (x)
    {
        vm->execute(x);
        player->setPosX(player->getPosX() + vm->fValue());
    }
    if (y)
    {
        vm->execute(y);
        player->setPosY(player->getPosY() + vm->fValue());
    }
    return true;
}

//=============================================================================
// POSFREEZE
//=============================================================================
PosFreeze::PosFreeze(int code) : StateController(code)
{
    value = NULL;
}

PosFreeze::~PosFreeze()
{
    safeDelete(value);
}

bool PosFreeze::execute(Char::Character *player)
{
    if (!player)
        return false;
    VirtualMachine *vm = player->getVM();
    vm->execute(value);
    player->setFreeze(vm->iValue() ? true : false);
    return true;
}

//=============================================================================
// HitDef
//=============================================================================
HitDef::HitDef(int code) : StateController(code)
{
    damage = NULL;
    power = NULL;
    stateno = NULL;
    p2stateno = NULL;
    shake.freq = NULL;
    shake.amplitude = NULL;
    shake.time = NULL;
    shake.phase = NULL;
    sound.group = NULL;
    sound.index = NULL;
    velx = NULL;
    vely = NULL;
    air.p2stateno = NULL;
    crouch.p2stateno = NULL;
    strength = NULL;
}

HitDef::~HitDef()
{
    safeDelete(damage);
    safeDelete(power);
    safeDelete(stateno);
    safeDelete(p2stateno);
    safeDelete(shake.freq);
    safeDelete(shake.amplitude);
    safeDelete(shake.phase);
    safeDelete(shake.time);
    safeDelete(sound.group);
    safeDelete(sound.index);
    safeDelete(velx);
    safeDelete(vely);
    safeDelete(air.p2stateno);
    safeDelete(crouch.p2stateno);
    safeDelete(strength);
}

bool HitDef::execute(Char::Character *player)
{
    if (!player)
        return false;
    VirtualMachine *vm = player->getVM();
    Char::HitInfo *info = new Char::HitInfo();
    if (!info)
        return true;
    EvalInt(vm, damage, info->damage);
    EvalInt(vm, power, info->power);
    EvalInt(vm, strength, info->strength);
    EvalFloat(vm, velx, info->velx);
    EvalFloat(vm, vely, info->vely);
    EvalFloat(vm, shake.freq, info->shake.freq);
    EvalFloat(vm, shake.amplitude, info->shake.amplitude);
    EvalFloat(vm, shake.phase, info->shake.phase);
    EvalInt(vm, shake.time, info->shake.time);
    EvalInt(vm, sound.group, info->sound.group);
    EvalInt(vm, sound.index, info->sound.index);
    info->sound.play = sound.group && sound.index;

    if (stateno)
    {
        EvalInt(vm, stateno, info->stateno)
    }
    else
    {
        info->stateno = -1;
    }

    if (p2stateno)
    {
        EvalInt(vm, p2stateno, info->p2stateno)
    }
    else
    {
        info->p2stateno = -1;
    }

    if (air.p2stateno)
    {
        EvalInt(vm, air.p2stateno, info->air.p2stateno)
    }
    else
    {
        info->air.p2stateno = -1;
    }

    if (crouch.p2stateno)
    {
        EvalInt(vm, crouch.p2stateno, info->crouch.p2stateno)
    }
    else
    {
        info->crouch.p2stateno = -1;
    }
    player->addHitDef(info);
    return true;
}

//=============================================================================
// Projectile
//=============================================================================
Projectile::Projectile(int code) : StateController(code)
{
    x = NULL;
    y = NULL;
    velX = NULL;
    velY = NULL;
    anim = NULL;
    hitAnim = NULL;
    shakeAmpl = NULL;
    shakeFreq = NULL;
    shakePhase = NULL;
    shakeTime = NULL;
    hitSoundGroup = NULL;
    hitSoundIndex = NULL;
}

Projectile::~Projectile()
{
    safeDelete(x);
    safeDelete(y);
    safeDelete(velX);
    safeDelete(velY);
    safeDelete(anim);
    safeDelete(hitAnim);
    safeDelete(shakeAmpl);
    safeDelete(shakeFreq);
    safeDelete(shakePhase);
    safeDelete(shakeTime);
    safeDelete(hitSoundGroup);
    safeDelete(hitSoundIndex);
}



bool Projectile::execute(Char::Character *player)
{
    Console::print("Adicionando projétil");
    if (!player)
        return false;
    VirtualMachine *vm = player->getVM();
    Char::ProjectileInfo pi;

    EvalFloat(vm, x, pi.x);
    EvalFloat(vm, y, pi.y);
    EvalFloat(vm, velX, pi.velX);
    EvalFloat(vm, velY, pi.velY);
    EvalInt(vm, anim, pi.anim);
    EvalInt(vm, hitAnim, pi.hit.anim);
    EvalFloat(vm, shakeAmpl, pi.hit.shake.amplitude);
    EvalFloat(vm, shakeFreq, pi.hit.shake.freq);
    EvalFloat(vm, shakePhase, pi.hit.shake.phase);
    EvalInt(vm, shakeTime, pi.hit.shake.time);
    EvalInt(vm, hitSoundGroup, pi.hit.sound.group);
    EvalInt(vm, hitSoundIndex, pi.hit.sound.index);
    if (hitSoundGroup && hitSoundIndex)
        pi.hit.sound.play = true;
    else pi.hit.sound.play = false;

    player->createProjectile(pi);
    return true;
}

//=============================================================================
// PlaySound
//=============================================================================
PlaySound::PlaySound(int code) : StateController(code)
{
    group = NULL;
    index = NULL;
}

PlaySound::~PlaySound()
{
    safeDelete(group);
    safeDelete(index);
}

bool PlaySound::execute(Char::Character *player)
{
    if (!player)
        return false;
    VirtualMachine *vm = player->getVM();
    int g = 0, i = 0;
    EvalInt(vm, group, g);
    EvalInt(vm, index, i);
    player->playSound(g, i);
    return true;
}

//=============================================================================
// VarSet
//=============================================================================
VarSet::VarSet(int code) : StateController(code)
{
    var = NULL;
    value = NULL;
}

VarSet::~VarSet()
{
    safeDelete(var);
    safeDelete(value);
}

bool VarSet::execute(Char::Character *player)
{
    if (!player)
        return false;
    VirtualMachine *vm = player->getVM();
    int i = 0;
    double v = 0;
    EvalInt(vm, var, i);
    EvalFloat(vm, value, v);
    player->setVar(i, v);
    return true;
}


};
