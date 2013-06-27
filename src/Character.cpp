#include <iostream>
#include <lua.hpp>
#include <cstdio>
#include <cstring>
#include <list>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include "Display.hpp"
#include "Character.hpp"
#include "Console.hpp"
#include "StateCompiler.hpp"
#include "Fight.hpp"
#include "InputManager.hpp"
#include "InputMachine.hpp"
#include "SpriteEffect.hpp"
#include "Sounds.hpp"
#include "Camera.hpp"
#include "Object.hpp"

using namespace std;
namespace Char
{

Character::Character(Fight& env) : Object(), fightEnviroment(env)
{
    isLoaded = false;
    luaVm = NULL;
    posX = 0;
    posY = 0;
    prevPosX = 0;
    prevPosY = 0;
    velX = 0;
    velY = 0;
    prevVelX = 0;
    prevVelY = 0;
    scaleX = 2.0f;
    scaleY = 2.0f;
    p2distX = 0;
    p2distY = 0;
    isPosFreezed = false;
    stateNumber = 0;
    time = 0;
    p2distX = 0;
    p2distY = 0;
    time = 0;
    ctrl = 0;
    physics = 0;
    xCoordRef = 0;
    yCoordRef = 0;
    drawPosX = 0;
    drawPosY = 0;
    stateNumber = 0;
    prevStateNumber = 0;
    inputNo = 0;
    sizeConst.groundBack = 20;
    sizeConst.groundFront = 20;
    stateCompiler = new Stat::StateCompiler();
    vm = new Stat::VirtualMachine(this);
    cmd = new Input::InputMachine(0);
    state = NULL;
    stateSm1 = NULL;
    charName = "";
    animator = NULL;
    facing = false;
    powerMax = 1000;
    power = powerMax;
    for (unsigned i = 0; i < n_max_vars; i++)
        vars[i] = 0;
}

Character::~Character()
{
    cout << "=============================================" << endl;
    cout << "   Liberando personagem..." << endl;
    cout << "=============================================" << endl;
    cout << "Liberando o personagem " << charName << endl;
    safeDelete(animator);
    safeDelete(vm);
    safeDelete(stateCompiler);
    safeDelete(cmd);
    lua_close(luaVm);
}

bool Character::open(string name)
{
    luaVm = luaL_newstate();
    Console::print("Character: carregando %s", name.c_str());
    cout << "=============================================" << endl;
    cout << "   Carregando personagem..." << endl;
    cout << "=============================================" << endl;
    cout << "Carregando " << name << "..." << endl;
    if (!luaVm)
    {
        Console::print("Character: Nao foi possivel criar a maquina virtual");
        cerr << "Nao foi possivel criar a maquina virtual Lua!" << endl;
        return false;
    }
    if (luaL_dofile(luaVm, name.c_str()) != LUA_OK)
    {
        Console::print("Character: erro no arquivo", name.c_str());
        cerr << "Nao foi possivel carregar o arquivo " << name.c_str() << endl;
        return false;

    }
    lua_pcall(luaVm, 0, 0, 0);

    lua_getglobal(luaVm, "states_file");
    const char *str = lua_tostring(luaVm, -1);
    if (!stateCompiler->open(string(str)))
    {
        lua_close(luaVm);
        return false;
    }
    lua_getglobal(luaVm, "sprite_file");
    str = lua_tostring(luaVm, -1);
    if (!sprites.open(string(str)))
    {
        lua_close(luaVm);
        return false;
    }

    lua_getglobal(luaVm, "animation_file");
    str = lua_tostring(luaVm, -1);
    if (!animations.open(string(str)))
    {
        sprites.close();
        lua_close(luaVm);
        return false;
    }
    animator = new Anim::Animator(animations);

    lua_getglobal(luaVm, "sounds_file");
    str = lua_tostring(luaVm, -1);
    sounds.open(string(str));

    animator = new Anim::Animator(animations);


    state = stateCompiler->get(0);
    if (!state)
    {
        cerr << "Nao existe estado 0!" << endl;
        exit(-1);
    }
    physics = state->physics;
    moveType = state->moveType;
    type = state->type;
    ctrl = 1;
    vm->execute(state->anim);
    animator->set(vm->iValue());

    stateSm1 = stateCompiler->get(-1);

    lua_getglobal(luaVm, "character_name");
    charName = string(lua_tostring(luaVm, -1));

    isLoaded = true;
    Console::print("Character: ok");
    cout << "Carregado com sucesso!" << endl;
    return true;
}

void Character::update()
{
    if (ctrl)
        // Primeiro verifica se comandos foram executados
        cmd->update();

    // Agora executa os estados paralelos
    if (stateSm1)
        stateSm1->execute(this);

    // Executa o estado atual do player
    // Atualiza as animações
    state->execute(this);
    animator->update();
    //if (stateNumber != prevStateNumber)
        time++;
}

void Character::drawBoxes(int xref, int yref)
{
    const vector<Col::CollisionBox> &d = animator->getDefenseBoxes(scaleX, scaleY, facing);
    Col::drawBoxes(d, xref, yref, al_map_rgba(0, 0, 255, 2));
    const vector<Col::CollisionBox> &a = animator->getAttackBoxes(scaleX, scaleY, facing);
    Col::drawBoxes(a, xref, yref, al_map_rgba(255, 0, 0, 2));
}


void Character::draw(int camx, int camy)
{
    // Para desenharmos
    const Spr::Sprite *spr = sprites.get(animator->group(), animator->index());
    double rx = camx + xCoordRef + posX * scaleX;
    double ry = camy + yCoordRef + posY * scaleY;

    double sprPosX;
    double sprPosY = (posY - animator->y()) * scaleY;
    if (facing)
        sprPosX = (posX + animator->x()) * scaleX;
    else
        sprPosX = (posX - animator->x()) * scaleX;

    Spr::Effect effect;
    effect.setAdditive(animator->additive());
    effect.setFlipVertical(animator->flipv());
    if (facing && animator->fliph())
        effect.setFlipHorizontal(false);
    else
        effect.setFlipHorizontal(animator->fliph() || facing);

    effect.setScaleX(scaleX);
    effect.setScaleY(scaleY);
    if (spr)
    {
        spr->draw(camx + xCoordRef + sprPosX, camy + (yCoordRef + sprPosY) , effect);
    }
    drawPosX = camx + xCoordRef + sprPosX;
    drawPosY = camy + yCoordRef + sprPosY;
    // desenha as caixas de colisão
    if (Display::showBoxes())
    {
        drawBoxes(rx, ry);

        al_draw_line(rx - sizeConst.groundBack * scaleX, ry, rx + sizeConst.groundFront * scaleX, ry, al_map_rgba(0,255,0, 10), 1);
        al_draw_line(rx, ry - 2, rx, ry + 3, al_map_rgba(0,255,0, 10), 1);
    }
}

int Character::getRightEdgeDistance()
{
    return Display::getWidth() - drawPosX;
}

int Character::getLeftEdgeDistance()
{
    return drawPosX;
}

int Character::getRightBodyEdgeDistance()
{
    return Display::getWidth() - drawPosX - sizeConst.groundFront;
}

int Character::getLeftBodyEdgeDistance()
{
    return drawPosX - sizeConst.groundBack;
}

void Character::drawDebugInfo(int ns)
{
    char buffer[1024];
    int drawPosY = ns * 400 + 4;
    Display::drawRectFill(0, drawPosY, 500, 80, al_map_rgba(10, 10, 10, 150));
    Display::drawText(SYS_FONT, al_map_rgb(255, 255, 255/2), 10, drawPosY, charName.c_str());
    drawPosY += Display::textLineHeight(SYS_FONT) + 1;

    sprintf(buffer, "FRAMES: %4d ACTION: %4d ELEM: %2d (%d) SPR: %3d,%2d ANIMTIME %d", Fight::getTime(),
            animator->number(), animator->frameNumber(), animator->frameTick(),
            animator->group(), animator->index(), animator->elapsed());
    Display::drawText(SYS_FONT, al_map_rgb(255, 255, 255), 10, drawPosY, buffer);
    drawPosY += Display::textLineHeight(SYS_FONT) + 1;
    sprintf(buffer, "STATENO: %4d TYPE: %d MOVE: %d PHYSICS: %d CTRL: %d STATETIME: %d", stateNumber,
            type, moveType, physics, ctrl, time);
    Display::drawText(SYS_FONT, al_map_rgb(255, 255, 255), 10, drawPosY, buffer);
    drawPosY += Display::textLineHeight(SYS_FONT) + 1;
    sprintf(buffer, "X: %04.2f Y: %04.2f VELX: %4.2f VELY: %4.2f FACE %d P2DIST: (%4.2f, %4.2f)", posX, posY, velX,
            velY, facing, p2distX, p2distY);
    Display::drawText(SYS_FONT, al_map_rgb(255, 255, 255), 10, drawPosY, buffer);
    drawPosY += Display::textLineHeight(SYS_FONT) + 1;

    sprintf(buffer, "BFUDabcxyz   EDGE: %3d, %3d  DX: %f", getLeftEdgeDistance(), getRightEdgeDistance(), getDeltaX());
    Display::drawText(SYS_FONT, al_map_rgb(255, 255, 255), 10, drawPosY, buffer);
    drawPosY += Display::textLineHeight(SYS_FONT) + 1;

    sprintf(buffer, "%d%d%d%d%d%d%d%d%d%d",
            Input::readSymbol(inputNo, Input::K_BACK),
            Input::readSymbol(inputNo, Input::K_FOWARD),
            Input::readSymbol(inputNo, Input::K_UP),
            Input::readSymbol(inputNo, Input::K_DOWN),
            Input::readSymbol(inputNo, Input::K_A),
            Input::readSymbol(inputNo, Input::K_B),
            Input::readSymbol(inputNo, Input::K_C),
            Input::readSymbol(inputNo, Input::K_X),
            Input::readSymbol(inputNo, Input::K_Y),
            Input::readSymbol(inputNo, Input::K_Z)
           );
    Display::drawText(SYS_FONT, al_map_rgb(255, 255, 255), 10, drawPosY, buffer);
    drawPosY += Display::textLineHeight(SYS_FONT) + 1;

    sprintf(buffer, "CX %3d, CY %3d", _CX, _CY);
    Display::drawText(SYS_FONT, al_map_rgb(255, 255, 255), 10, drawPosY, buffer);
    drawPosY += Display::textLineHeight(SYS_FONT) + 1;
}

void Character::setFacing(bool value)
{
    facing = value;

    Input::setPlayerInputFacing(inputNo, value);
}

void Character::setPlayerNumber(int no)
{
    inputNo = no;
    cmd->setInputNo(no);
}

void Character::gravityAccel()
{
    velY += 0.45;
}

bool Character::canTurn()
{
    switch (stateNumber)
    {
    case 0:
    case 2:
        // case 4:
        // case 5:
        return true;
    }
    return false;
}

int Character::getAnimNo() const
{
    return animator->number();
}

int Character::getAnimTime() const
{
    return animator->elapsed();
}

int Character::getAnimFrameNo() const
{
    return animator->frameNumber();
}
int Character::getAnimFrameTime() const
{
    return animator->frameTick();
}

void Character::changeAnim(int actionNo, int elem)
{
    animator->set(actionNo, elem);
}


const vector<Col::CollisionBox> Character::getAttackBoxes() const
{
    return animator->getAttackBoxes(scaleX, scaleY, facing);
}
const vector<Col::CollisionBox> Character::getDefenseBoxes() const
{
    return animator->getDefenseBoxes(scaleX, scaleY, facing);
}

const SizeConstants& Character::getSize() const
{
    return sizeConst;
}


bool Character::hasState(int stateNo)
{
    return stateCompiler->exist(stateNo);
}

bool Character::hasAnim(int actionNo)
{
    return animations.exist(actionNo);
}

void Character::changeState(int stateNo)
{
    if (!hasState(stateNo))
        return;
    prevStateNumber = stateNumber;
    const Stat::State *pstate = stateCompiler->get(stateNo);
    state = pstate;
    stateNumber = stateNo;
    type = state->type;
    physics = state->physics;
    moveType = state->moveType;
    time = 0;

    clearHitdefs();
    if (state->ctrl)
    {
        vm->execute(state->ctrl);
        ctrl = vm->iValue() ? true : false;
    }

    if (state->anim)
    {
        vm->execute(state->anim);
        changeAnim(vm->iValue());
    }

    if (state->velX)
    {
        vm->execute(state->velX);
        velX = vm->fValue();
    }

    if (state->velY)
    {
        vm->execute(state->velY);
        velY = vm->fValue();
    }
}

int Character::getStateNumber() const
{
    return stateNumber;
}

int Character::getStateTime() const
{
    return time;
}

int Character::getStateType() const
{
    return type;
}

void Character::setStateType(int value)
{
    type = value;
}

Stat::StateMoveType Character::getStateMoveType() const
{
    return moveType;
}

void Character::setStateMoveType(Stat::StateMoveType value)
{
    moveType = value;
}

double Character::getP2DistX() const
{
    return p2distX;
}

double Character::getP2DistY() const
{
    return p2distY;
}

void Character::setP2DistX(double value)
{
    p2distX = value;
}

void Character::setP2DistY(double value)
{
    p2distY = value;
}

Stat::VirtualMachine *Character::getVM()
{
    return vm;
}

bool Character::didCommand(const string str)
{
    return cmd->isCommandAsserted(str);
}

void Character::playSound(int group, int index)
{
    const Snd::Sample *sample = sounds.get(group, index);
    if (sample)
        sample->play();
}

void Character::setVar(unsigned var, double value)
{
    if (var >= n_max_vars)
        return;
    vars[var] = value;
}

double Character::getVar(int var)
{
    if ((size_t)var >= n_max_vars)
        return 0;
    return vars[var];
}

int Character::getPower()
{
    return power;
}
void Character::setPower(int po)
{
    power = po;
    if (power < 0)
        power = 0;
}

int Character::getPowerMax()
{
    return powerMax;
}
void Character::setPowerMax(int po)
{
    powerMax = po;
    if (powerMax <= 0)
        powerMax = 1;
}

void Character::addHitDef(HitInfo *info)
{
    if (info)
        hitDefs.push_back(info);
}

HitInfo *Character::getOneHit()
{
    if (hitDefs.empty())
        return NULL;
    HitInfo *hit = hitDefs.front();
    hitDefs.pop_front();
    return hit;
}

void Character::clearHitdefs()
{
    for (auto p : hitDefs)
        delete p;
    hitDefs.clear();
}

Projectile::Projectile(Fight& env,
                       Spr::SpriteLoader& spr_,
                       Anim::AnimLoader& anim_,
                       Character& owner_,
                       const ProjectileInfo& info_) :
    Object(), fightEnviroment(env), sprites(spr_), owner(owner_), animator(anim_), info(info_)
{
    animator.set(info.anim);
    setFacing(owner.getFacing());
    if (facing)
    {

        setPosX(-info.x + owner.getPosX());
    }
    else
    {
        setPosX(info.x + owner.getPosX());

    }
    setVelX(info.velX);
    setVelY(info.velY);

    setPosY(info.y + owner.getPosY());
    setScaleX(owner.getScaleX());
    setScaleY(owner.getScaleY());

    state = Alive;
    diesOnNextFrame = false;
    owner.getCoordReference(xCoordRef, yCoordRef);
}

Projectile::~Projectile()
{

}


void Character::createProjectile(const ProjectileInfo& info)
{
    Projectile *proj = new Projectile(fightEnviroment, sprites, animations, *this, info);
    if (proj)
        fightEnviroment.addProjectile(proj);
}

const vector<Col::CollisionBox> Projectile::getAttackBoxes() const
{
    return animator.getAttackBoxes(scaleX, scaleY, owner.getFacing());
}

Projectile::ProjectileState Projectile::getState() const
{
    return state;
}

void Projectile::update()
{
    if (diesOnNextFrame) {
        state = Dead;
        return;
    }
    if (animator.isLastTick() && state == Explod)
        diesOnNextFrame = true;
    if (drawPosX >= 700 || drawPosX <= -100)
        diesOnNextFrame = true;

    animator.update();
}

bool Projectile::isOwner(Character &c)
{
    if (&c == &owner)
        return true;
    return false;
}

Character& Projectile::getOwner()
{
    return owner;
}

void Projectile::hit()
{
    animator.set(info.hit.anim);
    setVelX(0);
    setVelY(0);
    state = Explod;
    fightEnviroment.addShakeAmpl(info.hit.shake.amplitude);
    fightEnviroment.addShakeFreq(info.hit.shake.freq);
    fightEnviroment.addShakeTime(info.hit.shake.time);
    fightEnviroment.addShakePhase(info.hit.shake.phase);
    if (info.hit.sound.play)
        owner.playSound(info.hit.sound.group, info.hit.sound.index);
}

void Projectile::draw(int camx, int camy)
{
    // Para desenharmos
    const Spr::Sprite *spr = sprites.get(animator.group(), animator.index());
    double rx = camx + xCoordRef + posX * scaleX;
    double ry = camy + yCoordRef + posY * scaleY;

    double sprPosX;
    double sprPosY = (posY - animator.y()) * scaleY;
    if (facing)
        sprPosX = (posX + animator.x()) * scaleX;
    else
        sprPosX = (posX - animator.x()) * scaleX;

    Spr::Effect effect;
    effect.setAdditive(animator.additive());
    effect.setFlipVertical(animator.flipv());
    if (facing && animator.fliph())
        effect.setFlipHorizontal(false);
    else
        effect.setFlipHorizontal(animator.fliph() || facing);

    effect.setScaleX(scaleX);
    effect.setScaleY(scaleY);
    if (spr)
    {
        spr->draw(camx + xCoordRef + sprPosX, camy + (yCoordRef + sprPosY) , effect);
    }
    drawPosX = camx + xCoordRef + sprPosX;
    drawPosY = camy + yCoordRef + sprPosY;

    if (Display::showBoxes())
        drawBoxes(rx, ry);
}


void Projectile::drawBoxes(int xref, int yref)
{
    const vector<Col::CollisionBox> &a = animator.getAttackBoxes(scaleX, scaleY, facing);
    Col::drawBoxes(a, xref, yref, al_map_rgba(255, 0, 0, 2));
}

}
