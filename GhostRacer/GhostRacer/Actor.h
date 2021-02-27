#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

class StudentWorld;
class GhostRacer;

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp
class Actor : public GraphObject
{
public:
	Actor(StudentWorld* game, int imageID, double startX, double startY, int dir, double size, unsigned int depth);

	virtual void doSomething() = 0;
	virtual bool isAlive() const = 0;

	//getters and setters for additional private members
	StudentWorld* getWorld() { return m_game; }
	int getVertSpeed() const { return m_vertSpeed; }
	void setVertSpeed(int speed) { m_vertSpeed = speed; }
	int getHorizSpeed() const { return m_horizSpeed; }
	void setHorizSpeed(int speed) { m_horizSpeed = speed; }

	bool offScreen() const;
	bool overlap(Actor* other);
	virtual void moveOnScreen();
	virtual bool isCollisionAvoidanceWorthy() const { return false; }

private:
	StudentWorld* m_game;
	int m_vertSpeed;
	int m_horizSpeed;
};

//SentientActor: contains all actors whose alive status depends on health points, and collision avoidance-worthy
class SentientActor : public Actor
{
public:
	SentientActor(StudentWorld* game, int imageID, double startX, double startY, int hp, int dir, double size);
	void decHP(int hp) { m_hp -= hp; }
	int getHP() const { return m_hp; }
	virtual bool isAlive() const { return m_hp > 0; }
	virtual bool isCollisionAvoidanceWorthy() const { return true; }
private:
	int m_hp;
};

//NoncollidingActor: goodies and oil slicks whose alive status is boolean
class NoncollidingActor : public Actor
{
public:
	NoncollidingActor(StudentWorld* game, int imageID, double startX, double startY, int dir, double size, unsigned int depth);
	virtual bool isAlive() const { return m_alive; }
	void kill() { m_alive = false; }
private:
	bool m_alive;
};

//MovingAgent: all occurences of SentientActor except GhostRacer, which have a movement plan distance
class MovingAgent : public SentientActor
{
public:
	MovingAgent(StudentWorld* game, int imageID, double startX, double startY, int hp, int dir, double size);
	int getMovementPlanDistance() { return m_movePlanDist; }
	void decMovementPlanDistance() { m_movePlanDist--; }
	void resetMovementPlanDistance() { m_movePlanDist = randInt(4, 32); }
	virtual void pickNewMovementPlan();
private:
	int m_movePlanDist;
};


class GhostRacer : public SentientActor
{
public:
	GhostRacer(StudentWorld* game, double startX, double startY);
	virtual void doSomething();
private:
	int holyWaterUnits;
};

class HumanPedestrian : public MovingAgent
{
public:
	HumanPedestrian(StudentWorld* game, double startX, double startY);
	virtual void doSomething();
};

class ZombiePedestrian : public MovingAgent
{
public:
	ZombiePedestrian(StudentWorld* game, double startX, double startY);
	virtual void doSomething();
};

class ZombieCab : public MovingAgent
{
public:
	ZombieCab(StudentWorld* game, double startX, double startY, int vertSpeed);
	virtual void pickNewMovementPlan();
	virtual void doSomething();
private:
	bool hasDamagedGhostRacer;
};

class BorderLine : public NoncollidingActor
{
public:
	BorderLine(StudentWorld* game, double startX, double startY, int colorID);
	virtual void doSomething();
};

class LostSoul : public NoncollidingActor
{
public:
	LostSoul(StudentWorld* game, double startX, double startY);
	virtual void doSomething();
};

#endif // ACTOR_H
