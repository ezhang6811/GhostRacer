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
SentientActor::SentientActor(StudentWorld* game, int imageID, double startX, double startY, int hp, int dir = 0, double size = 1.0, unsigned int depth = 0)
	:Actor(game, imageID, startX, startY, dir, size, depth), m_hp(hp)
{
}

NoncollidingActor::NoncollidingActor(StudentWorld* game, int imageID, double startX, double startY, int dir = 0, double size = 1.0, unsigned int depth = 0)
	:Actor(game, imageID, startX, startY, dir, size, depth), m_alive(true)
{
}

//ghost racer implementations
GhostRacer::GhostRacer(StudentWorld* game, double startX, double startY)
	:SentientActor(game, IID_GHOST_RACER, startX, startY, 100, 90, 4.0, 0), holyWaterUnits(10)
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

//Human pedestrian implementations
HumanPedestrian::HumanPedestrian(StudentWorld* game, double startX, double startY)
	:SentientActor(game, IID_HUMAN_PED, startX, startY, 2, 0, 2.0, 0)
{
}

void HumanPedestrian::doSomething()
{
	if (!isAlive())
		return;
	if (overlap(getWorld()->getPlayer()))
	{
		getWorld()->decLives();	//level ends
		return;
	}
}

//BorderLine implementations
BorderLine::BorderLine(StudentWorld* game, double startX, double startY, int colorID)
	:NoncollidingActor(game, colorID, startX, startY, 0, 2.0, 2)
{
	setVertSpeed(-4);
	setHorizSpeed(0);
}

void BorderLine::doSomething()
{
	if (!isAlive())
		return;
	
	moveOnScreen();

	if (offScreen())
		kill();
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