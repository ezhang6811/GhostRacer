#include "Actor.h"
#include "StudentWorld.h"
#include <cmath>

#define M_PI 3.1415926535898

//Actor class implementations
Actor::Actor(StudentWorld* game, int imageID, double startX, double startY, int dir = 0, double size = 1.0, unsigned int depth = 0)
	:GraphObject(imageID, startX, startY, dir, size, depth), m_game(game), m_vertSpeed(0), m_horizSpeed(0)
{
}

bool Actor::offScreen() const
{
	return (getX() < 0 || getY() < 0 || getX() > VIEW_WIDTH || getY() > VIEW_HEIGHT);
}

bool Actor::overlap(Actor* other)
{
	double deltaX = abs(other->getX() - getX());
	double deltaY = abs(other->getY() - getY());
	double radiusSum = other->getRadius() + getRadius();

	return (deltaX < radiusSum * .25 && deltaY < radiusSum * .6);
}

void Actor::moveOnScreen()
{
	int vert_speed = getVertSpeed() - getWorld()->getPlayer()->getVertSpeed();
	int horiz_speed = getHorizSpeed();
	double new_y = getY() + vert_speed;
	double new_x = getX() + horiz_speed;

	moveTo(new_x, new_y);
}

//SentientActor constructor
SentientActor::SentientActor(StudentWorld* game, int imageID, double startX, double startY, int hp, int dir = 0, double size = 1.0)
	:Actor(game, imageID, startX, startY, dir, size, 0), m_hp(hp)
{
}

//MovingAgent constructor
MovingAgent::MovingAgent(StudentWorld* game, int imageID, double startX, double startY, int hp, int dir, double size)
	: SentientActor(game, imageID, startX, startY, hp, dir, size), m_movePlanDist(0)
{
	setVertSpeed(-4);
}

void MovingAgent::pickNewMovementPlan()
{
	setHorizSpeed(randInt(-3, 3));
	while (getHorizSpeed() == 0)
		setHorizSpeed(randInt(-3, 3));

	resetMovementPlanDistance();

	if (getHorizSpeed() < 0)
		setDirection(180);
	else
		setDirection(0);
}

//ghost racer implementations
GhostRacer::GhostRacer(StudentWorld* game, double startX, double startY)
	:SentientActor(game, IID_GHOST_RACER, startX, startY, 100, 90, 4.0), holyWaterUnits(10)
{
	setVertSpeed(0);
}	//note: m_horizSpeed is not used in any movement calculations for GhostRacer objects

void GhostRacer::doSomething()
{
	if (!isAlive())
		return;

	int ch;

	//check for collisions
	if (getX() < LEFT_EDGE && getDirection() > 90)
	{
		decHP(10);
		if (!isAlive())
			getWorld()->playSound(SOUND_PLAYER_DIE);
		setDirection(82);
		getWorld()->playSound(SOUND_VEHICLE_CRASH);
	}
	else if (getX() > RIGHT_EDGE && getDirection() < 90)
	{
		decHP(10);
		if (!isAlive())
			getWorld()->playSound(SOUND_PLAYER_DIE);
		setDirection(98);
		getWorld()->playSound(SOUND_VEHICLE_CRASH);
	}
	else if (getWorld()->getKey(ch))
	{
		switch (ch)
		{
		case KEY_PRESS_LEFT:
			if (getDirection() < 114)
				setDirection(getDirection() + 8);
			break;
		case KEY_PRESS_RIGHT:
			if (getDirection() > 66)
				setDirection(getDirection() - 8);
			break;
		case KEY_PRESS_UP:
			if (getVertSpeed() < 5)
				setVertSpeed(getVertSpeed() + 1);
			break;
		case KEY_PRESS_DOWN:
			if (getVertSpeed() > -1)
				setVertSpeed(getVertSpeed() - 1);
			break;
		}

	}
	//perform movement
	double delta_x = cos(getDirection() * M_PI / 180) * 4.0;
	moveTo(getX() + delta_x, getY());
}

void GhostRacer::spin()
{
	int dirChange = 0;
	while (abs(dirChange) < 5 || getDirection() + dirChange < 60 && getDirection() + dirChange > 120)
		dirChange = randInt(-20, 20);
	setDirection(getDirection() + dirChange);
}

//Human pedestrian implementations
HumanPedestrian::HumanPedestrian(StudentWorld* game, double startX, double startY)
	:MovingAgent(game, IID_HUMAN_PED, startX, startY, 2, 0, 2.0)
{
}

void HumanPedestrian::doSomething()
{
	if (!isAlive())
		return;

	if (overlap(getWorld()->getPlayer()))
	{
		getWorld()->getPlayer()->decHP(getWorld()->getPlayer()->getHP());	//level ends
		return;
	}

	moveOnScreen();
	if (offScreen())
		decHP(getHP());

	decMovementPlanDistance();
	if (getMovementPlanDistance() > 0)
		return;
	
	pickNewMovementPlan();
}


//Zombie pedestrian implementations
ZombiePedestrian::ZombiePedestrian(StudentWorld* game, double startX, double startY)
	:MovingAgent(game, IID_ZOMBIE_PED, startX, startY, 2, 0, 3.0)
{
}

void ZombiePedestrian::doSomething()
{
	if (!isAlive())
		return;
	if (overlap(getWorld()->getPlayer()))
	{
		getWorld()->getPlayer()->decHP(5);
		decHP(2);
		return;
	}

	moveOnScreen();
	if (offScreen())
		decHP(2);

	if (getMovementPlanDistance() > 0)
	{
		decMovementPlanDistance();
		return;
	}

	pickNewMovementPlan();
}

//Zombie cab implementations
ZombieCab::ZombieCab(StudentWorld* game, double startX, double startY, int vertSpeed)
	:MovingAgent(game, IID_ZOMBIE_CAB, startX, startY, 3, 90, 4.0), hasDamagedGhostRacer(false)
{
	setVertSpeed(vertSpeed);
}

void ZombieCab::pickNewMovementPlan()
{
	setVertSpeed(getVertSpeed() + randInt(-2, 2));

	resetMovementPlanDistance();
}

void ZombieCab::doSomething()
{
	if (!isAlive())
		return;

	if (overlap(getWorld()->getPlayer()) && !hasDamagedGhostRacer)
	{
		getWorld()->playSound(SOUND_VEHICLE_CRASH);
		getWorld()->getPlayer()->decHP(20);
		if (getX() <= getWorld()->getPlayer()->getX())
		{
			setHorizSpeed(-5);
			setDirection(120 + randInt(0, 20));
		}
		else
		{
			setHorizSpeed(5);
			setDirection(60 - randInt(0, 20));
		}
		hasDamagedGhostRacer = true;
	}

	moveOnScreen();
	if (offScreen())
		decHP(getHP());

	if (getVertSpeed() > getWorld()->getPlayer()->getVertSpeed())
	{
		if (getWorld()->closestAbove(this) && getWorld()->closestAbove(this)->getY() - getY() < 96)
		{
			setVertSpeed(getVertSpeed() - 0.5);
			return;
		}
	}
	else
	{
		if (getWorld()->closestBelow(this) && getY() - getWorld()->closestBelow(this)->getY() < 96)
		{
			setVertSpeed(getVertSpeed() + 0.5);
			return;
		}
	}

	decMovementPlanDistance();
	if (getMovementPlanDistance() > 0)
		return;

	pickNewMovementPlan();
}

NoncollidingActor::NoncollidingActor(StudentWorld* game, int imageID, double startX, double startY, int dir = 0, double size = 1.0, unsigned int depth = 0)
	:Actor(game, imageID, startX, startY, dir, size, depth), m_alive(true)
{
}

//BorderLine implementations
BorderLine::BorderLine(StudentWorld* game, double startX, double startY, int colorID)
	:NoncollidingActor(game, colorID, startX, startY, 0, 2.0, 2)
{
	setVertSpeed(-4);
}

void BorderLine::doSomething()
{
	if (!isAlive())
		return;
	
	moveOnScreen();

	if (offScreen())
		kill();
}

OilSlick::OilSlick(StudentWorld* game, double startX, double startY)
	:NoncollidingActor(game, IID_OIL_SLICK, startX, startY, 0, randInt(2, 5), 2)
{
	setVertSpeed(-4);
}

void OilSlick::doSomething()
{
	moveOnScreen();
	if (offScreen())
		kill();
	
	if (overlap(getWorld()->getPlayer()))
	{
		getWorld()->playSound(SOUND_OIL_SLICK);
		getWorld()->getPlayer()->spin();
	}
}

LostSoul::LostSoul(StudentWorld* game, double startX, double startY)
	:NoncollidingActor(game, IID_SOUL_GOODIE, startX, startY, 0, 4.0, 2)
{
	setVertSpeed(-4);
}

void LostSoul::doSomething()
{
	moveOnScreen();

	if (offScreen())
		kill();

	if (overlap(getWorld()->getPlayer()))
	{
		//increase number of lost souls that have been saved
		kill();
		getWorld()->playSound(SOUND_GOT_SOUL);
	}
	setDirection(getDirection() - 10);
}