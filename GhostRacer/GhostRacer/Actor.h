#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

class StudentWorld;

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp
class Actor : public GraphObject
{
public:
	Actor(int imageID, double startX, double startY, int dir, double size, unsigned int depth);

	virtual void doSomething() = 0;
	virtual bool isAlive() const = 0;

	int getVertSpeed() const { return m_vertSpeed; }
	void setVertSpeed(int speed) { m_vertSpeed = speed; }
	int getHorizSpeed() const { return m_horizSpeed; }
	void setHorizSpeed(int speed) { m_horizSpeed = speed; }

	bool offScreen() const;
	bool overlap(Actor* other);

private:
	int m_vertSpeed;
	int m_horizSpeed;
};

//SentientActor: contains all actors whose alive status depends on health points, and collision avoidance-worthy
class SentientActor : public Actor
{
public:
	SentientActor(int imageID, double startX, double startY, int hp, int dir, double size, unsigned int depth);
	void decHP(int hp) { m_hp -= hp; }
	int getHP() const { return m_hp; }
	virtual bool isAlive() const { return m_hp > 0; }
private:
	int m_hp;
};

//NoncollidingActor: goodies and oil slicks whose alive status is boolean
class NoncollidingActor : public Actor
{
public:
	NoncollidingActor(int imageID, double startX, double startY, int dir, double size, unsigned int depth);
	virtual bool isAlive() const { return m_alive; }
	void kill() { m_alive = false; }
private:
	bool m_alive;
};


class GhostRacer : public SentientActor
{
public:
	GhostRacer(StudentWorld* game, double startX, double startY);
	virtual void doSomething();
	StudentWorld* getWorld() const;
private:
	StudentWorld* m_game;
	int holyWaterUnits;
};

class HumanPedestrian : public SentientActor
{
public:
	HumanPedestrian(GhostRacer* p, double startX, double startY);
	virtual void doSomething();
private:
	GhostRacer* m_player;
};

class BorderLine : public NoncollidingActor
{
public:
	BorderLine(GhostRacer* p, double startX, double startY, int colorID);
	virtual void doSomething();
private:
	GhostRacer* m_player;
};

class LostSoul : public NoncollidingActor
{
public:
	LostSoul(GhostRacer* p, double startX, double startY);
	virtual void doSomething();
private:
	GhostRacer* m_player;
};

#endif // ACTOR_H
