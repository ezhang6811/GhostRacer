#include "Actor.h"
#include "StudentWorld.h"

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp



//ghost racer implementations
GhostRacer::GhostRacer(double startX, double startY)
	:Actor(IID_GHOST_RACER, startX, startY, 90)
{

}

void GhostRacer::doSomething()
{

}


//BorderLine implementations
BorderLine::BorderLine(double startX, double startY, int colorID)
	:Actor(colorID, startX, startY)
{

}

void BorderLine::doSomething()
{

}