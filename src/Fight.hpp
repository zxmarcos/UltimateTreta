#pragma once

#include "Camera.hpp"
#include "Stage.hpp"
#include "Character.hpp"
#include <list>

class Game;

class Fight
{
public:
	Fight(Game& env);
	~Fight();
	int update();
	bool load(string char1, string char2, string stagef);
	static int getTime();
	int getCamX() const;
	int getCamY() const;
	void addProjectile(Char::Projectile *proj);
	void addShakeTime(int ticks);
    void addShakeFreq(double freq);
    void addShakeAmpl(double ampl);
    void addShakePhase(double phase);
    void reset();
    void triggerSecond();
private:
    enum {
        FIGHT_INTRO = 0,
        FIGHT_BATTLE,
        FIGHT_WIN,
    };
    int p1wins;
    int p2wins;
    Game& gameEnviroment;
    void roundUp();
    void setFightState(int n);
    int round;
    int fightState;
    bool isFading;
    bool dirFade;
    double fadeValue;
    double fadeIncrement;
    void fadeUpdate();
    void setHit(Char::Character *src, Char::Character *dst, Char::HitInfo *info);
    static int camX, camY;
    void updatePlayersGravity();
    void updatePlayersLand();
    void updatePlayersVelocity();
    void updatePlayersFacing();
    void updateCollisions();
    void updatePlayerGuard(Char::Character *p, Char::Character *p2);
    void drawLifeBars();
    void clampPositions(Char::Character *p);
	static int time;
	int matchTime;
	Char::Character *player1;
	Char::Character *player2;
	Stage::StageLoader *stage;
	bool isLoaded;
	Camera camera;
	std::list<Char::Projectile*> projectiles;
};


