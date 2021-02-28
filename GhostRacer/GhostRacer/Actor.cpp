#include "Actor.h"
#include "StudentWorld.h"
#include <cmath>

#define M_PI 3.1415926535898

//Actor class implementations
Actor::Actor(StudentWorld* game, int imageID, double startX, double startY, int dir = 0, double size = 1.0, unsigned int depth = 0)
	:GraphObject(imageID, startX, startY, dir, size, depth), m_game(game), m_vertSpeed(0), m_horizSpeed(0), m_alive(true)
{
}

GhostRacer* Actor::getPlayer()
{
	return m_game->getPlayer();
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
	int vert_speed = getVertSpeed() - getPlayer()->getVertSpeed();
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

	//check for wall collisions
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
		case KEY_PRESS_SPACE:
			if (holyWaterUnits > 0)
			{
				int delta_x = SPRITE_HEIGHT * cos(getDirection() * M_PI / 180);
				int delta_y = SPRITE_HEIGHT * sin(getDirection() * M_PI / 180);
				Actor* spray = new Spray(getWorld(), getX() + delta_x, getY() + delta_y, getDirection());
				getWorld()->addActor(spray);
				getWorld()->playSound(SOUND_PLAYER_SPRAY);
				holyWaterUnits--;
			}
			break;
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
	while (abs(dirChange) < 5)
		dirChange = randInt(-20, 20);
	if (getDirection() + dirChange > 60 && getDirection() + dirChange < 120)
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
	if (overlap(getPlayer()))
	{
		getPlayer()->decHP(getPlayer()->getHP());	//level ends
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

bool HumanPedestrian::beSprayedIfAppropriate()
{
	setHorizSpeed(getHorizSpeed() * -1);
	setDirection(getDirection() + 180);
	getWorld()->playSound(SOUND_PED_HURT);
	return true;
}

//Zombie pedestrian implementations
ZombiePedestrian::ZombiePedestrian(StudentWorld* game, double startX, double startY)
	:MovingAgent(game, IID_ZOMBIE_PED, startX, startY, 2, 0, 3.0), ticksToNextGrunt(0)
{
}

void ZombiePedestrian::doSomething()
{
	if (!isAlive())
		return;
	if (overlap(getPlayer()))
	{
		getPlayer()->decHP(5);
		decHP(2);
		if (!isAlive())
			getWorld()->playSound(SOUND_PED_DIE);
		return;
	}

	if (abs(getX() - getPlayer()->getX()) <= 30 && getY() > getPlayer()->getY())
	{
		setDirection(270);
		if (getX() < getPlayer()->getX())
			setHorizSpeed(1);
		else if (getX() > getPlayer()->getX())
			setHorizSpeed(-1);
		else
			setHorizSpeed(0);

		ticksToNextGrunt--;
		if (ticksToNextGrunt <= 0)
		{
			getWorld()->playSound(SOUND_ZOMBIE_ATTACK);
			ticksToNextGrunt = 20;
		}
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

bool ZombiePedestrian::beSprayedIfAppropriate()
{
	decHP(1);
	if (!isAlive())
	{
		getWorld()->playSound(SOUND_PED_DIE);
		if (!overlap(getPlayer()))
		{
			if (randInt(1, 5) == 1)
			{
				Actor* healingGoodie = new HealingGoodie(getWorld(), getX(), getY());
				getWorld()->addActor(healingGoodie);
			}
		}
		getWorld()->increaseScore(150);
	}
	else
		getWorld()->playSound(SOUND_PED_HURT);

	return true;
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

	if (overlap(getPlayer()) && !hasDamagedGhostRacer)
	{
		getWorld()->playSound(SOUND_VEHICLE_CRASH);
		getPlayer()->decHP(20);
		if (getX() <= getPlayer()->getX())
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

	if (getVertSpeed() > getPlayer()->getVertSpeed())
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

bool ZombieCab::beSprayedIfAppropriate()
{
	decHP(1);
	if (!isAlive())
	{
		getWorld()->playSound(SOUND_VEHICLE_DIE);
		if (randInt(1, 5) == 1)
		{
			Actor* oilSlick = new OilSlick(getWorld(), getX(), getY());
			getWorld()->addActor(oilSlick);
		}
		getWorld()->increaseScore(200);
		return true;
	}
	else
		getWorld()->playSound(SOUND_VEHICLE_HURT);
	return true;
}

//NoncollidingActor implementations
NoncollidingActor::NoncollidingActor(StudentWorld* game, int imageID, double startX, double startY, int dir = 0, double size = 1.0, unsigned int depth = 0)
	:Actor(game, imageID, startX, startY, dir, size, depth)
{
	setVertSpeed(-4);
}

bool NoncollidingActor::beSprayedIfAppropriate()
{
	if (isSprayable())
	{
		kill();
		return true;
	}
	return false;
}

//BorderLine implementations
BorderLine::BorderLine(StudentWorld* game, double startX, double startY, int colorID)
	:NoncollidingActor(game, colorID, startX, startY, 0, 2.0, 2)
{
}

void BorderLine::doSomething()
{
	if (!isAlive())
		return;
	
	moveOnScreen();

	if (offScreen())
		kill();
}

//OilSlick implementations
OilSlick::OilSlick(StudentWorld* game, double startX, double startY)
	:NoncollidingActor(game, IID_OIL_SLICK, startX, startY, 0, randInt(2, 5), 2)
{
}

void OilSlick::doSomething()
{
	moveOnScreen();
	if (offScreen())
		kill();
	
	if (overlap(getPlayer()))
	{
		getWorld()->playSound(SOUND_OIL_SLICK);
		getPlayer()->spin();
	}
}

//HealingGoodie implementation
HealingGoodie::HealingGoodie(StudentWorld* game, double startX, double startY)
	:NoncollidingActor(game, IID_HEAL_GOODIE, startX, startY, 0, 1.0, 2)
{
}

void HealingGoodie::doSomething()
{
	moveOnScreen();
	if (offScreen())
		kill();

	if (overlap(getPlayer()))
	{
		getPlayer()->heal();
		kill();
		getWorld()->playSound(SOUND_GOT_GOODIE);
		getWorld()->increaseScore(250);
	}
}

//HolyWaterGoodie implementations
HolyWaterGoodie::HolyWaterGoodie(StudentWorld* game, double startX, double startY)
	:NoncollidingActor(game, IID_HOLY_WATER_GOODIE, startX, startY, 90, 2.0, 2)
{
}

void HolyWaterGoodie::doSomething()
{
	moveOnScreen();
	if (offScreen())
		kill();

	if (overlap(getPlayer()))
	{
		getPlayer()->increaseHolyWater();
		kill();
		getWorld()->playSound(SOUND_GOT_GOODIE);
		getWorld()->increaseScore(50);
	}
}

//LostSoul implementations
LostSoul::LostSoul(StudentWorld* game, double startX, double startY)
	:NoncollidingActor(game, IID_SOUL_GOODIE, startX, startY, 0, 4.0, 2)
{
}

void LostSoul::doSomething()
{
	moveOnScreen();

	if (offScreen())
		kill();

	if (overlap(getPlayer()))
	{
		getWorld()->recordSoulSaved();
		kill();
		getWorld()->playSound(SOUND_GOT_SOUL);
		getWorld()->increaseScore(100);
	}
	setDirection(getDirection() - 10);
}

//Holy water projectile implementations
Spray::Spray(StudentWorld* game, double startX, double startY, int dir)
	:Actor(game, IID_HOLY_WATER_PROJECTILE, startX, startY, dir, 1.0, 1), maxTravelDistance(160)
{
}

void Spray::doSomething()
{
	if (!isAlive())
		return;

	if (getWorld()->sprayFirstAppropriateActor(this))
	{
		kill();
		return;
	}
	
	moveForward(SPRITE_HEIGHT);
	maxTravelDistance -= SPRITE_HEIGHT;

	if (offScreen() || maxTravelDistance <= 0)
		kill();
}