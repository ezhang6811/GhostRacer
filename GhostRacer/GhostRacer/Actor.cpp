#include "Actor.h"
#include "StudentWorld.h"
#include <math.h>

#define M_PI 3.1415926535898

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

Actor::Actor(int imageID, double startX, double startY, int dir = 0, double size = 1.0, unsigned int depth = 0)
	:GraphObject(imageID, startX, startY, dir, size, depth), m_vertSpeed(0)
{
}

//SentientActor constructor
SentientActor::SentientActor(int imageID, double startX, double startY, int hp, int dir = 0, double size = 1.0, unsigned int depth = 0)
	:Actor(imageID, startX, startY, dir, size, depth), m_hp(hp)
{
}

//ghost racer implementations
GhostRacer::GhostRacer(StudentWorld* game, double startX, double startY)
	:SentientActor(IID_GHOST_RACER, startX, startY, 100, 90, 4.0, 0), m_game(game), holyWaterUnits(10)
{
}

StudentWorld* GhostRacer::getWorld()
{
	return m_game;
}

void GhostRacer::doSomething()
{
	if (!isAlive())
		return;

	int ch;
	if (getWorld()->getKey(ch))
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


		//perform movement
		double delta_x = cos(getDirection() * M_PI/180) * 4.0;
		moveTo(getX() + delta_x, getY());
	}
}


//BorderLine implementations
BorderLine::BorderLine(GhostRacer* p, double startX, double startY, int colorID)
	:Actor(colorID, startX, startY), m_player(p), m_horizSpeed(0)
{
	setVertSpeed(-4);
}

void BorderLine::doSomething()
{
	if (!isAlive())
		return;
	int vert_speed = getVertSpeed() - m_player->getVertSpeed();
	int horiz_speed = m_horizSpeed;
	double new_y = getY() + vert_speed;
	double new_x = getX() + horiz_speed;

	moveTo(new_x, new_y);
}

bool BorderLine::isAlive()
{
	if (getX() < 0 || getY() < 0 || getX() > VIEW_WIDTH || getY() > VIEW_HEIGHT)
		return false;
	return true;
}