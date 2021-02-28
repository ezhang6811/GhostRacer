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
	virtual bool isAlive() const { return m_alive; }
	void kill() { m_alive = false; }

	//getters and setters for additional private members
	StudentWorld* getWorld() { return m_game; }
	GhostRacer* getPlayer();
	int getVertSpeed() const { return m_vertSpeed; }
	void setVertSpeed(int speed) { m_vertSpeed = speed; }
	int getHorizSpeed() const { return m_horizSpeed; }
	void setHorizSpeed(int speed) { m_horizSpeed = speed; }

	bool offScreen() const;
	bool overlap(Actor* other);
	void moveOnScreen();
	virtual bool beSprayedIfAppropriate() { return false; }
	virtual bool isCollisionAvoidanceWorthy() const { return false; }

private:
	StudentWorld* m_game;
	int m_vertSpeed;
	int m_horizSpeed;
	bool m_alive;
};

//SentientActor: contains all actors whose alive status depends on health points, and who are collision avoidance-worthy
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

//Ghost Racer class
class GhostRacer : public SentientActor
{
public:
	GhostRacer(StudentWorld* game, double startX, double startY);
	virtual void doSomething();
	void heal() { decHP(-10); }
	int getHolyWater() { return holyWaterUnits; }
	void increaseHolyWater() { holyWaterUnits += 10; }
	void spin();
private:
	int holyWaterUnits;
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

//Human Pedestrian class
class HumanPedestrian : public MovingAgent
{
public:
	HumanPedestrian(StudentWorld* game, double startX, double startY);
	virtual void doSomething();
	virtual bool beSprayedIfAppropriate();
};

//Zombie Pedestrian class
class ZombiePedestrian : public MovingAgent
{
public:
	ZombiePedestrian(StudentWorld* game, double startX, double startY);
	virtual void doSomething();
	virtual bool beSprayedIfAppropriate();
private:
	int ticksToNextGrunt;
};

//Zombie Cab class
class ZombieCab : public MovingAgent
{
public:
	ZombieCab(StudentWorld* game, double startX, double startY, int vertSpeed);
	virtual void pickNewMovementPlan();
	virtual void doSomething();
	virtual bool beSprayedIfAppropriate();
private:
	bool hasDamagedGhostRacer;
};

//NoncollidingActor: goodies and oil slicks whose alive status is boolean, and are not collision avoidance-worthy
class NoncollidingActor : public Actor
{
public:
	NoncollidingActor(StudentWorld* game, int imageID, double startX, double startY, int dir, double size, unsigned int depth);
	virtual bool beSprayedIfAppropriate();
	virtual bool isSprayable() const = 0;
};

//White and yellow border line class
class BorderLine : public NoncollidingActor
{
public:
	BorderLine(StudentWorld* game, double startX, double startY, int colorID);
	virtual void doSomething();
	virtual bool isSprayable() const { return false; }
};

//Oil slick class
class OilSlick : public NoncollidingActor
{
public:
	OilSlick(StudentWorld* game, double startX, double startY);
	virtual void doSomething();
	virtual bool isSprayable() const { return false; }
};

//Healing goodie class
class HealingGoodie : public NoncollidingActor
{
public:
	HealingGoodie(StudentWorld* game, double startX, double startY);
	virtual void doSomething();
	virtual bool isSprayable() const { return true; }
};

//Holy water goodie class
class HolyWaterGoodie : public NoncollidingActor
{
public:
	HolyWaterGoodie(StudentWorld* game, double startX, double startY);
	virtual void doSomething();
	virtual bool isSprayable() const { return true; }
};

//Lost soul goodie class
class LostSoul : public NoncollidingActor
{
public:
	LostSoul(StudentWorld* game, double startX, double startY);
	virtual void doSomething();
	virtual bool isSprayable() const { return false; }
};

//Holy water projectile class
class Spray : public Actor
{
public:
	Spray(StudentWorld* game, double startX, double startY, int dir);
	virtual void doSomething();
private:
	int maxTravelDistance;
};


#endif // ACTOR_H
