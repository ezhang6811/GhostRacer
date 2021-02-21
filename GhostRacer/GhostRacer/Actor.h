#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include "StudentWorld.h"

class StudentWorld;

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp
class Actor : public GraphObject
{
public:
	Actor(int imageID, double startX, double startY, int dir, double size, unsigned int depth);

	virtual void doSomething() = 0;
	virtual bool isAlive() = 0;

	int getVertSpeed() { return m_vertSpeed; }
	void setVertSpeed(int delta) { m_vertSpeed = delta; }

private:
	int m_vertSpeed;
};

//SentientActor: contains all actors whose alive status depends on health points, and collision avoidance-worthy
class SentientActor : public Actor
{
public:
	SentientActor(int imageID, double startX, double startY, int hp, int dir, double size, unsigned int depth);
	void decHP(int hp) { m_hp -= hp; }
	int getHP() { return m_hp; }
	virtual bool isAlive() { return m_hp > 0; }
private:
	int m_hp;
};

class GhostRacer : public SentientActor
{
public:
	GhostRacer(StudentWorld* game, double startX, double startY);
	virtual void doSomething();
	StudentWorld* getWorld();
private:
	StudentWorld* m_game;
	int holyWaterUnits;
};

class BorderLine : public Actor
{
public:
	BorderLine(GhostRacer* p, double startX, double startY, int colorID);
	virtual void doSomething();
	virtual bool isAlive();
private:
	GhostRacer* m_player;
	int m_horizSpeed;
};

#endif // ACTOR_H_
