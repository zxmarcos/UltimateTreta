//==============================================================================
// Lógica do sistema de lutas
// Marcos Medeiros
//==============================================================================
#include <cmath>
#include <string>
#include <sstream>
#include "Fight.hpp"
#include "StateCompiler.hpp"
#include "Display.hpp"
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include "Console.hpp"
#include "StateCompiler.hpp"
#include "Character.hpp"
#include "Game.hpp"

using namespace std;


const int defaultMatchTime = 60;
int Fight::time = 0;

int Fight::getTime()
{
    return time;
}

Fight::Fight(Game& env) : gameEnviroment(env)
{
    isLoaded = false;
    stage = NULL;
    player1 = NULL;
    player2 = NULL;
}

Fight::~Fight()
{
    safeDelete(player1);
    safeDelete(player2);
    safeDelete(stage);
    for (auto proj : projectiles)
    {
        delete proj;
    }
}

bool Fight::load(string char1, string char2, string stagef)
{
    safeDelete(player1);
    safeDelete(player2);
    safeDelete(stage);

    player1 = new Char::Character(*this);
    if (!player1->open(char1))
    {
        isLoaded = false;
        return false;
    }

    player2 = new Char::Character(*this);
    if (!player2->open(char2))
    {
        delete player1;
        isLoaded = false;
        return false;
    }

    stage = new Stage::StageLoader();

    if (!stage->open(stagef))
    {
        safeDelete(player1);
        safeDelete(player2);
        isLoaded = false;
        return false;
    }
    player1->setPos(stage->getP1StartX(), 0, false);
    player2->setPos(stage->getP2StartX(), 0, false);
    player1->setCoordReference(320, stage->getGroundOffset() * 2);
    player2->setCoordReference(320, stage->getGroundOffset() * 2);
    player1->setPlayerNumber(0);
    player1->setFacing(false);
    player2->setPlayerNumber(1);
    player2->setFacing(true);
    camera.setPlayers(player1, player2);
    camera.setConfig(stage->getCamera());
    isLoaded = true;
    matchTime = defaultMatchTime;
    return true;
}

void Fight::roundUp()
{
    round++;
    stage->reset();
    player1->setPos(stage->getP1StartX(), 0, false);
    player2->setPos(stage->getP2StartX(), 0, false);
    player1->setCoordReference(320, stage->getGroundOffset() * 2);
    player2->setCoordReference(320, stage->getGroundOffset() * 2);
    player1->setPlayerNumber(0);
    player1->setPower(player1->getPowerMax());
    player1->setFacing(false);
    player1->changeState(0);
    player2->setPlayerNumber(1);
    player2->setFacing(true);
    player2->changeState(0);
    player2->setPower(player2->getPowerMax());
    camera.setPlayers(player1, player2);
    camera.setConfig(stage->getCamera());
    camera.reset();
    matchTime = defaultMatchTime;
}

void Fight::reset()
{
    isFading = true;
    round = 0;
    setFightState(FIGHT_INTRO);
    fadeIncrement = 1.0 / 30.0;

    stage->reset();
    player1->setPos(stage->getP1StartX(), 0, false);
    player2->setPos(stage->getP2StartX(), 0, false);
    player1->setCoordReference(320, stage->getGroundOffset() * 2);
    player2->setCoordReference(320, stage->getGroundOffset() * 2);
    player1->setPlayerNumber(0);
    player1->setFacing(false);
    player2->setPlayerNumber(1);
    player2->setFacing(true);
    camera.setPlayers(player1, player2);
    camera.setConfig(stage->getCamera());
    camera.reset();
    matchTime = defaultMatchTime;
    p1wins = 0;
    p2wins = 0;
}

void Fight::updatePlayerGuard(Char::Character *p, Char::Character *enemy)
{
    double dx = abs(p->getP2DistX());
    const int minDistance = 150;
    auto checkAndGuard = [&]() -> void
    {
        if (p->didCommand("holdback"))
        {
            switch (p->getStateType())
            {
            case Stat::STATE_TYPE_STAND:
                if (p->getStateNumber() != 8)
                {
                    p->changeState(8);
                }
                break;
            case Stat::STATE_TYPE_CROUCH:
                if (p->getStateNumber() != 9)
                {
                    p->changeState(9);
                }
                break;
            }
        }
    };
    if (dx < minDistance && enemy->getStateMoveType() == Stat::STATE_MOVE_ATTACK)
    {
        checkAndGuard();
    }

    bool imProj = false;
    for (auto proj : projectiles)
    {
        if (&proj->getOwner() != p)
        {
            if (abs(proj->getPosX() - p->getPosX()) < minDistance)
            {
                checkAndGuard();
                imProj = true;
                break;
            }
        }
    }

    if (enemy->getStateMoveType() != Stat::STATE_MOVE_ATTACK && !imProj)
    {
        switch (p->getStateType())
        {
        case Stat::STATE_TYPE_STAND:
            if (p->getStateNumber() == 8)
            {
                p->changeState(0);
            }
            break;
        case Stat::STATE_TYPE_CROUCH:
            if (p->getStateNumber() == 9)
            {
                p->changeState(2);
            }
            break;
        }
    }
}

void Fight::updatePlayersGravity()
{
    // se estiver acima do chão, recebe aceleração da gravidade
    if ((player1->getPosY() < 0) && player1->getStateType() == Stat::STATE_TYPE_AIR)
    {
        player1->gravityAccel();
    }
    if ((player2->getPosY() < 0) && player2->getStateType() == Stat::STATE_TYPE_AIR)
    {
        player2->gravityAccel();
    }
}

void Fight::updatePlayersLand()
{
    if (player1->getPosY() >= 0 && player1->getStateType() == Stat::STATE_TYPE_AIR)
    {
        player1->setPosY(0);
        player1->setVelY(0);
    }
    if (player2->getPosY() >= 0 && player2->getStateType() == Stat::STATE_TYPE_AIR)
    {
        player2->setPosY(0);
        player2->setVelY(0);
    }
}

void Fight::clampPositions(Char::Character *p)
{
    const Char::SizeConstants &sz = p->getSize();
    int px = (p->getPosX()) * p->getScaleX() + Display::getWidth() / 2;

    if (camera.getX() + px <= 0)
    {
        p->setPosX((-camera.getX() - Display::getWidth() / 2) / p->getScaleX());
    }
    if (camera.getX() + px >= Display::getWidth())
    {
        p->setPosX((Display::getWidth() + -camera.getX() - Display::getWidth() / 2) / p->getScaleX());
    }

    if (p->getPosX() <= stage->getLeftLimit())
    {
        p->setPosX(stage->getLeftLimit());
    }
    if (p->getPosX() >= stage->getRightLimit())
    {
        p->setPosX(stage->getRightLimit());
    }
}

void Fight::updatePlayersVelocity()
{
    player1->updateVelocity();
    player2->updateVelocity();
    clampPositions(player1);
    clampPositions(player2);

    player1->setP2DistX(player2->getPosX() - player1->getPosX());
    player2->setP2DistX(player1->getPosX() - player2->getPosX());
    player1->setP2DistY(player2->getPosY() - player1->getPosY());
    player2->setP2DistY(player1->getPosY() - player2->getPosY());
}

void Fight::updatePlayersFacing()
{
    auto turnAnim = [&](Char::Character *p) -> int
    {
        if (p->getStateType() == Stat::STATE_TYPE_STAND)
        {
            return 6;
        }
        if (p->getStateType() == Stat::STATE_TYPE_CROUCH)
        {
            return 7;
        }
        return 6;
    };
    if (!player1->getFacing() && player1->getP2DistX() < 0)
    {
        if (player1->canTurn())
        {
            player1->setFacing(true);
            player1->changeAnim(turnAnim(player1));
        }
    }
    else if (player1->getFacing() && player1->getP2DistX() > 0)
    {
        if (player1->canTurn())
        {
            player1->setFacing(false);
            player1->changeAnim(turnAnim(player1));
        }
    }

    if (!player2->getFacing() && player2->getP2DistX() < 0)
    {
        if (player2->canTurn())
        {
            player2->setFacing(true);
            player2->changeAnim(turnAnim(player2));
        }
    }
    else if (player2->getFacing() && player2->getP2DistX() > 0)
    {
        if (player2->canTurn())
        {
            player2->setFacing(false);
            player2->changeAnim(turnAnim(player2));
        }
    }
}

int Fight::getCamX() const
{
    return camera.getX();
}

int Fight::getCamY() const
{
    return camera.getY();
}

void Fight::triggerSecond()
{
    if (fightState == FIGHT_BATTLE)
    {
        matchTime--;
    }
}

int Fight::update()
{
    if (!isLoaded)
    {
        return 0;
    }
    al_clear_to_color(al_map_rgb(0, 0, 0));

    stage->update();
    updatePlayersGravity();
    updateCollisions();
    updatePlayerGuard(player1, player2);
    updatePlayerGuard(player2, player1);

    if (fightState == FIGHT_BATTLE)
    {
        player1->update();
        player2->update();
        for (auto projIterator = projectiles.begin(); projIterator != projectiles.end(); )
        {
            auto proj = *projIterator;
            if (proj->getState() == Char::Projectile::Dead)
            {
                Console::print("Deletando projétil");
                delete proj;
                projIterator = projectiles.erase(projIterator);
                continue;
            }
            proj->update();
            projIterator++;
        }
    }
    camera.update();
    int camX = camera.getX();
    int camY = camera.getY();
    stage->drawBack(camX, camY);

    drawLifeBars();
    player1->draw(camX, camY);
    player2->draw(camX, camY);

    for (auto proj : projectiles)
    {
        proj->draw(camX, camY);
    }

    if (fightState == FIGHT_BATTLE)
    {
        updatePlayersVelocity();
        updatePlayersFacing();
        for (auto proj : projectiles)
        {
            proj->updateVelocity();
        }
    }

    stage->drawFront(camX, camY);

    if (fightState == FIGHT_BATTLE)
    {
        if (player1->getPower() <= 0 || player2->getPower() <= 0 || matchTime <= 0)
        {
            setFightState(FIGHT_WIN);
            if (player1->getPower() > player2->getPower())
            {
                p1wins++;
            }
            else if (player2->getPower() > player1->getPower())
            {
                p2wins++;
            }
        }
    }

    if (fightState == FIGHT_INTRO)
    {
        fadeUpdate();
        if (!isFading)
        {
            setFightState(FIGHT_BATTLE);
        }
    }
    if (fightState == FIGHT_WIN)
    {
        fadeUpdate();
        if (!isFading)
        {
            setFightState(FIGHT_INTRO);
            roundUp();

            if (p1wins >= 2 || p2wins >= 2)
            {
                gameEnviroment.changeState(Game::GAME_STATE_SELECT);
            }
        }
    }

    if (Display::showDebug())
    {
        camera.drawDebug();
        player1->drawDebugInfo(0);
        player2->drawDebugInfo(1);
    }
    time++;
    return 0;
}

void Fight::setFightState(int n)
{
    fightState = n;
    switch (n)
    {
    case FIGHT_INTRO:
        fadeValue = 1.0;
        dirFade = false;
        isFading = true;
        break;
    case FIGHT_BATTLE:
        fadeValue = 1.0;
        dirFade = true;
        isFading = false;
        break;
    case FIGHT_WIN:
        fadeValue = 0.0;
        dirFade = true;
        isFading = true;
        break;
    }
}


void Fight::drawLifeBars()
{
    const int clockWidth = 80;
    const int clockHeight = 40;
    const int lifeBarWidth = 240;
    const int lifeBarHeight = 20;
    const int sy = 30;
    const int clockY = sy - 8;
    const float rx = 3.0;
    const float ry = 3.0;

    const int p1sx = Display::getWidth() / 2 - clockWidth / 2 - lifeBarWidth;
    const int p2sx = Display::getWidth() / 2 + clockWidth / 2;
    const int csx = Display::getWidth() / 2 - clockWidth / 2 + 4;
    const float p1life = (float) player1->getPower() / (float) player1->getPowerMax();
    const float p2life = (float) player2->getPower() / (float) player2->getPowerMax();
    const int pWinY = sy + lifeBarHeight;
    stringstream ss;
    ss << matchTime;
    string s = ss.str();

    int tx = Display::getWidth() / 2 - Display::textWidth(NORMAL_FONT, s.c_str()) / 2;
    int ty = clockY + (clockHeight) / 2 - (Display::textLineHeight(NORMAL_FONT) + 4) / 2;
    al_draw_filled_rounded_rectangle(csx, clockY, csx + clockWidth - 8, clockY + clockHeight, rx, ry, al_map_rgba(0,0,0, 100));
    al_draw_rounded_rectangle(csx, clockY, csx + clockWidth - 8, clockY + clockHeight, rx, ry, al_map_rgb(135,205,0), 2);
    Display::drawText(NORMAL_FONT, al_map_rgb(255, 0, 0), tx, ty, s.c_str());
    Display::drawText(NORMAL_FONT, al_map_rgb(255, 255, 0), tx, ty + 2, s.c_str());

    // desenha o fundo vermelho
    al_draw_filled_rounded_rectangle(p1sx, sy, p1sx + lifeBarWidth, sy + lifeBarHeight, rx, ry, al_map_rgba(255,0,0,100));
    al_draw_filled_rounded_rectangle(p2sx, sy, p2sx + lifeBarWidth, sy + lifeBarHeight, rx, ry, al_map_rgba(255,0,0,100));

    // desenha a quantidade de energia atual
    int p1total = lifeBarWidth * p1life;
    int p2total = lifeBarWidth * p2life;
    int p1LifeStartX = p1sx + (lifeBarWidth - p1total);
    int p2LifeStartX = p2sx;
    al_draw_filled_rounded_rectangle(p1LifeStartX, sy, p1LifeStartX + p1total, sy + lifeBarHeight, rx, ry, al_map_rgb(20,155,0));
    al_draw_filled_rounded_rectangle(p2LifeStartX, sy, p2LifeStartX + p2total, sy + lifeBarHeight, rx, ry, al_map_rgb(20,155,0));

    // desenha o contorno
    al_draw_rounded_rectangle(p1sx, sy, p1sx + lifeBarWidth, sy + lifeBarHeight, rx, ry, al_map_rgb(135,205,0), 2);
    al_draw_rounded_rectangle(p2sx, sy, p2sx + lifeBarWidth, sy + lifeBarHeight, rx, ry, al_map_rgb(135,205,0), 2);


    const int p1WinX = p1sx;
    if (p1wins)
    {
        stringstream ss;
        for (int i = 0; i < p1wins; i++)
        {
            ss << '*';
        }
        Display::drawText(NORMAL_FONT, al_map_rgb(255, 0, 0), p1sx, pWinY, ss.str().c_str());
        Display::drawText(NORMAL_FONT, al_map_rgb(255, 255, 0), p1sx, pWinY + 2, ss.str().c_str());
    }
    if (p2wins)
    {
        stringstream ss;
        for (int i = 0; i < p2wins; i++)
        {
            ss << '*';
        }
        const int tw = Display::textWidth(NORMAL_FONT, ss.str().c_str());
        Display::drawText(NORMAL_FONT, al_map_rgb(255, 0, 0), p2sx + lifeBarWidth - tw, pWinY, ss.str().c_str());
        Display::drawText(NORMAL_FONT, al_map_rgb(255, 255, 0), p2sx + lifeBarWidth - tw, pWinY + 2, ss.str().c_str());

    }


}

void Fight::setHit(Char::Character *src, Char::Character *dst, Char::HitInfo *info)
{
    if (!info)
    {
        return;
    }

    if (dst->getStateNumber() != 8 || dst->getStateNumber() != 9)
    {
        switch (dst->getStateType())
        {
        case Stat::STATE_TYPE_STAND:
        {
            if (info->p2stateno < 0)
            {
                dst->changeState(20 + info->strength % 3);
            }
            else
            {
                dst->changeState(info->p2stateno);
            }
            break;
        }
        case Stat::STATE_TYPE_CROUCH:
        {
            if (info->crouch.p2stateno < 0)
            {
                dst->changeState(23 + info->strength % 3);
            }
            else
            {
                dst->changeState(info->crouch.p2stateno);
            }
            break;
        }
        case Stat::STATE_TYPE_AIR:
        {
            dst->changeState(28);
            dst->setVelY(-8);
            break;
        }
        }
        dst->setPower(dst->getPower() - info->damage);
    }
    else
    {
        dst->setPower(dst->getPower() - (info->damage * 0.5));
    }

    camera.addShakeAmpl(info->shake.amplitude);
    camera.addShakeTime(info->shake.time);
    camera.addShakeFreq(info->shake.freq);
    if (info->sound.play)
    {
        src->playSound(info->sound.group, info->sound.index);
    }

    dst->velAdd(info->velx, info->vely);
    safeDelete(info);
}

void Fight::updateCollisions()
{
    int p1x = player1->getPosX() * player1->getScaleX();
    int p1y = player1->getPosY() * player1->getScaleY();

    int p2x = player2->getPosX() * player2->getScaleX();
    int p2y = player2->getPosY() * player2->getScaleY();

    auto p1blue = player1->getDefenseBoxes();
    auto p1red = player1->getAttackBoxes();
    auto p2blue = player2->getDefenseBoxes();
    auto p2red = player2->getAttackBoxes();

    for (auto proj : projectiles)
    {
        int pjx = proj->getPosX() * proj->getScaleX();
        int pjy = proj->getPosY() * proj->getScaleY();
        auto pjRed = proj->getAttackBoxes();
        if (Col::collide(pjRed, pjx, pjy, p2blue, p2x, p2y) && !proj->isOwner(*player2))
        {
            proj->hit();
            if (player2->getStateNumber() != 8 || player2->getStateNumber() != 9)
            {
                if (player2->getStateMoveType() != Stat::STATE_MOVE_HIT)
                {
                    player2->setPower(player2->getPower() - 80);
                    switch (player2->getStateType())
                    {
                    case Stat::STATE_TYPE_STAND:
                        player2->changeState(20+rand()%2);
                        break;
                    case Stat::STATE_TYPE_CROUCH:
                        player2->changeState(23+rand()%2);
                        break;
                    case Stat::STATE_TYPE_AIR:
                        player2->changeState(28);
                        player2->setVelY(-5);
                        break;
                    }
                    player2->setVelX(-3);

                }

            }
        }
        if (Col::collide(pjRed, pjx, pjy, p1blue, p1x, p1y) && !proj->isOwner(*player1))
        {
            proj->hit();
            if (player1->getStateNumber() != 8 || player1->getStateNumber() != 9)
            {


                if (player1->getStateMoveType() != Stat::STATE_MOVE_HIT)
                {
                    player1->setPower(player1->getPower() - 80);
                    switch (player1->getStateType())
                    {
                    case Stat::STATE_TYPE_STAND:
                        player1->changeState(20+rand()%2);
                        break;
                    case Stat::STATE_TYPE_CROUCH:
                        player1->changeState(23+rand()%2);
                        break;
                    case Stat::STATE_TYPE_AIR:
                        player1->changeState(28);
                        player1->setVelY(-5);
                        break;
                    }
                    player1->setVelX(-3);
                }
            }
        }

        for (auto proj2 : projectiles)
        {
            // verifica se já explodiu
            if (proj2->getState() == Char::Projectile::Explod)
            {
                continue;
            }

            // verifica se são do mesmo dono
            if (&proj2->getOwner() != &proj->getOwner())
            {
                auto pj2Red = proj2->getAttackBoxes();
                int pj2x = proj2->getPosX() * proj2->getScaleX();
                int pj2y = proj2->getPosY() * proj2->getScaleY();
                if (Col::collide(pjRed, pjx, pjy, pj2Red, pj2x, pj2y))
                {
                    proj->hit();
                    proj2->hit();
                }
            }
        }
    }

    // Empurar o oponente
    if (Col::collide(p1blue, p1x, p1y, p2blue, p2x, p2y))
    {
        Char::Character *left, *right;

        if (player1->getLeftEdgeDistance() <= player2->getLeftEdgeDistance())
        {
            left = player1;
            right = player2;
        }
        else
        {
            left = player2;
            right = player1;
        }
        double rdx = right->getDeltaX();
        double ldx = left->getDeltaX();
        double lx = left->getPosX();
        double lx0 = left->getPrevPosX();

        double rx = right->getPosX();
        double rx0 = right->getPrevPosX();

        // quandos os dois chars estão se empurrando
        if (rdx != 0 && ldx != 0)
        {
            left->setPosX(lx - abs(rdx));
            right->setPosX(rx + abs(ldx));
        }
        else
        {
            if (rx0 >= rx)
            {
                left->setPosX(lx - abs(rdx));
            }

            if (lx0 <= lx)
            {
                right->setPosX(rx + abs(ldx));
            }
        }

    }

    if (Col::collide(p1red, p1x, p1y, p2red, p2x, p2y))
    {
        Char::HitInfo *info1 = player1->getOneHit();
        Char::HitInfo *info2 = player2->getOneHit();
        setHit(player1, player2, info1);
        setHit(player2, player1, info2);
    }

    if (Col::collide(p1red, p1x, p1y, p2blue, p2x, p2y))
    {
        Char::HitInfo *info = player1->getOneHit();
        if (info)
        {
            setHit(player1, player2, info);
        }

    }

    if (Col::collide(p1blue, p1x, p1y, p2red, p2x, p2y))
    {
        Char::HitInfo *info = player2->getOneHit();
        if (info)
        {
            setHit(player2, player1, info);
        }
    }



}


void Fight::addProjectile(Char::Projectile *proj)
{
    Console::print("Criando projétil");
    if (proj)
    {
        projectiles.push_back(proj);
    }
}

void Fight::addShakeTime(int ticks)
{
    camera.addShakeTime(ticks);
}

void Fight::addShakeFreq(double freq)
{
    camera.addShakeFreq(freq);
}

void Fight::addShakeAmpl(double ampl)
{
    camera.addShakeAmpl(ampl);
}

void Fight::addShakePhase(double phase)
{
    camera.addShakePhase(phase);
}


void Fight::fadeUpdate()
{

    ALLEGRO_COLOR color = al_map_rgba(0, 0, 0, (double) 255 * fadeValue);
    Display::drawRectFill(0, 0, Display::getWidth(), Display::getHeight(), color);
    if (dirFade)
    {
        fadeValue += fadeIncrement;
        if (fadeValue > 1.0)
        {
            isFading = false;
        }
    }
    else
    {
        fadeValue -= fadeIncrement;
        if (fadeValue < 0.0)
        {
            isFading = false;
        }
    }
}
