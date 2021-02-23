#include "Actor.h"
#include "StudentWorld.h"
#include <cmath>

#define M_PI 3.1415926535898

//Actor class implementations
Actor::Actor(int imageID, double startX, double startY, int dir = 0, double size = 1.0, unsigned int depth = 0)
	:GraphObject(imageID, startX, startY, dir, size, depth), m_vertSpeed(0), m_horizSpeed(0)
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

//SentientActor constructor
SentientActor::SentientActor(int imageID, double startX, double startY, int hp, int dir = 0, double size = 1.0, unsigned int depth = 0)
	:Actor(imageID, startX, startY, dir, size, depth), m_hp(hp)
{
}

NoncollidingActor::NoncollidingActor(int imageID, double startX, double startY, int dir = 0, double size = 1.0, unsigned int depth = 0)
	:Actor(imageID, startX, startY, dir, size, depth), m_alive(true)
{
}

//ghost racer implementations
GhostRacer::GhostRacer(StudentWorld* game, double startX, double startY)
	:SentientActor(IID_GHOST_RACER, startX, startY, 100, 90, 4.0, 0), m_game(game), holyWaterUnits(10)
{
	setVertSpeed(0);
}	//note: m_horizSpeed is not used in any movement calculations for GhostRacer objects

StudentWorld* GhostRacer::getWorld() const
{
	return m_game;
}

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
			m_game->playSound(SOUND_PLAYER_DIE);
		setDirection(82);
		m_game->playSound(SOUND_VEHICLE_CRASH);
	}
	else if (getX() > RIGHT_EDGE && getDirection() < 90)
	{
		decHP(10);
		if (!isAlive())
			m_game->playSound(SOUND_PLAYER_DIE);
		setDirection(98);
		m_game->playSound(SOUND_VEHICLE_CRASH);
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
HumanPedestrian::HumanPedestrian(GhostRacer* player, double startX, double startY)
	:SentientActor(IID_HUMAN_PED, startX, startY, 2, 0, 2.0, 0), m_player(player)
{
}

void HumanPedestrian::doSomething()
{
	if (!isAlive())
		return;
	if (overlap(m_player))
	{
		m_player->getWorld()->decLives();	//level ends
		return;
	}
}

//BorderLine implementations
BorderLine::BorderLine(GhostRacer* p, double startX, double startY, int colorID)
	:NoncollidingActor(colorID, startX, startY, 0, 2.0, 2), m_player(p)
{
	setVertSpeed(-4);
	setHorizSpeed(0);
}

void BorderLine::doSomething()
{
	if (!isAlive())
		return;
	int vert_speed = getVertSpeed() - m_player->getVertSpeed();
	int horiz_speed = getHorizSpeed();
	double new_y = getY() + vert_speed;
	double new_x = getX() + horiz_speed;

	moveTo(new_x, new_y);

	if (offScreen())
		kill();
}


LostSoul::LostSoul(GhostRacer* player, double startX, double startY)
	:NoncollidingActor(IID_SOUL_GOODIE, startX, startY, 0, 4.0, 2), m_player(player)
{
	setVertSpeed(-4);
}

void LostSoul::doSomething()
{
	int vert_speed = getVertSpeed() - m_player->getVertSpeed();
	int horiz_speed = getHorizSpeed();
	double new_y = getY() + vert_speed;
	double new_x = getX() + horiz_speed;

	moveTo(new_x, new_y);

	if (offScreen())
		kill();

	if (overlap(m_player))
	{
		//increase number of lost souls that have been saved
		kill();
		m_player->getWorld()->playSound(SOUND_GOT_SOUL);
	}
	setDirection(getDirection() - 10);
}